#!/bin/bash
# Strip LC_VERSION_MIN_IPHONEOS from cocos2d-x 3.15.1 prebuilt iOS libs so
# the arm64 slice can link into iOS-simulator builds on Apple Silicon Macs.
#
# Run from repo root:  ./tools/strip-prebuilt-platform.sh
#
# Idempotent: keeps a `.bak` of each lib and re-derives from backup each run.
set -e

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
EXT="$REPO_ROOT/cocos2d/external"
if [ ! -d "$EXT" ]; then
  echo "ERROR: $EXT not found. Run from a repo with cocos2d-x checked out." >&2
  exit 1
fi

# Inline the LC_VERSION_MIN_IPHONEOS stripper as a Python helper.
HELPER=$(mktemp -t strip_lc_version_min.XXXXXX.py)
trap 'rm -f "$HELPER"' EXIT
cat > "$HELPER" <<'PY'
import struct, sys
LC_VERSION_MIN_IPHONEOS = 0x25
MH_MAGIC_64 = 0xfeedfacf
def strip_lc(path):
    with open(path,'rb') as f: data=bytearray(f.read())
    if len(data) < 32: return False
    if struct.unpack('<I', data[:4])[0] != MH_MAGIC_64: return False
    _,_,_,_,ncmds,sizeofcmds,_,_ = struct.unpack('<IIIIIIII', data[:32])
    off = 32; new_cmds = []; removed = 0
    for _ in range(ncmds):
        cmd, cmdsize = struct.unpack('<II', data[off:off+8])
        if cmd == LC_VERSION_MIN_IPHONEOS:
            removed += 1
        else:
            new_cmds.append(data[off:off+cmdsize])
        off += cmdsize
    if not removed: return False
    nb = b''.join(new_cmds)
    pad = sizeofcmds - len(nb)
    struct.pack_into('<II', data, 16, ncmds - removed, len(nb))
    data[32:32+sizeofcmds] = nb + b'\x00' * pad
    with open(path,'wb') as f: f.write(data)
    return True

for p in sys.argv[1:]:
    try: strip_lc(p)
    except Exception as e: print(f"  err {p}: {e}", file=sys.stderr)
PY

process() {
  local lib_path=$1
  local lib_name=$(basename "$lib_path")
  local backup="${lib_path}.bak"
  echo "=== $lib_name ==="

  if [ ! -f "$backup" ]; then
    cp "$lib_path" "$backup"
  fi

  local wd=$(mktemp -d)

  # Extract arm64 slice (or copy thru if already thin arm64)
  if lipo -info "$backup" 2>&1 | grep -q "Non-fat"; then
    cp "$backup" "$wd/thin.a"
  else
    lipo -thin arm64 "$backup" -output "$wd/thin.a" 2>/dev/null \
      || lipo -extract arm64 "$backup" -output "$wd/thin.a"
  fi

  mkdir -p "$wd/objs"
  ( cd "$wd/objs" && ar -x ../thin.a )
  python3 "$HELPER" "$wd"/objs/*.o
  rm -f "$wd/new.a"
  xcrun libtool -static -o "$wd/new.a" "$wd"/objs/*.o 2>/dev/null \
    || ar rcs "$wd/new.a" "$wd"/objs/*.o
  cp "$wd/new.a" "$lib_path"
  rm -rf "$wd"
  echo "  patched $lib_path"
}

for rel in jpeg/prebuilt/ios/libjpeg.a \
           png/prebuilt/ios/libpng.a \
           tiff/prebuilt/ios/libtiff.a \
           freetype2/prebuilt/ios/libfreetype.a \
           curl/prebuilt/ios/libcurl.a \
           openssl/prebuilt/ios/libssl.a \
           openssl/prebuilt/ios/libcrypto.a \
           webp/prebuilt/ios/libwebp.a \
           websockets/prebuilt/ios/libwebsockets.a \
           chipmunk/prebuilt/ios/libchipmunk.a; do
  full="$EXT/$rel"
  if [ -f "$full" ]; then
    process "$full"
  else
    echo "  skip (missing): $full"
  fi
done

echo "DONE — prebuilt iOS arm64 slices are now platform-neutral; ld will treat as iOS-simulator."

# --- Engine source patches required for Xcode 17 / iOS 26 SDK -----------------
# CCFontAtlas.cpp: iOS 26 SDK redefined `iconv_t` from `void*` to `__tag_iconv_t*`.
# Inserting an explicit cast preserves source compatibility.
CCFA="$REPO_ROOT/cocos2d/cocos/2d/CCFontAtlas.cpp"
if [ -f "$CCFA" ] && ! grep -q "__premiumads_iconv_patched" "$CCFA"; then
  python3 - "$CCFA" <<'PY'
import sys
p = sys.argv[1]
src = open(p).read()
# `_iconv` is declared as `void*` in CCFontAtlas.h. iOS 26 SDK redefined
# iconv_t from `void*` to `__tag_iconv_t*`, so passing `_iconv` directly
# to iconv()/iconv_close() no longer matches. Cast at every call site.
src = src.replace("iconv_close(_iconv)",   "iconv_close((iconv_t)_iconv)")
src = src.replace("iconv(_iconv,",         "iconv((iconv_t)_iconv,")
src = src.replace("_iconv = iconv_open(",  "_iconv = (void*)iconv_open(")
src = "// __premiumads_iconv_patched\n" + src
open(p,'w').write(src)
print("  patched CCFontAtlas.cpp")
PY
fi

# CCFileUtils.cpp: iOS 26 SDK marks system() __API_UNAVAILABLE on iOS;
# extend the existing tvOS guard to also exclude iOS.
CCFU="$REPO_ROOT/cocos2d/cocos/platform/CCFileUtils.cpp"
if [ -f "$CCFU" ] && ! grep -q "__premiumads_system_patched" "$CCFU"; then
  python3 - "$CCFU" <<'PY'
import sys
p = sys.argv[1]
src = open(p).read()
# Engine source uses `#if !defined(CC_TARGET_OS_TVOS)` to gate `system()`.
# Extend to also exclude iOS (system() is __API_UNAVAILABLE on iOS 26+).
src = src.replace("#if !defined(CC_TARGET_OS_TVOS)",
                  "#if !defined(CC_TARGET_OS_TVOS) && !(TARGET_OS_IOS)")
src = "// __premiumads_system_patched\n" + src
open(p,'w').write(src)
print("  patched CCFileUtils.cpp")
PY
fi

echo "DONE — engine source patches applied (CCFontAtlas iconv_t cast, CCFileUtils system() guard)."
