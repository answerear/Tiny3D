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

#ifndef __T3D_RENDER_STATE_H__
#define __T3D_RENDER_STATE_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Kernel/T3DObject.h"
#include "Kernel/T3DCommon.h"


namespace Tiny3D
{
    class T3D_ENGINE_API HWBlendState : public Object
    {
    public:
        virtual ~HWBlendState();

    protected:
        HWBlendState();
    };

    class T3D_ENGINE_API HWDepthStencilState : public Object
    {
    public:
        virtual ~HWDepthStencilState();

        virtual void setDepthTestEnabled(bool enabled) = 0;
        virtual bool isDepthTestEnabled() const = 0;

        virtual void setDepthWriteEnabled(bool enabled) = 0;
        virtual bool isDepthWriteEnabled() const = 0;

        virtual void setDepthFunction(CompareFunction func) = 0;
        virtual CompareFunction getDepthFunction() const = 0;

        virtual void setStencilEnabled(bool enabled) = 0;
        virtual bool isStencilEnabled() const = 0;

        virtual void setStencilReadMask(uint8_t mask) = 0;
        virtual uint8_t getStencilReadMask() const = 0;

        virtual void setStencilWriteMask(uint8_t mask) = 0;
        virtual uint8_t getStencilWriteMask() const = 0;

        virtual void setStencilRef(uint8_t ref) = 0;
        virtual uint8_t getStencilRef() const = 0;

        virtual void setStencilFunction(CompareFunction func) = 0;
        virtual CompareFunction getStencilFunction() const = 0;

        virtual void setStencilOp(StencilOp stencilFail, StencilOp depthFail, StencilOp pass) = 0;

    protected:
        HWDepthStencilState();
    };

    class T3D_ENGINE_API HWRasterizerState : public Object
    {
    public:
        virtual ~HWRasterizerState();

    protected:
        HWRasterizerState();
    };

    class T3D_ENGINE_API HWSamplerState : public Object
    {
    public:
        virtual ~HWSamplerState();

    protected:
        HWSamplerState();
    };
}


#endif  /*__T3D_RENDER_STATE_H__*/
