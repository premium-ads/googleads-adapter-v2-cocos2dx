package net.premiumads.cocos2dx;

import net.premiumads.sdk.adapter.PremiumAdsAdapter;

public final class PremiumAdsAdapterBridge {
    private PremiumAdsAdapterBridge() {}

    public static void setDebug(boolean enabled) {
        PremiumAdsAdapter.setDebug(enabled);
    }

    /// Pushes an init/status line into the C++ HelloWorldScene log buffer so
    /// the on-screen log mirrors the same wording as the reference sample app.
    public static native void nativePushInitLog(String msg);
}
