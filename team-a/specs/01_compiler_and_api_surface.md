# Spec 01 — Compiler identification, original source structure, Win16 API surface

> **Team A spec.** Continues from [spec 00](00_ne_structure.md).
> Companion dumps: `team-a/dumps/01a_functions.txt`,
> `01b_imports_used.txt`, `01c_entrypoint_walk.txt`,
> `01d_signature_strings.txt`.

## 1.1  Compiler and runtime

The binary was built with **Borland C++ 3.x** for Windows 3.x. Evidence:

- An embedded copyright string is present in segment `1420`:
  `'Borland C++ - Copyright 1991 Borland Intl.'`
  (dump: `01d_signature_strings.txt`).
- The entry-point sequence at `1000:0000` matches the canonical
  Borland C runtime startup for Windows 3.x: a CALLF to the C
  startup routine, followed by a `MOV [seg], ES` initialization of
  the per-instance data segment, a `STOSB.REP` zeroing of the BSS
  segment between `0x5146`–`0xB4BE`, a DOS environment fetch via
  `INT 21h / AH=30h`, and a real-time-clock read via `INT 1Ah`
  (dump: `01c_entrypoint_walk.txt`).
- The non-resident-name table contains `MULTIPLEDATA` and the
  per-instance autodata layout in the way Borland's `c0w` startup
  expects (per spec 00).

For Team B's compilation, this means nothing — Team B's C99 source
compiles with any modern toolchain. For Team A's reading of the
disassembly, this means:

- Calling convention for non-callback internal functions is **Pascal
  by default** (Borland Win16 puts parameters in left-to-right order
  pushed onto the stack, callee cleans the stack). Functions that
  vary (printf-style varargs) are explicitly `cdecl`.
- Callbacks exported to Win16 (the 11 entries listed in spec 00) are
  `__pascal __far __export` with the standard `HWND, UINT, WPARAM,
  LPARAM` signature for window/dialog procedures, and Win16's
  `EnumXxx` signature for `ENUMFUNC`.
- The Borland C runtime's `assert()` macro embeds the failing
  function's name and source-file name as a string literal at every
  call site. That has handed us the original source-file structure
  for free (see §1.2).

## 1.2  Original source-file structure (from `assert()` macro strings)

The Borland C runtime embeds `"FunctionName()  :  source.c"` strings at
every `assert()` call site. Twelve original source files have been
recovered this way. Team A's working model of the original 1993
codebase organization is:

| Original `.c` file | Visible identifiers | Inferred role |
|---|---|---|
| `dialogs.c`   | `ShowTextByName`, `TextBoxSetup`, `OldTextBoxSetup`, `TextBoxDrawAndClose` | Generic dialog construction and text-box content |
| `godpal.c`    | `InitGodPalette`, `FreeGodPalette`, `ZeroGodPalette`, `PushPalette`, `PopPalette` | The system palette manager — owns the 256-color logical palette |
| `gr.c`        | `CreateDlgTemplate`, `GR_DialogOpenIndirect` | Graphics utility (`GR_*` prefix) — runtime dialog templates from `.pic`/`.txt` data |
| `gr_pic.c`    | `GR_PicRead`, `GR_PortDataToBitmap`, `LoadGifPicture`, `InvertBitmap`, **`PicDecompress`** | The `.pic` file loader. `PicDecompress` is the EDILZSS2 decompressor (matches Track A's reverse-engineered format). Also supports a GIF code path. |
| `gr_port.c`   | `GR_PortOpen`, `GR_PortClose`, `GR_PortChangeColor` | A "graphics port" abstraction over GDI device contexts — every drawing operation goes through this. |
| `init.c`      | `LoadAdvisors` | Application init; loads the advisor entity table |
| `load.c`      | `CivLoadGame`, `RLLDecode`, `RLLEncode` | Save/load. The save format is RLL-encoded (run-length-with-literal); the encoder/decoder pair is in this module. |
| **`mac.c`**   | `HandToHand`, `NewPtr`, `DisposPtr`, `DisposeHandle` | Mac Memory Manager emulation. `NewPtr`/`DisposPtr` are Apple's Handle/Pointer API. Their presence here means the Windows port retained the Mac memory model and emulates it on top of `GlobalAlloc`/`GlobalLock`/`GlobalUnlock`. |
| **`resmgr.c`**| `OpenResFile`, `CDisposeResourceMgr`, `RestoMem`, `ZapRes`, `NewResource`, `AddType`, `AddRFile` | Mac Resource Manager emulation. The Windows port loads `.pic`/`.txt`/`.fnt` files through a Mac-style `OpenResFile` → `RestoMem` → `NewResource` flow rather than through Win16's `FindResource`/`LoadResource`. |
| `shape.c`     | `TileNew`, `TileDispose`, `PortTileBlt` | Tile primitives — terrain tile creation/disposal and tile-blit onto a graphics port. |
| `wdwmap.c`    | `WdwMapOpen`, `CheckMapGrowLimit` | The main map window's open / event / paint code. Owns the `WDWMAPPROC` window procedure. |
| `wdwsmmap.c`  | `WdwSmMapOpen`, `DrawSmTiles` | The minimap window. Owns `WDWSMMAPPROC`. |
| `wdwstat.c`   | `WdwStatusOpen` | The status/sidebar window. Owns `WDWSTATUSPROC`. |
| `windows.c`   | `MenuZ` | Top-level window/menu setup. Likely owns `WinMain` and `RegisterClass`. |

**The architecture is "Mac game ported to Win16".** The presence of
`mac.c` + `resmgr.c` is the smoking gun: the original 1991 *Sid Meier's
Civilization* DOS version was followed by a Mac port; the 1993 Windows
version is structurally a Mac port adapted to Windows by emulating the
Mac Memory and Resource Managers on top of Win16 primitives. The
disassembly will show Mac-style API names at high level (`NewPtr`,
`OpenResFile`, `RestoMem`) which then bottom out in Win16 calls
(`GlobalAlloc`, `FindResource`, `LoadResource`).

**Implication for Team B:** the C99 reimplementation does *not*
preserve this two-layer Mac-on-Win shim. Team B writes a single C99
layer that does what the Mac shim + Win16 substrate together did,
expressed directly as C99 + SDL2. Specifically:

- `NewPtr` / `DisposPtr` → `malloc` / `free`.
- `Handle` (handle-to-handle indirection used to support compaction
  on memory-constrained systems) → plain pointer. No compaction in
  the modern reimplementation.
- `OpenResFile` / `NewResource` → a tiny resource-file reader that
  understands the on-disk `.pic` / `.pal` / `.txt` / `.fnt` formats
  directly. Spec 03 will describe this.

## 1.3  Function inventory

Ghidra discovered **1,142 functions** in `CIV.EXE` (dump
`01a_functions.txt`). Of these, the structurally interesting ones
recovered so far are listed below.

### 1.3.1 Eleven exported callbacks (from spec 0.5)

| Address | Symbol | Body | Role |
|---|---|---|---|
| `1000:0000` | `entry`           | 178 | C runtime startup stub (Borland `c0w`). |
| `1008:0d68` | `TIMERPROC`       |  43 | `SetTimer` callback. Short body — likely posts `WM_USER` and returns. |
| `1008:0e12` | `ENUMFUNC`        | 135 | `EnumXxx` callback (2-param). Used by font / object enumeration. |
| `1058:06ab` | `RANDOMRADIOPROC` |  79 | Subclass for radio-button group in a "Random …" dialog. |
| `1058:081a` | `RANDOMSTATICPROC`| 263 | Subclass for the static portion of the same dialog. |
| `1058:0921` | `RANDOMUSERPROC`  | 213 | Subclass for the user-input portion of the same dialog. |
| `1058:09f6` | `REGIONPROC`      |  79 | Subclass for a region-shaped custom widget. |
| `1098:1838` | `CIVDIALOGPROC`   | 191 | Application-wide dialog procedure (`CivDialog`). |
| `1208:0054` | `WDWMAPPROC`      | 203 | Main map window procedure (`wdwmap.c`). |
| `1210:0000` | `WDWSMMAPPROC`    |  77 | Minimap window procedure (`wdwsmmap.c`). |
| `1218:0000` | `WDWSTATUSPROC`   |  77 | Status window procedure (`wdwstat.c`). |

`WDWSMMAPPROC` and `WDWSTATUSPROC` are exactly the same size (77 bytes)
and both live at the start of their respective code segments. The most
likely shape for both is a thin dispatcher of the form
`switch(msg) { case WM_PAINT: jump to internal_painter; … default: return DefWindowProc; }`
where the internal painter is a separate function in the same segment.
Spec 02 will look at both and confirm or refute.

### 1.3.2 Imported API thunks (segment `1608`)

Segment `1608` is the thunk segment. Every entry there is a 4-byte
`JMPF` thunk to one external Win16 import. There are 157 such thunks,
each a function in Ghidra's discovery (the names are visible at the
end of `01a_functions.txt`). This is the "API surface" of `CIV.EXE`,
and exactly matches the six imported modules from spec 0.4.

## 1.4  Win16 API surface — the SDL2 substitution boundary

For each of the six imported modules, the list below shows the most
heavily-called APIs, sorted by call-site count. Source:
`01b_imports_used.txt`.

### 1.4.1 KERNEL — 667 call sites

| Calls | API | What it does | C99 replacement |
|---|---|---|---|
| 230 | `GLOBALUNLOCK`   | Release Mac-style movable-memory lock | (no-op — Team B uses plain pointers) |
| 206 | `GLOBALLOCK`     | Acquire Mac-style movable-memory lock | (no-op) |
|  63 | `GLOBALALLOC`    | Allocate a movable heap block | `malloc` |
|  61 | `GLOBALFREE`     | Free a heap block | `free` |
|  16 | `FREEPROCINSTANCE` | Release a thunk allocated for a far callback | (no-op — pointers don't need thunks) |
|  15 | `MAKEPROCINSTANCE` | Allocate a thunk for a far callback | (no-op) |
|  15 | `HMEMCPY`        | Huge-pointer memcpy | `memcpy` |
|  10 | `LSTRLEN`        | far-pointer `strlen` | `strlen` |
|   8 | `_LCLOSE`        | Win16 file close | `fclose` |
|   5 | `_LLSEEK`        | Win16 file seek | `fseek` |
|   5 | `_LOPEN`         | Win16 file open | `fopen` |
|   5 | `GETFREESPACE`   | Reports free system memory | (no-op or fixed value) |
|   5 | `GLOBALSIZE`     | Block size | tracked in `malloc` wrapper or removed |
|   4 | `_HREAD`         | Win16 huge read | `fread` |
|   2 | `_HWRITE`        | Win16 huge write | `fwrite` |
|   2 | `WAITEVENT`      | Yield to other tasks (cooperative scheduler) | (no-op) |
|   1 | `INITTASK`       | Win16 task init (called from `c0w` startup) | C runtime entry replaces this |
|   1 | `LOADRESOURCE`   | Load an `RT_*` resource into memory | Direct file read for the seven `.pic`/`.pal`/`.txt`/`.fnt` formats |
|   1 | `FINDRESOURCE`   | Locate an `RT_*` by ID | Direct file read |
|   1 | `FREERESOURCE`   | Release a loaded resource | `free` |
|   1 | `LSTRCPY`        | far-pointer `strcpy` | `strcpy` |
|   1 | `GETMODULEFILENAME` | Path of the loaded EXE | `argv[0]` / `GetModuleFileNameA` (host-dependent) |

Most KERNEL traffic is the `GLOBALLOCK`/`GLOBALUNLOCK` pair (436
combined call sites) — the Mac Memory Manager emulator from
`mac.c`/`resmgr.c` constantly locks and unlocks handle blocks. In
the C99 replacement, handles become plain pointers; the entire
locking dance disappears.

### 1.4.2 USER — 970 call sites

| Calls | API | C99 + SDL2 replacement |
|---|---|---|
| 238 | `SETRECT`         | `SDL_Rect` literal assignment (or `civ_rect_set`) |
| 111 | `OFFSETRECT`      | Helper |
|  63 | `FILLRECT`        | `SDL_FillRect` on the palette framebuffer |
|  35 | `SENDMESSAGE`     | Direct function call in Team B's flat-call architecture (messages don't exist; the case bodies of each `WndProc` become directly callable C functions) |
|  35 | `GETCLIENTRECT`   | Track widget rect in a struct, no system call |
|  33 | `SHOWWINDOW`      | Visibility flag on the widget struct; SDL draws or skips accordingly |
|  28 | `GETWINDOWRECT`   | Widget struct |
|  25 | `ENABLEMENUITEM`  | Menu-state struct + redraw |
|  24 | `FRAMERECT`       | SDL line draw |
|  23 | `PEEKMESSAGE`     | `SDL_PollEvent` in the main loop |
|  23 | `INFLATERECT`     | Helper |
|  22 | `SETWINDOWLONG`   | Set a field on the widget struct |
|  18 | `SETWINDOWWORD`   | Set a field on the widget struct |
|  17 | `MOVEWINDOW`      | Set `x`/`y` on the widget struct |
|  15 | `MESSAGEBEEP`     | SDL audio beep |
|  14 | `DISPATCHMESSAGE` | Main loop calls the appropriate widget handler |
|  13 | `TRANSLATEMESSAGE`| (no-op for our event model) |
|  12 | `GETDLGITEM`      | Look up control in a widget struct |
|  12 | `MODIFYMENU`      | Mutate menu struct |
|  11 | `GETDC`           | (no-op — we draw directly to the palette framebuffer) |
|  10 | `CREATEWINDOW`    | Construct a widget struct |
|  10 | `DEFDLGPROC`      | Default control handler (a small helper) |
|   9 | `MESSAGEBOX`      | Custom SDL widget |
|   8 | `BEGINPAINT` / `ENDPAINT` | (no-op — palette framebuffer is always drawable) |
|   8 | `GETFREESYSTEMRESOURCES` | Return a fixed safe value |
|   8 | `ISDIALOGMESSAGE` | (no-op) |
|   8 | `REGISTERCLASS`   | Register a widget type in our widget registry |
|   8 | `GETSYSTEMMETRICS`| Constants for screen size etc. |
|   7 | `DEFWINDOWPROC`   | Default widget handler |
|   7 | `GETMENU`         | Return the menu struct attached to a window |
|   5 | `DESTROYWINDOW` / `INVALIDATERECT` / `ENABLEWINDOW` / `SETFOCUS` / `GETWINDOWWORD` | Widget struct manipulation |
|   4 | `SETTIMER`        | SDL_TimerCallback or main-loop tick counter |
|   3 | `KILLTIMER` / `UPDATEWINDOW` / `VALIDATERECT` / `SETSCROLLPOS` / `SETSCROLLRANGE` / `SETWINDOWTEXT` / `DRAWMENUBAR` / `DRAWTEXT` / `REALIZEPALETTE` | Various |
|   ≤2 | (52 other APIs) | … |

The dominant pattern is "rectangle arithmetic in screen space": 238
`SETRECT`, 111 `OFFSETRECT`, 63 `FILLRECT`, 35 `GETCLIENTRECT`, 28
`GETWINDOWRECT`, 23 `INFLATERECT`. The original code does a lot of
manual layout against pixel coordinates. Team B's replacement keeps
the same approach (an `SDL_Rect`-based widget layout) rather than
introducing a higher-level layout system.

### 1.4.3 GDI — 552 call sites

| Calls | API | C99 + SDL2 replacement |
|---|---|---|
| 116 | `SELECTOBJECT`     | Set the "current" pen / brush / font / palette in a small drawing-state struct |
|  72 | `DELETEOBJECT`     | Free the object struct |
|  45 | `GETSTOCKOBJECT`   | Return one of the eight stock objects (BLACK_PEN, WHITE_BRUSH, SYSTEM_FONT, …) |
|  39 | `GETDEVICECAPS`    | Return constants for screen dimensions / depth |
|  31 | `LINETO`           | `SDL_RenderDrawLine` on the framebuffer |
|  29 | `SETBKMODE`        | Drawing-state struct field |
|  26 | `BITBLT`           | `SDL_BlitSurface` (palette framebuffer onto framebuffer) |
|  24 | `SETTEXTCOLOR`     | Drawing-state struct field |
|  20 | `SETBKCOLOR`       | Drawing-state struct field |
|  17 | `CREATEPEN`        | Construct a pen struct |
|  17 | `CREATESOLIDBRUSH` | Construct a brush struct |
|  17 | `MOVETO`           | Drawing-state cursor position |
|  15 | `TEXTOUT`          | Custom text renderer (palette-layer glyph blit, CJK-aware) |
|  13 | `CREATECOMPATIBLEDC` | (no-op — we have one persistent palette framebuffer) |
|  13 | `DELETEDC`         | (no-op) |
|   9 | `GETTEXTMETRICS`   | Return font metrics from the loaded font (CIVFONTS or substituted CJK font) |
|   6 | `GETTEXTEXTENT`    | Measure text via the loaded font |
|   6 | `SETTEXTALIGN`     | Drawing-state struct field |
|   4 | `ANIMATEPALETTE`   | Update palette framebuffer's palette entries |
|   3 | `GETBITMAPBITS` / `SETBITMAPBITS` / `GETBKCOLOR` / `GETBKMODE` / `GETTEXTCOLOR` | Drawing-state access |
|   2 | `CREATEBITMAP` / `GETPIXEL` / `SETPIXEL` | Direct framebuffer access |
|   1 | `CREATEPALETTE` / `CREATEDIBITMAP` / `CREATEFONT` / `CREATEFONTINDIRECT` / `CREATEPATTERNBRUSH` / `ADDFONTRESOURCE` / `REMOVEFONTRESOURCE` / `ENUMFONTFAMILIES` / `GETBITMAPDIMENSION` / `SETBITMAPDIMENSION` / `SETPALETTEENTRIES` / `UNREALIZEOBJECT` | Various |
|   0 | `POLYGON` / `POLYLINE` / `SETPOLYFILLMODE` / `GETSYSTEMPALETTEENTRIES` | Imported but never called from code — likely pulled in by a static lib for completeness |

GDI is dominated by the GDI-object lifecycle (`SELECT` / `DELETE` /
`GETSTOCK`) plus drawing primitives (`LINETO`, `BITBLT`, `TEXTOUT`).
The unimplemented APIs (POLYGON / POLYLINE / SETPOLYFILLMODE /
GETSYSTEMPALETTEENTRIES) confirm that the game does not use complex
filled polygons — straight-line rendering only.

### 1.4.4 COMMDLG — 4 call sites

| Calls | API | Use |
|---|---|---|
| 2 | `COMMDLGEXTENDEDERROR` | Error reporting after a file-dialog call |
| 1 | `GETOPENFILENAME` | Open-save dialog (probably `Load Game`) |
| 1 | `GETSAVEFILENAME` | Save-file dialog (probably `Save Game`) |

Team B replaces with a custom save/load dialog in SDL2 (already
required because the rest of the UI is custom).

### 1.4.5 MMSYSTEM — 4 call sites

| Calls | API | Use |
|---|---|---|
| 2 | `SNDPLAYSOUND`   | One-shot WAV playback |
| 2 | `MCISENDCOMMAND` | MIDI / CD-Audio command interface |

`SDL2_mixer` covers both: `Mix_PlayChannel` for `SNDPLAYSOUND`,
`Mix_PlayMusic` (or a small MIDI player) for `MCISENDCOMMAND`. Two
call sites each suggests the audio integration is minimal — likely
the title music and a small set of UI / event sounds.

### 1.4.6 WIN87EM — 0 call sites

`WIN87EM` is listed in the NE imports but no thunk in segment `1608`
is referenced from code. The library was linked for `c0w`'s
runtime-presence-check only; the program uses the FPU directly. **No
C99 replacement is needed.**

## 1.5  Entry-point disassembly walk

The disassembly walk in `01c_entrypoint_walk.txt` starts at
`1000:0000` and follows the Borland C runtime startup. The relevant
landmarks are:

```
1000:0000  CALLF  1608:005C        ; KERNEL.INITTASK
1000:0005  OR     AX,AX            ; check task handle
1000:0007  JNZ    1000:000C        ; nonzero → continue
1000:0009  JMP    1000:00CF        ; zero → bail
1000:0024  MOV    DI,0x5146        ; BSS start
1000:0027  MOV    CX,0xB4BE        ; BSS end
1000:002A  SUB    CX,DI
1000:002C  CLD
1000:002D  STOSB.REP ES:DI         ; zero BSS region [0x5146, 0xB4BE)
1000:0032  CALLF  1608:0020        ; KERNEL.WAITEVENT (cooperative)
1000:003B  CALLF  1608:0084        ; USER.INITAPP
1000:0049  INT    1A               ; BIOS time of day → DX:CX
```

The walk truncates before reaching `WinMain` because Ghidra's
auto-analysis did not propagate a `WinMain` symbol. The startup
stub eventually calls into the user's `WinMain`, which lives in
`windows.c` (per the source-file inventory in §1.2) and is the
"`MenuZ`" comment in the assertion strings. Spec 02 walks the rest
of the startup chain and identifies `WinMain` by address.

**Implication for Team B:** the entire C runtime startup + `WinMain`
is replaced by a standard C99 `main(int argc, char *argv[])` that
calls `SDL_Init`, allocates the palette framebuffer, registers
widgets, opens the save-game window, and enters the event loop. The
order of operations in §1.5's walk does not need to be preserved.

## 1.6  Open questions for spec 02+

1. Identify `WinMain` precisely. (Walk from the entry stub past the
   Borland C runtime to the first user-code function.)
2. Walk `WinMain`'s body: `RegisterClass` for each of the three
   top-level windows, `CreateWindow` for each, `LoadAccelerators`,
   `LoadMenu`, `SetTimer`, then `GetMessage` / `TranslateMessage` /
   `DispatchMessage` loop.
3. Identify the message-pump structure: cooperative-multitasking
   wait points, idle-time computation slots, frame timing.
4. For each of the three exported window procedures (`WDWMAPPROC`,
   `WDWSMMAPPROC`, `WDWSTATUSPROC`), list the messages it handles
   and the case bodies.
5. For `CIVDIALOGPROC`, list the messages it handles and how it
   dispatches to the 24 `RT_DIALOG` resources.
6. Locate `PicDecompress` (EDILZSS2 decoder) in the disassembly and
   produce spec 03 (asset file format).
7. Locate `RLLDecode` / `RLLEncode` and produce a save-file format
   spec.
8. Decide on Team B's drawing-state struct (current pen / brush /
   font / palette / text color / bg color / bg mode / text align /
   move-to cursor) — this captures the dozen GDI APIs that read or
   write drawing state in one place.

## Sign-off

- [ ] Team A: this spec accurately summarizes the compiler, source
  organization, and Win16 API surface observed in `team-a/dumps/01*`.
- [ ] Team B: this spec is implementable as a build-system /
  external-dependency plan and as an API-substitution boundary
  without further disassembly access.
