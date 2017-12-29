/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#ifndef T3D_OS_ANDROID

#include "HelloApp.h"

int main(int argc, char *argv[])
{
    HelloApp *theApp = new HelloApp();
    Tiny3D::Engine *theEngine = new Tiny3D::Engine();
    theEngine->startup();
    theEngine->setAppEventListener(theApp);
    theEngine->run();
    delete theEngine;
    delete theApp;
    return 0;
}

#else

#include <jni.h>
#include <string>

#include "HelloApp.h"

extern "C"
{
//    JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved)
//    {
//        JNIEnv *env = nullptr;
//
//        jint result = -1;
//
//        
//    }
    
    JNIEXPORT void JNICALL Java_com_tiny3d_engine_Tiny3DJniInterface_init(JNIEnv *env, jobject obj)
    {
        HelloApp *theApp = new HelloApp();
        Tiny3D::Engine *theEngine = new Tiny3D::Engine();
        theEngine->startup();
        theEngine->setAppEventListener(theApp);
        theEngine->run();
    }

    JNIEXPORT void JNICALL Java_com_tiny3d_engine_Tiny3DJniInterface_render(JNIEnv *env, jobject obj)
    {

    }
}

#endif