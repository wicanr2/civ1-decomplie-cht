# -*- coding: utf-8 -*-
# Ghidra Jython 2.7 post-script - spec 06 unit stats 表掃描
#
# 1991 manual 提供 28 個 unit stats ground-truth:
#   Settlers 0-1-1 [40] / Militia 1-1-1 [10] / Phalanx 1-2-1 [20] /
#   Legion 3-1-1 [20] / Musketeers 2-3-1 [30] / Riflemen 3-5-1 [30] /
#   Cavalry 2-1-2 [20] / Knights 4-2-2 [40] / Catapult 6-1-1 [40] /
#   Cannon 8-1-1 [40] / Chariot 4-1-2 [40] / Armor 10-5-3 [80] /
#   Mech 6-6-3 [50] / Artillery 12-2-2 [60] / Fighter 3-3-10 [60] /
#   Bomber 12-1-8 [120] / Trireme 1-0-3 [40] / Sail 1-1-3 [40] /
#   Frigate 2-2-3 [40] / Ironclad 4-4-4 [60] / Cruiser 6-6-6 [80] /
#   Battleship 18-12-4 [160] / Submarine 8-2-3 [50] / Carrier 1-12-5 [160] /
#   Transport 0-3-4 [50] / Nuclear 99-0-16 [160] / Diplomat 0-0-2 [30] /
#   Caravan 0-1-1 [50]
#
# 此 script 跑兩種策略:
#   A. byte pattern scan: 找 各種長度的 attack / defense / cost / move 排列
#   B. 字串 cross-ref: "Settlers" 等 STR# 132 字串不在 CIV.EXE (spec 05 確認),
#      但 STR# 134 "Veteran Units" 等可能在. 退一步找 "Civdata0" 開檔 + 結構
#      讀取的 function. 那 function 周圍可能有 hardcoded unit 表.
#
# 對 spec 02 §2.1.2 E 段已知 `FUN_11e8_0337` (data_units) decompile, 找其
# 內 hardcoded 初始化 (e.g. `*(byte*)(seg + N) = 0` 連續 28 個).

from __future__ import print_function, unicode_literals

import io
import os

from ghidra.app.decompiler import DecompInterface
from ghidra.util.task import ConsoleTaskMonitor


OUT_DIR = "/mnt/d/03_game_tmp/civ1-decomplie-cht/team-a/dumps"

# Manual ground-truth byte arrays
ATTACK_28  = [0,1,1,3,2,3,2,4,6,8,4,10,6,12,3,12,1,1,2,4,6,18,8,1,0,99,0,0]
DEFENSE_28 = [1,1,2,1,3,5,1,2,1,1,1,5,6,2,3,1,0,1,2,4,6,12,2,12,3,0,0,1]
MOVE_28    = [1,1,1,1,1,1,2,2,1,1,2,3,3,2,10,8,3,3,3,4,6,4,3,5,4,16,2,1]
COST_28    = [40,10,20,20,30,30,20,40,40,40,40,80,50,60,60,120,40,40,40,60,80,160,50,160,50,160,30,50]


def find_byte_pattern(program, pattern, label):
    mem = program.getMemory()
    pat_bytes = ''.join(chr(b) for b in pattern)
    addrs = []
    addr = mem.getMinAddress()
    while addr is not None and addr.compareTo(mem.getMaxAddress()) < 0:
        try:
            found = mem.findBytes(addr, pat_bytes, None, True, ConsoleTaskMonitor())
        except Exception as e:
            print("  find err: {0}".format(e))
            break
        if found is None: break
        addrs.append(found)
        addr = found.add(1)
        if len(addrs) > 20: break
    print("{0}: {1} hits".format(label, len(addrs)))
    for a in addrs[:10]:
        # show 8 byte context after
        print("  @ {0}".format(a))
    return addrs


def main():
    program = getCurrentProgram()
    print("Working on: {0}".format(program.getName()))

    out = io.open(os.path.join(OUT_DIR, "06_unit_stats_scan.txt"), "w", encoding="utf-8")
    out.write(u"# spec 06 unit stats byte pattern scan\n\n")

    # 1. full 28-byte arrays
    print("\n=== Full 28-byte arrays ===")
    for label, arr in [("attack", ATTACK_28), ("defense", DEFENSE_28),
                       ("move", MOVE_28), ("cost", COST_28)]:
        hits = find_byte_pattern(program, arr, label)
        out.write(u"{0} 28B: {1} hits ".format(label, len(hits)))
        for h in hits[:5]:
            out.write(u" @{0}".format(h))
        out.write(u"\n")

    # 2. shorter prefixes (10 byte = 10 unit) for partial hit
    print("\n=== 10-byte prefix arrays ===")
    for label, arr in [("attack", ATTACK_28), ("defense", DEFENSE_28),
                       ("move", MOVE_28), ("cost", COST_28)]:
        hits = find_byte_pattern(program, arr[:10], label + "[:10]")
        out.write(u"{0} 10B prefix: {1} hits".format(label, len(hits)))
        for h in hits[:5]:
            out.write(u" @{0}".format(h))
        out.write(u"\n")

    # 3. unique sequence: 99 (Nuclear attack) is unique
    print("\n=== Nuclear A=99 around (sentinel) ===")
    # 0x63 byte at offset 25 in attack array. Find 0x63 0x00 byte sequence
    hits = find_byte_pattern(program, [0x63, 0x00, 0x00, 0x00], "63 00 00 00")
    out.write(u"\n0x63 (=99) 4-byte: {0} hits".format(len(hits)))
    for h in hits[:8]:
        out.write(u" @{0}".format(h))
    out.write(u"\n")

    # 4. (a,d) struct: pack a/d adjacent
    print("\n=== Struct (a,d) consecutive ===")
    ad_pairs = []
    for i in range(28):
        ad_pairs.extend([ATTACK_28[i], DEFENSE_28[i]])
    hits = find_byte_pattern(program, ad_pairs, "(a,d)×28")
    out.write(u"\n(a,d) interleaved 56B: {0} hits".format(len(hits)))
    for h in hits[:5]:
        out.write(u" @{0}".format(h))
    out.write(u"\n")

    # 5. Struct (a,d,m,cost) 4 byte each
    adm_pairs = []
    for i in range(28):
        adm_pairs.extend([ATTACK_28[i], DEFENSE_28[i], MOVE_28[i], COST_28[i]])
    hits = find_byte_pattern(program, adm_pairs, "(a,d,m,cost)×28")
    out.write(u"\n(a,d,m,cost) interleaved 112B: {0} hits".format(len(hits)))
    for h in hits[:5]:
        out.write(u" @{0}".format(h))
    out.write(u"\n")

    # 6. decompile FUN_11e8_0337 (data_units init from spec 02 §2.1.2 E)
    print("\n=== Decompile FUN_11e8_0337 (data_units init) ===")
    af = program.getAddressFactory()
    fm = program.getFunctionManager()
    decomp = DecompInterface()
    decomp.openProgram(program)
    addr = af.getAddress("11e8:0337")
    func = fm.getFunctionContaining(addr)
    if func is None:
        # try Generic linear address (NE seg:off)
        addr = af.getAddress("0011e8:00000337")
        func = fm.getFunctionContaining(addr)
    if func is None:
        print("data_units func @11e8:0337 not found, listing functions in seg 11e8")
        all_funcs = fm.getFunctions(True)
        seg_matches = []
        while all_funcs.hasNext():
            f = all_funcs.next()
            if "11e8" in str(f.getEntryPoint()):
                seg_matches.append(f)
        print("seg 11e8 funcs: {0}".format(len(seg_matches)))
        for f in seg_matches[:10]:
            print("  {0} @ {1}".format(f.getName(), f.getEntryPoint()))
    else:
        print("Decompiling {0} @ {1}".format(func.getName(), func.getEntryPoint()))
        monitor = ConsoleTaskMonitor()
        res = decomp.decompileFunction(func, 60, monitor)
        if res is not None and res.decompileCompleted():
            with io.open(os.path.join(OUT_DIR, "06a_data_units_decompile.c"), "w", encoding="utf-8") as f:
                f.write(res.getDecompiledFunction().getC())
            print("Wrote 06a_data_units_decompile.c")

    out.close()
    print("\nDone. See 06_unit_stats_scan.txt")


main()
