package net.premiumads.cocos2dx.admob;

import android.app.Activity;
import androidx.annotation.NonNull;
import com.google.android.gms.ads.AdError;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.FullScreenContentCallback;
import com.google.android.gms.ads.LoadAdError;
import com.google.android.gms.ads.interstitial.InterstitialAd;
import com.google.android.gms.ads.interstitial.InterstitialAdLoadCallback;
import org.cocos2dx.lib.Cocos2dxActivity;

public final class InterstitialJNI {
    private static InterstitialAd sAd;

    public static native void nativeOnEvent(int code, String msg);

    public static void load(final String adUnitId) {
        final Activity act = (Activity) Cocos2dxActivity.getContext();
        act.runOnUiThread(new Runnable() { @Override public void run() {
            InterstitialAd.load(act, adUnitId, new AdRequest.Builder().build(),
                new InterstitialAdLoadCallback() {
                    @Override public void onAdLoaded(@NonNull InterstitialAd ad) {
                        sAd = ad;
                        ad.setFullScreenContentCallback(new FullScreenContentCallback() {
                            @Override public void onAdShowedFullScreenContent() { nativeOnEvent(4, ""); }
                            @Override public void onAdFailedToShowFullScreenContent(@NonNull AdError e) {
                                nativeOnEvent(5, e.getMessage() == null ? "" : e.getMessage());
                            }
                            @Override public void onAdDismissedFullScreenContent() {
                                sAd = null; nativeOnEvent(6, "");
                            }
                            @Override public void onAdImpression() { nativeOnEvent(2, ""); }
                            @Override public void onAdClicked() { nativeOnEvent(3, ""); }
                        });
                        nativeOnEvent(0, "");
                    }
                    @Override public void onAdFailedToLoad(@NonNull LoadAdError err) {
                        sAd = null;
                        nativeOnEvent(1, err.getMessage() == null ? "" : err.getMessage());
                    }
                });
        }});
    }

    public static boolean isReady() { return sAd != null; }

    public static void show() {
        final Activity act = (Activity) Cocos2dxActivity.getContext();
        act.runOnUiThread(new Runnable() { @Override public void run() {
            if (sAd != null) sAd.show(act);
            else nativeOnEvent(5, "no ad ready");
        }});
    }
}
