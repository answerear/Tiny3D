/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
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

#include "SceneGraph/T3DSGMesh.h"
#include "Resource/T3DModel.h"
#include "Resource/T3DModelManager.h"
#include "Resource/T3DMaterial.h"
#include "Resource/T3DMaterialManager.h"
#include "Misc/T3DModelData.h"
#include "Render/T3DHardwareBufferManager.h"


namespace Tiny3D
{
    SGMeshPtr SGMesh::create(VertexDataPtr vertexData, ObjectPtr submeshData, uint32_t uID /* = E_NID_AUTOMATIC */)
    {
        SGMeshPtr mesh = new SGMesh(uID);
        if (mesh != nullptr && mesh->init(vertexData, submeshData))
        {
            mesh->release();
        }
        else
        {
            T3D_SAFE_RELEASE(mesh);
        }
        return mesh;
    }

    SGMesh::SGMesh(uint32_t uID /* = E_NID_AUTOMATIC */)
        : SGGeometry(uID)
        , mSubMeshData(nullptr)
        , mMaterial(nullptr)
    {

    }

    SGMesh::~SGMesh()
    {
    }

    bool SGMesh::init(VertexDataPtr vertexData, ObjectPtr subData)
    {
        bool ret = false;

        mVertexData = vertexData;
        mSubMeshData = subData;

        SubMeshDataPtr submeshData = smart_pointer_cast<SubMeshData>(mSubMeshData);

        HardwareIndexBuffer::Type indexType = HardwareIndexBuffer::E_IT_32BITS;
        size_t indexCount = submeshData->mIndices.size() / sizeof(uint32_t);

        if (submeshData->mIs16Bits)
        {
            indexType = HardwareIndexBuffer::E_IT_16BITS;
            indexCount = submeshData->mIndices.size() / sizeof(uint16_t);
        }

        HardwareIndexBufferPtr indexBuffer = T3D_HARDWARE_BUFFER_MGR.createIndexBuffer(indexType, indexCount, HardwareBuffer::E_HBU_STATIC_WRITE_ONLY, false);

        if (indexBuffer != nullptr)
        {
            size_t indexSize = submeshData->mIndices.size();
            ret = indexBuffer->writeData(0, indexSize, &submeshData->mIndices[0]);

            if (ret)
            {
                mIndexData = IndexData::create(indexBuffer);
                mMaterial = T3D_MATERIAL_MGR.loadMaterial(submeshData->mMaterialName, Material::E_MT_DEFAULT);
            }
        }

        return ret;
    }

    Node::Type SGMesh::getNodeType() const
    {
        return E_NT_MESH;
    }

    NodePtr SGMesh::clone() const
    {
        SGMeshPtr mesh = create(mVertexData, mSubMeshData);
        cloneProperties(mesh);
        return mesh;
    }

    void SGMesh::cloneProperties(const NodePtr &node) const
    {
        SGGeometry::cloneProperties(node);
    }

    const String &SGMesh::getSubMeshName() const
    {
        SubMeshDataPtr submeshData = smart_pointer_cast<SubMeshData>(mSubMeshData);
        return submeshData->mName;
    }

    MaterialPtr SGMesh::getMaterial() const
    {
        return mMaterial;
    }

    Renderer::PrimitiveType SGMesh::getPrimitiveType() const
    {
        SubMeshDataPtr submeshData = smart_pointer_cast<SubMeshData>(mSubMeshData);
        return submeshData->mPrimitiveType;
    }

    VertexDataPtr SGMesh::getVertexData() const
    {
        return mVertexData;
    }

    IndexDataPtr SGMesh::getIndexData() const
    {
        return mIndexData;
    }

    bool SGMesh::isIndicesUsed() const
    {
        return (mIndexData != nullptr);
    }

    void SGMesh::updateTransform()
    {
        SGGeometry::updateTransform();
    }
}
