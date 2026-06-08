package com.tiny3d.lib;

import android.os.Bundle;

import org.libsdl.app.SDLActivity;

public class Tiny3DActivity extends SDLActivity
{
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Tiny3DGlobal.initAssetManager(this);
    }
}

//import android.app.Activity;
//import android.os.Bundle;

//public class Tiny3DActivity extends Activity {
//
//    Tiny3DGLSurfaceView mView;
//
//    @Override protected void onCreate(Bundle savedInstanceState) {
//        super.onCreate(savedInstanceState);
//        mView = new Tiny3DGLSurfaceView(getApplication());
//        setContentView(mView);
//    }
//
//    @Override protected void onPause() {
//        super.onPause();
//        mView.onPause();
//    }
//
//    @Override protected void onResume() {
//        super.onResume();
//        mView.onResume();
//    }
//}


