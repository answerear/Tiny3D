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


#ifndef __T3D_ENGINE_H__
#define __T3D_ENGINE_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    /**
     * @brief 引擎入口类
     */
    class T3D_ENGINE_API Engine : public Singleton<Engine>
    {
        T3D_DISABLE_COPY(Engine);

    public:
        /**
         * @brief 构造函数
         */
        Engine();

        /**
         * @brief 析构函数
         */
        virtual ~Engine();

        /**
         * @brief 启动引擎
         * @remarks 引擎的一切应用都要在调用本接口之后才有效。
         */
        bool startup(/*Window *window = nullptr*/);

        /**
         * @brief 运行引擎
         */
        bool run();

        /**
         * @brief 渲染一帧
         */
        void renderOneFrame();

//     protected:
//         /**
//         * @brief 窗口大小改变通知
//         * @param [in] w : 新的窗口宽度
//         * @param [in] h : 新的窗口高度
//         * @return void
//         */
//         virtual void windowResized(int32_t w, int32_t h) override;
// 
//         /**
//         * @brief 窗口移动通知
//         * @param [in] x : 新的窗口位置
//         * @param [in] y : 新的窗口位置
//         */
//         virtual void windowMoved(int32_t x, int32_t y) override;
// 
//         /**
//         * @brief 窗口事件循环通知
//         */
//         virtual void windowRender() override;
// 
//         /**
//         * @brief
//         */
//         virtual void windowClosed() override;

    protected:
        EventManager        *mEventMgr;             /// 事件管理器对象

        Window              *mWindow;               /// 窗口
        bool                mIsRunning;             /// 引擎是否在运行中
    };

    #define T3D_ENGINE      (Engine::getInstance())
}


#endif  /*__T3D_APP_EVENT_LISTENER_H__*/
