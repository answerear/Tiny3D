/*******************************************************************************
 * This file is part of Mesh-converter (A mesh converter for Tiny3D Engine)
 * Copyright (C) 2015-2017  Aaron Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
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

#ifndef __MCONV_OGRE_SERIALIZER_H__
#define __MCONV_OGRE_SERIALIZER_H__


#include "mconv_serializer.h"
#include "mconv_ogredata.h"

#include <T3DVector3.h>
#include <T3DQuaternion.h>


namespace mconv
{
    class OgreSerializer : public Serializer
    {
    public:
        OgreSerializer();
        virtual ~OgreSerializer();

        virtual bool load(const String &path, void *&pData) override;
        virtual bool save(const String &path, void *pData) override;

    protected:
        bool readChunk(Tiny3D::DataStream &stream, OgreMesh &mesh);
        bool readChunkData(Tiny3D::DataStream &stream, OgreChunkData &data);

        bool readMesh(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreMesh &mesh);
        bool readGeometry(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreGeometry &geometry);
        bool readGeometryVertexDeclaration(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreGeometry &geometry);
        bool readGeometryVertexBuffer(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreGeometry &geometry);

        bool readSubMesh(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreSubMesh &submesh);
        bool readSubMeshOperation(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreSubMesh &submesh);
        bool readSubMeshTextureAlias(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreSubMesh &submesh);

        bool readSubMeshNameTable(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreMesh &mesh);

        bool readBoneAssignment(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreBoneAssignment &assignment);
        bool readMeshBound(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreMesh &mesh);

        bool readSkeleton(const String &name, OgreSkeleton &skeleton);
        bool readSkeletonChunk(Tiny3D::DataStream &stream, OgreSkeleton &skeleton);
        bool readSkeletonBone(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreSkeleton &skeleton);
        bool readSkeletonBoneParent(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreSkeleton &skeleton);
        bool readSkeletonAnimation(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreSkeleton &skeleton);
        bool readAnimationTrack(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreAnimation &animation);
        bool readAnimationKeyframe(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreAnimation &animation, uint16_t bone);
        bool readAnimationKeyframeList(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreAnimation &animation, uint16_t bone);
        bool readSkeletonAnimationLink(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreSkeleton &skeleton);

        size_t readBools(Tiny3D::DataStream &stream, OgreChunkData &data, bool *value, size_t count = 1);
        size_t readBytes(Tiny3D::DataStream &stream, OgreChunkData &data, uint8_t *value, size_t count = 1);
        size_t readShorts(Tiny3D::DataStream &stream, OgreChunkData &data, uint16_t *value, size_t count = 1);
        size_t readInts(Tiny3D::DataStream &stream, OgreChunkData &data, uint32_t *value, size_t count = 1);
        size_t readFloats(Tiny3D::DataStream &stream, OgreChunkData &data, float *value, size_t count = 1);
        size_t readFloats(Tiny3D::DataStream &stream, OgreChunkData &data, double *value, size_t count = 1);
        size_t readObject(Tiny3D::DataStream &stream, OgreChunkData &data, Tiny3D::Vector3 &value);
        size_t readObject(Tiny3D::DataStream &stream, OgreChunkData &data, Tiny3D::Quaternion &value);

        String readString(Tiny3D::DataStream &stream, OgreChunkData &data);
        String readString(Tiny3D::DataStream &stream, OgreChunkData &data, size_t numChars);

        void swapEndian(void *data, size_t size, size_t count);
        void swapEndian(void *data, size_t size);

        String  mSrcPath;
        bool    mSwapEndian;
    };
}


#endif  /*__MCONV_OGRE_SERIALIZER_H__*/
