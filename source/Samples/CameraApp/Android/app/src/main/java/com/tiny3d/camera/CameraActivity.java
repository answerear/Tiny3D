package com.tiny3d.camera;

import com.tiny3d.lib.Tiny3DActivity;

import org.libsdl.app.SDLActivity;

/**
 * Created by aaronwang on 2018/1/6.
 */

public class CameraActivity extends Tiny3DActivity {
    static {
        System.loadLibrary("T3DPlatform");
        System.loadLibrary("T3DCore");
        System.loadLibrary("CameraApp");
    }

    // SDLActivity.onCreate() will iterate getLibraries() and System.loadLibrary() each.
    // This project has no "libmain.so" (the entry library is libCameraApp.so, already
    // loaded in the static block above), so override to drop the default "main" entry
    // and avoid an UnsatisfiedLinkError. SDL2 itself is still loaded by the engine libs.
    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL2",
        };
    }

    @Override
    protected String getMainSharedObject() {
        return "libCameraApp.so";
    }

    @Override
    protected String getMainFunction() {
        return "main";
    }
}
