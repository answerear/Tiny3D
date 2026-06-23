package com.tiny3d.directionallightapp;

import com.tiny3d.lib.Tiny3DActivity;

public class DirectionalLightAppActivity extends Tiny3DActivity {
    static {
        System.loadLibrary("T3DPlatform");
        System.loadLibrary("T3DCore");
        System.loadLibrary("DirectionalLightApp");
    }

    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL2",
        };
    }

    @Override
    protected String getMainSharedObject() {
        return "libDirectionalLightApp.so";
    }

    @Override
    protected String getMainFunction() {
        return "main";
    }
}
