package com.tiny3d.resourceapp;

import com.tiny3d.lib.Tiny3DActivity;

public class ResourceAppActivity extends Tiny3DActivity {
    static {
        System.loadLibrary("T3DPlatform");
        System.loadLibrary("T3DCore");
        System.loadLibrary("ResourceApp");
    }

    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL2",
        };
    }

    @Override
    protected String getMainSharedObject() {
        return "libResourceApp.so";
    }

    @Override
    protected String getMainFunction() {
        return "main";
    }
}
