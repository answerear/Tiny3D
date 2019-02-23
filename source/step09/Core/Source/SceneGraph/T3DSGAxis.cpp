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


#include "SceneGraph/T3DSGAxis.h"
#include "Render/T3DHardwareBufferManager.h"
#include "Render/T3DHardwareVertexBuffer.h"
#include "Render/T3DHardwareIndexBuffer.h"
#include "Render/T3DVertexArray.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    SGAxisPtr SGAxis::create(Real X, Real Y, Real Z, 
        ID uID /* = E_NID_AUTOMATIC */)
    {
        SGAxisPtr axis = new SGAxis(uID);
        axis->release();

        if (axis->init(X, Y, Z) != T3D_OK)
        {
            axis->release();
            axis = nullptr;
        }

        return axis;
    }

    //--------------------------------------------------------------------------

    SGAxis::SGAxis(ID uID /* = E_NID_AUTOMATIC */)
        : SGRenderable(uID)
        , mIsVertexDirty(false)
        , mVBO(nullptr)
    {

    }

    //--------------------------------------------------------------------------

    SGAxis::~SGAxis()
    {
        mVBO = nullptr;
    }

    //--------------------------------------------------------------------------

    TResult SGAxis::init(Real X, Real Y, Real Z)
    {
        TResult ret = T3D_OK;

        // 创建VBO
        HardwareVertexBufferPtr vbo
            = T3D_HARDWARE_BUFFER_MGR.createVertexBuffer(sizeof(Vertex), 6,
                HardwareVertexBuffer::E_HBU_DYNAMIC, false);

        // 创建顶点声明
        VertexDeclarationPtr decl 
            = T3D_HARDWARE_BUFFER_MGR.createVertexDeclaration();
        decl->addAttribute(VertexAttribute(0, 0, VertexAttribute::E_VAT_FLOAT3,
            VertexAttribute::E_VAS_POSITION));
        decl->addAttribute(VertexAttribute(0, sizeof(Vector3),
            VertexAttribute::E_VAT_COLOR, VertexAttribute::E_VAS_DIFFUSE));

        // 设置三个坐标轴顶点数据
        mVertices[0].color = Color4::RED;
        mVertices[1].color = Color4::RED;

        mVertices[2].color = Color4::GREEN;
        mVertices[3].color = Color4::GREEN;

        mVertices[4].color = Color4::BLUE;
        mVertices[5].color = Color4::BLUE;

        setAxisLength(AXIS_X, X);
        setAxisLength(AXIS_Y, Y);
        setAxisLength(AXIS_Z, Z);

        updateVertices();

        return ret;
    }

    //--------------------------------------------------------------------------

    bool SGAxis::isVAOUsed() const
    {
        return false;
    }

    //--------------------------------------------------------------------------

    
}

