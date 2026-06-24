package com.tiny3d.shadowapp;

import com.tiny3d.lib.Tiny3DActivity;

public class ShadowAppActivity extends Tiny3DActivity {
    static {
        System.loadLibrary("T3DPlatform");
        System.loadLibrary("T3DCore");
        System.loadLibrary("ShadowApp");
    }

    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL2",
        };
    }

    @Override
    protected String getMainSharedObject() {
        return "libShadowApp.so";
    }

    @Override
    protected String getMainFunction() {
        return "main";
    }
}
