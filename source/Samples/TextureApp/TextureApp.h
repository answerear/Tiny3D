/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#ifndef __TEXTURE_APP_H__
#define __TEXTURE_APP_H__


#include "../Common/SampleApp.h"


#define ARCHIVE_TYPE_FS         "FileSystem"
#define ARCHIVE_TYPE_ANDROID    "AndroidAsset"


class TextureApp : public SampleWindowApp
{
public:
    TextureApp();
    virtual ~TextureApp();

protected:  /// from Tiny3D::Application
    TResult applicationDidFinishLaunching(int32_t argc, char *argv[]) override;
    void applicationWillTerminate() override;
    void onPreRender() override;
    void onRender() override;
    void onPostRender() override;

    void buildCamera(Tiny3D::Transform3D *parent);
    void buildCube(Tiny3D::Transform3D *parent);
    
    Tiny3D::Texture2DPtr buildTexture();
    Tiny3D::MaterialPtr buildMaterial();
    Tiny3D::MeshPtr buildMesh(const Tiny3D::UUID &materialUUID);
    void buildAabb(Tiny3D::Mesh *mesh, Tiny3D::SubMesh *submesh, Tiny3D::AabbBound *bound);

    /// 建两张不参与绘制的程序化纹理，专门用来验收 GPU 读回
    void buildReadbackTextures();

    /// 故意选不是 32 对齐的宽度：GPU RowPitch 会大于 width*4，按行拷贝写错立刻出条纹
    static const uint32_t kReadbackWidth  = 61;
    static const uint32_t kReadbackHeight = 37;

    Tiny3D::Texture2DPtr   mReadbackTex {nullptr};   ///< kCPURead，允许 map
    Tiny3D::Texture2DPtr   mDeniedTex   {nullptr};   ///< kCPUNone 对照组，必须被拒
    Tiny3D::ReadbackHandle mReadbackHandle {};
    bool                   mReadbackDone {false};
};


#endif  /*__TEXTURE_APP_H__*/
