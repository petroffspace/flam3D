# flam3D

![flam3D: two 2D flam3 flames and two Apophysis 7X 3D flames](flam3/flam3D-title.png)

flam3D is [flam3](https://github.com/scottdraves/flam3) with the Apophysis 7X
"3D hack" added, so it renders the 3D flames of Apophysis 7X as well as
classic 2D flames. It adds:

- a z coordinate and the 3D camera (`cam_pitch`, `cam_yaw`,
  `cam_perspective`, `cam_zpos`, `cam_dof`)
- Apophysis' pre_/post_ variation ordering and `var_color`
- the 34 built-in 3D variations of Apophysis 7X, the 18 3D variations of its
  plugin pack, and the circlize, Spherical3D, scry_3D and zeta3D plugins
- Apophysis' handling of xaos dead ends (an xform whose links to all xforms
  are 0 ends the batch of iterations instead of aborting the render)

Flames without 3D features render exactly as with unpatched flam3. See the
"3D hack" section of `flam3/README.txt` for the full list of variations and
parameters.

Apophysis changed the 3D behaviour in 7X 15C: since then the 2D variations
(spherical, polar, julian, ...) pass z through and `linear` is 3D, while older
versions keep z only in the explicitly 3D variations and call the 3D linear
`linear3D`. flam3D picks the older behaviour from the flame's `version`
attribute (Apophysis 2.x, 7X.14 and earlier, 7X 15B and earlier) and the
newer one for everything else; `apo_pre15c="1"` or `"0"` on the `<flame>`
element overrides it, and flam3-genome writes it out so the setting survives.

## Credits

- **flam3** was created by Scott Draves, with Erik Reckase and other
  contributors (Copyright (C) 1992-2009 Spotworks LLC):
  [flam3.com](https://flam3.com),
  [github.com/scottdraves/flam3](https://github.com/scottdraves/flam3).
- **Apophysis** was created by Mark Townsend (2001-2004) and continued by
  Ronald Hordijk, Piotr Borys and Peter Sdobnov. Peter Sdobnov wrote the
  "3D hack" (2007-2008), and Georg Kiehne made **Apophysis 7X** (2009-2011):
  [github.com/xyrus02/apophysis-7x](https://github.com/xyrus02/apophysis-7x).
- The 3D plugin variations come from the Apophysis plugin pack; circlize and
  Spherical3D follow their JWildfire versions.
- flam3D: © petroffspace.com

## License

flam3 and Apophysis are licensed under the GNU GPL version 3 or later, so the
patched flam3 source (`flam3/`) and `flam3-3d-hack.patch` are under the GPL
too; see `flam3/COPYING`. The headers in `deps/` keep the licenses of libxml2
and libjpeg. The rest of this repository (the build, patch and release
scripts, the READMEs and the title image) is under the MIT license; see
`LICENSE`.

## Contents

| Path | What it is |
|---|---|
| `flam3/` | flam3 source with the patch applied, plus the built binaries |
| `flam3-3d-hack.patch` | the same changes as a patch against upstream flam3 |
| `LICENSE` | MIT license for the files outside `flam3/` (see "License") |
| `build.sh` | builds `flam3/` on this machine using the headers in `deps/` |
| `deps/` | libxml2 and libjpeg development headers (no root needed to build) |
| `make-patch.sh` | regenerates `flam3-3d-hack.patch` from `flam3/` (clones upstream) |
| `make-release.sh` | makes the release archive in `releases/` |
| `release/` | release file list, upstream's `libtool`, the upstream commit and the title card's flames and script |
| `releases/` | `flam3D-<version>.tar.gz`, `flam3D-<version>.patch` and their SHA-256 sums |

## Building on this machine

```
./build.sh
```

The binaries (`flam3-render`, `flam3-animate`, `flam3-genome`,
`flam3-convert`) end up in `flam3/`. Nothing is installed system-wide.

Ubuntu's `flam3-utils` package puts the unpatched flam3 in `/usr/bin`. Run the
flam3D binaries by path, or put them first on your `PATH` (run this from the
top of the repository):

```
export PATH=$PWD/flam3:$PATH
export flam3_palettes=$PWD/flam3/flam3-palettes.xml
```

`flam3_palettes` is needed because flam3 is not installed; without it,
numbered palettes render white.

## Making a release

After changing the source in `flam3/`, regenerate the patch, then make the
archive:

```
./make-patch.sh
./make-release.sh 1.0
```

A file added to the source must also be listed in `release/MANIFEST`.

This writes `releases/flam3D-1.0.tar.gz` (the patched source without build
files, binaries or experiments; it unpacks to `flam3D-1.0/`),
`releases/flam3D-1.0.patch` (a copy of `flam3-3d-hack.patch`) and
`releases/flam3D-1.0.sha256`.

To remake the title image `flam3/flam3D-title.png` after changing its flames
in `release/title-card.flam3`, build first and then run
`release/make-title-card.sh` (needs ImageMagick and the DejaVu fonts; the
render takes a few minutes).

## Building elsewhere (Linux, Raspberry Pi 5, 400, 2B)

Copy the release archive to the other machine and follow `flam3/README.md`,
which is also the README inside the archive.
