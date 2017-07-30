/*******************************************************************************
 * This file is part of Mesh-converter (A mesh converter for Tiny3D Engine)
 * Copyright (C) 2015-2017  Answer Wong
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

#ifndef __MCONV_OGRE_CONVERTER_H__
#define __MCONV_OGRE_CONVERTER_H__


#include "mconv_converter.h"
#include "mconv_vertex.h"
#include "mconv_ogredata.h"


namespace mconv
{
    class Serializer;
    class Node;
    class Mesh;
    class SubMeshes;
    class SubMesh;
    class Model;
    class Vertex;
    class VertexBuffers;
    class VertexBuffer;
    class Bone;
    class Skeleton;
    class Material;
    class Materials;

    struct OgreMesh;
    struct OgreSubMesh;
    struct OgreGeometry;
    struct OgreVertexBuffer;
    struct OgreVertexElement;
    struct OgreSkeleton;

    class OgreConverter : public ConverterImpl
    {
    public:
        OgreConverter(const Settings &settings);
        virtual ~OgreConverter();

    protected:
        virtual bool importScene() override;
        virtual bool convertToT3D() override;
        virtual bool exportScene() override;
        virtual void cleanup() override;

        bool processOgreMesh(const OgreMesh &mesh, Node *pRoot);
        bool processOgreGeometry(const OgreGeometry &geometry, Mesh *pMesh);
        bool processOgreVertexAttributes(const OgreGeometry &geometry, VertexBuffer *pVertexBuffer, size_t source);
        bool putVertexAttribute(const OgreVertexElement &element, VertexBuffer *pVertexBuffer);
        bool processOgreVertexBuffer(const OgreVertexBuffer &buffer, VertexBuffer *pVertexBuffer, size_t source);
        bool putVertexData(const std::vector<float> &vertices, size_t &index, const VertexAttributes &attributes, Vertex &vertex);
        bool processVertexSemantic(uint16_t semantic, VertexAttribute &attribute);
        bool processVertexType(uint16_t type, VertexAttribute &attribute);
        bool createMesh(Model *pModel, Mesh *&pMesh, SubMeshes *&pSubMeshes, size_t index);
        bool processOgreSubMeshes(const OgreMesh &mesh, Model *pModel);
        bool processOgreSubMesh(const OgreSubMesh &submesh, Mesh *pMesh, SubMeshes *pSubMeshes);
        bool processOgreBoneAssignment(const std::vector<OgreBoneAssignment> &assignments, Mesh *pMesh);

        bool processOgreSkeleton(const OgreSkeleton &skeleton, Model *pModel);
        bool processOgreSkin(const OgreSkeleton &skeleton, Model *pModel);
        bool processBone(Bone *pBone, const Matrix4 &m);
        bool processOgreBones(const OgreSkeleton &skeleton, Model *pModel);

        bool processOgreAnimations(const OgreSkeleton &skeleton, Model *pModel);

        bool searchVertexBuffer(Mesh *pMesh, VertexBuffer *&pVertexBuffer);
        bool searchVertexAttribute(VertexBuffer *pVertexBuffer, VertexAttribute::VertexType type);
        void putVertexBlendAndWeightAttributes(VertexBuffer *pVB);

        bool processOgreMaterial(const String &name, Node *pParent);
        bool convertOgreMaterial(const String &name, Material *pMaterial);

    protected:
        std::vector<Bone*> mBones;
        
        Serializer  *mMtrlImporter;

        Materials   *mCurMaterials;

        void    *mSrcData;
        void    *mDstData;

        void    *mSrcMtrlData;
    };
}


#endif  /*__MCONV_OGRE_CONVERTER_H__*/
