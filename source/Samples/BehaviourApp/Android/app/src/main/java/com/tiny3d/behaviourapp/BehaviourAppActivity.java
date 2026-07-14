package com.tiny3d.behaviourapp;

import com.tiny3d.lib.Tiny3DActivity;

public class BehaviourAppActivity extends Tiny3DActivity {
    static {
        System.loadLibrary("T3DPlatform");
        System.loadLibrary("T3DCore");
        System.loadLibrary("BehaviourApp");
    }

    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL2",
        };
    }

    @Override
    protected String getMainSharedObject() {
        return "libBehaviourApp.so";
    }

    @Override
    protected String getMainFunction() {
        return "main";
    }
}
