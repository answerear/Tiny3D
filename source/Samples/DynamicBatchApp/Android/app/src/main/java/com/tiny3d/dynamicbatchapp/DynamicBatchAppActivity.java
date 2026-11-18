package com.tiny3d.dynamicbatchapp;

import com.tiny3d.lib.Tiny3DActivity;

public class DynamicBatchAppActivity extends Tiny3DActivity {
    static {
        System.loadLibrary("T3DPlatform");
        System.loadLibrary("T3DCore");
        System.loadLibrary("DynamicBatchApp");
    }

    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL2",
        };
    }

    @Override
    protected String getMainSharedObject() {
        return "libDynamicBatchApp.so";
    }

    @Override
    protected String getMainFunction() {
        return "main";
    }
}
