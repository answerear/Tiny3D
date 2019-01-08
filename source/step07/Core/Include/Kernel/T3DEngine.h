/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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
#include "T3DTypeDef.h"
#include "DataStruct/T3DVariant.h"


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
         * @brief 初始化引擎
         * @param [in] appPath : 应用程序路径
         * @param [in] config : 配置文件
         * @remarks 引擎的一切应用都要在调用本接口之后才有效。
         */
        TResult init(const String &appPath, const String &config = "Tiny3D.cfg");

        /**
         * @brief 运行引擎
         */
        bool run();

        /**
         * @brief 渲染一帧
         */
        void renderOneFrame();

        /**
         * @brief 程序进入后台调用本接口告知引擎
         */
        void appDidEnterBackground();

        /**
         * @brief 程序回到前台调用本接口告知引擎
         */
        void appWillEnterForeground();

        /**
         * @brief 获取应用程序路径，不包含程序名称
         */
        const String &getAppPath() const { return mAppPath;  }

        /**
         * @brief 获取应用程序名称，不包含路径
         */
        const String &getAppName() const { return mAppName; }

        /**
         * @brief 获取插件路径
         */
        const String &getPluginsPath() const { return mPluginsPath; }

    protected:
        /**
         * @brief 初始化应用程序
         */
        TResult initApplication();

        /** 
         * @brief 初始化日志系统 
         */
        TResult initLogSystem();

        /**
         * @brief 初始化事件系统
         */
        TResult initEventSystem();

        /**
         * @brief 初始化档案结构系统
         */
        TResult initArchives();

        /**
         * @brief 加载配置文件
         * @param [in] cfgPath : 配置文件名
         * @return 调用成功返回 T3D_ERR_OK
         */
        TResult loadConfig(const String &cfgPath);

        /**
         * @brief 创建渲染窗口
         */
        TResult createRenderWindow(const String &title);

    protected:
        Logger              *mLogger;           /**< 日志对象 */
        EventManager        *mEventMgr;         /**< 事件管理器对象 */

        Window              *mWindow;           /**< 窗口 */
        bool                mIsRunning;         /**< 引擎是否在运行中 */

        ArchiveManagerPtr   mArchiveMgr;        /**< 档案管理对象 */

        String              mAppPath;           /**< 程序路径 */
        String              mAppName;           /**< 程序名称 */
        String              mPluginsPath;       /**< 插件路径 */

        Settings            mSettings;          /**< 引擎配置项 */
    };

    #define T3D_ENGINE      (Engine::getInstance())
}


#endif  /*__T3D_APP_EVENT_LISTENER_H__*/
