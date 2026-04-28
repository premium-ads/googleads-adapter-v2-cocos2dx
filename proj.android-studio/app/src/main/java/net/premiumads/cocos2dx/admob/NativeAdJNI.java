package net.premiumads.cocos2dx.admob;

import android.app.Activity;
import android.util.DisplayMetrics;
import android.view.View;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.TextView;
import androidx.annotation.NonNull;
import com.google.android.gms.ads.AdListener;
import com.google.android.gms.ads.AdLoader;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.LoadAdError;
import com.google.android.gms.ads.nativead.MediaView;
import com.google.android.gms.ads.nativead.NativeAd;
import com.google.android.gms.ads.nativead.NativeAdView;
import net.premiumads.cocos2dx.sample.R;
import org.cocos2dx.lib.Cocos2dxActivity;

public final class NativeAdJNI {
    private static NativeAd sAd;
    private static NativeAdView sAdView;

    public static native void nativeOnEvent(int code, String msg);

    public static void load(final String adUnitId) {
        final Activity act = (Activity) Cocos2dxActivity.getContext();
        act.runOnUiThread(new Runnable() { @Override public void run() {
            AdLoader.Builder b = new AdLoader.Builder(act, adUnitId);
            b.forNativeAd(new NativeAd.OnNativeAdLoadedListener() {
                @Override public void onNativeAdLoaded(@NonNull NativeAd ad) {
                    sAd = ad;
                    nativeOnEvent(0, "");
                }
            });
            b.withAdListener(new AdListener() {
                @Override public void onAdFailedToLoad(@NonNull LoadAdError err) {
                    sAd = null;
                    nativeOnEvent(1, err.getMessage() == null ? "" : err.getMessage());
                }
                @Override public void onAdImpression() { nativeOnEvent(2, ""); }
                @Override public void onAdClicked()    { nativeOnEvent(3, ""); }
            });
            b.build().loadAd(new AdRequest.Builder().build());
        }});
    }

    public static boolean isReady() { return sAd != null; }

    public static void bind(final float xDesign, final float yDesign,
                            final float wDesign, final float hDesign,
                            final float designH) {
        final Activity act = (Activity) Cocos2dxActivity.getContext();
        act.runOnUiThread(new Runnable() { @Override public void run() {
            if (sAd == null) return;
            // Remove any previous overlay view but DO NOT call destroyInternal
            // (it nulls sAd, which we still need for the new binding).
            if (sAdView != null) {
                FrameLayout prevRoot = act.findViewById(android.R.id.content);
                if (prevRoot != null) prevRoot.removeView(sAdView);
                sAdView = null;
            }

            sAdView = (NativeAdView) act.getLayoutInflater().inflate(R.layout.native_ad_template, null);
            ((TextView) sAdView.findViewById(R.id.ad_headline)).setText(sAd.getHeadline());
            ((TextView) sAdView.findViewById(R.id.ad_body)).setText(sAd.getBody());
            ((Button) sAdView.findViewById(R.id.ad_call_to_action)).setText(sAd.getCallToAction());
            MediaView mv = sAdView.findViewById(R.id.ad_media);
            if (sAd.getMediaContent() != null) mv.setMediaContent(sAd.getMediaContent());
            sAdView.setHeadlineView(sAdView.findViewById(R.id.ad_headline));
            sAdView.setBodyView(sAdView.findViewById(R.id.ad_body));
            sAdView.setCallToActionView(sAdView.findViewById(R.id.ad_call_to_action));
            sAdView.setMediaView(mv);
            sAdView.setNativeAd(sAd);

            DisplayMetrics dm = act.getResources().getDisplayMetrics();
            float scale = dm.heightPixels / designH;
            int wPx = (int)(wDesign * scale);
            int hPx = (int)(hDesign * scale);
            int leftPx = (int)(xDesign * scale);
            int topPx = dm.heightPixels - (int)((yDesign + hDesign) * scale);

            FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(wPx, hPx);
            lp.leftMargin = leftPx;
            lp.topMargin = topPx;

            FrameLayout root = act.findViewById(android.R.id.content);
            root.addView(sAdView, lp);
        }});
    }

    public static void destroy() {
        final Activity act = (Activity) Cocos2dxActivity.getContext();
        act.runOnUiThread(new Runnable() { @Override public void run() { destroyInternal(act); }});
    }

    private static void destroyInternal(Activity act) {
        if (sAdView != null) {
            FrameLayout root = act.findViewById(android.R.id.content);
            if (root != null) root.removeView(sAdView);
            sAdView = null;
        }
        if (sAd != null) { sAd.destroy(); sAd = null; }
        nativeOnEvent(4, "");
    }
}
