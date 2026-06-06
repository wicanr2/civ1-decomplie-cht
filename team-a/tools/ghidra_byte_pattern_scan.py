# -*- coding: utf-8 -*-
# Ghidra Jython 2.7 post-script - byte-pattern scan 找 gr_pic.c 家族函式。
#
# 假設：Borland C 16-bit Win16 把 assert() 內字串引用編成 PUSH imm16
# (opcode 0x68 LL HH)，這 imm16 = 字串在 DGROUP 內的 offset。
# segment 1420 是 DGROUP 內 assert 字串集中地：
#
#   0x2300  GR_PicRead()  :  gr_pic.c
#   0x234A  GR_PortDataToBitmap()  :  gr_pic.c
#   0x2372  LoadGifPicture()  :  gr_pic.c
#   0x23B2  InvertBitmap()  :  gr_pic.c
#   0x23CE  PicDecompress()  :  gr_pic.c
#
# Strategy: 掃過所有 instruction，找 mnemonic = "PUSH" 且 operand 為
# 上述 immediate 值之一者。記錄包含該 instruction 的 function。
#
# 找到的 function 接著 decompile 到 team-a/dumps/03a_<name>.c。

from __future__ import print_function, unicode_literals

import io
import os

from ghidra.app.decompiler import DecompInterface
from ghidra.util.task import ConsoleTaskMonitor


OUT_DIR = "/mnt/d/03_game_tmp/civ1-decomplie-cht/team-a/dumps"

# (name, target imm16 offset within DGROUP)
TARGETS = [
    ("GR_PicRead",          0x2300),
    ("GR_PortDataToBitmap", 0x234A),
    ("LoadGifPicture",      0x2372),
    ("InvertBitmap",        0x23B2),
    ("PicDecompress",       0x23CE),
]


def open_out(name):
    return io.open(os.path.join(OUT_DIR, name), "w", encoding="utf-8")


def scan_for_pushes(program, target_imm16):
    """掃過所有 instruction 找 PUSH imm16 = target_imm16。
       回傳 set of Function 物件。"""
    found = set()
    fm = program.getFunctionManager()
    listing = program.getListing()
    instr_iter = listing.getInstructions(True)
    count = 0
    while instr_iter.hasNext():
        inst = instr_iter.next()
        count += 1
        if inst.getMnemonicString() != "PUSH":
            continue
        # PUSH 可能有一個 operand；運算值 = scalar / register / address
        if inst.getNumOperands() < 1:
            continue
        op0 = inst.getOpObjects(0)
        if not op0:
            continue
        for obj in op0:
            v = None
            # Scalar (ghidra.program.model.scalar.Scalar)
            try:
                v = obj.getUnsignedValue()
            except AttributeError:
                pass
            if v is None:
                continue
            if v == target_imm16:
                func = fm.getFunctionContaining(inst.getAddress())
                if func is not None:
                    found.add(func)
    return found, count


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
        # 為避免重複掃，做一次完整掃存 cache
        # 但 instruction iter 不能 rewind，所以對每個 target 各掃一遍
        for name, imm in TARGETS:
            found, total = scan_for_pushes(program, imm)
            if not found:
                print("[03a] {0}: PUSH 0x{1:X} 未找到 (掃 {2} instructions)".format(
                    name, imm, total))
                continue
            print("[03a] {0}: PUSH 0x{1:X} 找到 {2} 個 function".format(
                name, imm, len(found)))
            # 最佳猜測 = 第一個 (通常 assert 一個 function 只引用一次)
            for i, func in enumerate(sorted(found, key=lambda f: f.getEntryPoint().getOffset())):
                ep = func.getEntryPoint()
                body_sz = func.getBody().getNumAddresses() if func.getBody() else 0
                fname = "{0}_{1}".format(name, i) if i > 0 else name
                c_text = decompile_function(decomp, func)
                if c_text is None:
                    print("    {0} @ {1}: decompile FAILED".format(fname, ep))
                    continue
                out_name = "03a_{0}.c".format(fname.lower())
                out = open_out(out_name)
                out.write("/* CIV.EXE gr_pic.c family decompile via byte-pattern scan\n")
                out.write(" * function : {0}\n".format(fname))
                out.write(" * address  : {0}\n".format(ep))
                out.write(" * body size: {0} bytes\n".format(body_sz))
                out.write(" *\n")
                out.write(" * Located by scanning for instruction `PUSH 0x{0:X}` (= assert\n".format(imm))
                out.write(" * string offset for `{0}()  :  gr_pic.c` in DGROUP).\n".format(name))
                out.write(" *\n")
                out.write(" * Source: Ghidra DecompInterface (Pcode).\n")
                out.write(" * Variable names are Ghidra synthetic (uVar1 / iVar2).\n")
                out.write(" */\n\n")
                out.write(unicode(c_text))
                out.close()
                print("    {0} @ {1}: {2} bytes -> {3}".format(fname, ep, body_sz, out_name))
    finally:
        decomp.closeProgram()
    print("Scan complete; dumps under {0}".format(OUT_DIR))


main()
