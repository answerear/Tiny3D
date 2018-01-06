package com.tiny3d.lib;

/**
 * Created by aaronwang on 2017/12/25.
 */

public class Tiny3DJniInterface {
    static {
        System.loadLibrary("T3DPlatform");
        System.loadLibrary("T3DCore");
        System.loadLibrary("Demo_Hello");
    }

    public static native void init();
    public static native void render();
}
