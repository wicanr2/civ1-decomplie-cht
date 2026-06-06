#!/usr/bin/env python3
"""在 CIV.EXE 內 grep 14 文明 + 14 領袖 + 地形名稱，找 hardcoded master tables。"""
import sys
from pathlib import Path


def find_strings(data, candidates):
    for name in candidates:
        idx = 0
        hits = []
        while True:
            n = data.find(name.encode("ascii"), idx)
            if n < 0:
                break
            hits.append(n)
            idx = n + 1
        if hits:
            ctx_off = hits[0]
            ctx = data[max(0, ctx_off - 8) : ctx_off + 20]
            ctx_text = "".join(
                chr(b) if 32 <= b < 127 else "."
                for b in ctx
            )
            print(f"  {name:<14}  hits={len(hits)}  @ 0x{hits[0]:06X}  ctx={ctx_text!r}")


def main():
    data = Path(sys.argv[1]).read_bytes()

    print("=== 14 civilizations (Fandom main page) ===")
    find_strings(data, [
        "Roman", "Babylonian", "German", "Egyptian", "American",
        "Greek", "Mongol", "Russian", "Zulu", "French",
        "Aztec", "Chinese", "English", "Indian",
    ])

    print("\n=== 14 leader candidates ===")
    find_strings(data, [
        "Caesar", "Hammurabi", "Frederick", "Cleopatra", "Lincoln",
        "Alexander", "Genghis", "Stalin", "Shaka", "Napoleon",
        "Montezuma", "Mao", "Elizabeth", "Gandhi",
    ])

    print("\n=== Terrain types ===")
    find_strings(data, [
        "Ocean", "Forest", "Swamp", "Plains", "Tundra", "Mountain",
        "Hill", "Desert", "Grassland", "Jungle", "Arctic", "River",
    ])

    print("\n=== Tech tree samples ===")
    find_strings(data, [
        "Alphabet", "Pottery", "Bronze", "Wheel", "Map Making",
        "Currency", "Mathematics", "Astronomy", "Navigation",
        "Engineering", "Iron Working", "Chivalry", "Feudalism",
        "Gunpowder", "Industrialization", "Steam Engine",
        "Computers", "Space Flight",
    ])

    print("\n=== Government types ===")
    find_strings(data, [
        "Despotism", "Monarchy", "Republic", "Democracy", "Communism",
        "Anarchy",
    ])

    print("\n=== Wonders ===")
    find_strings(data, [
        "Pyramids", "Hanging Gardens", "Colossus", "Lighthouse",
        "Oracle", "Great Wall", "Magellan", "Apollo Program",
    ])

    print("\n=== Units ===")
    find_strings(data, [
        "Settlers", "Militia", "Phalanx", "Legion", "Cavalry",
        "Catapult", "Knight", "Musketeers", "Cannon", "Riflemen",
        "Trireme", "Sail", "Frigate", "Ironclad", "Cruiser",
        "Battleship", "Submarine", "Carrier", "Transport",
        "Fighter", "Bomber", "Nuclear", "Diplomat", "Caravan",
    ])


if __name__ == "__main__":
    sys.exit(main())
