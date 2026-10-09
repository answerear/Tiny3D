package com.tiny3d.rmlui;

import com.tiny3d.lib.Tiny3DActivity;

/**
 * RmlUi sample activity.
 */
public class RmlUiActivity extends Tiny3DActivity {
    static {
        System.loadLibrary("T3DPlatform");
        System.loadLibrary("T3DCore");
        System.loadLibrary("RmlUiApp");
    }

    // SDLActivity.onCreate() will iterate getLibraries() and System.loadLibrary() each.
    // This project has no "libmain.so" (the entry library is libRmlUiApp.so, already
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
        return "libRmlUiApp.so";
    }

    @Override
    protected String getMainFunction() {
        return "main";
    }
}
