# Goldenrecomp — Project North Star

**Updated:** 2026-06-22

## What this repo is

Goldenrecomp is a **third-party N64→native static recompilation** of GoldenEye 007
(RecompUI / N64ModernRuntime based). In the context of Sean's work it is **not a
product with its own ship axis** — it is a **tool/dependency** used by the GoldenSouls
port (`E:\Goldeneye\GoldenSouls`). The governing product contract is
`E:\Goldeneye\PROJECT_NORTH_STAR.md` (GoldenSouls). Read that first.

## Ship criterion (for OUR use of this repo)

The ONLY active reason we edit Goldenrecomp: **extract correct, clean audio assets**
(music tracks + any SFX) from the original ROM via the headless audio-dump harness, for
GoldenSouls to ship as `assets/music/*.wav`. "Done" = the dump harness produces audio
that matches the N64 original by ear (no ring/clip/silence corruption), for the mapped
track set, with a clean process exit.

We do NOT develop Goldenrecomp as a playable recomp, add features, fix its UI, or chase
parity beyond what the audio extraction needs.

## Current phase: audio-dump fidelity

- Harness INTEGRITY is done (Codex, 2026-06-22): 43/43 mapped tracks dump as valid
  RIFF WAVs with clean exits (aspMain dispatch guard, alLoadParam/ADPCM wavetable
  guards, watchdog + silence-pad + `_Exit(0)` no-teardown finalize).
- Audio QUALITY is NOT done (Opus took the lane 2026-06-22): a content audit of the 43
  WAVs found 15 with a Nyquist/HF ring (incl. intro/introswoosh/dam/watch), 2 clip-rail
  runaways (silo/deathsolo), 6 near-silent, 8 weak. Current target: crack the ring.

## Forbidden moves

- Don't develop this as a game/recomp. Audio extraction only.
- Don't commit broad recompiler refactors. Keep changes scoped to the audio-dump path
  (`src/main/main.cpp` dump funcs, `rsp/aspMain.cpp` dump-gated guards, the
  N64ModernRuntime `pi.cpp` dump guards) and gate them behind `GE_AUDIO_DUMP` /
  `GE_AUDIO_TRACK` so normal recomp play is unaffected.
- Don't lose Codex's working-tree audio fixes — they are mirrored as patch files under
  `GoldenSouls/tools/goldenrecomp_audio_patch/`. The generated `rsp/aspMain.cpp` is
  gitignored; its fix lives only in the patch bundle + the live generated file.
- Don't claim "audio fixed" from "valid WAV + clean exit" — that's integrity, not
  quality. Quality = by-ear / spectral parity with the N64 original.

## Approved next moves

1. **Crack the Nyquist ring** (Opus, in progress) — localize whether the alternating-
   sample artifact enters in the RSP synth output (`audio_data` into `queue_samples`),
   the resampler, or the dump write path. Fix at the source, re-extract, re-audit.
2. Fix the clip-rail tracks (silo/deathsolo) and the near-silent set, OR confirm they
   are source-faithful (some GE slots are genuinely short/quiet).
3. Re-extract the mapped set into `GoldenSouls/assets/music/` and hand to a human listen.

## Coordination

Two agents (Opus + Codex) touch this tree. As of 2026-06-22 Opus owns the whole audio
lane (Sean's call); Codex stands down on audio. Live channel:
`E:\Goldeneye\GoldenSouls\docs\AI_CHANNEL.md`.
