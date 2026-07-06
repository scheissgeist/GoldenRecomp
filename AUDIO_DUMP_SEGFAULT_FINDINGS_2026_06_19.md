# Audio-Dump Segfault — Findings (Opus, 2026-06-19)

> 2026-07-06 Codex note: this is a historical findings note. The "uncommitted"
> working-tree audio dump code referenced below was preserved on the
> `goldensouls-audio-dump` branch in commits `9880697` / `f36cd0b` / `91c72af`
> and nested runtime commits `7f18af1` / `0796548`.

Investigating why `GE_AUDIO_DUMP` extraction (extract_music_tracks.py, used to make
GoldenSouls/assets/music/*.wav) currently SEGFAULTS for every track and produces noise.

## TL;DR
The crash has TWO independent causes. One is fixed (in the working tree, not committed);
the dominant one is a recomp-synth bug in the **uncommitted** audio-dump validation work
and needs its author ("theboy") to finish.

## Cause 1 — teardown race (FIXED in working tree, src/main/main.cpp queue_samples)
On hitting the sample budget, `queue_samples` (the AUDIO CALLBACK THREAD) called
`ultramodern::quit(); std::exit(0);`. `std::exit` runs atexit + global dtors while the
game/RSP/video threads are still live on the same process -> use-after-free teardown ->
segfault. The WAV was already finalized before this, so it corrupted no data but killed
the process noisily.
**Fix applied:** replaced `std::exit(0)` with `return;` after `ultramodern::quit()`.
quit() unblocks `recomp::start()` on the MAIN thread for a clean teardown — the same path
`ge_audio_dump_quit_from_game()` (main.cpp:308) already uses. Normal play verified intact.
This is correct and isolated; adopt it into the audio-dump branch.

## Cause 2 — alAdpcmPull invalid-wavetable OOB (DOMINANT, NOT fixed — your lane)
After Cause 1 is fixed, most tracks STILL segfault, now inside the ADPCM voice synth:
```
GE_AUDIO_DUMP: invalid wavetable alAdpcmPull type ptr=0x000000F7 owner=0x802B763C wavetable=0x802ADBA0
GE_AUDIO_DUMP: invalid alAdpcmPull wavetable ... outp=0x803B4596 outCount=4284481536 cmd=0x802CAF30
```
- The wavetable's `type` field is 0xF7 (valid is 0/1) and `outCount` is 0xFF6A0000 — the
  voice's wavetable pointer is reading corrupt RDRAM.
- `ge_audio_dump_validate_adpcm_pull_native` (pi.cpp:1132) CORRECTLY detects this
  (validate_wavetable, pi.cpp:521 `if (type > 1) invalid`) and returns 1 with
  `ctx->r2 = cmd_ptr` to skip the command — but the crash still happens and the "invalid"
  line loops, so **the recompiled alAdpcmPull is not honoring the validator's skip at the
  right point** (hook placement / it reads OOB before or regardless of the skip).
- This validator layer is itself UNCOMMITTED work in lib/N64ModernRuntime/librecomp/src/
  pi.cpp — i.e. an in-progress attempt to survive this exact corruption. It detects but
  doesn't yet prevent the OOB.

## Key proof the synth is NOT inherently broken
id=44 (M_INTROSWOOSH) ran far enough to finalize a WAV of REAL MUSIC (zerocross 0.143,
not noise) before crashing. So the synth CAN produce clean intro music — the wavetable
goes corrupt mid-render and the validator's skip doesn't stop the OOB. The 41 already-
shipped clean WAVs in GoldenSouls/assets/music/ corroborate: this path worked before the
current uncommitted changes.

## What's needed (recomp/audio lane)
Make the alAdpcmPull skip actually prevent the OOB read when validate_adpcm_pull returns
invalid — fix the hook placement in the recompiled alAdpcmPull so the corrupt wavetable
voice is dropped BEFORE the pull reads outCount/samples, not after it logs. OR find why
the voice's wavetable pointer corrupts for these tracks during a headless dump (no real
game state). Once that lands, intro/introswoosh extract clean and GoldenSouls' triggers
(commit 7bc1a42, already wired) play them.

## Status
- Cause 1: fixed in working tree (uncommitted, sits within theboy's larger uncommitted
  main.cpp audio-dump block — not separately committable without taking that WIP).
- Cause 2: open, recomp-synth lane.

## UPDATE (Opus, 2026-06-19 ~17:35) — Cause 1 FULLY FIXED; Cause 2 pinned to RSP microcode

### Cause 1 fix REFINED + VERIFIED
The `std::exit(0) -> return` swap was not enough — the clean `ultramodern::quit()`
teardown STILL races the **RT64 "Present" render thread** (gdb: SIGSEGV in thread
"RT64 Present" during teardown; graphics are half-disabled in audio_dump_mode so the
present thread derefs null on shutdown). The WAV is already finalized + fclose'd before
this, so it's a pure teardown race.
**Final fix (main.cpp queue_samples, sample-budget branch):** `audio_dump_finalize();
fflush(stderr); std::_Exit(0);` — hard-exit with NO atexit/dtors/RT64 shutdown. The only
artifact (the WAV) is already closed. VERIFIED: every run that reaches the sample budget
now exits 0 (was 139) with a valid WAV, 3/3 + 4/4 across tracks. Normal play intact.

### Cause 2 is the RSP audio microcode (aspMain), NOT alAdpcmPull
With Cause 1 fixed, the residual failures are: (a) intermittent NOISE seconds in the
output, and (b) a mid-synthesis crash `Unhandled jump target 0xXXXX in microcode aspMain
(rsp/aspMain.cpp:151)`. Both are the SAME bug: the audio-command dispatch loop
(aspMain.cpp:124-154) reads a command word from RDRAM at r28 (the ABI cmd-list ptr),
extracts opcode `r1=(r26>>23)&0xFE`, indexes the DMEM dispatch table at 0x10, and
`jr $2`. When r28 walks past valid command data (or the DMEM dispatch table at 0x10 is
corrupt — note the in-progress `ge_asp_restore_dispatch_table()` scaffolding at :147),
the looked-up target is garbage (0x0000/0x7FFF) -> unhandled jump. Garbage opcodes also
emit the noise. The earlier "invalid alAdpcmPull wavetable type=0xF7" logs are a
DOWNSTREAM symptom of the same corrupt voice state, not the cause — the alAdpcmPull
validator correctly detects+skips (funcs_6.c:13049 `goto L_7001A5CC`), it's not where the
crash is.

**Proof the synth fundamentally works:** id=44 (M_INTROSWOOSH) produced multiple clean
seconds (zerocross 0.02-0.03) and finalized; the noise is intermittent microcode
derailment, not a dead synth. M_DAM (id 9) derails earliest (worst hit); M_INTRO (id 2)
is partly clean, partly noise.

**What's needed (recomp/RSP lane):** fix the aspMain audio-command dispatch so r28 / the
0x10 dispatch table stay valid in headless dump mode (the `ge_asp_restore_dispatch_table`
+ `debug_command_index` scaffolding is the in-progress attempt). Once aspMain stops
derailing, intro/dam/all tracks dump clean and the GoldenSouls triggers (commit 7bc1a42)
play them. This is RSP-microcode-accuracy work, mid-investigation by the author of the
uncommitted ge_asp_* helpers.
