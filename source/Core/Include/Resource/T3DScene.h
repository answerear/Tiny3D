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
#include "Resource/T3DResource.h"
#include "Kernel/T3DGameObject.h"


namespace Tiny3D
{
    TCLASS()
    class T3D_ENGINE_API Scene : public Resource
    {
        TRTTI_ENABLE(Resource)
        TRTTI_FRIEND

    public:
        static ScenePtr create(const String &name);
        
        ~Scene() override = default;

        Type getType() const override;

        virtual GameObject *getRootGameObject() const { return mRootGameObject; }

        virtual Transform3D *getRootTransform() const { return mRootTransform; }

        virtual void update();

        virtual TResult addCamera(Camera *camera);

        virtual TResult removeCamera(Camera *camera);

        virtual TResult removeCamera(const UUID &uuid);

        virtual TResult removeCamera(const String &name);

        virtual const CameraList &getCameras() const { return mCameras; }

        virtual TResult addGameObject(GameObject *go);

        virtual TResult removeGameObject(GameObject *go);

        virtual TResult removeGameObject(const UUID &uuid);

        virtual GameObject *getGameObject(const UUID &uuid) const
        {
            const auto it = mGameObjects.find(uuid);
            return it != mGameObjects.end() ? it->second : nullptr;
        }

        virtual bool isSceneRoot(TransformNode *node) const { return node->getParent() == nullptr; }

#if defined(T3D_EDITOR)
        virtual Camera *getEditorCamera() const { return nullptr; }

        virtual GameObject *getEditorGameObject() const { return nullptr; }

        virtual Transform3D *getEditorRootTransform() const { return nullptr; }

        virtual void setRuntimeScene(Scene *scene) {}

        virtual Scene *getRuntimeScene() const { return nullptr; }
#endif

        TResult init();
        
    protected:
        Scene(const String &name);

        ResourcePtr clone() const override;

        void cloneProperties(const Resource * const src) override;

        TResult onLoad(Archive *archive) override;
        
        TResult onUnload() override;

        void onPostLoad() override;

        void onPostInit() override;

        void onAddComponentForLoadingResource(Component *component) override;

    private:
        using GameObjects = TUnorderedMap<UUID, GameObjectPtr, UUIDHash, UUIDEqual>;
        
        Scene() : Scene("") {}

        TPROPERTY(RTTRFuncName="RootGameObject", RTTRFuncType="getter")
        const UUID &getRootGameObjectUUID() const { return mRootGameObjectUUID; }

        TPROPERTY(RTTRFuncName="RootGameObject", RTTRFuncType="setter")
        void setRootGameObjectUUID(const UUID &uuid) { mRootGameObjectUUID = uuid; }

        TPROPERTY(RTTRFuncName="GameObjects", RTTRFuncType="getter")
        const GameObjects &getGameObjects() const { return mGameObjects; }

        TPROPERTY(RTTRFuncName="GameObjects", RTTRFuncType="setter")
        void setGameObjects(const GameObjects &gameObjects) { mGameObjects = gameObjects; }
        
    protected:
        /// 场景根节点对应的 game object
        GameObjectPtr   mRootGameObject { nullptr };
        /// 根节点
        Transform3DPtr  mRootTransform {nullptr};
        /// 场景相机列表
        CameraList      mCameras {};
        /// 场景根节点 game object 对应的 UUID
        UUID            mRootGameObjectUUID {};
        /// 所有属于这个场景的 game object
        GameObjects     mGameObjects {};
    };

#if defined (T3D_EDITOR)
    class T3D_ENGINE_API EditorScene : public Scene, public Singleton<EditorScene>
    {
    public:
        Camera *getEditorCamera() const override { return mSceneCamera; }

        GameObject *getEditorGameObject() const override { return mRootGameObject; }

        Transform3D *getEditorRootTransform() const override { return mRootTransform; }

        void setRuntimeScene(Scene *scene) override { mRuntimeScene = scene;}

        Scene *getRuntimeScene() const override { return mRuntimeScene; }

        virtual GameObject *getRuntimeRootGameObject() const = 0;

        virtual Transform3D *getRuntimeRootTransform() const = 0;

    protected:
        EditorScene(const String &name) : Scene(name) {}
        
        /// 运行时的游戏场景
        Scene           *mRuntimeScene {nullptr};
        /// 编辑器场景相机
        CameraPtr       mSceneCamera {nullptr};
    };

    #define T3D_EDITOR_SCENE    (EditorScene::getInstance())
#endif
}


#endif    /*__T3D_SCENE_H__*/
