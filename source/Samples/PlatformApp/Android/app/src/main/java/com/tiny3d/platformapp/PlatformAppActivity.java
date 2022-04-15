package com.tiny3d.platformapp;

import com.tiny3d.lib.Tiny3DActivity;

import org.libsdl.app.SDLActivity;

/**
 * Created by aaronwang on 2018/1/6.
 */

public class PlatformAppActivity extends Tiny3DActivity {
    static {
        System.loadLibrary("T3DPlatform");
        System.loadLibrary("T3DCore");
        System.loadLibrary("T3DLog");
        System.loadLibrary("T3DMath");
        System.loadLibrary("PlatformApp");
    }

    protected String getMainSharedObject() {
        return "libPlatformApp.so";
    }

    protected String getMainFunction() {
        return "main";
    }
}
