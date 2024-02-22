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


namespace Tiny3D
{
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

            for (auto itr = mObjects.begin(); itr != mObjects.end(); ++itr)
            {
                Object *obj = *itr;

                ss.str("");
                ss << "Leak Object : " << typeid(*obj).name() << " ReferCount : " << obj->referCount() << "\n";                
                printInfo(ss.str());
            }

            ss.str("");

            ss << "Total leak objects " << mObjects.size();
        }
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
}

