# Clean-room protocol

## Why

The 1993 *Civilization for Windows* source code is not publicly available and is, by all reports, lost. Several open-source projects (notably OpenCiv1, a C# port of the 1991 DOS version) reverse-engineered the game from disassembly. Their code is MIT-licensed and would be technically usable, but the goal of this project is an **independent** reconstruction: a codebase whose authorship history can be traced back to specifications written from disassembly observations alone, with no derivation from any prior reverse-engineered implementation.

This both eliminates ambiguity around IP provenance and forces the implementation to confront the original binary's behavior directly, which surfaces details that derivative ports tend to paper over.

## The two-team split

| Team | Inputs | Outputs | Forbidden |
|------|--------|---------|-----------|
| A — disassembly | Ghidra project on `CIV.EXE`, Track A's EDILZSS2 format spec | `team-a/specs/*.md`, `team-a/dumps/*.txt` | Writing C / C++ / any compilable code; reading any file under `team-b/`; reading OpenCiv1 / Freeciv / CivOne |
| B — implementation | `team-a/specs/*.md` only | `team-b/src/**`, `team-b/tests/**` | Opening Ghidra; loading `CIV.EXE` into any disassembler; reading OpenCiv1 or any other Civ reimplementation; reading `team-a/dumps/` (specs only) |

`team-a/dumps/` exists for Team A's own audit trail — it is **not** part of the interface. Team B never reads it.

`team-a/notes/` is Team A's scratch, also not part of the interface, and gitignored from PR review attention.

## Sign-off workflow

Each spec file (`team-a/specs/NN_subsystem.md`) carries a footer:

```markdown
## Sign-off

- [ ] Team A: this spec accurately describes the behavior observed in CIV.EXE for the named subsystem.
- [ ] Team B: this spec is implementable without further disassembly access; ambiguities have been escalated to Team A.
```

Both boxes must be ticked (in commits, by the human / agent acting in each role) before Team B begins implementing that subsystem. Ambiguities discovered mid-implementation come back as questions in the spec PR — Team A revises the spec; Team B does not return to the disassembly to "look it up".

## Agent / human role discipline

When the same operator (human or AI agent) serves both roles across different sessions:

1. **Never both roles in the same session.** A session is bound to one team. Switching teams requires ending the current session.
2. **Knowledge from a Team A session does not silently leak into a Team B session.** The only Team-A → Team-B channel is committed specs.
3. **Conversation memory is treated the same way.** If a memory note was written during a Team A session, a subsequent Team B session reads only the spec, not the memory note. (In practice this means Team A memories should describe *what was specified*, not *what the disassembly looked like*.)

## What can be reused from Track A (the prior project)

The parent project `civ1_cht` did extensive Track A work (Win16 NE inline patching). The following Track A outputs are **data**, not implementation, and can be cleanly pulled into this project:

- EDILZSS2 decompression *format specification* (a description of the byte layout, not the decoder implementation)
- The list of `RT_DIALOG` resource IDs and their structural fields
- The translated Chinese strings (`inline_translations.json`, `dialog_translations.json`) as a translation catalog, under CC BY-SA 4.0

The following Track A outputs are **implementation** and must not be pulled in:

- The Python decoder for EDILZSS2 (Team B re-implements from the format spec)
- The NE binary patcher scripts
- The Win16 GDI workarounds

See `REUSE_FROM_TRACK_A.md` for the precise inventory.

## Validation

There is no behavioral oracle (no DOSBox, no `wine` running the original binary, no reference port). Validation comes from:

1. **Spec-derived unit tests.** Team A's spec includes worked examples (e.g., "given input bytes `0x12 0x34 …`, the EDILZSS2 decoder must produce output bytes `0xAB 0xCD …`"). Team B's tests assert this.
2. **Asset round-trip.** Decoded `.pic` / `.pal` files re-encoded back to bytes must match the originals (this validates the loader/saver pair, not behavior).
3. **Visual layout comparison.** Final UI screenshots may be compared against Track A's frozen Big5-patched binary screenshots for *layout* parity, not for *behavioral* parity.

Anything that cannot be tested by one of these three means is escalated as an open question in the spec, not resolved by running the original game.
