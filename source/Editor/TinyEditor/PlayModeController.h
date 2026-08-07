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

#pragma once


#include "EditorPrerequisites.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)

    /**
     * @brief Play / Stop 与业务插件加载的协调者
     * @remarks
     *   业务 DLL 在打开工程时就加载，而不是等到点 Play。这一点和 Unity 一致，原因
     *   在于反序列化：场景里挂着的自定义 Behaviour，如果加载场景时类型还没注册，
     *   会被静默丢弃（见 T3DRttrArchive），用户再随手保存一次场景，脚本挂载信息就
     *   永久没了。所以类型必须在场景之前就位。
     *
     *   而「编译时 DLL 被占用」这个真实痛点由影子拷贝解决，不靠推迟加载来回避。
     */
    class PlayModeController
        : public EventHandler
        , public Allocator
        , public Singleton<PlayModeController>
    {
    public:
        PlayModeController() = default;

        ~PlayModeController() override = default;

        /**
         * @brief 工程打开后、加载场景之前调用
         * @remarks 顺序不能颠倒：先让业务类型注册进 RTTR，场景里的自定义组件才认得出来
         */
        TResult onProjectOpened();

        /// 工程关闭前调用，卸载业务插件并清理影子副本
        void onProjectClosing();

        /// 当前是否在 Play 态
        bool isPlaying() const { return T3D_AGENT.isPlaying(); }

        /// 是否具备进入 Play 的条件（工程已打开且有运行时场景）
        bool canPlay() const;

        /**
         * @brief 进入 Play 态
         * @return 成功返回 T3D_OK；编译失败时不进入 Play 并返回错误码
         * @remarks 先编译业务代码，产物有更新则热重载，再进 Play。场景在进入前会被
         *          保存，Stop 时从磁盘还原，因此 Play 期间对场景的改动不会留下来
         */
        TResult play();

        /// 退出 Play 态并还原场景
        TResult stop();

        /**
         * @brief 只编译 Runtime 变体，不做打包
         * @remarks Runtime 变体平时不参与构建，误用编辑器专有 API 要到导出时才暴露。
         *          这个入口让用户随时确认代码仍然是可发布的
         */
        TResult validateRuntimeBuild();

        /// 业务插件当前是否已加载
        bool isGamePluginLoaded() const { return !mLoadedShadowName.empty(); }

    protected:
        /// 影子拷贝 + loadPluginFromPath
        TResult loadGamePlugin();

        /// 卸载业务插件，并删掉刚解锁的那份影子副本
        TResult unloadGamePlugin();

        /**
         * @brief 热重载业务插件
         * @remarks 严格按序：销毁场景中所有对象 → 卸载旧 DLL（RTTR 自动注销旧类型）
         *          → 加载新 DLL → 重新加载场景。任何一个指向旧 DLL 内对象的残留引用
         *          都会在 FreeLibrary 之后变成悬垂指针，所以销毁必须彻底
         */
        TResult reloadGamePlugin();

        /// 场景有改动则先落盘，Stop 时要靠磁盘上的这份还原
        TResult saveSceneIfDirty();

        /// 卸载当前运行时场景，从磁盘重新加载同一个场景并通知各视图重建
        TResult reloadScene();

        /// 清掉编辑器侧对场景对象的引用，避免卸载后变成悬垂指针
        void clearEditorReferences();

    protected:
        /// 当前已加载的影子副本逻辑名，为空表示业务插件未加载
        String mLoadedShadowName {};
        /// 已加载影子副本对应的产物修改时间，用来判断产物是否有更新
        long_t mLoadedAssemblyTime {0};
    };

    #define PLAY_MODE_CTRL (PlayModeController::getInstance())

    NS_END
}
