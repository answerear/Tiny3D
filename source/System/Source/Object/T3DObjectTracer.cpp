/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/


#include "Object/T3DObjectTracer.h"
#include <sstream>

#if defined (T3D_OS_WINDOWS)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <windows.h>
#elif defined (T3D_OS_LINUX) || defined (T3D_OS_OSX) \
    || defined (T3D_OS_ANDROID) || defined (T3D_OS_IOS)
    #include <dlfcn.h>
#endif


namespace Tiny3D
{
    namespace
    {
        /**
         * @brief   判断对象的类型信息是否还在地址空间里
         * @remarks 多态对象的 vtable 和它后面挂的 RTTI 都属于定义该类的那个模块。
         *          插件 DLL 被 FreeLibrary 之后这段地址就解除映射了，此时对
         *          该对象求 typeid 等于去读一段没有映射的内存，必然访问违例。
         *          这里用 vptr 反查模块，查不到就说明定义它的模块已经卸载了.
         */
        bool isTypeInfoStillMapped(const Object *object)
        {
            if (object == nullptr)
            {
                return false;
            }

            // 多态对象的首个字段就是 vptr
            const void *vptr = *reinterpret_cast<const void * const *>(object);
            if (vptr == nullptr)
            {
                return false;
            }

#if defined (T3D_OS_WINDOWS)
            HMODULE module = nullptr;
            return GetModuleHandleExA(
                GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
                    | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                reinterpret_cast<LPCSTR>(vptr), &module) != FALSE;
#elif defined (T3D_OS_LINUX) || defined (T3D_OS_OSX) \
    || defined (T3D_OS_ANDROID) || defined (T3D_OS_IOS)
            Dl_info info {};
            return dladdr(const_cast<void*>(vptr), &info) != 0
                && info.dli_fname != nullptr;
#else
            return true;
#endif
        }
    }

    ObjectTracer::ObjectTracer(bool enabled /* = false */)
        : mIsEnabled(enabled)
        , mStream(nullptr)
    {

    }

    ObjectTracer::~ObjectTracer()
    {

    }

    void ObjectTracer::dumpMemoryInfo() const
    {
        if (mIsEnabled)
        {
            printInfo("Dump memory leak =================================>\n");

            std::stringstream ss;

            int32_t i = 0;

            for (auto itr = mObjects.begin(); itr != mObjects.end(); ++itr)
            {
                Object *obj = *itr;

                ss.str("");
                ss << "Leak Object #" << i << ": " << getTypeName(obj) << ", Address : 0x" << obj << ", ReferCount : " << obj->referCount() << "\n";
                printInfo(ss.str());
                i++;
            }

            ss.str("");

            ss << "Total leak objects " << mObjects.size() << "\n";
            printInfo(ss.str());
        }
    }

    //--------------------------------------------------------------------------

    void ObjectTracer::captureTypeNames()
    {
        if (!mIsEnabled)
        {
            return;
        }

        ScopeLock lock(&mMutex);

        for (auto itr = mObjects.begin(); itr != mObjects.end(); ++itr)
        {
            Object *obj = *itr;
            if (!isTypeInfoStillMapped(obj))
            {
                continue;
            }

            mTypeNames[obj] = typeid(*obj).name();
        }
    }

    //--------------------------------------------------------------------------

    String ObjectTracer::getTypeName(const Object *object) const
    {
        auto itr = mTypeNames.find(const_cast<Object*>(object));
        if (itr != mTypeNames.end())
        {
            return itr->second;
        }

        if (isTypeInfoStillMapped(object))
        {
            return typeid(*object).name();
        }

        // 类型名在这里已经拿不到了：定义它的模块（通常是插件 DLL）已经卸载。
        // 想看到真实类型名，就要在卸载之前调用 captureTypeNames()
        return "<unknown type, defining module already unloaded>";
    }

    void ObjectTracer::dumpMemoryInfo(FileDataStream &fs) const
    {
        mStream = &fs;
        dumpMemoryInfo();
        mStream = nullptr;
    }

    void ObjectTracer::printInfo(const String &str) const
    {
        if (mStream != nullptr)
        {
            DataStream &stream = *mStream;
            stream << str;
        }
        else
        {
            T3D_CONSOLE.print(str.c_str());
        }
    }

    T3D_INSTANTIATE_SINGLETON(ObjectTracer)
}

