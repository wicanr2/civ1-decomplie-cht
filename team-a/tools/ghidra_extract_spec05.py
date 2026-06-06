# -*- coding: utf-8 -*-
# Ghidra Jython 2.7 post-script - spec 05 game data tables walk
#
# spec 02 §2.1.2 E 段 WinMain 連續呼叫 7 個 game-data init function:
#
#   FUN_1008_059a()           # 推測：載入文明資料表
#   FUN_10b8_0000()           # 推測：載入科技樹
#   FUN_1008_0b4a()           # 推測：載入政府型態
#   FUN_1090_0488()           # 推測：載入建築
#   FUN_11e8_0337()           # 推測：載入單位
#   FUN_11e0_0000()           # 推測：地圖 / 城市 init
#   FUN_10e8_2d46()           # 推測：載入 advisor 文本
#
# 對每個 function decompile + 走它呼叫的子 function chain，找：
# - 連續同型結構 record (14 個文明 / N 個科技 / N 個單位)
# - hardcoded data 表的 segment offset
# - assert string 名 (Borland 把 source-level identifier 烤進 binary)

from __future__ import print_function, unicode_literals

import io
import os

from ghidra.app.decompiler import DecompInterface
from ghidra.util.task import ConsoleTaskMonitor


OUT_DIR = "/mnt/d/03_game_tmp/civ1-decomplie-cht/team-a/dumps"

# spec 02 §2.1.2 E 段的 7 個 game-data init function
TARGETS = [
    ("data_civs",      "1008:059a"),  # 推測：14 文明
    ("data_tech",      "10b8:0000"),  # 推測：科技樹
    ("data_govs",      "1008:0b4a"),  # 推測：5 政府型態
    ("data_builds",    "1090:0488"),  # 推測：建築
    ("data_units",     "11e8:0337"),  # 推測：單位
    ("data_map_init",  "11e0:0000"),  # 推測：地圖/城市
    ("data_advisors",  "10e8:2d46"),  # 推測：advisor 文本
]


def open_out(name):
    return io.open(os.path.join(OUT_DIR, name), "w", encoding="utf-8")


def decompile_function(decomp, func):
    monitor = ConsoleTaskMonitor()
    res = decomp.decompileFunction(func, 60, monitor)
    if res is None or not res.decompileCompleted():
        return None
    return res.getDecompiledFunction().getC()


def main():
    program = getCurrentProgram()
    print("Working on: {0}".format(program.getName()))
    af = program.getAddressFactory()
    fm = program.getFunctionManager()
    decomp = DecompInterface()
    decomp.openProgram(program)

    try:
        for name, addr_str in TARGETS:
            addr = af.getAddress(addr_str)
            if addr is None:
                print("[05a] {0}: address parse failed".format(name))
                continue
            func = fm.getFunctionAt(addr)
            if func is None:
                # 可能 addr 在 function 中間
                func = fm.getFunctionContaining(addr)
            if func is None:
                print("[05a] {0} @ {1}: no function".format(name, addr_str))
                continue

            ep = func.getEntryPoint()
            body_sz = func.getBody().getNumAddresses() if func.getBody() else 0
            c_text = decompile_function(decomp, func)
            if c_text is None:
                print("[05a] {0} @ {1}: decompile FAILED".format(name, ep))
                continue

            out_name = "05a_{0}.c".format(name)
            out = open_out(out_name)
            out.write("/* CIV.EXE game-data init decompile (spec 02 §2.1.2 E 段)\n")
            out.write(" * task     : {0}\n".format(name))
            out.write(" * address  : {0}\n".format(ep))
            out.write(" * body size: {0} bytes\n".format(body_sz))
            out.write(" *\n")
            out.write(" * spec 02 §2.1.2 E 段對應假設角色：\n")
            out.write(" *   {0}\n".format(name))
            out.write(" *\n")
            out.write(" * Source: Ghidra DecompInterface (Pcode).\n")
            out.write(" * 變數名 Ghidra synthetic (uVar1/iVar2)。\n")
            out.write(" */\n\n")
            out.write(unicode(c_text))
            out.close()
            print("[05a] {0} @ {1}: {2} bytes -> {3}".format(
                name, ep, body_sz, out_name))
    finally:
        decomp.closeProgram()
    print("game-data decompiles written to {0}".format(OUT_DIR))


main()
