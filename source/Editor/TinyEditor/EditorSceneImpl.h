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

    T3D_DECLARE_SMART_PTR(EditorSceneImpl);
    
    class EditorSceneImpl : public EditorScene
    {
    public:
        static EditorSceneImplPtr create(const String &name);
        
        EditorSceneImpl(const String &name);
        
        ~EditorSceneImpl() override;

        GameObject *getRootGameObject() const override;

        Transform3D *getRootTransform() const override;

        void update() override;

        TResult addCamera(Camera *camera) override;

        TResult removeCamera(Camera *camera) override;

        TResult removeCamera(const UUID &uuid) override;

        TResult removeCamera(const String &name) override;

        const CameraList &getCameras() const override;
        
        TResult addGameObject(GameObject *go) override;

        TResult removeGameObject(GameObject *go) override;

        TResult removeGameObject(const UUID &uuid) override;

        GameObject *getGameObject(const UUID &uuid) const override;

        bool isSceneRoot(TransformNode *node) const override;

        GameObject *getRuntimeRootGameObject() const override;

        Transform3D *getRuntimeRootTransform() const override;

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
        /// 编辑器场景渲染目标
        RenderTargetPtr mSceneTarget {nullptr};
        /// 编辑器场景渲染纹理，给 imgui 用
        ImTextureID     mSceneRT {nullptr};
        /// 游戏场景渲染目标
        RenderTargetPtr mGameTarget {nullptr};
        /// 游戏场景渲染纹理，给 imgui 用
        ImTextureID     mGameRT {nullptr};
        /// 游戏场景根节点
        GameObjectPtr   mRuntimeRootGO {nullptr};
        Transform3DPtr  mRuntimeRootXform {nullptr};

        float mGameRTWidth {1920};
        float mGameRTHeight {1080};
    };

    #define EDITOR_SCENE (*(static_cast<EditorSceneImpl*>(EditorScene::getInstancePtr())))

    NS_END
}
