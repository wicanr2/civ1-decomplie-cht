# Spec 00 — Binary identity, NE container, external dependencies

> **Team A spec.** This document is the only thing Team B reads about
> what the original `CIV.EXE` *is*. Team B does not look at the binary,
> the dump (`team-a/dumps/00_ne_structure.txt`), or any disassembler.

## 0.1  Binary identity

The work-of-record is the pristine 1993 *Civilization for Windows*
executable as shipped by MicroProse:

| Property | Value |
|---|---|
| File name | `CIV.EXE` |
| Size | 833,024 bytes |
| MD5 | `DCC4399E57AEFE490914AC2F5A95CD92` |
| SHA-256 | `F606568096376F8ED8989D74D17F36AB3B18ADFA7A2D56D89A4F29BEE5D94672` |
| Container | Win16 New Executable (NE), targeting Windows 3.0A or later |
| Module name | `WINCIV` |
| Module description | `CIVILIZATION for Windows` |

A separate copy of this file that has been in-place modified for Big5
display (file size 832,512, MD5 `336FF646…`) exists in the parent
project's Track A workspace. **That patched binary is not the
work-of-record.** Team A always disassembles the pristine 833,024-byte
file.

## 0.2  Memory model and target

The NE header reports:

- **Target OS:** Windows (NE exe-type byte = `0x02`).
- **Required Windows version:** 3.0A or later (field `0x030A`).
- **App type:** Windows Protected-Mode (WIN_PM), `MULTIPLEDATA`,
  `PROTECTED_MODE_ONLY`. So this is a 286-protected-mode, segmented
  16-bit application; the per-instance data segment is the
  multi-instance `DGROUP` autodata segment.
- **Autodata segment index:** 133 (the last segment).
- **Initial heap reservation:** 3,072 bytes.
- **Initial stack reservation:** 9,216 bytes.
- **Entry point:** `CS:IP = segment 1 : 0x0000` (i.e. the first byte of
  the first code segment).
- **Initial stack pointer:** `SS:SP = segment 133 : 0x0000` (the SP wraps
  to the end of the autodata segment on entry, which is the standard
  Win16 convention).

**Implication for Team B:** the program assumes 16-bit segmented memory.
The replacement is a single 32/64-bit flat address space. Anywhere the
original code uses a far pointer (segment + offset) to address game
data, the corresponding C99 representation is just a plain pointer or
an index into a global table. Anywhere the original uses 16-bit `int`
arithmetic with documented wrap-around semantics, the C99 representation
must explicitly use `int16_t`/`uint16_t` to preserve overflow behavior;
otherwise plain `int` is fine.

## 0.3  Segmentation summary

There are **133 segments** total. The pattern, read off the segment
table:

- **Segments 1 through 69:** code. Each one is `MOVABLE | PRELOAD |
  HAS_RELOC | DISCARD` with `flags = 0x1D50` and no `DATA` bit. Sizes
  range from a few hundred bytes to ~40 KB; the largest single code
  segment is **segment 7 at 40,351 bytes**.
- **Segments 70 through 132:** small data segments, all
  `DATA | MOVABLE | PRELOAD` with `flags = 0x0C51`. Most are very small
  (a few bytes to a few hundred bytes), which is the signature of
  per-translation-unit `_DATA` segments from a 16-bit C compiler
  emitting one segment per source file's static data.
- **Segment 133 (autodata):** the application's combined `DGROUP`
  default data segment, 46,278 bytes, `DATA | MOVABLE | PRELOAD |
  HAS_RELOC`. This holds the program's writable globals.

**Implication for Team B:** the segmentation is an artifact of the
toolchain, not a design statement. Team B does not preserve segment
boundaries. All globals collapse to a single flat data area; all code
collapses to whatever module structure the C99 implementation chooses.

A per-segment relocation-record count is captured in the dump as a
proxy for "API-call density per code segment". The code segments with
the largest reloc counts (and therefore the most external calls) are:

| Segment | Reloc records | Plausible role |
|---|---|---|
| 7  | 2,297 | Largest single code segment; very high API call density. Likely the main window procedure / drawing dispatcher. |
| 63 | 1,010 | Secondary high-traffic code segment. |
| 16 | 954 | Another heavy segment. |
| 45 | 868 | Another heavy segment. |
| 30 | 775 | Another heavy segment. |
| 57 | 762 | Another heavy segment. |

Team A will name these segments in subsequent specs as their function
becomes clear; for now they are just `seg_7` etc.

## 0.4  External Win16 dependencies (the SDL2 replacement boundary)

The NE header lists exactly **six** imported modules:

| Imported module | What it provides | C99 + SDL2 replacement |
|---|---|---|
| `KERNEL` | Memory allocation, file I/O, system metrics, modal loops | C standard library + a small platform shim |
| `USER` | Windows / dialogs / menus / message queue / cursors / clipboard | SDL2 event loop + custom widget/dialog system |
| `GDI` | Device contexts, bitmaps, palettes, blits, text, regions, brushes, pens | SDL2 software rendering to an indexed palette framebuffer |
| `WIN87EM` | 8087 floating-point emulation library | Native FPU (no replacement needed); see §0.6 |
| `MMSYSTEM` | `sndPlaySound`, MIDI, multimedia timer | SDL2_mixer (or SDL2 audio + a small MIDI player) |
| `COMMDLG` | Standard File-Open / File-Save dialogs | Custom file picker built on the SDL2 widget system |

**There is no DDEML, no SHELL, no DRIVER, no embedded VBX, no winsock,
no other module.** This is the complete external surface. Team B's
implementation only has to substitute these six.

Per-call API inventory (which specific entries are imported from each
of these six modules, and at which call sites) is deferred to spec
01 — it requires the disassembly pass.

## 0.5  Exported callbacks (the only ordinal-exposed entry points)

The resident-name table exports the module name and **eleven** named
ordinals. All eleven are Windows callback procedures; none are
public-API entry points for the application. Their slot names and
positional ordinals are:

| Ordinal | Exported name | Role inferred from name |
|---|---|---|
| 1  | `ENUMFUNC`         | Generic `EnumXxx` callback (window/font/object enumeration) |
| 2  | `WDWMAPPROC`       | `WndProc` for the main map window |
| 3  | `WDWSMMAPPROC`     | `WndProc` for the small / minimap window |
| 4  | `WDWSTATUSPROC`    | `WndProc` for the status / city-info window |
| 5  | `TIMERPROC`        | `SetTimer` callback (the per-tick game heartbeat) |
| 6  | `DLGPROC`          | Generic dialog procedure |
| 7  | `RANDOMRADIOPROC`  | Subclass / dialog proc for radio-button group in a "Random …" dialog |
| 8  | `RANDOMSTATICPROC` | Subclass for the static portion of the same dialog |
| 9  | `RANDOMUSERPROC`   | Subclass for the user-input portion of the same dialog |
| 10 | `REGIONPROC`       | Subclass / paint proc for a region-shaped widget |
| 11 | `CIVDIALOGPROC`    | Application-wide dialog proc (`CivDialog`) |

**Implication for Team B:** the original architecture is a *multi-window*
Win16 application — at least three top-level child windows (main map,
mini-map, status panel), each with its own `WndProc`, plus a custom
"Random …" modeless dialog with three subclassed regions, plus a
custom-painted region widget, plus an application-wide dialog proc.

In the SDL2 replacement, "main map / minimap / status panel" are
three separate widget areas of one SDL2 window; each maps to a
self-contained C module with an event-handling function that takes
the same role as the original `WndProc`. The single SDL2 event loop
dispatches input to whichever widget the cursor / focus is over.

## 0.6  Floating-point convention (WIN87EM)

The presence of `WIN87EM` in the import list means the binary was
linked against Microsoft's 8087-emulation library. On a 1990s machine
without an FPU (e.g. a 386SX), `WIN87EM.DLL` traps the x87 instructions
and emulates them. On a 386DX or later with an FPU present, the same
instructions execute natively.

**Implication for Team B:** Team B's C99 code uses native `float` and
`double` (and these are IEEE 754 single/double precision on every host
platform we target). No emulation is needed. However, all
floating-point arithmetic that affects observable state — combat
random rolls, science accumulation, gold rate, etc. — must produce the
same numeric result as the original. Whether the original uses `float`
(32-bit) or `double` (64-bit), and at what precision intermediate
expressions are evaluated, is **TBD** until Team A finishes the
disassembly pass on the relevant code. Spec 01+ will pin this down.

## 0.7  Resource directory contents

The NE resource table reports the following bindings. These are the
only resources embedded in the EXE.

### Cursors

Sixteen `RT_CURSOR` resources numbered `#1` through `#16`, each 512
bytes on disk, paired with sixteen `RT_GROUP_CURSOR` entries
`#128`–`#143`. Standard Win16 cursor grouping (one group per cursor;
each `RT_GROUP_CURSOR` references one `RT_CURSOR`). Team A will name
these in spec 02 once the dialog-to-cursor wiring is understood.

### Icon

One `RT_ICON` `#1` (2,560 bytes) paired with one `RT_GROUP_ICON`
`#128`. This is the application icon.

### Menu

One `RT_MENU` `#128` (1,536 bytes). This is the application's main menu
bar. Team A will dump its structure in spec 02 as
`team-a/dumps/02_menu_128.txt`, and the corresponding spec entry will
list every menu item with its command ID.

### Dialogs

**Twenty-four** `RT_DIALOG` resources, each on a 512-byte slot. The
IDs are:

```
129, 130, 131, 133, 135, 136, 137, 138, 139, 140, 141,
142, 143, 144, 145, 146, 147, 148, 149, 150, 151,
666, 999, 2000
```

Notable:

- IDs `132` and `134` are *missing* from the sequence between `131` and
  `135` — likely originally allocated and then deleted during
  development.
- IDs `666`, `999`, and `2000` are out-of-band and almost certainly
  serve special roles (about-box, copy-protection prompt, or
  splash/loading dialog). Team A will determine which is which in
  spec 02.

Team A will produce one dialog-by-dialog spec in spec 02, listing
every control (button / static / edit / listbox), its ID, its label,
its tab order, and the dialog procedure that owns it.

### Accelerator table

One `RT_ACCELERATOR` `#128` (512 bytes). The keyboard shortcut table
that pairs with `RT_MENU` `#128`.

### String table

**There is no `RT_STRING` resource.** All user-facing strings are
inline in the code or data segments. This is the typical 1993 idiom.
Team B implements localization by extracting strings into a separate
catalog (one entry per string), not by re-populating an `RT_STRING`.

### Raw data

**There is no `RT_RCDATA` resource.** The game's asset files (the
`.pic`, `.pal`, `.txt`, `.fnt` resources) live on disk alongside
`CIV.EXE`; they are not embedded in the binary. Loading them is
documented in spec 03 (asset loaders, including EDILZSS2).

### Version info

**There is no `RT_VERSION` resource.** No Win16 version-info block is
embedded.

## 0.8  Compiler and runtime — preliminary

The reported NE linker version is **5.10**, which matches multiple
1990s linkers (Microsoft LINK 5.10 and Borland TLINK 5.x are both
candidates). The presence of `WIN87EM` as a separate import (rather
than the in-binary Borland math library) and the use of `MULTIPLEDATA`
+ explicit `DGROUP` autodata is consistent with the Microsoft C 7 /
C/C++ 7.0 toolchain that was the dominant 1992–1993 Win16 vendor for
MicroProse.

**Final compiler identification is TBD pending Ghidra string-table and
prologue-pattern scan.** Spec 01 will pin this down with concrete
evidence (compiler signature strings, function prologue/epilogue
templates, and calling convention). Why it matters: knowing the
calling convention (`__pascal` vs `__cdecl`) determines argument-order
conventions in the disassembly, which directly affects how Team A
describes function signatures in subsequent specs.

## 0.9  Open questions for spec 01+

1. Confirm compiler identity (Microsoft C 7.0 vs Borland C++ 3.1) via
   in-binary signature strings.
2. Determine the dominant calling convention for non-exported
   functions (`__pascal` for callbacks; for internal helpers either is
   possible).
3. Identify the entry sequence in `CS:IP = 1:0` and walk through to
   `WinMain` (the standard Win16 entry is `LibMain` /
   `_WEP` / `WinMain` after C runtime setup).
4. List every imported procedure from each of the six modules
   (KERNEL, USER, GDI, WIN87EM, MMSYSTEM, COMMDLG), with the count of
   call sites for each. This is the "API surface" Team B's SDL2
   substitution must cover.
5. Map the 11 exported callbacks to actual `RegisterClass`,
   `CreateDialog`, and `SetTimer` calls so Team B knows which window /
   dialog / timer they belong to.
6. Decide naming for code segments 1–69 by inspecting their
   contents (currently named only by index).

## Sign-off

- [ ] Team A: this spec accurately describes the structural facts about
  the work-of-record `CIV.EXE` (MD5 `DCC4399E…`) as observed in
  `team-a/dumps/00_ne_structure.txt`.
- [ ] Team B: this spec is implementable as scaffolding (build setup,
  external dependency plan, callback module boundaries) without
  further disassembly access.
