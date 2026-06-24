package com.tiny3d.skeletalanimationapp;

import com.tiny3d.lib.Tiny3DActivity;

public class SkeletalAnimationAppActivity extends Tiny3DActivity {
    static {
        System.loadLibrary("T3DPlatform");
        System.loadLibrary("T3DCore");
        System.loadLibrary("SkeletalAnimationApp");
    }

    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL2",
        };
    }

    @Override
    protected String getMainSharedObject() {
        return "libSkeletalAnimationApp.so";
    }

    @Override
    protected String getMainFunction() {
        return "main";
    }
}
