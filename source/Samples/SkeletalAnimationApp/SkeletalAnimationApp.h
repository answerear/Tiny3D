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

#ifndef __SKELETAL_ANIMATION_APP_H__
#define __SKELETAL_ANIMATION_APP_H__


#include "../Common/SampleApp.h"


class SkeletalAnimationApp : public SampleWindowApp
{
public:
    SkeletalAnimationApp();
    ~SkeletalAnimationApp() override;

protected:  /// from Tiny3D::Application
    bool applicationDidFinishLaunching(int32_t argc, char *argv[]) override;

    void applicationWillTerminate() override;

protected:
    void buildCamera(Tiny3D::Transform3D *parent);
    void buildArm(Tiny3D::Transform3D *parent, const Tiny3D::Vector3 &pos, const Tiny3D::Radian &yAngles);
    void buildPlane(Tiny3D::Transform3D *parent);
    
    Tiny3D::ShaderPtr buildShader(const String &name, const String &forwardVS, const String &shadowVS);
    Tiny3D::PassPtr buildShadowPass(const String &vs);
    Tiny3D::PassPtr buildForwardPass(const String &vs);
    
    Tiny3D::Texture2DPtr buildArmTexture();
    Tiny3D::MaterialPtr buildArmMaterial(Tiny3D::Shader *shader);
    Tiny3D::MeshPtr buildArmMesh(const Tiny3D::UUID &materialUUID);
    Tiny3D::SkeletalAnimationPtr buildArmSkeletalAnimation(Tiny3D::Bones &bones);
    void buildAnimationTracks(const String &name, const Tiny3D::Radian &targetAngle, uint32_t duration, Tiny3D::AnimationTracks &tracks);
    
    Tiny3D::Texture2DPtr buildPlaneTexture();
    Tiny3D::MaterialPtr buildPlaneMaterial(Tiny3D::Shader *shader);
    Tiny3D::MeshPtr buildPlaneMesh(const Tiny3D::UUID &materialUUID);

    void buildAabb(Tiny3D::Mesh *mesh, Tiny3D::SubMesh *submesh, Tiny3D::AabbBound *bound);

    Tiny3D::SkinnedMeshPtr mCubeMesh {nullptr};
    Tiny3D::MeshPtr mPlaneMesh {nullptr};
    
    Tiny3D::MaterialPtr mCubeMaterial {nullptr};
    Tiny3D::MaterialPtr mPlaneMaterial {nullptr};
};


#endif  /*__SKELETAL_ANIMATION_APP_H__*/
