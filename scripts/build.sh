#!/usr/bin/env bash
# Build Mayhem + Key Copier together and package the result.
#   scripts/build.sh [mayhem-tag-or-branch] [version-suffix]        default: latest stable release, suffix -kc1
# Output: dist/key-copier_mayhem-<ref>.zip  (FIRMWARE/, APPS/ - unpack into the root of the SD card) and SHA256SUMS.
# The version string (and so the ppma compatibility hash) gets the suffix, so stock .ppma files from the SD card are
# NOT loaded by this firmware (they were linked against a different image and would crash).
set -euo pipefail
HERE="$(cd "$(dirname "$0")/.." && pwd)"
REF="${1:-}"
[ -n "$REF" ] || REF="$(curl -fsSL https://api.github.com/repos/portapack-mayhem/mayhem-firmware/releases/latest | python3 -c 'import json,sys;print(json.load(sys.stdin)["tag_name"])')"
SUFFIX="${2:--kc1}"
SAFE="${REF//\//_}"
VER="$SAFE$SUFFIX"
WORK="$HERE/build/mayhem-$SAFE"
OUT="$HERE/dist"
mkdir -p "$OUT" "$HERE/build"
echo "== Mayhem $REF + Key Copier (version string $VER)"

rm -rf "$WORK"
git clone --depth 1 --branch "$REF" https://github.com/portapack-mayhem/mayhem-firmware.git "$WORK"
git -C "$WORK" submodule update --init --recursive --depth 1
rm -rf "$WORK/firmware/application/external/key_copier"
cp -R "$HERE/app/key_copier" "$WORK/firmware/application/external/key_copier"
python3 "$HERE/scripts/integrate.py" "$WORK"

docker image inspect portapack-dev >/dev/null 2>&1 || docker build -t portapack-dev -f "$WORK/dockerfile-nogit" "$WORK"
mkdir -p "$WORK/build"
# parallel make occasionally races on libopencm3 on the first run; a retry resumes where it stopped
for i in 1 2 3; do docker run --rm -e VERSION_STRING="$VER" -v "$WORK":/havoc portapack-dev -j4 && break; done

B="$WORK/build/firmware"
[ -f "$B/application/key_copier.ppma" ] || { echo "key_copier.ppma was not produced"; exit 1; }
STAGE="$OUT/stage-$SAFE"
rm -rf "$STAGE"; mkdir -p "$STAGE/FIRMWARE" "$STAGE/APPS"
cp "$B/portapack-mayhem-firmware.bin" "$STAGE/FIRMWARE/portapack-mayhem_$VER.bin"
cp "$B"/application/*.ppma "$STAGE/APPS/"
cp "$B"/standalone/*/*.ppmp "$STAGE/APPS/" 2>/dev/null || true
cat > "$STAGE/INSTALL.txt" <<T
Mayhem $REF with the Key Copier app (unofficial build, $VER).

1. Back up your SD card (FIRMWARE, APPS, SETTINGS).
2. Unpack this archive into the ROOT of the SD card (merge with the existing folders). Your SETTINGS are not touched.
3. On the PortaPack: Utilities > Flash Utility > choose portapack-mayhem_$VER.bin
4. Utilities > Key Copier.

APPS must come from this archive: apps built for a different firmware are ignored (or crash), which is why this firmware
carries the "$VER" version string. To go back, flash an official .bin and restore your APPS backup.
https://github.com/osahv/portapack-keycopier
T
(cd "$STAGE" && zip -q -r "../key-copier_mayhem-$SAFE.zip" FIRMWARE APPS INSTALL.txt)
(cd "$OUT" && shasum -a 256 "key-copier_mayhem-$SAFE.zip" > "SHA256SUMS-$SAFE")
echo "built: $OUT/key-copier_mayhem-$SAFE.zip"
