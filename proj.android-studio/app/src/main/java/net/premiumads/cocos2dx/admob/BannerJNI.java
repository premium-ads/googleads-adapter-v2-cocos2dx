package net.premiumads.cocos2dx.admob;

import android.app.Activity;
import android.util.DisplayMetrics;
import android.view.Gravity;
import android.view.View;
import android.widget.FrameLayout;
import androidx.annotation.NonNull;
import com.google.android.gms.ads.AdListener;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdSize;
import com.google.android.gms.ads.AdView;
import com.google.android.gms.ads.LoadAdError;
import org.cocos2dx.lib.Cocos2dxActivity;

public final class BannerJNI {
    private static AdView sAdView;
    private static FrameLayout sContainer;

    public static native void nativeOnEvent(int code, String msg);

    public static void create(final String adUnitId, final int sizeOrdinal, final int position) {
        final Activity act = (Activity) Cocos2dxActivity.getContext();
        act.runOnUiThread(new Runnable() {
            @Override public void run() {
                if (sAdView != null) destroy();
                sAdView = new AdView(act);
                sAdView.setAdUnitId(adUnitId);
                sAdView.setAdSize(adSizeFor(act, sizeOrdinal));
                sAdView.setAdListener(new AdListener() {
                    @Override public void onAdLoaded() { nativeOnEvent(0, ""); }
                    @Override public void onAdFailedToLoad(@NonNull LoadAdError err) {
                        nativeOnEvent(1, err.getMessage() == null ? "" : err.getMessage());
                    }
                    @Override public void onAdImpression() { nativeOnEvent(2, ""); }
                    @Override public void onAdClicked()    { nativeOnEvent(3, ""); }
                    @Override public void onAdOpened()     { nativeOnEvent(4, ""); }
                    @Override public void onAdClosed()     { nativeOnEvent(5, ""); }
                });
                sContainer = new FrameLayout(act);
                int gravity = (position == 0)
                    ? Gravity.TOP    | Gravity.CENTER_HORIZONTAL
                    : Gravity.BOTTOM | Gravity.CENTER_HORIZONTAL;
                FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(
                        FrameLayout.LayoutParams.WRAP_CONTENT,
                        FrameLayout.LayoutParams.WRAP_CONTENT,
                        gravity);
                sContainer.addView(sAdView);
                FrameLayout root = act.findViewById(android.R.id.content);
                root.addView(sContainer, lp);
                sAdView.loadAd(new AdRequest.Builder().build());
            }
        });
    }

    private static AdSize adSizeFor(Activity act, int ordinal) {
        switch (ordinal) {
            case 1: return AdSize.MEDIUM_RECTANGLE;          // 300x250 MREC
            case 2: return AdSize.LEADERBOARD;               // 728x90
            case 3: {                                        // Adaptive
                DisplayMetrics dm = act.getResources().getDisplayMetrics();
                int widthDp = (int) (dm.widthPixels / dm.density);
                return AdSize.getCurrentOrientationAnchoredAdaptiveBannerAdSize(act, widthDp);
            }
            default: return AdSize.BANNER;                   // 320x50
        }
    }

    public static void show() {
        final Activity act = (Activity) Cocos2dxActivity.getContext();
        act.runOnUiThread(new Runnable() { @Override public void run() {
            if (sContainer != null) sContainer.setVisibility(View.VISIBLE);
        }});
    }

    public static void hide() {
        final Activity act = (Activity) Cocos2dxActivity.getContext();
        act.runOnUiThread(new Runnable() { @Override public void run() {
            if (sContainer != null) sContainer.setVisibility(View.GONE);
        }});
    }

    public static void destroy() {
        final Activity act = (Activity) Cocos2dxActivity.getContext();
        act.runOnUiThread(new Runnable() { @Override public void run() {
            if (sAdView != null) { sAdView.destroy(); sAdView = null; }
            if (sContainer != null) {
                FrameLayout root = act.findViewById(android.R.id.content);
                if (root != null) root.removeView(sContainer);
                sContainer = null;
            }
        }});
    }
}
