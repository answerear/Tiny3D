package com.tiny3d.textureapp;

import com.tiny3d.lib.Tiny3DActivity;

import org.libsdl.app.SDLActivity;

/**
 * Created by aaronwang on 2018/1/6.
 */

public class TextureAppActivity extends Tiny3DActivity {
    static {
        System.loadLibrary("T3DPlatform");
        System.loadLibrary("T3DCore");
        System.loadLibrary("T3DLog");
        System.loadLibrary("T3DMath");
        System.loadLibrary("TextureApp");
    }

    protected String getMainSharedObject() {
        return "libTextureApp.so";
    }

    protected String getMainFunction() {
        return "main";
    }
}
