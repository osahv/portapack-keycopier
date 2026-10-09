#!/usr/bin/env bash
# Build Mayhem + Key Copier together.  Usage: scripts/build.sh [mayhem-tag] [version-suffix]
# Output: dist/full-<tag><suffix>/{firmware .bin, ppfw tar, APPS/*.ppma}
# The version string (and so the ppma compatibility hash) gets the suffix, so stock .ppma files from the SD card are
# NOT loaded by this firmware (they would crash: they were linked against a different image).
set -euo pipefail
TAG="${1:-v2.4.0}"
SUFFIX="${2:--kc1}"
VER="$TAG$SUFFIX"
HERE="$(cd "$(dirname "$0")/.." && pwd)"
WORK="$HERE/build/full-$TAG"
OUT="$HERE/dist/full-$VER"
mkdir -p "$OUT/APPS" "$HERE/build"

if [ ! -d "$WORK" ]; then
  git clone --depth 1 --branch "$TAG" https://github.com/portapack-mayhem/mayhem-firmware.git "$WORK"
  git -C "$WORK" submodule update --init --recursive --depth 1
fi
git -C "$WORK" checkout -- firmware/application/external/external.cmake firmware/application/external/external.ld
rm -rf "$WORK/firmware/application/external/key_copier"
cp -R "$HERE/app/key_copier" "$WORK/firmware/application/external/key_copier"
python3 "$HERE/scripts/integrate.py" "$WORK"

docker image inspect portapack-dev >/dev/null 2>&1 || docker build -t portapack-dev -f "$WORK/dockerfile-nogit" "$WORK"
rm -rf "$WORK/build"
# parallel make occasionally races on libopencm3 on the first run; a retry resumes where it stopped
for i in 1 2 3; do docker run --rm -e VERSION_STRING="$VER" -v "$WORK":/havoc portapack-dev -j4 && break; done

B="$WORK/build/firmware"
[ -f "$B/application/key_copier.ppma" ] || { echo "key_copier.ppma was not produced"; exit 1; }
cp "$B/portapack-mayhem-firmware.bin" "$OUT/portapack-mayhem_$VER.bin"
cp "$B/portapack-mayhem_OCI.ppfw.tar" "$OUT/mayhem_${VER}_OCI.ppfw.tar"
cp "$B"/application/*.ppma "$OUT/APPS/"
cp "$B"/standalone/*/*.ppmp "$OUT/APPS/" 2>/dev/null || true
echo "built: $OUT  ($(ls "$OUT/APPS" | wc -l) apps)"
