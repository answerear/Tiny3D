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


#include "Component/T3DComponent.h"
#include "Scene/T3DSceneNode.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    T3D_IMPLEMENT_CLASS_1(Component, Object);

    //--------------------------------------------------------------------------

    ID Component::makeGlobalID() const
    {
        static ID uID = 0;
        return ++uID;
    }

    //--------------------------------------------------------------------------

    Component::Component(ID uID /* = E_CID_AUTOMATIC */)
        : mID(E_CID_INVALID)
        , mSceneNode(nullptr)
    {
        if (uID != E_CID_AUTOMATIC)
        {
            mID = uID;
        }
        else
        {
            mID = makeGlobalID();
        }
    }

    //--------------------------------------------------------------------------

    Component::~Component()
    {
    }

    //--------------------------------------------------------------------------

    TResult Component::cloneProperties(ComponentPtr newObj) const
    {
        newObj->mSceneNode = mSceneNode;
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void Component::onAttachSceneNode(SceneNode *node)
    {
        mSceneNode = node;
    }

    //--------------------------------------------------------------------------

    void Component::onDetachSceneNode(SceneNode *node)
    {
        mSceneNode = nullptr;
    }

    //--------------------------------------------------------------------------

    void Component::update()
    {

    }
}


//------------------------------------------------------------------------------
//                                  RTTR
//------------------------------------------------------------------------------

RTTR_REGISTRATION
{
    using namespace rttr;

    registration::class_<Tiny3D::Component>("Tiny3D::Component")
        .enumeration<Tiny3D::Component::ComponentID>("Tiny3D::Component::ComponentID")
        (
            value("E_CID_AUTOMATIC", Tiny3D::Component::ComponentID::E_CID_AUTOMATIC),
            value("E_CID_INVALID", Tiny3D::Component::ComponentID::E_CID_INVALID)
        )
        .method("clone", &Tiny3D::Component::clone)
        .method("getID", &Tiny3D::Component::getID)
        .method("getSceneNode", &Tiny3D::Component::getSceneNode)
        .property("ID", &Tiny3D::Component::mID, registration::protected_access)
        (
            metadata(TXT_DESCRIPTION, "The identifier of the component.")
        );
}

