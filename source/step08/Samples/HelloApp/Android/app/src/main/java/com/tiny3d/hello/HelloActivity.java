package com.tiny3d.hello;

import com.tiny3d.lib.Tiny3DActivity;

import org.libsdl.app.SDLActivity;

/**
 * Created by aaronwang on 2018/1/6.
 */

public class HelloActivity extends Tiny3DActivity {
    static {
        System.loadLibrary("T3DPlatform");
        System.loadLibrary("T3DCore");
        System.loadLibrary("Demo_HelloApp");
    }

    protected String getMainSharedObject() {
        return "libDemo_HelloApp.so";
    }

    protected String getMainFunction() {
        return "main";
    }
}
