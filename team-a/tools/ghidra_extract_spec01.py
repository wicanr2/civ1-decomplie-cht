# -*- coding: utf-8 -*-
# Ghidra Jython 2.7 post-script.
#
# Dumps four artifacts from the analyzed CIV.EXE project into
# /mnt/d/03_game_tmp/civ1-decomplie-cht/team-a/dumps/:
#
#   01a_functions.txt        - every discovered function with address,
#                              segment, name, body size, and parameter
#                              count
#   01b_imports_used.txt     - per-module per-API call site count
#   01c_entrypoint_walk.txt  - disassembly starting at the binary's
#                              entry point, walking forward until the
#                              first instruction that reads from a
#                              different segment or 256 instructions
#                              (whichever first)
#   01d_signature_strings.txt- strings present in the binary that
#                              identify the compiler / runtime
#                              (Borland, Microsoft, Watcom signatures,
#                              CRT messages, etc.)
#
# Run via:
#   /opt/ghidra/support/analyzeHeadless /root/ghidra-projects civ1 \
#       -process CIV.EXE -noanalysis \
#       -postScript ghidra_extract_spec01.py \
#       -scriptPath /mnt/d/03_game_tmp/civ1-decomplie-cht/team-a/tools

from __future__ import print_function

import os
import re

from ghidra.program.model.address import Address
from ghidra.program.model.listing import Function, Instruction
from ghidra.program.model.symbol import SymbolType, SourceType
from ghidra.program.model.mem import MemoryBlock
from ghidra.program.flatapi import FlatProgramAPI
from ghidra.app.util.bin import ByteArrayProvider


OUT_DIR = "/mnt/d/03_game_tmp/civ1-decomplie-cht/team-a/dumps"


# --- helpers -------------------------------------------------------------

def open_out(name):
    path = os.path.join(OUT_DIR, name)
    return open(path, "w")


def addr_str(a):
    # Ghidra Address.toString() already yields e.g. "0001:0000" for segmented.
    return a.toString() if a else "<none>"


def segment_of(a):
    # For segmented memory, the segment is everything before ':' in the
    # address string. Flat addresses will have just hex.
    s = addr_str(a)
    if ":" in s:
        return s.split(":", 1)[0]
    return s


# --- 01a: function list --------------------------------------------------

def dump_functions(program):
    fm = program.getFunctionManager()
    funcs = list(fm.getFunctions(True))  # forward iteration
    funcs.sort(key=lambda f: f.getEntryPoint().getOffset())
    out = open_out("01a_functions.txt")
    out.write("# CIV.EXE — discovered functions ({0} total)\n".format(len(funcs)))
    out.write("# fields: addr  seg  name  body_size  param_count  source\n\n")
    for f in funcs:
        ep = f.getEntryPoint()
        body = f.getBody()
        sz = body.getNumAddresses() if body else 0
        params = len(f.getParameters())
        sym = program.getSymbolTable().getPrimarySymbol(ep)
        src = sym.getSource().toString() if sym else "?"
        out.write("{0:<14}  {1:<6}  {2:<48}  {3:>6}  {4:>3}  {5}\n".format(
            addr_str(ep),
            segment_of(ep),
            f.getName(),
            sz,
            params,
            src,
        ))
    out.close()
    print("[01a] wrote {0} functions".format(len(funcs)))


# --- 01b: imports inventory ---------------------------------------------

_NAME_RE = re.compile(r'NAME="([^"]+)"')


def _load_win16_api_module_map():
    """Parse Ghidra's bundled win16/*.exports XML files. Returns a
       dict upper-case-API-name -> module-name. The .exports files map
       Win16 module to exported API ordinals; we invert that."""
    api2mod = {}
    base = "/opt/ghidra/Ghidra/Features/Base/data/symbols/win16"
    if not os.path.isdir(base):
        return api2mod
    # Only the six modules CIV.EXE actually imports
    wanted = ("kernel", "user", "gdi", "win87em", "mmsystem", "commdlg")
    for fname in os.listdir(base):
        if not fname.endswith(".exports"):
            continue
        stem = fname[:-len(".exports")]
        if stem not in wanted:
            continue
        mod = stem.upper()
        path = os.path.join(base, fname)
        try:
            f = open(path)
            for line in f:
                for m in _NAME_RE.finditer(line):
                    api_name = m.group(1).upper()
                    # The LIBRARY tag also has a NAME= attribute; skip
                    # values that look like filenames.
                    if api_name.endswith(".EXE") or api_name.endswith(".DLL"):
                        continue
                    if api_name not in api2mod:
                        api2mod[api_name] = mod
            f.close()
        except Exception:
            pass
    return api2mod


def dump_imports(program):
    """Count call sites per imported Win16 API by following references
       to the FAR JMP thunks Ghidra discovered (these are the real
       call targets that appear in instructions, not the EXTERNAL
       pseudo-symbols)."""
    api2mod = _load_win16_api_module_map()
    rm = program.getReferenceManager()
    fm = program.getFunctionManager()
    rows = []
    seen = set()
    for f in fm.getFunctions(True):
        ep = f.getEntryPoint()
        name = f.getName().upper()
        # Thunk heuristic: 4-byte (FAR JMP) function whose name appears
        # in the Win16 export tables.
        body_sz = f.getBody().getNumAddresses() if f.getBody() else 0
        if name in api2mod and body_sz <= 8:
            ref_count = 0
            for _ in rm.getReferencesTo(ep):
                ref_count += 1
            rows.append((api2mod[name], name, ref_count, addr_str(ep)))
            seen.add(name)
    # Also fold in any external symbols that did not surface as thunks
    sm = program.getSymbolTable()
    for s in sm.getExternalSymbols():
        nm = s.getName().upper()
        if nm in seen:
            continue
        mod = api2mod.get(nm, "?")
        ref_count = 0
        for _ in rm.getReferencesTo(s.getAddress()):
            ref_count += 1
        rows.append((mod, nm, ref_count, addr_str(s.getAddress())))
    # group by module
    rows.sort(key=lambda r: (r[0], r[1]))
    out = open_out("01b_imports_used.txt")
    out.write("# CIV.EXE — imported Win16 API call inventory\n")
    out.write("# fields: module  name  call_sites  stub_addr\n\n")
    cur_mod = None
    mod_totals = {}
    for mod, name, n, addr in rows:
        if mod != cur_mod:
            out.write("\n## {0}\n".format(mod))
            cur_mod = mod
        out.write("  {0:<32}  {1:>5}  {2}\n".format(name, n, addr))
        mod_totals[mod] = mod_totals.get(mod, 0) + n
    out.write("\n## Totals\n")
    for mod, total in sorted(mod_totals.items()):
        out.write("  {0:<12} {1} call sites total\n".format(mod, total))
    out.close()
    print("[01b] wrote {0} imported symbols across {1} modules".format(
        len(rows), len(mod_totals)))


# --- 01c: entry-point walk ----------------------------------------------

def dump_entrypoint_walk(program):
    listing = program.getListing()
    entry = None
    # Find entry point symbol
    for s in program.getSymbolTable().getAllSymbols(True):
        if s.getName().lower() in ("entry", "_entry", "_start", "start"):
            entry = s.getAddress()
            break
    if entry is None:
        # fallback: use program.getImageBase() or the AddressFactory entry
        try:
            entry = program.getSymbolTable().getExternalEntryPointIterator().next()
        except Exception:
            entry = None
    out = open_out("01c_entrypoint_walk.txt")
    out.write("# CIV.EXE — entry-point disassembly walk\n")
    if entry is None:
        out.write("entry point not found via symbol table\n")
        out.close()
        return
    out.write("# starting at {0}\n\n".format(addr_str(entry)))
    inst = listing.getInstructionAt(entry)
    count = 0
    last_seg = segment_of(entry)
    while inst is not None and count < 256:
        a = inst.getAddress()
        seg = segment_of(a)
        if seg != last_seg:
            out.write(
                "\n# --- segment crossed: {0} -> {1} ---\n\n".format(last_seg, seg)
            )
            last_seg = seg
        out.write("{0}  {1:<40}  {2}\n".format(
            addr_str(a),
            str(inst),
            "; " + " ".join(["{0:02X}".format(b & 0xFF) for b in inst.getBytes()]),
        ))
        # Stop at first unconditional call to anything named WinMain-like
        flow = inst.getFlows()
        if flow:
            for tgt in flow:
                fn = program.getFunctionManager().getFunctionAt(tgt)
                if fn and re.search(r"WinMain|WEP|LibMain", fn.getName(),
                                    re.IGNORECASE):
                    out.write(
                        "\n# Entry reaches Win16 application body at {0} ({1})\n"
                        .format(addr_str(tgt), fn.getName()))
                    out.close()
                    return
        inst = inst.getNext()
        count += 1
    out.write("\n# walk stopped after {0} instructions\n".format(count))
    out.close()


# --- 01d: compiler signature strings -------------------------------------

COMPILER_PATTERNS = [
    (r"Borland C\+\+",          "Borland C++"),
    (r"Turbo C\+\+",            "Borland Turbo C++"),
    (r"Borland",                "Borland (generic)"),
    (r"Microsoft C/C\+\+",      "Microsoft C/C++"),
    (r"MSVC",                   "Microsoft Visual C/C++"),
    (r"Microsoft\(R\) C",       "Microsoft C"),
    (r"WATCOM",                 "Watcom C/C++"),
    (r"PharLap",                "Phar Lap"),
    (r"Phar Lap",               "Phar Lap"),
    (r"DGROUP",                 "DGROUP token (any C compiler)"),
    (r"_TEXT",                  "_TEXT segment name token"),
    (r"\\WIN3\\",               "MSC 7 Windows SDK build path"),
    (r"CMAIN",                  "Borland C startup symbol"),
    (r"_acrtused",              "MSC C runtime token"),
    (r"abnormal program",       "C runtime abort message"),
    (r"runtime error",          "C runtime error message"),
    (r"floating point",         "FPU subsystem message"),
    (r"R6\d\d\d",               "MSC runtime error code (R60xx-R69xx)"),
    (r"_TURBOHOST",             "Borland Turbo host"),
    (r"^M\\",                   "MS linker token"),
]


def dump_signatures(program):
    out = open_out("01d_signature_strings.txt")
    out.write("# CIV.EXE — compiler / runtime signature strings\n\n")
    found = {}
    # Iterate all defined strings in the program
    for s in program.getListing().getDefinedData(True):
        v = s.getValue()
        if not isinstance(v, (str, unicode)):
            continue
        if len(v) < 3:
            continue
        for patt, label in COMPILER_PATTERNS:
            if re.search(patt, v):
                found.setdefault(label, []).append((addr_str(s.getAddress()), v[:100]))
                break
    if not found:
        out.write("(no compiler-signature pattern matched any defined string)\n")
    else:
        for label, hits in sorted(found.items()):
            out.write("## {0}\n".format(label))
            for addr, txt in hits[:20]:
                out.write("  {0}  {1!r}\n".format(addr, txt))
            if len(hits) > 20:
                out.write("  ... and {0} more\n".format(len(hits) - 20))
            out.write("\n")
    # Also dump every defined string whose contents look like a build
    # path or filename so we can spot embedded debug paths.
    out.write("## Paths and filenames (heuristic)\n")
    path_re = re.compile(r"^[A-Z]:\\|\\WIN3?\\|\.\.\.\\|\.C$|\.OBJ$|\.LIB$", re.IGNORECASE)
    path_hits = 0
    for s in program.getListing().getDefinedData(True):
        v = s.getValue()
        if isinstance(v, (str, unicode)) and path_re.search(v):
            out.write("  {0}  {1!r}\n".format(addr_str(s.getAddress()), v[:120]))
            path_hits += 1
            if path_hits >= 60:
                out.write("  ... truncated at 60 hits\n")
                break
    out.close()
    print("[01d] wrote signature summary")


# --- entry ---------------------------------------------------------------

def main():
    program = getCurrentProgram()
    print("Working on program: {0}".format(program.getName()))
    dump_functions(program)
    dump_imports(program)
    dump_entrypoint_walk(program)
    dump_signatures(program)
    print("All four dumps written to {0}".format(OUT_DIR))


main()
