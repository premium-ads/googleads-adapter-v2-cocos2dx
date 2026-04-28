# PremiumAds Adapter v2 — Cocos2d-x Sample

[![Cocos2d-x 3.15.1](https://img.shields.io/badge/cocos2d--x-3.15.1-blue.svg)]()
[![GMA Android 23.6.0](https://img.shields.io/badge/GMA%20Android-23.6.0-green.svg)]()
[![GMA iOS 13.x](https://img.shields.io/badge/GMA%20iOS-13.x-green.svg)]()
[![License: MIT](https://img.shields.io/badge/license-MIT-yellow.svg)]()

A self-contained Cocos2d-x 3.15.1 sample demonstrating end-to-end integration
of the **PremiumAds AdMob mediation adapter v2** on Android and iOS, covering
all six ad formats supported by the legacy adapter line:

- Banner (incl. MREC 300×250, Leaderboard, Adaptive)
- Interstitial
- Rewarded
- Rewarded Interstitial
- App Open
- Native

The repo doubles as a **drop-in plugin reference**: copy
`Classes/PremiumAdsAdapter/` and `Classes/AdMobBridge/` into your own
Cocos2d-x project. Full integration guide and AdMob console setup are
published at <https://docs.premiumads.net>.

## Quick start

```bash
# 1. Clone this repo
git clone https://github.com/premium-ads/googleads-adapter-v2-cocos2dx.git
cd googleads-adapter-v2-cocos2dx

# 2. Install Cocos2d-x 3.15.1 and symlink the engine
mkdir -p ~/cocos-installs
curl -L -o /tmp/cocos.zip \
  https://github.com/cocos2d/cocos2d-x/archive/refs/tags/cocos2d-x-3.15.1.zip
unzip -q /tmp/cocos.zip -d ~/cocos-installs/
(cd ~/cocos-installs/cocos2d-x-cocos2d-x-3.15.1 && python3 download-deps.py -r=yes)
ln -s ~/cocos-installs/cocos2d-x-cocos2d-x-3.15.1 cocos2d

# 3. Android
cd proj.android-studio && ./gradlew assembleDebug

# 4. iOS — Apple Silicon Mac simulator: run tools/strip-prebuilt-platform.sh first
bash tools/strip-prebuilt-platform.sh
cd proj.ios_mac/ios && pod install
xcodebuild -workspace GoogleAdsCocos2dxSample.xcworkspace \
           -scheme GoogleAdsCocos2dxSample-mobile \
           -sdk iphonesimulator build CODE_SIGNING_ALLOWED=NO
```

## Project layout

```
Classes/
├── PremiumAdsAdapter/        # Drop-in plugin (publisher copies this)
└── AdMobBridge/              # Hand-rolled GMA C++ bridge (one class per format)
proj.android-studio/          # Android Studio project (AGP 8.2 / Gradle 8.2 / NDK 27)
proj.ios_mac/                 # Xcode project + Podfile
tools/strip-prebuilt-platform.sh  # iOS sim linker fix on Apple Silicon
```

## Configure your AdMob IDs

Before building, fill in your own AdMob app ID and ad unit IDs in:

- `Classes/HelloWorldScene.cpp` — six `k*Id` constants per platform
- `proj.android-studio/app/src/main/AndroidManifest.xml` — `APPLICATION_ID` meta-data
- `proj.ios_mac/ios/Info.plist` — `GADApplicationIdentifier`

Each is currently set to `<your admob ad unit>` / `<your admob app id>` placeholders.

A NextGen sample (`googleads-adapter-nextgen-cocos2dx`) will follow once
Native is supported by the NextGen adapter line.

## License

MIT — see [LICENSE](LICENSE).
