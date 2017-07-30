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

#include "SceneGraph/T3DSGSkeleton.h"
#include "Render/T3DHardwareBufferManager.h"
#include "Render/T3DHardwareVertexBuffer.h"
#include "Resource/T3DModel.h"
#include "SceneGraph/T3DSGBone.h"


namespace Tiny3D
{
    SGSkeletonPtr SGSkeleton::create(SGBonePtr bone, uint32_t uID /* = E_NID_AUTOMATIC */)
    {
        SGSkeletonPtr skeleton = new SGSkeleton();

        if (skeleton != nullptr && skeleton->init(bone))
        {
            skeleton->release();
        }
        else
        {
            T3D_SAFE_RELEASE(skeleton);
        }

        return skeleton;
    }

    SGSkeleton::SGSkeleton(uint32_t uID /* = E_NID_AUTOMATIC */)
        : SGGeometry(uID)
        , mSkeleton(nullptr)
    {

    }

    SGSkeleton::~SGSkeleton()
    {

    }

    bool SGSkeleton::init(SGBonePtr bone)
    {
        mSkeleton = bone;

        std::vector<BoneVertex> vertices;
        bool ret = buildSkeletonVertices(bone, vertices);

        if (ret)
        {
            HardwareVertexBufferPtr vb = T3D_HARDWARE_BUFFER_MGR.createVertexBuffer(sizeof(BoneVertex), vertices.size(), HardwareVertexBuffer::E_HBU_DYNAMIC, false);
            ret = vb->writeData(0, sizeof(BoneVertex) * vertices.size(), &vertices[0]);

            VertexDeclarationPtr decl = T3D_HARDWARE_BUFFER_MGR.createVertexDeclaration();
            decl->addElement(VertexElement(0, 0, VertexElement::E_VET_FLOAT3, VertexElement::E_VES_POSITION));
            decl->addElement(VertexElement(0, 12, VertexElement::E_VET_COLOR, VertexElement::E_VES_DIFFUSE));

            mVertexData = VertexData::create(decl);
            mVertexData->addVertexBuffer(vb);
        }

        return ret;
    }

    bool SGSkeleton::buildSkeletonVertices(SGBonePtr bone, std::vector<BoneVertex> &vertices)
    {
        SGBonePtr parentBone = smart_pointer_cast<SGBone>(bone->getParent());

        if (parentBone != nullptr)
        {
            const Transform &t0 = parentBone->getLocalToWorldTransform();
            const Transform &t1 = bone->getLocalToWorldTransform();

            const Vector3 &p0 = t0.getTranslate();
            const Vector3 &p1 = t1.getTranslate();

            BoneVertex vertex;
            vertex.position = p0;
            vertex.color = Color4::WHITE;
            vertices.push_back(vertex);
            vertex.position = p1;
            vertex.color = Color4::WHITE;
            vertices.push_back(vertex);

            T3D_LOG_DEBUG("Bone from joint %s (%.6f, %.6f, %.6f) to joint %s (%.6f, %.6f, %.6f)", 
                parentBone->getName().c_str(), p0[0], p0[1], p0[2], bone->getName().c_str(), p1[0], p1[1], p1[2]);
        }

        bool ret = true;

        const Children &children = bone->getChildren();
        auto itr = children.begin();
        while (itr != children.end())
        {
            SGBonePtr child = smart_pointer_cast<SGBone>(*itr);
            ret = ret && buildSkeletonVertices(child, vertices);
            ++itr;
        }

        return true;
    }

    void SGSkeleton::updateTransform()
    {
        updateVertices();
    }

    void SGSkeleton::updateVertices()
    {
        std::vector<BoneVertex> vertices;
        bool ret = buildSkeletonVertices(mSkeleton, vertices);

        if (ret)
        {
            HardwareVertexBufferPtr vb = mVertexData->getVertexBuffer(0);
            ret = vb->writeData(0, sizeof(BoneVertex) * vertices.size(), &vertices[0]);
        }
    }

    SGNode::Type SGSkeleton::getNodeType() const
    {
        return E_NT_SKELETON;
    }

    NodePtr SGSkeleton::clone() const
    {
        SGSkeletonPtr skeleton = SGSkeleton::create(mSkeleton);
        cloneProperties(skeleton);
        return skeleton;
    }

    void SGSkeleton::cloneProperties(const NodePtr &node) const
    {
        SGGeometry::cloneProperties(node);
    }

    MaterialPtr SGSkeleton::getMaterial() const
    {
        return nullptr;
    }

    Renderer::PrimitiveType SGSkeleton::getPrimitiveType() const
    {
        return Renderer::E_PT_LINE_LIST;
    }

    VertexDataPtr SGSkeleton::getVertexData() const
    {
        return mVertexData;
    }

    IndexDataPtr SGSkeleton::getIndexData() const
    {
        return nullptr;
    }

    bool SGSkeleton::isIndicesUsed() const
    {
        return false;
    }
}
