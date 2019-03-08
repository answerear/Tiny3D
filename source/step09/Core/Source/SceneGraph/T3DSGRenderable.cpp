/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
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


#include "SceneGraph/T3DSGRenderable.h"
#include "SceneGraph/T3DDefaultSceneMgr.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    SGRenderable::SGRenderable(ID uID /* = E_NID_AUTOMATIC */)
        : SGTransform3D(uID)
        , mPrev(nullptr)
        , mNext(nullptr)
    {

    }

    //--------------------------------------------------------------------------

    SGRenderable::~SGRenderable()
    {

    }

    //--------------------------------------------------------------------------

    void SGRenderable::setCameraMask(uint32_t mask)
    {
        if (mCameraMask == 0)
        {
            // 之前没有设置过，直接添加到默认场景里面
            mCameraMask = mask;

            if (getParent() != nullptr)
            {
                DefaultSceneMgr::getInstance().addRenderable(this);
            }
        }
        else if (mCameraMask != mask)
        {
            if (getParent() != nullptr)
            {
                DefaultSceneMgr::getInstance().removeRenderable(this);
                mCameraMask = mask;
                DefaultSceneMgr::getInstance().addRenderable(this);

                NodePtr node = getFirstChild();

                while (node != nullptr)
                {
                    SGNodePtr child = smart_pointer_cast<SGNode>(node);
                    child->setCameraMask(mask);
                    node = node->getNextSibling();
                }
            }
            else
            {
                mCameraMask = mask;
            }
        }
    }

    //--------------------------------------------------------------------------

    void SGRenderable::onAttachParent(NodePtr parent)
    {
        if (getCameraMask() != 0)
        {
            DefaultSceneMgr::getInstance().addRenderable(this);
        }
    }

    //--------------------------------------------------------------------------

    void SGRenderable::onDetachParent(NodePtr parent)
    {
        if (getCameraMask() != 0)
        {
            DefaultSceneMgr::getInstance().removeRenderable(this);
        }
    }
}
