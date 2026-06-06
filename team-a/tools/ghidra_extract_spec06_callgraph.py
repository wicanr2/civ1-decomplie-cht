# -*- coding: utf-8 -*-
# Ghidra Jython 2.7 post-script - spec 06 §6.9 binary offset Ghidra callgraph walk
#
# R13 task: 從 FUN_11e8_0337 (spec 02 §2.1.2 E 段「data_units init」, 實際是
# 載 STR# 143/144/145/135 names) 開始往 callgraph 走, 找哪個 function 才是
# 真正初始化 unit stats const table.
#
# Strategy:
#   1. 找 FUN_11e8_0337 entry @ 11e8:0337
#   2. dump 其 callers (倒推 game-data init chain)
#   3. dump 其 callees (深入找 stats init)
#   4. 每個 callee decompile + 統計「連續 MOV [addr], imm」pattern
#      (unit struct init 的特徵: 一連串 byte/word 寫 globals)
#   5. 也找 import table 內 4 個 MMSYSTEM imports (補 spec 08 §8.2)

from __future__ import print_function, unicode_literals

import io
import os

from ghidra.app.decompiler import DecompInterface
from ghidra.util.task import ConsoleTaskMonitor


OUT_DIR = "/mnt/d/03_game_tmp/civ1-decomplie-cht/team-a/dumps"


def dump_callers_callees(out, fm, ref_mgr, func, label):
    if func is None:
        out.write(u"{0}: function None\n".format(label))
        return
    out.write(u"\n=== {0}: {1} @ {2} ===\n".format(
        label, func.getName(), func.getEntryPoint()))
    out.write(u"  body size: {0} bytes\n".format(func.getBody().getNumAddresses()))

    # Callees (function calls this calls)
    callees = func.getCalledFunctions(ConsoleTaskMonitor())
    out.write(u"  Callees ({0}):\n".format(len(callees)))
    for c in callees:
        out.write(u"    {0} @ {1}\n".format(c.getName(), c.getEntryPoint()))

    # Callers (functions calling this)
    callers = func.getCallingFunctions(ConsoleTaskMonitor())
    out.write(u"  Callers ({0}):\n".format(len(callers)))
    for c in callers:
        out.write(u"    {0} @ {1}\n".format(c.getName(), c.getEntryPoint()))


def decompile_with_mov_count(out, decomp, func, label):
    if func is None:
        return
    monitor = ConsoleTaskMonitor()
    res = decomp.decompileFunction(func, 30, monitor)
    if res is None or not res.decompileCompleted():
        out.write(u"  decompile failed: {0}\n".format(label))
        return
    code = res.getDecompiledFunction().getC()
    # 數「DAT_xxxx_yyyy = 數字」pattern (Ghidra synthetic 寫 globals)
    n_dat_write = 0
    lines = code.split('\n')
    for line in lines:
        # 粗略: 含 "DAT_" 且含 " = " 且不含 function call
        if "DAT_" in line and " = " in line and "(" not in line.split(" = ", 1)[1]:
            n_dat_write += 1
    out.write(u"\n  {0}: {1} DAT_*=imm 寫 globals (推測 struct init)\n".format(
        label, n_dat_write))

    # 把整段 C dump 到單檔
    fn_name = func.getName().replace("/", "_").replace(":", "_")
    decomp_path = os.path.join(OUT_DIR, "06b_callgraph_{0}.c".format(fn_name))
    with io.open(decomp_path, "w", encoding="utf-8") as f:
        f.write(code)
    out.write(u"  written: {0}\n".format(decomp_path))


def list_mmsystem_imports(out, program):
    """spec 08 §8.2 補: 4 個 MMSYSTEM imports 確切 API"""
    out.write(u"\n=== MMSYSTEM imports (spec 08 §8.2) ===\n")
    sym_table = program.getSymbolTable()
    # 找所有 external symbol with "MMSYSTEM" parent or in name
    externs = sym_table.getExternalSymbols()
    found = []
    while externs.hasNext():
        sym = externs.next()
        name = sym.getName()
        parent_name = ""
        try:
            parent = sym.getParentSymbol()
            if parent:
                parent_name = parent.getName()
        except Exception:
            pass
        if "MMSYSTEM" in name or "MMSYSTEM" in parent_name or \
                "SNDPLAY" in name.upper() or "WAVE" in name.upper() or \
                "MCI" in name.upper() or "PLAYSOUND" in name.upper():
            found.append((sym.getName(), parent_name, sym.getAddress()))

    out.write(u"  found {0} MMSYSTEM-related external symbols\n".format(len(found)))
    for name, parent, addr in found:
        out.write(u"    {0}.{1} @ {2}\n".format(parent, name, addr))


def main():
    program = getCurrentProgram()
    print("Working on: {0}".format(program.getName()))

    out = io.open(os.path.join(OUT_DIR, "06_callgraph_unit_init.txt"),
                  "w", encoding="utf-8")
    out.write(u"# spec 06 §6.9 R13 callgraph walk for unit stats init\n\n")

    af = program.getAddressFactory()
    fm = program.getFunctionManager()
    ref_mgr = program.getReferenceManager()
    decomp = DecompInterface()
    decomp.openProgram(program)

    # FUN_11e8_0337 — data_units init from spec 02 §2.1.2 E
    # NE 16-bit segment:offset 11e8:0337
    addr_candidates = ["11e80337", "11e8:0337"]
    target_func = None
    for addr_str in addr_candidates:
        try:
            addr = af.getAddress(addr_str)
            if addr is not None:
                f = fm.getFunctionContaining(addr)
                if f is not None:
                    target_func = f
                    break
        except Exception as e:
            print("addr parse err: {0} for {1}".format(e, addr_str))
            continue

    if target_func is None:
        out.write(u"FUN_11e8_0337 not found, listing all functions starting with FUN_11e8\n")
        all_funcs = fm.getFunctions(True)
        while all_funcs.hasNext():
            f = all_funcs.next()
            if "11e8" in str(f.getEntryPoint()):
                out.write(u"  {0} @ {1}\n".format(f.getName(), f.getEntryPoint()))
        out.close()
        return

    dump_callers_callees(out, fm, ref_mgr, target_func, "data_units (FUN_11e8_0337)")

    # 每個 callee decompile + 統計 DAT_* writes
    out.write(u"\n=== Decompile callees + DAT_* write count ===\n")
    callees = target_func.getCalledFunctions(ConsoleTaskMonitor())
    for c in callees:
        decompile_with_mov_count(out, decomp, c,
                                  "{0} @ {1}".format(c.getName(), c.getEntryPoint()))

    # 也分析 target_func 本體
    decompile_with_mov_count(out, decomp, target_func,
                              "data_units self ({0})".format(target_func.getEntryPoint()))

    # MMSYSTEM imports (spec 08 §8.2)
    list_mmsystem_imports(out, program)

    out.close()
    print("Done. See 06_callgraph_unit_init.txt + 06b_callgraph_*.c")


main()
