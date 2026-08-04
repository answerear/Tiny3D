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

#ifndef __T3D_SCENE_MANAGER_H__
#define __T3D_SCENE_MANAGER_H__


#include "Resource/T3DResourceManager.h"


namespace Tiny3D
{
#if defined (T3D_EDITOR)
    /// 编辑器场景工厂函数类型
    using EditorSceneCreator = TFunction<ScenePtr(const String &name)>;
#endif
    
    /**
     * \brief 场景管理实现接口，定义场景的创建、加载、保存与卸载
     */
    class T3D_ENGINE_API SceneManagerImpl : public ResourceManager
    {
    public:
        /**
         * \brief 创建空白场景
         * \param [in] name : 场景名称
         * \return 新 Scene 智能指针
         */
        virtual ScenePtr createScene(const String &name) = 0;

#if defined (T3D_EDITOR)
        /**
         * \brief 通过自定义工厂创建编辑器场景
         * \param [in] name : 场景名称
         * \param [in] creator : 编辑器场景创建函数
         * \return 新 Scene 智能指针
         */
        virtual ScenePtr createEditorScene(const String &name, const EditorSceneCreator &creator) = 0;
#endif

        /**
         * \brief 按名称从 Archive 加载场景
         * \param [in] archive : 资源档案
         * \param [in] name : 场景名称或路径
         * \return 成功返回 Scene 智能指针，失败返回 nullptr
         */
        virtual ScenePtr loadScene(Archive *archive, const String &name) = 0;

        /**
         * \brief 按 UUID 从 Archive 加载场景
         * \param [in] archive : 资源档案
         * \param [in] uuid : 场景 UUID
         * \return 成功返回 Scene 智能指针，失败返回 nullptr
         */
        virtual ScenePtr loadScene(Archive *archive, const UUID &uuid) = 0;

        /**
         * \brief 按场景 UUID 保存到 Archive
         * \param [in] archive : 目标档案
         * \param [in] scene : 待保存场景
         * \return 成功返回 T3D_OK
         */
        virtual TResult saveScene(Archive *archive, Scene *scene) = 0;

        /**
         * \brief 按指定文件名保存场景到 Archive
         * \param [in] archive : 目标档案
         * \param [in] filename : 保存文件名
         * \param [in] scene : 待保存场景
         * \return 成功返回 T3D_OK
         */
        virtual TResult saveScene(Archive *archive, const String &filename, Scene *scene) = 0;

        /**
         * \brief 卸载场景资源
         * \param [in,out] scene : 待卸载场景智能指针，成功后由基类 unload 置空
         * \return 成功返回 T3D_OK
         */
        virtual TResult unloadScene(ScenePtr &scene);
    };

    /**
     * \brief 场景管理器门面，委托 SceneManagerImpl 执行具体场景操作
     */
    class T3D_ENGINE_API SceneManager : public Object, public Singleton<SceneManager>
    {
    public:
        /**
         * \brief 创建 SceneManager 实例
         * \return 新 SceneManager 智能指针
         */
        static SceneManagerPtr create();

        /// 析构 SceneManager（默认）
        ~SceneManager() override = default;

        /**
         * \brief 设置底层场景管理实现
         * \param [in] impl : 实现对象指针
         */
        void setSceneManagerImpl(SceneManagerImpl *impl) { mImpl = impl; }

        /**
         * \brief 创建空白场景
         * \param [in] name : 场景名称
         * \return mImpl 非空时返回新 Scene，否则 nullptr
         */
        ScenePtr createScene(const String &name);

#if defined (T3D_EDITOR)
        /**
         * \brief 通过自定义工厂创建编辑器场景
         * \param [in] name : 场景名称
         * \param [in] creator : 编辑器场景创建函数
         * \return mImpl 非空时返回新 Scene，否则 nullptr
         */
        ScenePtr createEditorScene(const String &name, const EditorSceneCreator &creator);
#endif

        /**
         * \brief 按名称从 Archive 加载场景
         * \param [in] archive : 资源档案
         * \param [in] name : 场景名称或路径
         * \return mImpl 非空时返回 Scene，否则 nullptr
         */
        ScenePtr loadScene(Archive *archive, const String &name);

        /**
         * \brief 按 UUID 从 Archive 加载场景
         * \param [in] archive : 资源档案
         * \param [in] uuid : 场景 UUID
         * \return mImpl 非空时返回 Scene，否则 nullptr
         */
        ScenePtr loadScene(Archive *archive, const UUID &uuid);

        /**
         * \brief 按场景 UUID 保存到 Archive
         * \param [in] archive : 目标档案
         * \param [in] scene : 待保存场景
         * \return mImpl 非空时返回 impl 结果，否则 T3D_ERR_NOT_IMPLEMENT
         */
        TResult saveScene(Archive *archive, Scene *scene);

        /**
         * \brief 按指定文件名保存场景到 Archive
         * \param [in] archive : 目标档案
         * \param [in] filename : 保存文件名
         * \param [in] scene : 待保存场景
         * \return mImpl 非空时返回 impl 结果，否则 T3D_ERR_NOT_IMPLEMENT
         */
        TResult saveScene(Archive *archive, const String &filename, Scene *scene);

        /// 获取当前活动场景
        ScenePtr getCurrentScene() const { return mCurrentScene; }

        /**
         * \brief 设置当前活动场景
         * \param [in] scene : 活动场景指针
         */
        void setCurrentScene(Scene *scene) { mCurrentScene = scene; }

        /**
         * \brief 卸载当前活动场景
         * \return T3D_EDITOR 下卸载 mCurrentScene 的运行时场景并解绑；否则卸载 mCurrentScene；mImpl 为空返回 T3D_ERR_NOT_IMPLEMENT
         */
        TResult unloadScene();

        /**
         * \brief 卸载指定场景
         * \param [in,out] scene : 待卸载场景智能指针
         * \return mImpl 非空时返回 impl 结果，否则 T3D_ERR_NOT_IMPLEMENT
         */
        TResult unloadScene(ScenePtr &scene);

        /**
         * \brief 卸载所有已加载场景资源
         * \return mImpl 非空时返回 unloadAllResources 结果，否则 T3D_ERR_NOT_IMPLEMENT
         */
        TResult unloadAllScenes();
        
    protected:
        /// 默认构造
        SceneManager() = default;

    protected:
        /// 底层场景管理实现
        SceneManagerImplPtr mImpl {nullptr};
        /// 当前活动场景
        ScenePtr            mCurrentScene {nullptr};
    };

    
    #define T3D_SCENE_MGR   (SceneManager::getInstance())
    

    /**
     * \brief 内置场景管理实现，基于 ResourceManager 管理 Scene 资源
     */
    class BuiltinSceneManager : public SceneManagerImpl
    {
    public:
        /**
         * \brief 创建 BuiltinSceneManager 实例
         * \return 新 BuiltinSceneManager 智能指针
         */
        static BuiltinSceneManagerPtr create();

        /// 析构 BuiltinSceneManager（默认）
        ~BuiltinSceneManager() override = default;

        /**
         * \brief 创建空白 Scene 资源
         * \param [in] name : 场景名称
         * \return 新 Scene 智能指针
         */
        ScenePtr createScene(const String &name) override;

#if defined (T3D_EDITOR)
        /**
         * \brief 通过 creator 或默认 Scene::create 创建编辑器场景
         * \param [in] name : 场景名称
         * \param [in] creator : 编辑器场景创建函数；creator 为 nullptr 时返回 nullptr
         * \return 新 Scene 智能指针
         */
        ScenePtr createEditorScene(const String &name, const EditorSceneCreator &creator) override;
#endif

        /**
         * \brief 按名称从 Archive 加载场景
         * \param [in] archive : 资源档案
         * \param [in] name : 场景名称或路径
         * \return 成功返回 Scene 智能指针，失败返回 nullptr
         */
        ScenePtr loadScene(Archive *archive, const String &name) override;

        /**
         * \brief 按 UUID 从 Archive 加载场景
         * \param [in] archive : 资源档案
         * \param [in] uuid : 场景 UUID
         * \return 成功返回 Scene 智能指针，失败返回 nullptr
         */
        ScenePtr loadScene(Archive *archive, const UUID &uuid) override;

        /**
         * \brief 按场景 UUID 保存到 Archive
         * \param [in] archive : 目标档案
         * \param [in] scene : 待保存场景
         * \return 成功返回 T3D_OK
         */
        TResult saveScene(Archive *archive, Scene *scene) override;

        /**
         * \brief 按指定文件名保存场景到 Archive
         * \param [in] archive : 目标档案
         * \param [in] filename : 保存文件名
         * \param [in] scene : 待保存场景
         * \return 成功返回 T3D_OK
         */
        TResult saveScene(Archive *archive, const String &filename, Scene *scene) override;
        
    protected:
        /// 默认构造
        BuiltinSceneManager() = default;
        
        /**
         * \brief 创建 Scene 资源；T3D_EDITOR 且 argc==1 时通过 creator 创建
         * \param [in] name : 资源名称
         * \param [in] argc : 可变参数个数（0 或 1）
         * \param [in] args : T3D_EDITOR 下可为 EditorSceneCreator*
         * \return 新 Scene 智能指针
         */
        ResourcePtr newResource(const String &name, int32_t argc, va_list args) override;

        /**
         * \brief 从数据流反序列化场景（忽略 name，委托 loadResource(stream)）
         * \param [in] name : 资源名称（未使用）
         * \param [in,out] stream : 输入数据流
         * \return 反序列化得到的 Scene 智能指针
         */
        ResourcePtr loadResource(const String &name, DataStream &stream) override;

        /**
         * \brief 从数据流反序列化场景
         * \param [in,out] stream : 输入数据流
         * \return 反序列化得到的 Scene 智能指针
         */
        ResourcePtr loadResource(DataStream &stream) override;
        
        /**
         * \brief 将 Scene 序列化写入数据流
         * \param [in,out] stream : 输出数据流
         * \param [in] res : 待保存的 Scene 资源（须为 kScene 类型）
         * \return 序列化结果
         */
        TResult saveResource(DataStream &stream, Resource *res) override;
    };

    T3D_EXTERN_SINGLETON(SceneManager)
}


#endif    /*__T3D_SCENE_MANAGER_H__*/
