# -*- coding: utf-8 -*-
# Ghidra Jython 2.7 post-script — spec 02 extraction.
#
# 對 11 個導出 callback 與 entry stub 做 decompile，輸出到
# team-a/dumps/02a_<name>.c。另外從 entry 追到使用者代碼起點
# (WinMain 候選) 並 decompile，輸出 02b_winmain_chain.c。
#
# 執行：
#   /opt/ghidra/support/analyzeHeadless /root/ghidra-projects civ1 \
#       -process CIV.EXE -noanalysis \
#       -postScript ghidra_extract_spec02.py \
#       -scriptPath /mnt/d/03_game_tmp/civ1-decomplie-cht/team-a/tools

from __future__ import print_function, unicode_literals

import io
import os
import re

from ghidra.app.decompiler import DecompInterface
from ghidra.util.task import ConsoleTaskMonitor


OUT_DIR = "/mnt/d/03_game_tmp/civ1-decomplie-cht/team-a/dumps"

CALLBACKS = [
    "entry",
    "TIMERPROC",
    "ENUMFUNC",
    "DLGPROC",
    "CIVDIALOGPROC",
    "WDWMAPPROC",
    "WDWSMMAPPROC",
    "WDWSTATUSPROC",
    "RANDOMRADIOPROC",
    "RANDOMSTATICPROC",
    "RANDOMUSERPROC",
    "REGIONPROC",
]


def open_out(name):
    # UTF-8 — decompiled bodies contain Big5 literal strings from
    # the Track A patched binary.
    return io.open(os.path.join(OUT_DIR, name), "w", encoding="utf-8")


def find_function_by_name(program, name):
    fm = program.getFunctionManager()
    for f in fm.getFunctions(True):
        if f.getName() == name:
            return f
    return None


def decompile_function(decomp, func):
    monitor = ConsoleTaskMonitor()
    res = decomp.decompileFunction(func, 60, monitor)
    if res is None or not res.decompileCompleted():
        return None
    return res.getDecompiledFunction().getC()


def dump_callbacks(program, decomp):
    found = {}
    for cb in CALLBACKS:
        f = find_function_by_name(program, cb)
        if f is None:
            print("[02a] {0}: NOT FOUND".format(cb))
            continue
        ep = f.getEntryPoint()
        body_sz = f.getBody().getNumAddresses() if f.getBody() else 0
        c_text = decompile_function(decomp, f)
        if c_text is None:
            print("[02a] {0} @ {1}: decompile FAILED".format(cb, ep))
            continue
        out_name = "02a_{0}.c".format(cb.lower())
        out = open_out(out_name)
        out.write("/* CIV.EXE callback decompile\n")
        out.write(" * function : {0}\n".format(cb))
        out.write(" * address  : {0}\n".format(ep))
        out.write(" * body size: {0} bytes\n".format(body_sz))
        out.write(" *\n")
        out.write(" * Source: Ghidra DecompInterface (Pcode).\n")
        out.write(" * NOT a faithful original-source recovery; the original\n")
        out.write(" * Borland C++ source has been lost. Variable names are\n")
        out.write(" * Ghidra synthetic (uVar1 / iVar2). Use as a structural\n")
        out.write(" * guide only.\n")
        out.write(" */\n\n")
        # c_text comes back as a Java String; coerce to unicode for the
        # UTF-8 writer.
        out.write(unicode(c_text))
        out.close()
        found[cb] = (ep, body_sz)
        print("[02a] {0} @ {1}: {2} bytes -> {3}".format(cb, ep, body_sz, out_name))
    return found


def walk_entry_to_winmain(program, decomp):
    """From 'entry', follow the C runtime startup chain (callf to
       Borland _c0w internals) and stop at the first user-code call
       that targets RegisterClass or LoadMenu (these indicate WinMain
       is right there). Decompile that function."""
    fm = program.getFunctionManager()
    entry = find_function_by_name(program, "entry")
    if entry is None:
        print("[02b] entry not found")
        return

    visited = set()
    queue = [entry]
    winmain_candidate = None

    # The plan: traverse the call graph forward from entry. Any
    # function that contains a call to REGISTERCLASS or LOADMENU is a
    # WinMain candidate.
    while queue and winmain_candidate is None:
        f = queue.pop(0)
        if f.getEntryPoint() in visited:
            continue
        visited.add(f.getEntryPoint())
        # Inspect every instruction in this function for calls to
        # known Win16-app-init APIs.
        body = f.getBody()
        listing = program.getListing()
        it = listing.getInstructions(body, True)
        while it.hasNext():
            inst = it.next()
            flows = inst.getFlows()
            for tgt in flows:
                tgt_fn = fm.getFunctionAt(tgt)
                if tgt_fn is None:
                    continue
                tgt_name = tgt_fn.getName().upper()
                if tgt_name in ("REGISTERCLASS", "LOADMENU", "LOADACCELERATORS"):
                    winmain_candidate = f
                    break
                if tgt_fn.getEntryPoint() not in visited:
                    queue.append(tgt_fn)
            if winmain_candidate is not None:
                break

    out = open_out("02b_winmain_chain.c")
    out.write("/* CIV.EXE startup chain walk\n")
    out.write(" *\n")
    out.write(" * Walked the call graph forward from 'entry' looking for\n")
    out.write(" * the first function that calls REGISTERCLASS, LOADMENU,\n")
    out.write(" * or LOADACCELERATORS. Such a function is the application's\n")
    out.write(" * WinMain (or equivalent).\n")
    out.write(" */\n\n")
    if winmain_candidate is None:
        out.write("// No WinMain candidate found within the reachable call\n")
        out.write("// graph from entry.\n")
        out.close()
        print("[02b] no WinMain candidate found")
        return
    out.write("// WinMain candidate: {0} @ {1}\n\n".format(
        winmain_candidate.getName(), winmain_candidate.getEntryPoint()))
    c_text = decompile_function(decomp, winmain_candidate)
    if c_text:
        out.write(unicode(c_text))
    else:
        out.write(u"// decompile failed for {0}\n".format(winmain_candidate.getName()))
    out.close()
    print("[02b] WinMain candidate: {0} @ {1}".format(
        winmain_candidate.getName(), winmain_candidate.getEntryPoint()))


def main():
    program = getCurrentProgram()
    print("Working on: {0}".format(program.getName()))
    decomp = DecompInterface()
    decomp.openProgram(program)
    try:
        dump_callbacks(program, decomp)
        walk_entry_to_winmain(program, decomp)
    finally:
        decomp.closeProgram()
    print("Spec 02 dumps written to {0}".format(OUT_DIR))


main()
