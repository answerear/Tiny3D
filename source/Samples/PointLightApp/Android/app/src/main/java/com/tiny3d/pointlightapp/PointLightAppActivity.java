package com.tiny3d.pointlightapp;

import com.tiny3d.lib.Tiny3DActivity;

public class PointLightAppActivity extends Tiny3DActivity {
    static {
        System.loadLibrary("T3DPlatform");
        System.loadLibrary("T3DCore");
        System.loadLibrary("PointLightApp");
    }

    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL2",
        };
    }

    @Override
    protected String getMainSharedObject() {
        return "libPointLightApp.so";
    }

    @Override
    protected String getMainFunction() {
        return "main";
    }
}
