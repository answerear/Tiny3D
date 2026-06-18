package com.tiny3d.sceneapp;

import com.tiny3d.lib.Tiny3DActivity;

public class SceneAppActivity extends Tiny3DActivity {
    static {
        System.loadLibrary("T3DPlatform");
        System.loadLibrary("T3DCore");
        System.loadLibrary("SceneApp");
    }

    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL2",
        };
    }

    @Override
    protected String getMainSharedObject() {
        return "libSceneApp.so";
    }

    @Override
    protected String getMainFunction() {
        return "main";
    }
}
