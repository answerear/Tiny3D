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


#include "PlayModeController.h"

#include "EditorEventDefine.h"
#include "EditorSceneImpl.h"
#include "ProjectManager.h"
#include "ScriptBuildSystem.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)

    //--------------------------------------------------------------------------

    TResult PlayModeController::onProjectOpened()
    {
        const ProjectSettings &settings = PROJECT_MGR.getProjectSettings();

        const String pluginName = settings.GamePluginName.empty()
            ? PROJECT_MGR.getProjectName() : settings.GamePluginName;

        SCRIPT_BUILD_SYS.attachProject(PROJECT_MGR.getProjectPath(), pluginName,
            settings.ScriptsRelativePath);

        if (!SCRIPT_BUILD_SYS.hasScripts())
        {
            // 支持业务插件之前建的工程没有 Scripts 目录，纯资源工程照常使用
            EDITOR_LOG_INFO("Project has no game plugin scripts, skipping script build.");
            return T3D_OK;
        }

        // 上次会话残留的影子副本，此刻一定已经解锁，全部清掉
        SCRIPT_BUILD_SYS.cleanShadowAssemblies("");

        // 产物过期就先编一次。编不过也不该拦住工程打开——用户很可能正是要进来
        // 改代码把它修好的，只是这种情况下场景里的自定义组件会缺失。
        if (SCRIPT_BUILD_SYS.needsBuild(ScriptBuildSystem::Variant::kEditor))
        {
            String output;
            if (T3D_FAILED(SCRIPT_BUILD_SYS.build(ScriptBuildSystem::Variant::kEditor, output)))
            {
                EDITOR_LOG_ERROR("Failed to build game plugin while opening the project. "
                    "Custom components in scenes will not be recognized until it builds.");
                return T3D_OK;
            }
        }

        return loadGamePlugin();
    }

    //--------------------------------------------------------------------------

    void PlayModeController::onProjectClosing()
    {
        if (isPlaying())
        {
            T3D_AGENT.exitPlayMode();
        }

        unloadGamePlugin();

        // 卸载后所有影子副本都已解锁，扫一遍清空
        SCRIPT_BUILD_SYS.cleanShadowAssemblies("");
        SCRIPT_BUILD_SYS.detachProject();
    }

    //--------------------------------------------------------------------------

    bool PlayModeController::canPlay() const
    {
        if (!PROJECT_MGR.isProjectOpened())
        {
            return false;
        }

        return EditorScene::getInstancePtr() != nullptr
            && EDITOR_SCENE.getRuntimeScene() != nullptr;
    }

    //--------------------------------------------------------------------------

    TResult PlayModeController::loadGamePlugin()
    {
        if (isGamePluginLoaded())
        {
            return T3D_OK;
        }

        if (!SCRIPT_BUILD_SYS.hasScripts())
        {
            return T3D_OK;
        }

        const String assembly =
            SCRIPT_BUILD_SYS.getAssemblyPath(ScriptBuildSystem::Variant::kEditor);

        if (!Dir::exists(assembly))
        {
            EDITOR_LOG_WARNING("Game plugin assembly [%s] not found, nothing to load.",
                assembly.c_str());
            return T3D_ERR_FILE_NOT_EXIST;
        }

        String shadowDir;
        String shadowName;

        TResult ret = SCRIPT_BUILD_SYS.shadowCopy(shadowDir, shadowName);
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        ret = T3D_AGENT.loadPluginFromPath(shadowName, shadowDir);
        if (T3D_FAILED(ret))
        {
            EDITOR_LOG_ERROR("Failed to load game plugin [%s] from [%s] !",
                shadowName.c_str(), shadowDir.c_str());
            return ret;
        }

        mLoadedShadowName = shadowName;
        mLoadedAssemblyTime = Dir::getLastWriteTime(assembly);

        EDITOR_LOG_INFO("Game plugin [%s] loaded.", shadowName.c_str());

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult PlayModeController::unloadGamePlugin()
    {
        if (!isGamePluginLoaded())
        {
            return T3D_OK;
        }

        // FreeLibrary 之前必须落实销毁：延迟队列里可能还躺着本 DLL 定义的
        // Behaviour。GameObject::destroy 只排队，真正的 onDestroy 要等帧末，
        // 而关闭工程这条路径上已经没有下一帧了，队列会一直留到 Agent 析构时
        // 才处理，那时插件早就卸了，虚调用直接踩到已卸载的代码段
        flushPendingDestroys();

        const String name = mLoadedShadowName;

        // 先清状态：即便卸载失败，也不该让上层以为插件还是可用的
        mLoadedShadowName.clear();
        mLoadedAssemblyTime = 0;

        TResult ret = T3D_AGENT.unloadPlugin(name);
        if (T3D_FAILED(ret))
        {
            EDITOR_LOG_ERROR("Failed to unload game plugin [%s] !", name.c_str());
            return ret;
        }

        // 此刻 FreeLibrary 已经执行，文件解锁了，顺手把这份影子删掉，
        // 影子目录因此稳态只有当前加载的那一份
        SCRIPT_BUILD_SYS.cleanShadowAssemblies("");

        EDITOR_LOG_INFO("Game plugin [%s] unloaded.", name.c_str());

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult PlayModeController::reloadGamePlugin()
    {
        EDITOR_LOG_INFO("Reloading game plugin ...");

        // 场景先落盘，下面要把内存里的对象全销毁，靠这份磁盘数据还原
        TResult ret = saveSceneIfDirty();
        if (T3D_FAILED(ret))
        {
            EDITOR_LOG_ERROR("Failed to save scene before reloading game plugin !");
            return ret;
        }

        // 销毁必须彻底。场景里可能有旧 DLL 里定义的 Behaviour 实例，FreeLibrary
        // 之后它们的 vtable 指针就指向已卸载的代码段了，任何一次虚调用都会崩。
        clearEditorReferences();

        Scene *scene = EDITOR_SCENE.getRuntimeScene();
        if (scene != nullptr)
        {
            scene->getRootTransform()->removeFromParent();
            T3D_SCENE_MGR.unloadScene();
            EDITOR_SCENE.setRuntimeScene(nullptr);
        }

        ret = unloadGamePlugin();
        if (T3D_FAILED(ret))
        {
            EDITOR_LOG_ERROR("Game plugin reload aborted during unload. "
                "Restart the editor to recover.");
            return ret;
        }

        ret = loadGamePlugin();
        if (T3D_FAILED(ret))
        {
            EDITOR_LOG_ERROR("Game plugin reload failed to load the new assembly.");
            return ret;
        }

        // 新类型已注册，这时候恢复场景才认得出自定义组件
        ret = reloadScene();
        if (T3D_FAILED(ret))
        {
            EDITOR_LOG_ERROR("Failed to restore scene after reloading game plugin !");
            return ret;
        }

        EDITOR_LOG_INFO("Game plugin reloaded.");

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void PlayModeController::clearEditorReferences()
    {
        // Inspector 与 Hierarchy 都持有选中的 GameObject，卸载 DLL 前必须放开。
        // 用同步 sendEvent 而不是 postEvent，异步的话等真正处理时对象已经没了。
        EventParamGameObjectSelected param(nullptr);
        sendEvent(kEvtGameObjectSelected, &param);
    }

    //--------------------------------------------------------------------------

    void PlayModeController::flushPendingDestroys()
    {
        // 顺序与 Agent::endFrame 保持一致：组件的 onDestroy 里还会用到 GameObject
        GameObject::destroyComponents();
        GameObject::destroyGameObjects();
    }

    //--------------------------------------------------------------------------

    TResult PlayModeController::saveSceneIfDirty()
    {
        if (!PROJECT_MGR.isSceneModified())
        {
            return T3D_OK;
        }

        return PROJECT_MGR.saveProject();
    }

    //--------------------------------------------------------------------------

    TResult PlayModeController::reloadScene()
    {
        Scene *current = EDITOR_SCENE.getRuntimeScene();

        // 记住是哪个场景。重载走的是「卸载 + 按同一 UUID 从磁盘重新加载」，
        // 而不是内存快照，这样反序列化出来的对象能正常落进资源缓存，
        // 后续保存、按 UUID 反查都和平时走一样的路径。
        UUID uuid = (current != nullptr)
            ? current->getUUID() : PROJECT_MGR.getProjectSettings().StartupSceneUUID;

        if (current != nullptr)
        {
            clearEditorReferences();
            current->getRootTransform()->removeFromParent();
            T3D_SCENE_MGR.unloadScene();
            EDITOR_SCENE.setRuntimeScene(nullptr);

            // 新旧场景来自同一个文件，UUID 逐个相同。旧对象不清干净就重新加载，
            // 新组件会因为 UUID 已被占用而注册不进全局表
            flushPendingDestroys();
        }

        ScenePtr scene = T3D_ASSET_MGR.loadScene(uuid);
        if (scene == nullptr)
        {
            EDITOR_LOG_ERROR("Failed to reload scene (uuid: %s) !",
                uuid.toString().c_str());
            return T3D_ERR_RES_LOAD_FAILED;
        }

        EDITOR_SCENE.setRuntimeScene(scene);
        EDITOR_SCENE.getRuntimeRootTransform()->addChild(scene->getRootTransform());

        // 新相机身上还没有渲染目标，不绑的话渲染管线会拿着空指针去 setRenderTarget
        EDITOR_SCENE.bindGameRenderTarget();

        // 各视图持有的节点树都失效了，通知它们按新场景重建
        EventParamOpenScene param(scene);
        sendEvent(kEvtOpenScene, &param);

        EventParamModifyScene modified(false);
        sendEvent(kEvtModifyScene, &modified);

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult PlayModeController::play()
    {
        if (isPlaying() || mPendingModeChange)
        {
            return T3D_OK;
        }

        if (!canPlay())
        {
            EDITOR_LOG_WARNING("Cannot enter play mode : no project or scene is open.");
            return T3D_ERR_FAIL;
        }

        mPendingModeChange = true;
        T3D_AGENT.postFrameEndTask([this]()
            {
                mPendingModeChange = false;
                doPlay();
            });

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult PlayModeController::stop()
    {
        if (!isPlaying() || mPendingModeChange)
        {
            return T3D_OK;
        }

        mPendingModeChange = true;
        T3D_AGENT.postFrameEndTask([this]()
            {
                mPendingModeChange = false;
                doStop();
            });

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult PlayModeController::doPlay()
    {
        if (isPlaying())
        {
            return T3D_OK;
        }

        if (!canPlay())
        {
            EDITOR_LOG_WARNING("Cannot enter play mode : no project or scene is open.");
            return T3D_ERR_FAIL;
        }

        if (SCRIPT_BUILD_SYS.hasScripts())
        {
            if (SCRIPT_BUILD_SYS.needsBuild(ScriptBuildSystem::Variant::kEditor))
            {
                String output;
                TResult ret = SCRIPT_BUILD_SYS.build(
                    ScriptBuildSystem::Variant::kEditor, output);

                if (T3D_FAILED(ret))
                {
                    // 带着编译错误进 Play 只会跑到旧代码，反而更让人困惑，直接中止
                    EDITOR_LOG_ERROR("Play aborted : game plugin failed to build.");
                    return ret;
                }
            }

            const String assembly =
                SCRIPT_BUILD_SYS.getAssemblyPath(ScriptBuildSystem::Variant::kEditor);

            if (!isGamePluginLoaded())
            {
                TResult ret = loadGamePlugin();
                if (T3D_FAILED(ret))
                {
                    EDITOR_LOG_ERROR("Play aborted : failed to load game plugin.");
                    return ret;
                }
            }
            else if (Dir::exists(assembly)
                && Dir::getLastWriteTime(assembly) > mLoadedAssemblyTime)
            {
                // 产物比当前加载的这份新，说明代码改过了，热重载让新代码生效
                TResult ret = reloadGamePlugin();
                if (T3D_FAILED(ret))
                {
                    EDITOR_LOG_ERROR("Play aborted : failed to reload game plugin.");
                    return ret;
                }
            }
        }

        // Play 期间对场景的改动不应该留下来，进入前先落盘，Stop 时按这份还原
        TResult ret = saveSceneIfDirty();
        if (T3D_FAILED(ret))
        {
            EDITOR_LOG_ERROR("Play aborted : failed to save the scene.");
            return ret;
        }

        T3D_AGENT.enterPlayMode();

        EDITOR_LOG_INFO("Entered play mode.");

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult PlayModeController::doStop()
    {
        if (!isPlaying())
        {
            return T3D_OK;
        }

        T3D_AGENT.exitPlayMode();

        // 还原到点 Play 之前的状态。脚本在 Play 期间怎么改场景都无所谓，
        // 这一步整个丢弃重来。
        TResult ret = reloadScene();
        if (T3D_FAILED(ret))
        {
            EDITOR_LOG_ERROR("Failed to restore the scene after leaving play mode !");
        }

        EDITOR_LOG_INFO("Left play mode.");

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult PlayModeController::validateRuntimeBuild()
    {
        if (!SCRIPT_BUILD_SYS.hasScripts())
        {
            EDITOR_LOG_WARNING("This project has no game plugin scripts to validate.");
            return T3D_ERR_NOT_FOUND;
        }

        EDITOR_LOG_INFO("Validating runtime build ...");

        String output;
        TResult ret = SCRIPT_BUILD_SYS.build(ScriptBuildSystem::Variant::kRuntime, output);

        if (T3D_FAILED(ret))
        {
            EDITOR_LOG_ERROR("Runtime build failed. The most common cause is using "
                "editor-only API (EditorScene, PrefabUtility, Scene::getEditorCamera, "
                "resource overloads taking a UUID) in game code.");
        }
        else
        {
            EDITOR_LOG_INFO("Runtime build succeeded, the game code is publishable.");
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    NS_END
}
