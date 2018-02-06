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
	        virtual int32_t processEvent(uint32_t evid, EventParam *param, TINSTANCE sender) override; 

    // 声明事件处理函数
    #define T3D_DECLARE_EVENT_HANDLE(func)	\
            int32_t func(EventParam *param, TINSTANCE sender);

    #define T3D_DECLARE_EVENT_FILTER()	\
	    protected:	\
		    int32_t setupEventFilter();

    // 开始实现事件处理函数
    #define T3D_BEGIN_EVENT_MAP(theClass, classBase) \
	    int32_t theClass::processEvent(uint32_t evid, EventParam *param, TINSTANCE sender) \
	    { \
		    int32_t ret = T3D_ERR_OK;   \
            typedef int32_t (EventHandler::*Func)(uint32_t, EventParam*, TINSTANCE);  \
            Func baseFunc = static_cast<Func>(&classBase::processEvent); 

    // 事件处理函数响应调用
    #define T3D_ON_EVENT(eid, func)	\
		    if (eid == evid)	\
		    {	\
			    ret = func(param, sender);	\
		    }

    // 结束事件处理函数
    #define T3D_END_EVENT_MAP()	\
            if (ret != T3D_ERR_OK)   \
            {   \
                ret = (this->*baseFunc)(evid, param, sender); \
            }   \
            return ret; \
        }


    // 开始注册事件过滤
    #define T3D_BEGIN_EVENT_FILTER(theClass, classBase)	\
	    int32_t theClass::setupEventFilter()	\
	    {	\
		    classBase::setupEventFilter();	\
            int32_t ret = T3D_ERR_OK;   \
            do  \
            {   

    // 注册事件过滤
    #define T3D_EVENT_FILTER(eid)	\
		        ret = registerEvent(eid);   \
                if (ret != T3D_ERR_OK)  \
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
