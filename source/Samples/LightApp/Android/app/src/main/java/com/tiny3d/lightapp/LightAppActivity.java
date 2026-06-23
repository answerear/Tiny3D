package com.tiny3d.lightapp;

import com.tiny3d.lib.Tiny3DActivity;

public class LightAppActivity extends Tiny3DActivity {
    static {
        System.loadLibrary("T3DPlatform");
        System.loadLibrary("T3DCore");
        System.loadLibrary("LightApp");
    }

    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL2",
        };
    }

    @Override
    protected String getMainSharedObject() {
        return "libLightApp.so";
    }

    @Override
    protected String getMainFunction() {
        return "main";
    }
}
