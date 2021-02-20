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


#include "Component/T3DMesh.h"
#include "Resource/T3DModel.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    T3D_IMPLEMENT_CLASS_1(Mesh, Renderable);

    //--------------------------------------------------------------------------

    MeshPtr Mesh::create(ID uID /* = E_CID_AUTOMATIC */)
    {
        MeshPtr mesh = new Mesh(uID);
        mesh->release();
        return mesh;
    }

    //--------------------------------------------------------------------------

    Mesh::Mesh(ID uID /* = E_CID_AUTOMATIC */)
        : Renderable(uID)
        , mModel(nullptr)
    {

    }

    //--------------------------------------------------------------------------

    Mesh::~Mesh()
    {

    }

    //--------------------------------------------------------------------------

    void Mesh::setModel(ModelPtr model)
    {
        if (model != mModel)
        {

        }
    }

    //--------------------------------------------------------------------------

    ComponentPtr Mesh::clone() const
    {
        MeshPtr newObj = Mesh::create();
        TResult ret = cloneProperties(newObj);

        if (T3D_FAILED(ret))
        {
            newObj = nullptr;
        }

        return newObj;
    }

    //--------------------------------------------------------------------------

    TResult Mesh::cloneProperties(ComponentPtr newObj) const
    {
        TResult ret = Renderable::cloneProperties(newObj);

        if (ret == T3D_OK)
        {

        }

        return ret;
    }

    //--------------------------------------------------------------------------

    MaterialPtr Mesh::getMaterial() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    VertexArrayObjectPtr Mesh::getVertexArrayObject() const
    {
        return nullptr;
    }
}
