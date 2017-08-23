/***************************************************************************************************
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
 **************************************************************************************************/

#include "SceneGraph/T3DSGTransform2D.h"
#include "SceneGraph/T3DSGCamera.h"
#include "Misc/T3DEntrance.h"
#include "Render/T3DRenderer.h"


namespace Tiny3D
{
    SGTransform2DPtr SGTransform2D::create(uint32_t unID /* = E_NID_AUTOMATIC */)
    {
        SGTransform2D *node = new SGTransform2D(unID);
        SGTransform2DPtr ptr(node);
        node->release();
        return ptr;
    }

    SGTransform2D::SGTransform2D(uint32_t unID /* = E_NID_AUTOMATIC */)
        : SGNode(unID)
        , mPosition(Vector2::ZERO)
        , mOrientation(Quaternion::IDENTITY)
        , mScale(Real(1.0), Real(1.0))
    {
        mWorldTransform.setTranslate(Vector3(mPosition.x(), mPosition.y(), Real(0.0)));
        mWorldTransform.setOrientation(mOrientation);
        mWorldTransform.setScale(Vector3(mScale.x(), mScale.y(), Real(1.0)));
    }

    SGTransform2D::~SGTransform2D()
    {

    }

    Node::Type SGTransform2D::getNodeType() const
    {
        return E_NT_TRANSFORM2D;
    }

    void SGTransform2D::onAttachParent(const NodePtr &parent)
    {
        SGNode::onAttachParent(parent);
        setDirty(true, true);
    }

    void SGTransform2D::onDetachParent(const NodePtr &parent)
    {
        SGNode::onDetachParent(parent);
    }

    void SGTransform2D::updateTransform()
    {
        getLocalToWorldTransform();
        SGNode::updateTransform();
    }

    const Transform &SGTransform2D::getLocalToWorldTransform()
    {
        if (isDirty())
        {
            Node *parent = getParent();

            // 这里就算是3D变换结点也直接跳过忽略，因为这里不受Z值影响的变换
            while (parent != nullptr && (parent->getNodeType() != E_NT_TRANSFORM2D))
                parent = parent->getParent();

            RendererPtr renderer = T3D_ENTRANCE.getActiveRenderer();
            ViewportPtr viewport = renderer->getViewport();
            SGCameraPtr camera = viewport->getCamera();

            Real width = Real(viewport->getActualWidth()) / camera->getAspectRatio();// *Real(0.5);
            Real height = Real(viewport->getActualHeight());

            Real x = mPosition.x() / width;
            Real y = mPosition.y() / height;
            Real z(0.0);
            Vector3 pos(x, y, z);
            Vector3 scal(mScale.x(), mScale.y(), Real(1.0));

            if (parent != nullptr)
            {
                SGTransform2D *node = (SGTransform2D *)parent;
                const Transform &transform = node->getLocalToWorldTransform();
                mWorldTransform.applyTransform(transform, pos, mOrientation, scal);
            }
            else
            {
                mWorldTransform.setTranslate(pos);
                mWorldTransform.setOrientation(mOrientation);
                mWorldTransform.setScale(scal);
                mWorldTransform.update();
            }

            setDirty(false);
        }

        return mWorldTransform;
    }

    NodePtr SGTransform2D::clone() const
    {
        SGTransform2DPtr node = create();
        cloneProperties(node);
        return node;
    }

    void SGTransform2D::cloneProperties(const NodePtr &node) const
    {
        SGNode::cloneProperties(node);

        const SGTransform2DPtr &newNode = smart_pointer_cast<SGTransform2D>(node);
        newNode->mPosition = mPosition;
        newNode->mOrientation = mOrientation;
        newNode->mScale = mScale;
        newNode->mWorldTransform = mWorldTransform;
    }
}

