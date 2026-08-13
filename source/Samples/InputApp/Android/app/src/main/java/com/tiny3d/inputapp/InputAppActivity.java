package com.tiny3d.inputapp;

import com.tiny3d.lib.Tiny3DActivity;

public class InputAppActivity extends Tiny3DActivity {
    static {
        System.loadLibrary("T3DPlatform");
        System.loadLibrary("T3DCore");
        System.loadLibrary("InputApp");
    }

    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL2",
        };
    }

    @Override
    protected String getMainSharedObject() {
        return "libInputApp.so";
    }

    @Override
    protected String getMainFunction() {
        return "main";
    }
}
