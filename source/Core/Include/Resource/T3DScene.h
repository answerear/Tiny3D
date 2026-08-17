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

#ifndef __T3D_SCENE_H__
#define __T3D_SCENE_H__


#include "Component/T3DTransformNode.h"
#include "Component/T3DBehaviour.h"
#include "Resource/T3DResource.h"
#include "Kernel/T3DGameObject.h"


namespace Tiny3D
{
    /**
     * \brief 场景资源，管理 GameObject 层级、相机列表及 Behaviour 生命周期调度
     */
    TCLASS()
    class T3D_ENGINE_API Scene : public Resource
    {
        TRTTI_ENABLE(Resource)
        TRTTI_FRIEND

    public:
        /**
         * \brief 创建 Scene 资源
         * \param [in] name : 场景名称
         * \return 新创建的 Scene 智能指针
         */
        static ScenePtr create(const String &name);
        
        /// 析构 Scene（默认）
        ~Scene() override = default;

        /**
         * \brief 返回资源类型标识
         * \return Type::kScene
         */
        Type getType() const override;

        /// 获取场景根 GameObject
        virtual GameObject *getRootGameObject() const { return mRootGameObject; }

        /// 获取场景根 Transform3D
        virtual Transform3D *getRootTransform() const { return mRootTransform; }

        /**
         * \brief 每帧更新：对根节点执行 update 与 lateUpdate
         * \remarks mRootGameObject 为 nullptr 时不执行
         */
        virtual void update();

        /**
         * \brief 固定步长更新入口，由 Agent 的 FixedUpdate 循环调用
         * \remarks mRootGameObject 为 nullptr 时不执行
         */
        virtual void fixedUpdate();

        /**
         * \brief 将 Behaviour 加入 pending-start 队列，等待首帧 update 前 flush
         * \param [in] b : 待延迟 Start 的 Behaviour；为 nullptr 时忽略
         */
        void enqueuePendingStart(Behaviour *b);

        /**
         * \brief 刷新 pending-start 队列：对已激活且未 Start 的 Behaviour 调用 invokeStart
         * \remarks 交换队列避免 onStart 内再 addComponent 导致迭代失效；尚未激活的 Behaviour 保留到后续帧
         */
        virtual void flushPendingStart();

        /**
         * \brief 按渲染顺序将相机加入场景
         * \param [in] camera : 待添加相机，不可为 nullptr
         * \return 成功返回 T3D_OK；UUID 重复返回 T3D_ERR_DUPLICATED_ITEM
         */
        virtual TResult addCamera(Camera *camera);

        /**
         * \brief 按 order 键移除相机
         * \param [in] camera : 待移除相机
         * \return 成功返回 T3D_OK；未找到返回 T3D_ERR_NOT_FOUND
         */
        virtual TResult removeCamera(Camera *camera);

        /**
         * \brief 按 UUID 移除相机
         * \param [in] uuid : 相机 UUID
         * \return 成功返回 T3D_OK；未找到返回 T3D_ERR_NOT_FOUND
         */
        virtual TResult removeCamera(const UUID &uuid);

        /**
         * \brief 按 GameObject 名称移除相机
         * \param [in] name : 相机所在 GameObject 名称
         * \return 成功返回 T3D_OK；未找到返回 T3D_ERR_NOT_FOUND
         */
        virtual TResult removeCamera(const String &name);

        /// 获取按 order 排序的场景相机列表
        virtual const CameraList &getCameras() const { return mCameras; }

        /**
         * \brief 将 GameObject 注册到场景映射表
         * \param [in] go : 待添加 GameObject；为 nullptr 返回 T3D_ERR_INVALID_PARAM
         * \return 成功返回 T3D_OK
         */
        virtual TResult addGameObject(GameObject *go);

        /**
         * \brief 按 GameObject UUID 从场景映射表移除
         * \param [in] go : 待移除 GameObject
         * \return 始终返回 T3D_OK
         */
        virtual TResult removeGameObject(GameObject *go);

        /**
         * \brief 按 UUID 从场景映射表移除 GameObject
         * \param [in] uuid : GameObject UUID
         * \return 始终返回 T3D_OK
         */
        virtual TResult removeGameObject(const UUID &uuid);

        /**
         * \brief 按 UUID 查找场景中的 GameObject
         * \param [in] uuid : GameObject UUID
         * \return 找到返回指针，否则返回 nullptr
         */
        virtual GameObject *getGameObject(const UUID &uuid) const
        {
            const auto it = mGameObjects.find(uuid);
            return it != mGameObjects.end() ? it->second : nullptr;
        }

        /**
         * \brief 返回场景 GameObject 扁平表（只读）
         */
        const GameObjects &getAllGameObjects() const { return mGameObjects; }

        /**
         * \brief 反序列化/实例化后，对整棵子树 Behaviour 同步 Awake + OnEnable 并投递 pending-start
         * \param [in] root : 子树根 GameObject；为 nullptr 时不执行
         */
        void awakeHierarchy(GameObject *root);

        /// 判断 TransformNode 是否为场景根（无父节点）
        virtual bool isSceneRoot(TransformNode *node) const { return node->getParent() == nullptr; }

        /**
         * \brief 输出完整场景树字符串（从根节点递归遍历）
         * \param [in] verbose : 为 true 时输出各节点 Component 列表、[BONE] 标记及变换信息
         * \return 场景树文本；mRootTransform 为 nullptr 时返回 "[EMPTY SCENE]"
         */
        virtual String printSceneHierarchy(bool verbose = false);

#if defined(T3D_EDITOR)
        /// 编辑器场景：获取编辑器相机（基类默认 nullptr）
        virtual Camera *getEditorCamera() const { return nullptr; }

        /// 编辑器场景：获取编辑器根 GameObject（基类默认 nullptr）
        virtual GameObject *getEditorGameObject() const { return nullptr; }

        /// 编辑器场景：获取编辑器根 Transform（基类默认 nullptr）
        virtual Transform3D *getEditorRootTransform() const { return nullptr; }

        /// 编辑器场景：绑定运行时场景（基类空实现）
        virtual void setRuntimeScene(Scene *scene) {}

        /// 编辑器场景：获取绑定的运行时场景（基类默认 nullptr）
        virtual Scene *getRuntimeScene() const { return nullptr; }
#endif

        /**
         * \brief 初始化场景：创建 SceneRoot GameObject 及根 Transform3D
         * \return 成功返回 T3D_OK
         */
        TResult init();
        
    protected:
        /**
         * \brief 构造场景资源
         * \param [in] name : 场景名称
         */
        Scene(const String &name);

        /**
         * \brief 克隆场景（当前实现仅 create 同名新实例，不复制属性）
         * \return 新 Scene 智能指针
         */
        ResourcePtr clone() const override;

        /**
         * \brief 克隆属性（当前实现为空）
         * \param [in] src : 源 Scene 资源
         */
        void cloneProperties(const Resource * const src) override;

        /**
         * \brief 从 Archive 加载场景资源
         * \param [in] archive : 资源档案
         * \return 基类 onLoad 的返回值
         */
        TResult onLoad(Archive *archive) override;
        
        /**
         * \brief 卸载场景：销毁根 GameObject 并清空映射表与相机列表
         * \return 基类 onUnload 的返回值
         */
        TResult onUnload() override;

        /**
         * \brief 加载后恢复层级关系，并对整树 Behaviour 执行 Awake → OnEnable 及 pending-start 投递
         * \remarks 按 mRootGameObjectUUID 恢复 mRootGameObject 与 mRootTransform
         */
        void onPostLoad() override;

        /**
         * \brief 初始化完成后在 T3D_EDITOR 下将本场景设为 EditorScene 的运行时场景
         */
        void onPostInit() override;

        /**
         * \brief 加载时登记需延迟加载资源的 Geometry 或 Skybox 组件
         * \param [in] component : 待加载资源的组件
         */
        void onAddComponentForLoadingResource(Component *component) override;

    private:
        using GameObjects = TUnorderedMap<UUID, GameObjectPtr, UUIDHash, UUIDEqual>;

        /// 默认构造，委托 Scene("")
        Scene() : Scene("") {}

        /// RTTR 序列化：获取根 GameObject UUID
        TPROPERTY(RTTRFuncName="RootGameObject", RTTRFuncType="getter")
        const UUID &getRootGameObjectUUID() const { return mRootGameObjectUUID; }

        /// RTTR 序列化：设置根 GameObject UUID
        TPROPERTY(RTTRFuncName="RootGameObject", RTTRFuncType="setter")
        void setRootGameObjectUUID(const UUID &uuid) { mRootGameObjectUUID = uuid; }

        /// RTTR 序列化：获取场景 GameObject 扁平表
        TPROPERTY(RTTRFuncName="GameObjects", RTTRFuncType="getter")
        const GameObjects &getGameObjects() const { return mGameObjects; }

        /// RTTR 序列化：设置场景 GameObject 扁平表
        TPROPERTY(RTTRFuncName="GameObjects", RTTRFuncType="setter")
        void setGameObjects(const GameObjects &gameObjects) { mGameObjects = gameObjects; }
        
    protected:
        /// 场景根 GameObject
        GameObjectPtr   mRootGameObject { nullptr };
        /// 场景根 Transform3D
        Transform3DPtr  mRootTransform {nullptr};
        /// 按 order 排序的场景相机列表
        CameraList      mCameras {};
        /// 场景根 GameObject UUID（序列化用）
        UUID            mRootGameObjectUUID {};
        /// 场景中所有 GameObject 的 UUID 映射表
        GameObjects     mGameObjects {};
        /// 等待首帧调用 onStart 的 Behaviour 队列
        TList<BehaviourPtr> mPendingStart {};
    };

#if defined (T3D_EDITOR)
    /**
     * \brief Scene 视图网格着色模式，只作用于编辑器相机
     */
    enum class SceneDrawMode : uint32_t
    {
        /// 正常着色填充
        kShaded = 0,
        /// 只画三角形边
        kWireframe,
        /// 先着色再叠线框
        kShadedWireframe,
    };

    /**
     * \brief 编辑器场景，扩展 Scene 并提供编辑器相机与运行时场景绑定
     */
    class T3D_ENGINE_API EditorScene : public Scene, public Singleton<EditorScene>
    {
    public:
        /// 获取编辑器场景相机
        Camera *getEditorCamera() const override { return mSceneCamera; }

        /// 获取编辑器根 GameObject
        GameObject *getEditorGameObject() const override { return mRootGameObject; }

        /// 获取编辑器根 Transform3D
        Transform3D *getEditorRootTransform() const override { return mRootTransform; }

        /**
         * \brief 绑定运行时游戏场景
         * \param [in] scene : 运行时 Scene 指针
         */
        void setRuntimeScene(Scene *scene) override { mRuntimeScene = scene;}

        /// 获取绑定的运行时场景
        Scene *getRuntimeScene() const override { return mRuntimeScene; }

        /**
         * \brief 同时 flush 编辑器场景与 runtime 场景的 pending-start
         * \remarks Agent 只对 getCurrentScene()（编辑器里是 EditorScene）调用 flush。
         *          场景加载 / 进 Play 的 Awake 把 Start 投到 runtime Scene，必须两边都冲。
         */
        void flushPendingStart() override
        {
            Scene::flushPendingStart();
            if (mRuntimeScene != nullptr)
            {
                mRuntimeScene->flushPendingStart();
            }
        }

        /// 获取 Scene 视图着色模式
        SceneDrawMode getDrawMode() const { return mDrawMode; }

        /**
         * \brief 设置 Scene 视图着色模式
         * \param [in] mode : Shaded / Wireframe / Shaded Wireframe
         */
        void setDrawMode(SceneDrawMode mode) { mDrawMode = mode; }

        /// 获取运行时场景根 GameObject（纯虚，由派生类实现）
        virtual GameObject *getRuntimeRootGameObject() const = 0;

        /// 获取运行时场景根 Transform（纯虚，由派生类实现）
        virtual Transform3D *getRuntimeRootTransform() const = 0;

    protected:
        /**
         * \brief 构造编辑器场景
         * \param [in] name : 场景名称
         */
        EditorScene(const String &name) : Scene(name) {}
        
        /// 绑定的运行时游戏场景
        Scene           *mRuntimeScene {nullptr};
        /// 编辑器场景相机
        CameraPtr       mSceneCamera {nullptr};
        /// Scene 视图着色模式，不序列化
        SceneDrawMode   mDrawMode {SceneDrawMode::kShaded};
    };

    #define T3D_EDITOR_SCENE    (EditorScene::getInstance())

    T3D_EXTERN_SINGLETON(EditorScene)
#endif
}


#endif    /*__T3D_SCENE_H__*/
