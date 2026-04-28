package net.premiumads.cocos2dx;

import android.app.Application;
import android.util.Log;
import com.google.android.gms.ads.MobileAds;
import com.google.android.gms.ads.initialization.AdapterStatus;
import com.google.android.gms.ads.initialization.InitializationStatus;
import com.google.android.gms.ads.initialization.OnInitializationCompleteListener;
import java.util.Map;

public final class MainApplication extends Application {
    private static final String TAG = "PremiumAdsSample";

    @Override
    public void onCreate() {
        super.onCreate();
        try { PremiumAdsAdapterBridge.nativePushInitLog("Initializing Google Mobile Ads SDK..."); } catch (UnsatisfiedLinkError ignored) {}
        MobileAds.initialize(this, new OnInitializationCompleteListener() {
            @Override
            public void onInitializationComplete(InitializationStatus status) {
                Log.i(TAG, "MobileAds initialized: " + status.getAdapterStatusMap());
                try {
                    PremiumAdsAdapterBridge.nativePushInitLog("SDK initialized. Adapters:");
                    for (Map.Entry<String, AdapterStatus> e : status.getAdapterStatusMap().entrySet()) {
                        String state = (e.getValue().getInitializationState() == AdapterStatus.State.READY) ? "Ready" : "Not Ready";
                        PremiumAdsAdapterBridge.nativePushInitLog("  " + e.getKey() + ": " + state);
                    }
                } catch (UnsatisfiedLinkError ignored) {}
            }
        });
    }
}
