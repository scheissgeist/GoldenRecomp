# Leak sweep: GoldenRecomp (2026-10-27)

Repo: `scheissgeist/GoldenRecomp` (public; a GitHub **fork** of the upstream GoldenEye recomp project).
Scope: current tree and full git history on every branch, for (1) copyrighted / ROM-derived
material, (2) secrets, (3) personal info.

## Bottom line

- **No ROM, ROM dump, or N64Recomp output (`RecompiledFuncs/`) has ever been committed** on any branch.
- **No secrets** in the tree or in history.
- **Copyrighted material found:**
  - One ROM-derived file in history: `rsp/aspMain.cpp`, RSPRecomp output generated from the ROM's audio microcode.
  - Game and film artwork still in HEAD: the app icons are a capture of the in-game watch menu, and the launcher background is a vector trace of a Bond film scene.
- **Personal info:** only minor items, both on the `goldensouls-audio-dump` branch only. Local `E:\Goldeneye\...` project paths, and Sean's address in commit author metadata.
- The worst items all came from upstream commits. Because this repo is a fork, purging them here would not remove them from GitHub. See "About history purges" below.

## Findings

"In HEAD" means present on the tip of `main` (`f31b5d1`). `goldensouls-audio-dump` (`c238a45`) is noted separately where it differs.

| # | Path / commit | Type | Size | Likely origin | In HEAD | Recommended action |
|---|---|---|---|---|---|---|
| 1 | `rsp/aspMain.cpp`: added `6d8d423` (2025-03-06), deleted `08bf9e0` (2025-04-22) | ROM-derived generated code. RSPRecomp output of the `aspMain` audio microcode, read from ROM offset `0x22DA0` per `aspMain.us.toml`. 2,272 lines. | 73,090 B | Upstream author, committed before `rsp/.gitignore` existed | n | Already ignored (`rsp/.gitignore`) and now also by root `/rsp/*.cpp`. **History purge needed** to fully remove it, but it is also in upstream history; see below. |
| 2 | `icons/{16,32,48,64,128,256,512}.png` + `.ico`: added `9d8020f` (2025-03-12) | Game artwork. Screen capture of GoldenEye's in-game watch menu ("Q WATCH v2.01 BETA", "GoldenEye Key"). | ~1.08 MB total (512.png 237 KB, 512.ico 270 KB, 256.ico 268 KB) | Upstream contributor | **y** | **Remove from tree**: replace with original artwork (Sean's call; `src/main/main.cpp:122` and `icons/app.rc` load these, so swap files rather than delete). History purge optional; also upstream. |
| 3 | `assets/mm-clipped.svg`: blob `603672d` (`1e6f838`), replaced by `ee4c0dc` (`cdd62f9`) | Film/key-art derived image. Vector trace of a Bond scene (Brosnan-likeness figure, jungle cable-car tower). Used as the launcher background (`assets/launcher.rml:27`). | 770,728 B (HEAD); 1,289,115 B (old) | Upstream contributor | **y** | **Remove from tree**: replace with original art. Same purge caveat as #2. |
| 4 | `icons/*.ico` (old set, before `9d8020f`) | Fan-drawn Mario-style "?" block icon, left over from the template this project started from | ~252 KB | Template / upstream | n | No action (Nintendo trade dress, low risk, history only). |
| 5 | `src/game/scene_table.cpp` | Majora's Mask place names (warp table), left over from the Zelda64Recomp template | 24,677 B | Template / upstream | y | Low risk (names only, from an open-source project). Optional tidy-up: remove from tree. |
| 6 | `patches/*.c`, `patches/structs.h`, `patches/externs.h` (all versions) | Decompilation-derived C (GoldenEye decomp structs/functions, `RECOMP_PATCH` overrides). No embedded data tables, textures, display lists or text. Hex values are struct offsets. | up to 95 KB | Upstream / goldeneye_src decomp | y | No action. Hand-written decomp code is standard for recomp projects and is not ROM output. |
| 7 | `patches/syms.ld` | Symbol/address file. Mostly dummy `0x8F00xxxx` recomp hooks; a handful of game RAM addresses (`sShotViewPos`, `D_800C9Bxx`, display-list addresses) | 78 lines | Upstream | y | No action. Addresses, not game content. |
| 8 | `N64Recomp.exe`, `RSPRecomp.exe` (3 versions each) | Tool binaries (the recompilers, not game output). Embedded build paths `V:\N64Recomp\out\build\x64-Debug\*.pdb`, no usernames. Already matched by `*.exe` in `.gitignore` but tracked anyway. | 2.9 MB / 1.5 MB in HEAD | Upstream | y | Not a leak. Optional: build from source instead of tracking binaries. |
| 9 | `goldensouls-audio-dump` branch (`9880697`, `f36cd0b`, `91c72af`, `c238a45`) | Audio-extraction **code** (`GE_AUDIO_DUMP` WAV writer in `src/main/main.cpp`, `audio_cents_patch.cpp`) and two notes. No WAVs, ROM segments or generated funcs committed. `aspMain.us.toml` there points at `../007/bin/aspboot.text.bin`, an extracted ROM segment that lives **outside** the repo. | n/a | Sean | n (branch only) | No purge needed. New root ignores (`*.bin`, `*.wav`, `/rsp/*.cpp`, `_tmp_*`) guard against dump output landing in the repo. See the GoldenSouls note below. |
| 10 | `PROJECT_NORTH_STAR.md` (`fc88107`, dump branch) | Personal info (minor). Local paths `E:\Goldeneye\GoldenSouls`, `E:\Goldeneye\PROJECT_NORTH_STAR.md`, `...\docs\AI_CHANNEL.md`. No username, host or IP. | n/a | Sean | n (branch only) | No action needed. Optionally genericize paths if the branch is ever merged. |
| 11 | Commit author metadata `9880697`, `f36cd0b`, `91c72af`, `c238a45`, `fc88107`, `7469e47` | Personal info. Sean's personal Gmail (`sean…@gmail.com`) as author email. | n/a | Sean's git config | n (all six are on `goldensouls-audio-dump` only) | No purge (rewriting shared history for this isn't worth it). Going forward, set `git config user.email` to the GitHub `…@users.noreply.github.com` address and enable "Block command line pushes that expose my email". |

### Not findings (checked and clean)

- **ROMs** (`.z64/.n64/.v64/.rom/.ndd`, any blob ≥ 4 MB): none. The largest blob ever committed is `assets/ChiaroBold.otf` (6.2 MB font).
- **Generated output** (`RecompiledFuncs/`, `RecompiledPatches/`, `funcs_*.c`, `recomp_overlays.inl`, `patches.bin/.elf`, `dump.toml`): never committed.
- **Extracted assets** (`.wav/.mp3/.ogg/.aif*/.sbk/.ctl/.tbl/.seq/.bin/.raw`, texture dumps): none.
- **Submodule `lib/ge`** → `kholdfuzion/goldeneye_src`: a pointer only; no decomp content is stored in this repo. That repo was not scanned (out of scope).
- **Secrets:** none. No `.env`, key, cert, keystore or credential files have ever been present.
- **Personal info:** no emails in file content (only in commit metadata, #11), no phone numbers, no street addresses, no IPs at all (private or public).

## .gitignore

Already covered before this sweep: `*.z64`, `*.elf`, `*.map`, `RecompiledFuncs/`, `RecompiledPatches/`,
`dump.toml`, `data_dump.toml`, `*.exe`; `rsp/.gitignore` covers `aspMain.cpp`/`njpgdspMain.cpp`;
`patches/.gitignore` covers `*.bin/*.elf`.

Added in this PR (none of these match any tracked file):

```
*.Z64 *.n64 *.N64 *.v64 *.V64 *.rom *.ndd *.bin   # other ROM formats + raw ROM segments
/rsp/*.cpp                                        # all RSPRecomp output, not just the two named files
*.wav *.aiff *.aifc                               # GE_AUDIO_DUMP output
_tmp_*                                            # audio probe debris (mirrors the dump branch)
```

No secrets were found, so no env-var replacements were needed.

## About history purges

This repo is a fork. Items #1–#3 were introduced by upstream commits and still exist in
the upstream repo's history. GitHub also shares objects across a fork network, so old
commits can stay reachable by hash. A `git filter-repo` here alone would therefore **not**
take them off the internet. Options for Sean, in rough order of effort:

1. **Do nothing to history; fix HEAD.** Replace the icons and launcher art (#2, #3).
   Most of the practical exposure is in what the default branch ships.
2. **Detach the fork** (GitHub Support "detach fork" request), then `git filter-repo --path rsp/aspMain.cpp --invert-paths` (plus the image paths if wanted), force-push, and ask GitHub Support to purge cached views and unreachable objects. This is the only route that actually removes #1 from this repo.
3. Raise #1–#3 with the upstream maintainers, since the material originated there.

No history was rewritten and nothing was force-pushed, deleted or deployed in this sweep.

## Out of scope but worth a look

- **GoldenSouls.** The notes on the dump branch say this repo's purpose is to extract the GoldenEye music
  from the ROM "for GoldenSouls to ship as `assets/music/*.wav`" (43 tracks). If the GoldenSouls repo,
  or any release of it, is public and contains those WAVs, that is copyrighted game audio and the
  most serious exposure in this project family. Not checked here; recommend including it in the next sweep.
- **`scheissgeist/N64ModernRuntime`** (submodule fork on the dump branch) holds the `pi.cpp` dump
  hooks. Not scanned.

## Method

All commands were run against a fresh clone after `git fetch --all --tags`. Refs scanned:
`main`, `claude/funny-allen-mpmtzj` (both `f31b5d1`) and `origin/goldensouls-audio-dump` (`c238a45`).
There are no tags. That is 56 commits and 309 unique blobs across 169 paths.

1. **Inventory.** `git rev-list --all --objects | git cat-file --batch-check` gave every blob with size
   and path, sorted by size. Path lists for each tip were diffed against the all-history list to find
   files that exist only in history.
2. **Copyrighted material.**
   - Filename/extension pass for ROM, dump, audio, archive, ELF/map and generated-output names.
   - Size pass: no blob ≥ 4 MB except a font.
   - Every C/C++ blob ranked by hex-literal density to spot embedded data tables; the top hits were read.
   - Recompiler configs (`us.toml`, `aspMain.us.toml`, `patches.toml`) read to learn what output they generate and where it goes.
   - Every image/SVG in HEAD and the earlier icon set was rendered (Chromium/ImageMagick) and looked at.
3. **Secrets.**
   - `gitleaks 8.21.2`: `gitleaks git --log-opts=--all` (history) and `gitleaks dir` (tree), with redaction. 0 findings each.
   - Second pass: `git log -p --all -i -G` with `api[_-]?key|secret|passw(or)?d|token|bearer|authorization|PRIVATE KEY|AKIA…|ghp_|gho_|github_pat_|sk-…|xox[bap]-|AIza…|client_secret|webhook`. Every hit was game code (`tokenFind`), GPL text, RmlUi CSS or npm package names.
4. **Personal info.**
   - `git log -p --all` grepped for email addresses and IPv4 addresses: none.
   - Every blob (including binaries, via `strings`) grepped for emails, Windows/Unix user paths, US-style phone numbers and private IP ranges.
   - Commit author/committer metadata reviewed with `git log --all --format='%an <%ae>'`.
