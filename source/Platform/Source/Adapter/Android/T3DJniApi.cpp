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

#include "Adapter/Android/T3DJniApi.h"

#include <SDL.h>
#include <string>


namespace Tiny3D
{
    // 当默认 FindClass 因 ClassLoader 上下文问题（如早期 native 线程使用系统 ClassLoader）
    // 找不到 app 类时，借助 SDL 持有的 Activity 实例获取 app ClassLoader，再 loadClass 兜底。
    // 入参 className 为 JNI 形式（"com/tiny3d/lib/Tiny3DGlobal"），内部转换为点号形式。
    static jclass FindClassWithAppLoader(JNIEnv *pEnv, const char *className)
    {
        if (pEnv == nullptr || className == nullptr)
            return nullptr;

        jclass result = nullptr;
        jobject activity = (jobject)SDL_AndroidGetActivity();
        if (activity == nullptr)
            return nullptr;

        do
        {
            // 通过 Activity 实例方法 getClassLoader() 拿到 app ClassLoader。
            jclass activityClass = pEnv->GetObjectClass(activity);
            if (activityClass == nullptr)
                break;

            jmethodID getClassLoader = pEnv->GetMethodID(
                activityClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
            if (getClassLoader == nullptr)
            {
                if (pEnv->ExceptionCheck()) pEnv->ExceptionClear();
                pEnv->DeleteLocalRef(activityClass);
                break;
            }

            jobject classLoader = pEnv->CallObjectMethod(activity, getClassLoader);
            pEnv->DeleteLocalRef(activityClass);
            if (pEnv->ExceptionCheck()) { pEnv->ExceptionClear(); classLoader = nullptr; }
            if (classLoader == nullptr)
                break;

            // java/lang/ClassLoader 为 boot class，FindClass 必定成功，规避 app 类互相找不到。
            jclass classLoaderClass = pEnv->FindClass("java/lang/ClassLoader");
            if (classLoaderClass == nullptr)
            {
                if (pEnv->ExceptionCheck()) pEnv->ExceptionClear();
                pEnv->DeleteLocalRef(classLoader);
                break;
            }

            jmethodID loadClass = pEnv->GetMethodID(
                classLoaderClass, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
            pEnv->DeleteLocalRef(classLoaderClass);
            if (loadClass == nullptr)
            {
                if (pEnv->ExceptionCheck()) pEnv->ExceptionClear();
                pEnv->DeleteLocalRef(classLoader);
                break;
            }

            // 将 "a/b/C" 转为 "a.b.C"
            std::string dotted(className);
            for (size_t i = 0; i < dotted.size(); ++i)
            {
                if (dotted[i] == '/')
                    dotted[i] = '.';
            }

            jstring jName = pEnv->NewStringUTF(dotted.c_str());
            if (jName != nullptr)
            {
                result = (jclass)pEnv->CallObjectMethod(classLoader, loadClass, jName);
                if (pEnv->ExceptionCheck()) { pEnv->ExceptionClear(); result = nullptr; }
                pEnv->DeleteLocalRef(jName);
            }

            pEnv->DeleteLocalRef(classLoader);
        } while (0);

        pEnv->DeleteLocalRef(activity);

        return result;
    }

    bool GetClassStaticMethodID(JNIEnv *pEnv, JNICallParam &param, const char *className, const char *methodName, const char *methodParam)
    {
        bool ret = false;

        param.classID = 0;
        param.methodID = 0;

        do
        {
            if (pEnv == nullptr)
                break;

            param.classID = pEnv->FindClass(className);
            if (param.classID == 0)
            {
                // FindClass 失败会留下挂起的 ClassNotFoundException/NoClassDefFoundError，
                // 必须清除，否则在 CheckJNI(-Xcheck:jni) 下一次 JNI 调用即触发 Runtime abort。
                if (pEnv->ExceptionCheck())
                {
                    pEnv->ExceptionClear();
                }

                // 兜底：用 app ClassLoader 再解析一次（解决系统 ClassLoader 找不到 app 类的问题）。
                param.classID = FindClassWithAppLoader(pEnv, className);
                if (param.classID == 0)
                    break;
            }

            param.methodID = pEnv->GetStaticMethodID(param.classID, methodName, methodParam);
            if (param.methodID == 0)
            {
                // GetStaticMethodID 失败同样会留下挂起的 NoSuchMethodError。
                if (pEnv->ExceptionCheck())
                {
                    pEnv->ExceptionClear();
                }
                break;
            }

            ret = true;
        } while (0);

        return ret;
    }

    void DeleteLocalRef(JNIEnv *pEnv, JNICallParam &param)
    {
        if (pEnv != nullptr && param.classID)
        {
            pEnv->DeleteLocalRef(param.classID);
        }
    }
}
