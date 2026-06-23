package com.tiny3d.spotlightapp;

import com.tiny3d.lib.Tiny3DActivity;

public class SpotLightAppActivity extends Tiny3DActivity {
    static {
        System.loadLibrary("T3DPlatform");
        System.loadLibrary("T3DCore");
        System.loadLibrary("SpotLightApp");
    }

    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL2",
        };
    }

    @Override
    protected String getMainSharedObject() {
        return "libSpotLightApp.so";
    }

    @Override
    protected String getMainFunction() {
        return "main";
    }
}
