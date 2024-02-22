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


#ifndef __T3D_OBJECT_TRACER_H__
#define __T3D_OBJECT_TRACER_H__


#include "T3DSystemPrerequisites.h"
#include "Object/T3DObject.h"


namespace Tiny3D
{
    /**
     * @class   ObjectTracer
     * @brief   一个跟踪内存的类，能够跟踪所有Object其派生类内存泄漏的情况
     * @remarks  这里的跟踪当对象数量很大的时候会有一定的性能损耗， 
     *           建议这里在定位完内存问题就关闭掉跟踪.
     */
    class T3D_SYSTEM_API ObjectTracer : public Singleton<ObjectTracer>
    {
        friend class Agent;
        friend class Object;

    public:
        /**
         * @fn  ObjectTracer::ObjectTracer(bool enabled = false);
         * @brief   默认构造函数
         * @param [in]  enabled (Optional) : 是否开启内存跟踪功能，默认是关闭的.
         */
        ObjectTracer(bool enabled = false);

        /**
         * @fn  virtual ObjectTracer::~ObjectTracer();
         * @brief   析构函数
         */
        virtual ~ObjectTracer();

        /**
         * @fn  bool ObjectTracer::isTracingEnabled() const
         * @brief   获取内存跟踪是否打开
         * @return  返回内存跟踪器工作状态.
         */
        bool isTracingEnabled() const { return mIsEnabled; }

        /**
         * @fn  void ObjectTracer::dumpMemoryInfo() const;
         * @brief   输出当前内存信息到对应平台Console
         * @note 该输出会输出到对应平台的console上，详细平台如下：
         *  - 在windows上，这个会输出到console上，并且会输出到vs的Output窗口里
         *  - 在iOS和Mac OS X上，这个会输出到xcode的Console窗口里
         *  - 在Android上，这个会输出到logcat里.
         */
        void dumpMemoryInfo() const;

        /**
         * @fn  void ObjectTracer::dumpMemoryInfo(FileDataStream &fs) const;
         * @brief   输出当前内存信息到文件流里
         * @param [in]  fs  : 文件流.
         */
        void dumpMemoryInfo(FileDataStream &fs) const;

    protected:
        /**
         * @fn  void ObjectTracer::addObject(Object *object)
         * @brief   添加一个对象到内存跟踪器里
         * @param [in,out]  object  If non-null, the object.
         */
        void addObject(Object *object)
        {
            ScopeLock lock(&mMutex);
            if (mIsEnabled)
                mObjects.insert(object);
        }

        /**
         * @fn  void ObjectTracer::removeObject(Object *object)
         * @brief   从内存跟踪器里移除一个对象
         * @param [in,out]  object  If non-null, the object.
         */
        void removeObject(Object *object)
        {
            ScopeLock lock(&mMutex);
            if (mIsEnabled)
                mObjects.erase(object);
        }

        /**
         * @fn  void ObjectTracer::printInfo(const String &str) const;
         * @brief   输出信息
         * @param [in]  str : 信息字符串.
         */
        void printInfo(const String &str) const;

        typedef TSet<Object*>           Objects;
        typedef Objects::iterator       ObjectsItr;
        typedef Objects::const_iterator ObjectsConstItr;
        typedef Objects::value_type     ObjectsValue;

        Mutex   mMutex {};
        bool    mIsEnabled {true}; /**< 是否开启了内存跟踪 */
        Objects mObjects {};    /**< 对象集合 */

        mutable FileDataStream  *mStream;       /**< 临时输出对象，用于dumpMemoryInfo的时候 */
    };
}


#endif  /*__T3D_OBJECT_TRACER_H__*/
