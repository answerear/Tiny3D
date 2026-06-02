package com.tiny3d.lib;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.os.Build;
import android.provider.Settings;
import android.util.DisplayMetrics;

import org.libsdl.app.SDLActivity;

/**
 * Native(C++) 引擎层通过 JNI 调用的全局信息接口。
 *
 * 注意：native 端（T3DAndroidDir.cpp / T3DAndroidDeviceInfo.cpp）通过
 * pEnv->FindClass("com/tiny3d/lib/Tiny3DGlobal") + GetStaticMethodID 精确匹配
 * 以下方法的【方法名】与【签名】，任何方法缺失或签名不符都会在 CheckJNI 下触发
 * Runtime abort。新增/修改方法时务必与 C++ 调用处保持一致。
 *
 * 所有方法在库加载早期（onCreate 之前 Context 可能为 null）或发生异常时，
 * 必须返回安全默认值（空串 / 0），绝不返回 null、绝不抛出异常。
 */
public class Tiny3DGlobal {

    private static Context getAppContext() {
        try {
            return SDLActivity.getContext();
        } catch (Throwable t) {
            return null;
        }
    }

    /** APK 路径。  签名: ()Ljava/lang/String; */
    public static String GetApkPath() {
        try {
            Context ctx = getAppContext();
            if (ctx != null) {
                ApplicationInfo info = ctx.getApplicationInfo();
                if (info != null && info.sourceDir != null) {
                    return info.sourceDir;
                }
            }
        } catch (Throwable t) {
            // fall through to default
        }
        return "";
    }

    /** 软件版本（应用 versionName）。  签名: ()Ljava/lang/String; */
    public static String GetSoftwareVersion() {
        try {
            Context ctx = getAppContext();
            if (ctx != null) {
                PackageInfo pi = ctx.getPackageManager().getPackageInfo(ctx.getPackageName(), 0);
                if (pi != null && pi.versionName != null) {
                    return pi.versionName;
                }
            }
        } catch (Throwable t) {
            // fall through to default
        }
        return "";
    }

    /** 操作系统版本。  签名: ()Ljava/lang/String; */
    public static String GetOSVersion() {
        try {
            String release = Build.VERSION.RELEASE;
            if (release != null) {
                return "Android " + release;
            }
        } catch (Throwable t) {
            // fall through to default
        }
        return "";
    }

    /** CPU 核数。  签名: ()I */
    public static int GetCPUCores() {
        try {
            int cores = Runtime.getRuntime().availableProcessors();
            if (cores > 0) {
                return cores;
            }
        } catch (Throwable t) {
            // fall through to default
        }
        return 0;
    }

    /** 设备名（厂商 + 型号）。  签名: ()Ljava/lang/String; */
    public static String GetDeviceName() {
        try {
            String manufacturer = Build.MANUFACTURER != null ? Build.MANUFACTURER : "";
            String model = Build.MODEL != null ? Build.MODEL : "";
            String name = (manufacturer + " " + model).trim();
            return name;
        } catch (Throwable t) {
            // fall through to default
        }
        return "";
    }

    /** 设备 ID（ANDROID_ID）。  签名: ()Ljava/lang/String; */
    public static String GetDeviceID() {
        try {
            Context ctx = getAppContext();
            if (ctx != null) {
                String id = Settings.Secure.getString(
                        ctx.getContentResolver(), Settings.Secure.ANDROID_ID);
                if (id != null) {
                    return id;
                }
            }
        } catch (Throwable t) {
            // fall through to default
        }
        return "";
    }

    /** 屏幕宽度（像素）。  签名: ()I */
    public static int GetScreenWidth() {
        try {
            DisplayMetrics dm = getDisplayMetrics();
            if (dm != null) {
                return dm.widthPixels;
            }
        } catch (Throwable t) {
            // fall through to default
        }
        return 0;
    }

    /** 屏幕高度（像素）。  签名: ()I */
    public static int GetScreenHeight() {
        try {
            DisplayMetrics dm = getDisplayMetrics();
            if (dm != null) {
                return dm.heightPixels;
            }
        } catch (Throwable t) {
            // fall through to default
        }
        return 0;
    }

    /** 屏幕 DPI。  签名: ()F */
    public static float GetScreenDPI() {
        try {
            DisplayMetrics dm = getDisplayMetrics();
            if (dm != null) {
                return dm.densityDpi;
            }
        } catch (Throwable t) {
            // fall through to default
        }
        return 0.0f;
    }

    private static DisplayMetrics getDisplayMetrics() {
        try {
            Context ctx = getAppContext();
            if (ctx != null && ctx.getResources() != null) {
                return ctx.getResources().getDisplayMetrics();
            }
        } catch (Throwable t) {
            // fall through to null
        }
        return null;
    }
}
