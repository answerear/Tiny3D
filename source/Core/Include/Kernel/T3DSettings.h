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

#ifndef __T3D_SETTINGS_H__
#define __T3D_SETTINGS_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    /**
     * \brief 渲染窗口与后端相关配置（由 Agent 读入后用于建窗 / 选渲染器）
     */
    TSTRUCT()
    struct RenderSettings
    {
        TPROPERTY()
        bool    fullscreen = false;   ///< 是否全屏

        TPROPERTY()
        bool    resizable = false;    ///< 窗口是否可调整大小

        TPROPERTY()
        int32_t width = 1280;         ///< 窗口宽度（像素）

        TPROPERTY()
        int32_t height = 720;         ///< 窗口高度（像素）

        TPROPERTY()
        int32_t x = 100;              ///< 窗口左上角 X

        TPROPERTY()
        int32_t y = 100;              ///< 窗口左上角 Y

        TPROPERTY()
        int32_t colorDepth = 32;      ///< 色深（位）

        TPROPERTY()
        String  title = "Tiny3D Sample";  ///< 窗口标题（建窗时会再拼版本信息）

        TPROPERTY()
        int32_t MSAA = 16;            ///< MSAA 采样数配置项（建窗实现中当前硬编码 Count=4）

        TPROPERTY()
        bool    vsync = false;        ///< 是否垂直同步

        TPROPERTY()
        String  renderer = "Direct3D11";  ///< 活动 RHI 渲染器名称，须已通过插件注册

        TPROPERTY()
        String  iconPath = "";        ///< 窗口图标路径，空表示未指定
    };

    /**
     * \brief 日志系统配置
     */
    TSTRUCT()
    struct LogSettings
    {
        TPROPERTY()
        int32_t appID = 1000;         ///< 应用 ID

        TPROPERTY()
        String  tag = "Tiny3D";       ///< 日志标签

        TPROPERTY()
        String level = "DEBUG";       ///< 日志级别字符串

        TPROPERTY()
        int32_t expired = 7;          ///< 日志过期天数

        TPROPERTY()
        int32_t cacheSize = 50;       ///< 日志缓存条数

        TPROPERTY()
        int32_t cacheTime = 10000;    ///< 日志缓存时间（ms）
    };

    /**
     * \brief 插件加载配置
     */
    TSTRUCT()
    struct PluginSettings
    {
        TPROPERTY()
        String  pluginPath = ".";     ///< 相对应用路径的插件目录

        TPROPERTY()
        TList<String> plugins;        ///< 启动时按序 loadPlugin 的名称列表
    };

    /// 组件更新顺序表：类名列表，下标越小越先更新
    using ComponentUpdateOrders = TList<String>;

    /**
     * \brief 组件更新顺序配置；构造时写入默认顺序
     * \remarks 默认顺序：Transform3D → Camera → Geometry → Behaviour。
     *          未列出的类型进入无序更新队列；Behaviour 派生类落在 Behaviour 段位。
     */
    TSTRUCT()
    struct ComponentSettings
    {
        TPROPERTY()
        ComponentUpdateOrders updateOrders;

        ComponentSettings()
        {
            updateOrders.emplace_back("Transform3D");
            updateOrders.emplace_back("Camera");
            updateOrders.emplace_back("Geometry");
            // 脚本组件段位：让继承自 Behaviour 的脚本具有确定的更新次序，
            // 而不是落入无序更新队列（见 Behaviour 设计文档 §4.4）
            updateOrders.emplace_back("Behaviour");
        }
    };

    /**
     * \brief 全局时间配置，由 Agent::initTime 注入 Time 单例
     */
    TSTRUCT()
    struct TimeSettings
    {
        /// 固定步长 (ms)，默认 20ms = 50Hz，供 Behaviour::onFixedUpdate
        TPROPERTY()
        uint64_t fixedDeltaTimeMS = 20;

        /// 单帧真实 dt 上限 (ms)，用于断点 / 切回前台等墙钟跳变钳制
        TPROPERTY()
        uint64_t maximumDeltaTimeMS = 333;

        /// 初始时间缩放千分比，1000 = 1.0x
        TPROPERTY()
        uint32_t timeScalePermille = 1000;
    };

    /**
     * \brief 引擎总配置聚合体（配置文件反序列化目标 / init 入参）
     */
    TSTRUCT()
    struct T3D_ENGINE_API Settings
    {
        TPROPERTY()
        RenderSettings  renderSettings {};

        TPROPERTY()
        LogSettings     logSettings {};

        TPROPERTY()
        PluginSettings  pluginSettings {};

        TPROPERTY()
        ComponentSettings   componentSettins {};

        TPROPERTY()
        TimeSettings        timeSettings {};
    };
}


#endif    /*__T3D_SETTINGS_H__*/
