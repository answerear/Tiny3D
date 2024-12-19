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


#define ARCHIVE_TYPE_FS     "FileSystem"


class TextureApp : public SampleWindowApp
{
public:
    TextureApp();
    virtual ~TextureApp();

protected:
    bool applicationDidFinishLaunching(int32_t argc, char *argv[]) override;

    void buildCamera(Tiny3D::Transform3D *parent);
    void buildCube(Tiny3D::Transform3D *parent);
    
    Tiny3D::Texture2DPtr buildTexture();
    Tiny3D::MaterialPtr buildMaterial();
    Tiny3D::MeshPtr buildMesh(const Tiny3D::UUID &materialUUID);
    void buildAabb(Tiny3D::Mesh *mesh, Tiny3D::SubMesh *submesh, Tiny3D::AabbBound *bound);
};


#endif  /*__TEXTURE_APP_H__*/
