# WWF WrestleMania Arcade portable C port — r6h4

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

The included GitHub Actions workflow installs the pinned libdragon SDK, fetches the historical source, regenerates translated animation tables, converts the needed Bret WIMP frames, builds `wm_arcade_r6h4.z64`, and publishes it on the quota-free `rom-build` branch.

Local N64 build requires a libdragon installation and `N64_INST`:

```sh
make -j2
```

Output: `wm_arcade_r6h4.z64`

## r6h2 hardware renderer hotfix

The first r6 real-hardware test exposed an RSP/display queue timeout while two
large CI8 wrestler sprites were active. This hotfix keeps the game/core logic
unchanged and changes only the N64 renderer:

- explicit TMEM-sized CI8 horizontal strips
- synchronous `rdpq_detach_wait()` + `display_show()` frame completion
- `rdpq_debug_start()` validator enabled for hardware diagnosis

The goal is to eliminate the missing upper sprite strips and the `display_get`
RSP wait timeout before adding more gameplay systems.


## r6h2 two-layer Bret fix

The hardware captures showed that r6h1 fixed the RSP queue crash but not the
missing upper body. The original source explains why: `bret_ani_init` starts
`hrt_stand*_anim` with `change_anim1a` and independently starts
`hrt_torso*_anim` with `change_anim2a`. In other words, standing/walking Bret
is a two-animation composite. r6h2 carries both visual states in the portable
core and RDP-composites the H2TW2A/H4TW4A torso layer over the primary frame.
It also resolves near-perfect wrestler overlap in one tick so two identical
Brets do not visually collapse into one sprite.


## r6h4 attachment-metadata scanner

r6h2 proved the original second animation channel is required, but it drew that
channel at the wrestler root and produced separated body pieces. r6h3 then proved
that treating raw WIMP entry bytes +32/+34 as the runtime IANI2X/IANI2Y values was
wrong: the real HRT_WLK.IMG produced implausible values before the ROM build.

r6h4 therefore stops guessing. The converter preserves all nine signed 16-bit
words in the unknown 18-byte tail of every WIMP image-directory entry. On N64,
C-Left/C-Right cycles each adjacent candidate pair and C-Down reverses the
attachment-delta convention. The debug HUD shows the raw offsets and values live.
The r6h1 synchronous/TMEM-safe renderer remains intact. Once a candidate assembles
Bret correctly, that mapping can be locked into the portable converter; if none
works, the evidence points to LOAD2 deriving PWRD values from another WIMP structure
(such as point-table data) rather than copying raw image-entry words directly.
