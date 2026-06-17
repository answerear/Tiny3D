package com.tiny3d.textureapp;

import com.tiny3d.lib.Tiny3DActivity;

public class TextureAppActivity extends Tiny3DActivity {
    static {
        System.loadLibrary("T3DPlatform");
        System.loadLibrary("T3DCore");
        System.loadLibrary("TextureApp");
    }

    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL2",
        };
    }

    @Override
    protected String getMainSharedObject() {
        return "libTextureApp.so";
    }

    @Override
    protected String getMainFunction() {
        return "main";
    }
}
