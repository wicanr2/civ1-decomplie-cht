# -*- coding: utf-8 -*-
# Ghidra Jython 2.7 post-script - LoadGifPicture decoder family extraction.
#
# 從 spec 01 §1.2 / signature strings 知道 gr_pic.c 含這些函式 (依
# Borland assert() 字串位置):
#
#   GR_PicRead          @ string 1420:2300
#   GR_PortDataToBitmap @ string 1420:234a
#   LoadGifPicture      @ string 1420:2372  <- 我們要解的 LZW decoder
#   InvertBitmap        @ string 1420:23b2
#   PicDecompress       @ string 1420:23ce  <- EDILZSS2 (Track A 已知)
#
# 各 function 內第一個對應字串的 PUSH 通常是 assert() 的 __FILE__ /
# __FUNCSIG__ - 所以「reference 該字串的函式」就是該函式自己。
#
# 對每個目標, 找 ref 該 assert string 的 function -> decompile ->
# 輸出到 team-a/dumps/03a_<name>.c。

from __future__ import print_function, unicode_literals

import io
import os

from ghidra.app.decompiler import DecompInterface
from ghidra.util.task import ConsoleTaskMonitor


OUT_DIR = "/mnt/d/03_game_tmp/civ1-decomplie-cht/team-a/dumps"

# (target function name, addr of its assert() string in segment 1420)
TARGETS = [
    ("GR_PicRead",          "1420:2300"),
    ("GR_PortDataToBitmap", "1420:234a"),
    ("LoadGifPicture",      "1420:2372"),
    ("InvertBitmap",        "1420:23b2"),
    ("PicDecompress",       "1420:23ce"),
]


def open_out(name):
    return io.open(os.path.join(OUT_DIR, name), "w", encoding="utf-8")


def find_function_referencing(program, addr_str):
    """Find first function that references the given address."""
    af = program.getAddressFactory()
    target = af.getAddress(addr_str)
    if target is None:
        return None
    rm = program.getReferenceManager()
    fm = program.getFunctionManager()
    for ref in rm.getReferencesTo(target):
        from_addr = ref.getFromAddress()
        func = fm.getFunctionContaining(from_addr)
        if func is not None:
            return func
    return None


def decompile_function(decomp, func):
    monitor = ConsoleTaskMonitor()
    res = decomp.decompileFunction(func, 60, monitor)
    if res is None or not res.decompileCompleted():
        return None
    return res.getDecompiledFunction().getC()


def main():
    program = getCurrentProgram()
    print("Working on: {0}".format(program.getName()))
    decomp = DecompInterface()
    decomp.openProgram(program)
    try:
        for name, addr in TARGETS:
            func = find_function_referencing(program, addr)
            if func is None:
                print("[03a] {0}: assert string @ {1} has no ref / no function".format(name, addr))
                continue
            ep = func.getEntryPoint()
            body_sz = func.getBody().getNumAddresses() if func.getBody() else 0
            c_text = decompile_function(decomp, func)
            if c_text is None:
                print("[03a] {0} @ {1}: decompile FAILED".format(name, ep))
                continue
            out_name = "03a_{0}.c".format(name.lower())
            out = open_out(out_name)
            out.write("/* CIV.EXE gr_pic.c family decompile\n")
            out.write(" * function : {0}\n".format(name))
            out.write(" * address  : {0}\n".format(ep))
            out.write(" * body size: {0} bytes\n".format(body_sz))
            out.write(" *\n")
            out.write(" * Located via Borland assert() string ref at {0}.\n".format(addr))
            out.write(" * Source: Ghidra DecompInterface (Pcode).\n")
            out.write(" * Use as structural guide; variable names are Ghidra synthetic.\n")
            out.write(" */\n\n")
            out.write(unicode(c_text))
            out.close()
            print("[03a] {0} @ {1}: {2} bytes -> {3}".format(name, ep, body_sz, out_name))
    finally:
        decomp.closeProgram()
    print("LoadGifPicture family dumps written to {0}".format(OUT_DIR))


main()
