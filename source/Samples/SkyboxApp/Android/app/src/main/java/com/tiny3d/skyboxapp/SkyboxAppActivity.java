package com.tiny3d.skyboxapp;

import com.tiny3d.lib.Tiny3DActivity;

public class SkyboxAppActivity extends Tiny3DActivity {
    static {
        System.loadLibrary("T3DPlatform");
        System.loadLibrary("T3DCore");
        System.loadLibrary("SkyboxApp");
    }

    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL2",
        };
    }

    @Override
    protected String getMainSharedObject() {
        return "libSkyboxApp.so";
    }

    @Override
    protected String getMainFunction() {
        return "main";
    }
}
