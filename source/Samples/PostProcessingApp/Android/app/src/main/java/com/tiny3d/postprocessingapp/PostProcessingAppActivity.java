package com.tiny3d.postprocessingapp;

import com.tiny3d.lib.Tiny3DActivity;

public class PostProcessingAppActivity extends Tiny3DActivity {
    static {
        System.loadLibrary("T3DPlatform");
        System.loadLibrary("T3DCore");
        System.loadLibrary("PostProcessingApp");
    }

    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL2",
        };
    }

    @Override
    protected String getMainSharedObject() {
        return "libPostProcessingApp.so";
    }

    @Override
    protected String getMainFunction() {
        return "main";
    }
}
