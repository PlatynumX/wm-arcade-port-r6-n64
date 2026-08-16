# WWF WrestleMania Arcade portable C port — r6

N64-first, portable-core source translation experiment based on the original Midway WWF WrestleMania source tree.

r6 is the first two-wrestler fight sandbox. It keeps the real source-derived Bret animation/art pipeline from r5, but now renders two independently animated fighters and adds a portable combat/AI layer so real N64 hardware can exercise something much closer to a match.

## r6 hardware target

- Nintendo 64 / libdragon is the primary target.
- Stock 4 MB RAM remains the design floor.
- Real Midway Bret CI8 frames and palettes are generated in CI from the historical source assets.
- RDPQ does sprite blits, TLUT palettes and horizontal mirroring.
- The core contains no libdragon dependencies.

## What changed from r5

- Two simultaneous source-art Bret fighters.
- P1 controller plus CPU opponent.
- CPU chase, facing, walk/run and punch/kick behavior.
- Portable health, hit detection, one-hit-per-swing gating, damage, stun and knockback.
- Painter-style wrestler depth sorting by ring Y.
- Fighter separation to avoid occupying the exact same anchor point.
- Tighter anchor bounds based on the r5 real-hardware video, which showed source registration points could move most of a sprite outside the ring.
- Health bars and combat telemetry.
- L toggles CPU AI; R resets the match.

The r6 attack contact windows are deliberately conservative visual-frame windows. They are not being claimed as the original arcade hitbox logic; translating the original attack/hitbox command semantics is the next source-accuracy step.

## Controls

- Analog stick / D-pad: move P1
- Z + movement: run
- A: punch
- B: kick
- L: toggle CPU AI
- R: reset match
- C-Up: replay translated FINISEQ command-stream smoke test
- Start: toggle debug HUD

## Host verification

```sh
cmake -S . -B build-host -DCMAKE_BUILD_TYPE=Release
cmake --build build-host --parallel
ctest --test-dir build-host --output-on-failure
./build-host/wm_headless
```

## N64 build

The included GitHub Actions workflow installs the pinned libdragon SDK, fetches the historical source, regenerates translated animation tables, converts the needed Bret WIMP frames, builds `wm_arcade_r6.z64`, and publishes it on the quota-free `rom-build` branch.

Local N64 build requires a libdragon installation and `N64_INST`:

```sh
make -j2
```

Output: `wm_arcade_r6.z64`
