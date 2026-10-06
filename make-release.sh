#!/bin/sh
# Make the flam3D release: releases/flam3D-<version>.tar.gz with the patched
# source from ./flam3 (no build files, binaries or local experiments), plus
# a copy of the patch named releases/flam3D-<version>.patch and SHA-256
# checksums.
#
#   ./make-release.sh [version]      (default 1.0)
#
# All files in the archive get the same timestamp, so make does not try to
# regenerate the autotools files after unpacking.
set -e

VERSION=${1:-1.0}
NAME=flam3D-$VERSION
TOP=$(cd "$(dirname "$0")" && pwd)
OUT=$TOP/releases
STAGE=$(mktemp -d)
trap 'rm -rf "$STAGE"' EXIT

cd "$TOP/flam3"
mkdir -p "$STAGE/$NAME" "$OUT"

# The files listed in release/MANIFEST (upstream's files as patched plus the
# ones flam3D adds), except libtool: configure generates it, so the release
# carries upstream's copy (release/libtool.upstream)
grep -v '^libtool$' "$TOP/release/MANIFEST" > "$STAGE/files"
tar cf - -T "$STAGE/files" | tar xf - -C "$STAGE/$NAME"
cp "$TOP/release/libtool.upstream" "$STAGE/$NAME/libtool"
chmod 755 "$STAGE/$NAME/libtool"

tar --sort=name --owner=0 --group=0 --numeric-owner \
    --mtime="$(date -u +%Y-%m-%d) 00:00:00Z" \
    -czf "$OUT/$NAME.tar.gz" -C "$STAGE" "$NAME"
cp "$TOP/flam3-3d-hack.patch" "$OUT/$NAME.patch"
(cd "$OUT" && sha256sum "$NAME.tar.gz" "$NAME.patch" > "$NAME.sha256")

echo "made $OUT/$NAME.tar.gz"
