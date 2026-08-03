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

#ifndef __SKYBOX_APP_H__
#define __SKYBOX_APP_H__


#include "../Common/SampleApp.h"


class SkyboxApp : public SampleWindowApp
{
public:
    SkyboxApp();
    ~SkyboxApp() override;

    TResult go(int32_t argc, char *argv[]) override;

protected:  /// from Tiny3D::Application
    TResult applicationDidFinishLaunching(int32_t argc, char *argv[]) override;

    void applicationWillTerminate() override;

protected:
    void buildCamera(Tiny3D::Transform3D *parent);

    /// 把天空盒组件挂到场景根节点上，作为全局天空盒
    void buildSkybox(Tiny3D::Scene *scene);

    /// 程序化立方体：ShadowApp 同款 lit/shadow shader + TextureApp 的 blocks UV
    void buildCube(Tiny3D::Transform3D *parent);

    /// 地面平面，用于接收立方体投影（与 ShadowApp 同款）
    void buildPlane(Tiny3D::Transform3D *parent);

    Tiny3D::ShaderPtr buildShader();
    Tiny3D::PassPtr buildShadowPass();
    Tiny3D::PassPtr buildForwardPass();

    Tiny3D::Texture2DPtr buildCubeTexture();
    Tiny3D::MaterialPtr buildCubeMaterial(Tiny3D::Shader *shader);
    Tiny3D::MeshPtr buildCubeMesh(const Tiny3D::UUID &materialUUID);

    Tiny3D::Texture2DPtr buildPlaneTexture();
    Tiny3D::MaterialPtr buildPlaneMaterial(Tiny3D::Shader *shader);
    Tiny3D::MeshPtr buildPlaneMesh(const Tiny3D::UUID &materialUUID);

    void buildAabb(Tiny3D::Mesh *mesh, Tiny3D::SubMesh *submesh, Tiny3D::AabbBound *bound);

    /// 给材质挂上 Forward 光照 / 阴影所需的占位参数
    void setupLitMaterialParams(Tiny3D::Material *material);

    Tiny3D::MeshPtr         mCubeMesh {nullptr};
    Tiny3D::MeshPtr         mPlaneMesh {nullptr};
    Tiny3D::MaterialPtr     mCubeMaterial {nullptr};
    Tiny3D::MaterialPtr     mPlaneMaterial {nullptr};

    Tiny3D::ID              mOrbitTimerID {Tiny3D::T3D_INVALID_TIMER_ID};

    /// 相机绕 Y 轴的公转角，单位弧度
    Tiny3D::Real            mCameraYaw {0.0f};
};


#endif  /*__SKYBOX_APP_H__*/
