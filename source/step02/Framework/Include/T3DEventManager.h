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

#ifndef __T3D_EVENT_MANAGER_H__
#define __T3D_EVENT_MANAGER_H__


#include "T3DEventPrerequisites.h"
#include "T3DEventInstance.h"


namespace Tiny3D
{
    const int32_t T3D_MAX_EVENT_QUEUE = 2;	/// 最大事件队列数

    class T3D_FRAMEWORK_API EventManager : public Singleton<EventManager>
    {
        friend class EventHandler;

        T3D_DISABLE_COPY(EventManager);

        /** 事件项 */
        struct EventItem
        {
            EventItem(uint32_t evid, EventParam *pEventParam, TINSTANCE receiver, TINSTANCE sender)
                : mEventID(evid)
                , mEventParam(pEventParam)
                , mReceiver(receiver)
                , mSender(sender)
            {}

            uint32_t        mEventID;       /// 事件ID
            EventParam      *mEventParam;   /// 事件参数对象
            TINSTANCE       mReceiver;      /// 事件接收者实例句柄
            TINSTANCE       mSender;        /// 事件发送者实例句柄
        };

    public:
        /** 
         * @brief Constructor 
         * @param [in] maxEvents : 所有事件的数量
         * @param [in] maxHandlingDuration : 处理事件的最长事件
         * @param [in] maxCallStacks : 能嵌套处理事件调用栈的最大深度
         * @see
         *  - getMaxHandlingDuration()
         *  - getMaxCallStackLevel()
         */
        EventManager(uint32_t maxEvents, int32_t maxHandlingDuration,
            int32_t maxCallStacks);

        /** Destructor */
        virtual ~EventManager();

        /**
         * @brief 发送同步事件，事件处理函数处理返回前，sendEvent是不会返回的。
         * @param [in] evid : 事件ID
         * @param [in] param : 事件附加参数
         * @param [in] receiver : 事件接收者实例。
         * @param [in] sender : 事件发送者实例
         * @return 
         *  - 当receiver是固定实例时，有事件接收者处理则返回true，
         *  否则返回false表示事件ID错误、事件接收者错误或者接收者没处理事件。
         *  - 当receiver是T3D_INVALID_INSTANCE时，有事件接收者处理则返回true，
         *  否则返回false表示事件ID错误、事件接收者错误或者接收者没处理事件。
         *  - 当receiver是T3D_BROADCAST_INSTANCE时，如果所有对象都被触发到并且
         *  处理完则返回true，如果中间因为某个事件处理函数太长事件导致后续事件
         *  无法处理，则返回false表示有事件未处理完。
         *  - 当嵌套调用sendEvent多次后，导致调用栈过深时，也会返回false。 调用栈
         *  层次可以通过getMaxCallStackLevel()获得。
         * @see
         *  - getMaxCallStackLevel()
         */
        bool sendEvent(uint32_t evid, EventParam *param, 
            TINSTANCE receiver, TINSTANCE sender);

        /**
         * @brief 发送异步事件，事件处理函数会再后面统一派发，这里不会触发。
         * @param [in] evid : 事件ID
         * @param [in] param : 事件附加参数
         * @param [in] receiver : 事件接收者实例
         * @param [in] sender : 事件发送者实例
         * @return 只要事件ID没错误，都返回true，否则返回false。
         * @note 需要注意一点就是调用返回true也不代表接收者一定能接收到。
         *      因为这个接口是把所有事件存放到事件队列，然后在dispatch()调用的
         *      地方统一派发，但是如果事件队列事件太多导致处理事件太长，或者
         *      某一个事件影响处理事件太长影响到整个队列处理时间，则后续的事件
         *      会被丢弃。详细的派发事件整体时间可以通过调用
         *      getMaxHandlingDuration()接口查询获得。
         * @see
         *  - getMaxHandlingDuration()
         */
        bool postEvent(uint32_t evid, EventParam *param, 
            TINSTANCE receiver, TINSTANCE sender);

        /**
         * @brief 通过传入instance实例句柄获取回对象指针。
         * @param [in] instance : 对象实例句柄
         * @param [in][out] handler : 返回的对象指针
         * @return instance是有效合法句柄返回true，否则返回false。
         */
        bool getEventHandler(TINSTANCE instance, EventHandler *&handler);

        /**
         * @brief 判断传入的事件对象指针是否有效收发事件实例。
         * @param [in] handler : 事件处理对象指针
         * @return 有效收发事件实例则返回true，否则返回false。
         */
        bool isValidHandler(EventHandler *handler);

        /**
         * @brief 派发事件，用于派发事件队列里面的事件
         * @return 当事件全部派发完，则返回true，如果事件处理事件过长，超过
         *  getMaxHandlingDuration()返回值时，则会中断处理返回false。 剩余没有
         *  来得及处理的事件会在下一次调用dispatchEvent时候优先派发。 同样的，
         *  如果调用栈太深，也会中断处理返回false。 调用栈层次可以通过
         *  getMaxCallStackLevel()获得。
         * @see
         *  - getMaxHandlingDuration()
         *  - getMaxCallStackLevel()
         */
        bool dispatchEvent();

        /**
         * @brief 暂停事件派发
         * @note 在暂停派发期间，任何sendEvent和postEvent调用都会让所有事件排队，
         *  即不管sendEvent还是postEvent都会被当成postEvent处理。 
         * @see
         *  - resumeDispatching();
         */
        void pauseDispatching();

        /**
         * @brief 继续事件派发
         * @param [in] dispatchImmdiately : 是否马上派发队列里所有事件
         * @note 当dispatchImmdiately为true的时候，会马上派发事件，但是如果事件
         *  处理事件太长，超过了getMaxHandlingDuration()返回值，则会直接中断派发
         *  并且返回false。 同样的，如果嵌套事件处理调用栈超过了
         *  getMaxCallStackLevel()返回值，也会中断处理返回
         * @see
         *  - pauseDispatching()
         *  - getMaxHandlingDuration()
         *  - getMaxCallStackLevel()
         */
        bool resumeDispatching(bool dispatchImmdiately);

        /**
         * @brief 事件派发是否被暂停
         */
        bool isPaused();

        /**
         * @brief 获取当前处理事件时间的限制。
         */
        int32_t getMaxHandlingDuration();

        /**
         * @brief 获取当前处理事件嵌套的调用栈层次深度。
         */
        int32_t getMaxCallStackLevel();

    protected:
        /**
         * @brief 注册事件处理对象
         * @param [in] handler : 需要收发事件处理对象
         * @return 注册成功返回事件实例句柄，否则返回T3D_INVALID_INSTANCE。
         * @note 只有调用了本接口才能收发事件。
         * @see
         *  - unregisterHandler()
         */
        TINSTANCE registerHandler(EventHandler *handler);

        /**
         * @brief 反注册事件处理对象
         * @param [in] instance : 实例句柄
         * @return 实例句柄有效合法时并调用成功返回true，否则返回false。
         */
        bool unregisterHandler(TINSTANCE instance);

        /**
        * @brief 注册事件
        * @note 注册事件后，可以只接收到关注的事件，不关注的事件无法接收到
        * @param [in] evid : 事件ID
        * @param [in] instance : 关注该事件ID的实例句柄
        * @return 函数调用成功返回true，否则返回false.
        */
        bool registerEvent(uint32_t evid, TINSTANCE instance);

        /**
        * @brief 反注册事件
        * @note 反注册事件后，事件处理对象无法收到该事件
        * @param [in] evid : 事件ID
        * @param [in] instance : 关注该事件ID的处理对象
        * @return 函数调用成功返回true，否则返回false.
        */
        bool unregisterEvent(uint32_t evid, TINSTANCE instance);

        /**
        * @brief 获取当前待处理事件队列的第一个事件.
        * @param [in] Item : 事件项
        * @param [in] bRemovable : 是否移除该事件，默认为移除该事件.
        * @return 调用成功返回true，否则返回false.
        */
        bool peekEvent(EventItem &Item, bool bRemovable = true);

        /**
        * @brief 清除所有事件队列中的事件.
        * @return void
        */
        void clearEventQueue();

    private:
        typedef std::vector<EventHandler*>  HandlerList;
        typedef HandlerList::iterator       HandlerListItr;
        typedef HandlerList::const_iterator HandlerListConstItr;

        typedef std::list<EventItem>        EventList;
        typedef EventList::iterator         EventListItr;
        typedef EventList::const_iterator   EventListConstItr;

        typedef std::set<TINSTANCE>         EventInstSet;
        typedef EventInstSet::iterator      EventInstSetItr;
        typedef EventInstSet::value_type	EventInstSetValue;

        typedef std::vector<EventInstSet>   EventFilterList;
        typedef EventFilterList::iterator   EventFilterListItr;

        HandlerList	m_EventHandlers;                    /// 事件处理对象链表
        EventList   m_EventQueue[T3D_MAX_EVENT_QUEUE];  /// 待处理事件队列

        EventFilterList	m_EventFilters;                 /// 事件过滤表

        int32_t			m_nCurrentQueue;                /// 当前待处理事件队列

        EventList		m_EventCache;                   /// 暂存事件缓存
        bool			m_bDispatchPaused;              /// 暂停派发事件标识
    };

    #define T3D_EVENT_MGR   (EventManager::getInstance())
}


#endif  /*__T3D_EVENT_MANAGER_H__*/
