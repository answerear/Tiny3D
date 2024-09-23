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

// #define TEST_SCENE


namespace Tiny3D
{
    NS_BEGIN(Editor)

    T3D_DECLARE_SMART_PTR(EditorScene);
    
    class EditorScene
        : public Singleton<EditorScene>
        , public Scene
    {
    public:
        static EditorScenePtr create(const String &name);
        
        EditorScene(const String &name);
        
        ~EditorScene() override;

        void setRuntimeScene(Scene *scene) override { mRuntimeScene = scene; }

        Scene *getRuntimeScene() const override { return mRuntimeScene; }

        const GameObjectPtr &getRootGameObject() const override;

        const Transform3DPtr &getRootTransform() const override;

        void update() override;

        TResult addCamera(Camera *camera) override;

        TResult removeCamera(Camera *camera) override;

        TResult removeCamera(const UUID &uuid) override;

        TResult removeCamera(const String &name) override;

        const CameraList &getCameras() const override;

        Camera *getEditorCamera() const override;

        GameObject *getEditorGameObject() const override;

        Transform3D *getEditorRootTransform() const; 

        void build();

        void refreshSceneRenderTarget();

        void refreshGameRenderTarget(const ImVec2 &size);
        
        RenderTarget *getSceneRenderTarget() { return mSceneTarget; }

        ImTextureID getSceneRT() const { return mSceneRT; }

        RenderTarget *getGameRenderTarget() { return mGameTarget; }

        ImTextureID getGameRT() const { return mGameRT; }

#if defined(TEST_SCENE)
    protected:  // for test
        void buildCamera(Transform3D *parent);
        void buildCube(Transform3D *parent);
        Texture2DPtr buildTexture();
        MaterialPtr buildMaterial();
        MeshPtr buildMesh();
        void buildAabb(Mesh *mesh, SubMesh *submesh, AabbBound *bound);
#endif
        
    protected:
        /// 编辑器场景相机
        CameraPtr       mSceneCamera {nullptr};
        /// 编辑器场景渲染目标
        RenderTargetPtr mSceneTarget {nullptr};
        /// 编辑器场景渲染纹理，给 imgui 用
        ImTextureID     mSceneRT {nullptr};
        /// 游戏场景渲染目标
        RenderTargetPtr mGameTarget {nullptr};
        /// 游戏场景渲染纹理，给 imgui 用
        ImTextureID     mGameRT {nullptr};
        /// 游戏场景根节点
        GameObjectPtr   mRoot {nullptr};
        /// 运行时的游戏场景
        Scene           *mRuntimeScene {nullptr};

        float mGameRTWidth {1920};
        float mGameRTHeight {1080};
    };

    #define EDITOR_SCENE (EditorScene::getInstance())

    NS_END
}
