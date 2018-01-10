package com.tiny3d.lib;

//import android.content.Context;
//import android.opengl.GLSurfaceView;
//
//import javax.microedition.khronos.egl.EGLConfig;
//import javax.microedition.khronos.opengles.GL10;
//
///**
// * Created by aaronwang on 2017/12/23.
// */
//
//public class Tiny3DGLSurfaceView extends GLSurfaceView {
//    private static final String TAG = "GLES3JNI";
//    private static final boolean DEBUG = true;
//
//    public Tiny3DGLSurfaceView(Context context) {
//        super(context);
//        // Pick an EGLConfig with RGB8 color, 16-bit depth, no stencil,
//        // supporting OpenGL ES 2.0 or later backwards-compatible versions.
//        setEGLConfigChooser(8, 8, 8, 0, 16, 0);
//        setEGLContextClientVersion(3);
//        setRenderer(new Renderer());
//    }
//
//    private static class Renderer implements GLSurfaceView.Renderer {
//        public void onDrawFrame(GL10 gl) {
////            GLES3JNILib.step();
//            Tiny3DJniInterface.render();
//        }
//
//        public void onSurfaceChanged(GL10 gl, int width, int height) {
////            GLES3JNILib.resize(width, height);
//        }
//
//        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
////            GLES3JNILib.init();
//            Tiny3DJniInterface.init();
//        }
//    }
//}
