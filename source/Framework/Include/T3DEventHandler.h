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

#ifndef __T3D_EVENT_HANDLER_H__
#define __T3D_EVENT_HANDLER_H__


#include "T3DEventPrerequisites.h"


namespace Tiny3D
{
    using EventHandle = TFunction<bool(EventParam*, TINSTANCE sender)>;
    
    class T3D_FRAMEWORK_API EventHandler
    {
        friend class EventManager;

    public:
        /**
         * @brief Constructor
         * @param [in] canAutoRegister : 是否自动注册启动事件收发功能
         */
        EventHandler(bool canAutoRegister = true);

        /**
         * @brief Destructor
         */
        virtual ~EventHandler();

        /**
         * @brief 发送同步事件给指定对象
         */
        TResult sendEvent(EventID evid, EventParam *param, TINSTANCE receiver);

        /**
         * @brief 发送异步事件给指定对象
         */
        TResult postEvent(EventID evid, EventParam *param, TINSTANCE receiver);

        /**
         * @brief 发送同步事件给关注事件的对象
         */
        TResult sendEvent(EventID evid, EventParam *param);

        /**
         * @brief 发送异步事件给关注事件的对象
         */
        TResult postEvent(EventID evid, EventParam *param);

        /**
         * @brief 获取实例句柄
         */
        TINSTANCE instance() const { return mInstance; }

        /**
        * @brief 统一处理事件函数
        * @param [in] evid : 事件ID
        * @param [in] param : 事件附加参数对象
        * @param [in] sender : 事件发送实例句柄
        * @return 有处理事件返回true，否则返回false。
        */
        TResult processEvent(EventID evid, EventParam *param, TINSTANCE sender);

    protected:
        /**
         * @brief 注册对象，返回实例句柄，只有注册才能有效收发事件
         */
        TINSTANCE registerHandler();

        /**
         * @brief 反注册对象，调用后无法再继续收发事件
         */
        TResult unregisterHandler();

        /**
         * @brief 建立事件过滤器，只有在过滤器里面的才会收到不定对象的事件通知
         */
        TResult setupEventFilter();

        /**
         * @brief 注册关注的事件到过滤器里面
         * @param [in] evid : 事件ID 
         */
        TResult registerEvent(EventID evid, const EventHandle &handle);

        /**
         * @brief 反注册关注的事件，反注册后，过滤器里面没有该事件
         * @param [in] evid : 事件ID
         */
        TResult unregisterEvent(EventID evid);

        /**
         * @brief 反注册所有关注的事件
         */
        void unregisterAllEvent();

    private:
        using EventMap = TMap<uint32_t, EventHandle>;
        
        /// 实例句柄
        TINSTANCE   mInstance;
        /// 事件响应函数列表
        EventMap    mEventMap;
    };
}


#endif  /*__T3D_EVENT_HANDLER_H__*/
