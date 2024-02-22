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

#ifndef __T3D_EVENT_MACRO_H__
#define __T3D_EVENT_MACRO_H__


#include "T3DEventPrerequisites.h"


namespace Tiny3D
{
    #define T3D_INVALID_INSTANCE        EventManager::INVALID_INSTANCE
    #define T3D_BROADCAST_INSTANCE      EventManager::BROADCAST_INSTANCE
    #define T3D_MULTICAST_INSTANCE      EventManager::MULTICAST_INSTANCE

    // 声明事件MAP
    #define T3D_DECLARE_EVENT_MAP() \
        public: \
            TResult eventProc(EventID evid, EventParam *param, TINSTANCE sender);  \
        protected:  \
	        virtual TResult processEvent(EventID evid, EventParam *param, TINSTANCE sender) override; 

    // 声明事件处理函数
    #define T3D_DECLARE_EVENT_HANDLE(func)	\
            TResult func(EventParam *param, TINSTANCE sender);

    #define T3D_DECLARE_EVENT_FILTER()	\
	    protected:	\
		    TResult setupEventFilter();

    // 开始实现事件处理函数
    #define T3D_BEGIN_EVENT_MAP(theClass, classBase) \
	    TResult theClass::processEvent(EventID evid, EventParam *param, TINSTANCE sender) \
	    { \
		    TResult ret = eventProc(evid, param, sender);   \
            if (ret == T3D_ERR_FWK_NONE_HANDLER)    \
            {   \
                ret = classBase::processEvent(evid, param, sender); \
            }   \
            return ret; \
        }   \
        TResult theClass::eventProc(EventID evid, EventParam *param, TINSTANCE sender) \
        {   \
            TResult ret = T3D_ERR_FWK_NONE_HANDLER; \

    // 事件处理函数响应调用
    #define T3D_ON_EVENT(eid, func)	\
		    if (eid == evid)	\
		    {	\
			    ret = func(param, sender);	\
		    }

    // 结束事件处理函数
    #define T3D_END_EVENT_MAP()	\
            return ret; \
        }


    // 开始注册事件过滤
    #define T3D_BEGIN_EVENT_FILTER(theClass, classBase)	\
	    TResult theClass::setupEventFilter()	\
	    {	\
		    classBase::setupEventFilter();	\
            TResult ret = T3D_OK;   \
            do  \
            {   

    // 注册事件过滤
    #define T3D_EVENT_FILTER(eid)	\
		        ret = registerEvent(eid);   \
                if (T3D_FAILED(ret))  \
                    break;

    // 结束事件过滤
    #define T3D_END_EVENT_FILTER()	\
            } while (0);    \
            return ret; \
	    }

    // 建立事件过滤
    #define T3D_SETUP_EVENT_FILTER()	\
	    setupEventFilter();
}


#endif  /*__T3D_EVENT_MACRO_H__*/
