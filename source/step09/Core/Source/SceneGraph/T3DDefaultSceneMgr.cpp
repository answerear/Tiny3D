/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Agent)
 * Copyright (C) 2015-2019  Answer Wong
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


#include "SceneGraph/T3DDefaultSceneMgr.h"
#include "Render/T3DRenderQueue.h"
#include "SceneGraph/T3DSGTransform3D.h"
#include "SceneGraph/T3DSGCamera.h"
#include "SceneGraph/T3DSGLight.h"
#include "SceneGraph/T3DSGModel.h"
#include "SceneGraph/T3DSGMesh.h"
#include "SceneGraph/T3DSGAxis.h"
#include "SceneGraph/T3DSGBillboard.h"
#include "SceneGraph/T3DSGQuad.h"
#include "SceneGraph/T3DSGBox.h"
#include "SceneGraph/T3DSGSphere.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    DefaultSceneMgrPtr DefaultSceneMgr::create()
    {
        DefaultSceneMgrPtr mgr = new DefaultSceneMgr();
        mgr->release();
        return mgr;
    }

    //--------------------------------------------------------------------------

    DefaultSceneMgr::DefaultSceneMgr()
        : mRoot(nullptr)
        , mCurCamera(nullptr)
        , mRenderQueue(nullptr)
    {

    }

    //--------------------------------------------------------------------------

    DefaultSceneMgr::~DefaultSceneMgr()
    {
        mRoot = nullptr;
        mCurCamera = nullptr;
        mRenderQueue = nullptr;
    }

    //--------------------------------------------------------------------------

    TResult DefaultSceneMgr::renderScene(SGCameraPtr camera)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    SGNodePtr DefaultSceneMgr::getRoot() const
    {
        return mRoot;
    }

    //--------------------------------------------------------------------------

    SGTransform3DPtr DefaultSceneMgr::createTransform3D(SGNodePtr parent,
        ID uID /* = Node::E_NID_AUTOMATIC */)
    {
        SGTransform3DPtr node = SGTransform3D::create(uID);

        if (parent != nullptr)
        {
            parent->addChild(node);
        }

        return node;
    }

    //--------------------------------------------------------------------------

    SGCameraPtr DefaultSceneMgr::createCamera(SGNodePtr parent,
        ID uID /* = Node::E_NID_AUTOMATIC */)
    {
        SGCameraPtr node = SGCamera::create(uID);

        if (parent != nullptr)
        {
            parent->addChild(node);
        }

        return node;
    }

    //--------------------------------------------------------------------------

    SGLightPtr DefaultSceneMgr::createLight(SGNodePtr parent, 
        ID uID /* = Node::E_NID_AUTOMATIC */)
    {
        SGLightPtr node = SGLight::create(uID);

        if (parent != nullptr)
        {
            parent->addChild(node);
        }

        return node;
    }

    //--------------------------------------------------------------------------

    SGModelPtr DefaultSceneMgr::createModel(SGNodePtr parent, 
        ID uID /* = Node::E_NID_AUTOMATIC */)
    {
        SGModelPtr node = SGModel::create(uID);

        if (parent != nullptr)
        {
            parent->addChild(node);
        }

        return node;
    }

    //--------------------------------------------------------------------------

    SGMeshPtr DefaultSceneMgr::createMesh(SGNodePtr parent, 
        ID uID /* = Node::E_NID_AUTOMATIC */)
    {
        SGMeshPtr node = SGMesh::create(uID);

        if (parent != nullptr)
        {
            parent->addChild(node);
        }

        return node;
    }

    //--------------------------------------------------------------------------

    SGAxisPtr DefaultSceneMgr::createAxis(SGNodePtr parent, 
        ID uID /* = Node::E_NID_AUTOMATIC */)
    {
        SGAxisPtr node = SGAxis::create(uID);

        if (parent != nullptr)
        {
            parent->addChild(node);
        }

        return node;
    }

    //--------------------------------------------------------------------------

    SGQuadPtr DefaultSceneMgr::createQuad(SGNodePtr parent, 
        ID uID /* = Node::E_NID_AUTOMATIC */)
    {
        SGQuadPtr node = SGQuad::create(uID);

        if (parent != nullptr)
        {
            parent->addChild(node);
        }

        return node;
    }

    //--------------------------------------------------------------------------

    SGBillboardPtr DefaultSceneMgr::createBillboard(SGNodePtr parent, 
        ID uID /* = Node::E_NID_AUTOMATIC */)
    {
        SGBillboardPtr node = SGBillboard::create(uID);

        if (parent != nullptr)
        {
            parent->addChild(node);
        }

        return node;
    }

    //--------------------------------------------------------------------------

    SGBoxPtr DefaultSceneMgr::createBox(SGNodePtr parent, 
        ID uID /* = Node::E_NID_AUTOMATIC */)
    {
        SGBoxPtr node = SGBox::create(uID);

        if (parent != nullptr)
        {
            parent->addChild(node);
        }

        return node;
    }

    //--------------------------------------------------------------------------

    SGSpherePtr DefaultSceneMgr::createSphere(SGNodePtr parent, 
        ID uID /* = Node::E_NID_AUTOMATIC */)
    {
        SGSpherePtr node = SGSphere::create(uID);

        if (parent != nullptr)
        {
            parent->addChild(node);
        }

        return node;
    }
}

