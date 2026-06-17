package com.tiny3d.geometryapp;

import com.tiny3d.lib.Tiny3DActivity;
import org.libsdl.app.SDLActivity;

public class GeometryAppActivity extends Tiny3DActivity {
    static {
        System.loadLibrary("T3DPlatform");
        System.loadLibrary("T3DCore");
        System.loadLibrary("GeometryApp");
    }

    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL2",
        };
    }

    @Override
    protected String getMainSharedObject() {
        return "libGeometryApp.so";
    }

    @Override
    protected String getMainFunction() {
        return "main";
    }
}
