package com.tiny3d.computeapp;

import com.tiny3d.lib.Tiny3DActivity;

public class ComputeAppActivity extends Tiny3DActivity {
    static {
        System.loadLibrary("T3DPlatform");
        System.loadLibrary("T3DCore");
        System.loadLibrary("ComputeApp");
    }

    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL2",
        };
    }

    @Override
    protected String getMainSharedObject() {
        return "libComputeApp.so";
    }

    @Override
    protected String getMainFunction() {
        return "main";
    }
}
