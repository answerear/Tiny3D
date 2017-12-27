package com.tiny3d.engine;

/**
 * Created by aaronwang on 2017/12/25.
 */

public class Tiny3DJniInterface {
    static {
        System.loadLibrary("Tiny3D");
    }

    public static native void init();
    public static native void render();
}
