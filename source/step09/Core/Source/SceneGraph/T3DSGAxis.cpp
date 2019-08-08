﻿/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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


#include "SceneGraph/T3DSGAxis.h"
#include "Render/T3DRenderQueue.h"
#include "Render/T3DHardwareBufferManager.h"
#include "Render/T3DHardwareVertexBuffer.h"
#include "Render/T3DHardwareIndexBuffer.h"
#include "Render/T3DVertexArrayObject.h"
#include "Bound/T3DSphereBound.h"
#include "Resource/T3DGPUProgram.h"
#include "Resource/T3DGPUProgramManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    /** 坐标轴顶点数据格式 */
    struct AxisVertex
    {
        AxisVertex()
            : position(Vector3::ZERO)
            , diffuse(ColorRGB::WHITE)
        {
            
        }

        Vector3     position;   /**< 位置 */
        ColorRGB     diffuse;    /**< 颜色 */
    };

    //--------------------------------------------------------------------------

    SGAxisPtr SGAxis::create(Real X, Real Y, Real Z, 
        ID uID /* = E_NID_AUTOMATIC */)
    {
        SGAxisPtr axis = new SGAxis(uID);
        axis->release();

        if (axis->init(X, Y, Z) != T3D_OK)
        {
            axis = nullptr;
        }

        return axis;
    }

    //--------------------------------------------------------------------------

    SGAxis::SGAxis(ID uID /* = E_NID_AUTOMATIC */)
        : SGRenderable(uID)
        , mVAO(nullptr)
        , mBound(nullptr)
    {

    }

    //--------------------------------------------------------------------------

    SGAxis::~SGAxis()
    {
        mVAO = nullptr;
    }

    //--------------------------------------------------------------------------

    TResult SGAxis::init(Real X, Real Y, Real Z)
    {
        TResult ret = T3D_OK;

        const size_t MAX_VERTICES = 6;

        AxisVertex vertices[MAX_VERTICES];

        // 设置三个坐标轴顶点数据
        vertices[0].diffuse = ColorRGB::RED;
        vertices[1].diffuse = ColorRGB::RED;

        vertices[2].diffuse = ColorRGB::GREEN;
        vertices[3].diffuse = ColorRGB::GREEN;

        vertices[4].diffuse = ColorRGB::BLUE;
        vertices[5].diffuse = ColorRGB::BLUE;

        vertices[1].position[0] = X;
        vertices[3].position[1] = Y;
        vertices[5].position[3] = Z;

        do 
        {
            mVAO = T3D_HARDWARE_BUFFER_MGR.createVertexArrayObject(false);
            if (mVAO == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                T3D_LOG_ERROR(LOG_TAG_SCENE, "Create VAO for SGAxis failed !");
                break;
            }

            ret = mVAO->beginBinding();
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_SCENE, "Binding VAO for SGAxis failed !");
                break;
            }

            ShaderPtr vertexShader;

            // 创建顶点声明
            VertexDeclarationPtr decl
                = T3D_HARDWARE_BUFFER_MGR.createVertexDeclaration(vertexShader);
            if (decl == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                T3D_LOG_ERROR(LOG_TAG_SCENE, "Create vertex declaration for \
                    SGAxis failed !");
                break;
            }

            decl->addAttribute(VertexAttribute(0, 0, 
                VertexAttribute::E_VAT_FLOAT3,
                VertexAttribute::E_VAS_POSITION, 0));
            decl->addAttribute(VertexAttribute(0, sizeof(Vector3),
                VertexAttribute::E_VAT_COLOR, 
                VertexAttribute::E_VAS_DIFFUSE, 0));

            // 创建VBO
            HardwareVertexBufferPtr vbo
                = T3D_HARDWARE_BUFFER_MGR.createVertexBuffer(sizeof(AxisVertex),
                    MAX_VERTICES, vertices, HardwareVertexBuffer::E_HBU_STATIC, 
                    false, false);
            if (vbo == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                T3D_LOG_ERROR(LOG_TAG_SCENE, "Create vertex buffer for \
                    SGAxis failed !");
                break;
            }

//             size_t size = sizeof(AxisVertex) * MAX_VERTICES;
//             size_t bytesOfWritten = vbo->writeData(0, size, vertices);
//             if (bytesOfWritten != size)
//             {
//                 ret = T3D_ERR_HW_BUFFER_WRITE;
//                 T3D_LOG_ERROR(LOG_TAG_SCENE, "Write vertices data for SGAxis \
//                     failed !");
//                 break;
//             }

            mVAO->setVertexDeclaration(decl);
            mVAO->addVertexBuffer(vbo);
            mVAO->setPrimitiveType(Renderer::E_PT_LINE_LIST);

            mVAO->endBinding();

            // 构建碰撞球，以三个轴最短边作为半径
            Real radius = REAL_INF;
            radius = Math::min(radius, X);
            radius = Math::min(radius, Y);
            radius = Math::min(radius, Z);
            mBound = SphereBound::create(this);
            mBound->setParams(Vector3::ZERO, radius);

            // 需要刷新碰撞体的世界变换
            setDirty(true);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    Node::Type SGAxis::getNodeType() const
    {
        return E_NT_AXIS;
    }

    //--------------------------------------------------------------------------

    Real SGAxis::getAxisLength(Axis axis) const
    {
        Real length = 0.0f;

        switch (axis)
        {
        case AXIS_X:
            length = mAxisX;
            break;
        case AXIS_Y:
            length = mAxisY;
            break;
        case AXIS_Z:
            length = mAxisZ;
            break;
        default:
            break;
        }

        return length;
    }

    //--------------------------------------------------------------------------

    NodePtr SGAxis::clone() const
    {
        SGAxisPtr axis = new SGAxis();
        axis->release();

        if (cloneProperties(axis) != T3D_OK)
        {
            axis = nullptr;
        }

        return axis;
    }

    //--------------------------------------------------------------------------

    TResult SGAxis::cloneProperties(NodePtr node) const
    {
        TResult ret = SGRenderable::cloneProperties(node);

        if (ret == T3D_OK)
        {
            SGAxisPtr axis = smart_pointer_cast<SGAxis>(node);
            ret = axis->init(getAxisLength(AXIS_X), getAxisLength(AXIS_Y),
                getAxisLength(AXIS_Z));
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void SGAxis::updateTransform()
    {
        // 更新碰撞体
        if (isDirty())
        {
            mBound->updateBound(getLocalToWorldTransform());
        }

        SGRenderable::updateTransform();
    }

    //--------------------------------------------------------------------------

    void SGAxis::frustumCulling(BoundPtr bound, RenderQueuePtr queue)
    {
        if (bound->test(mBound))
        {
            // 在视锥体内，放进去渲染队列，准备渲染
            queue->addRenderable(RenderQueue::E_GRPID_INDICATOR, this);
        }
    }

    //--------------------------------------------------------------------------

    MaterialPtr SGAxis::getMaterial() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    VertexArrayObjectPtr SGAxis::getVertexArrayObject() const
    {
        return mVAO;
    }
}

