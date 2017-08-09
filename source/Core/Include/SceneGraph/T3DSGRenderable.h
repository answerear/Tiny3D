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

#ifndef __T3D_SG_RENDERABLE_H__
#define __T3D_SG_RENDERABLE_H__


#include "SceneGraph/T3DSGNode.h"
#include "Resource/T3DMaterial.h"
#include "Render/T3DIndexData.h"
#include "Render/T3DVertexData.h"
#include "Render/T3DRenderer.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGRenderable : public SGNode
    {
    protected:
        SGRenderable(uint32_t unID = E_NID_AUTOMATIC);

    public:
        /**
         * @brief Destructor
         */
        virtual ~SGRenderable();

        /**
         * @brief ������Ⱦ���ʶ���
         * @note ���п���Ⱦ��������Ҫ��д���ӿ��Է��ض�Ӧ����Ⱦ���ʶ���
         */
        virtual MaterialPtr getMaterial() const = 0;

        /**
         * @brief ���ص�ǰ��Ⱦ��������仯����
         */
        virtual const Matrix4 &getWorldMatrix() const;

        /**
         * @brief ������ȾԪ����
         * @see Renderer::PrimitiveType
         */
        virtual Renderer::PrimitiveType getPrimitiveType() const = 0;

        /**
         * @brief ���ض������ݶ���
         */
        virtual VertexDataPtr getVertexData() const = 0;

        /**
         * @brief �����������ݶ���
         */
        virtual IndexDataPtr getIndexData() const = 0;

        /**
         * @brief �Ƿ�ʹ�ö�������
         */
        virtual bool isIndicesUsed() const = 0;
    };
}


#endif  /*__T3D_SG_RENDERABLE_H__*/

