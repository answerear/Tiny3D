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


#ifndef __T3D_D3D11_SAMPLER_STATE_H__
#define __T3D_D3D11_SAMPLER_STATE_H__


#include "T3DD3D11Prerequisites.h"


namespace Tiny3D
{
    class D3D11SamplerCreator : public SamplerCreator
    {
        static const char * const SAMPLER_TYPE;

        /**
         * @brief 重写ShaderCreator::getType() 接口
         */
        virtual String getType() const override;

        /**
         * @brief 重写ShaderCreator::createObject() 接口
         */
        virtual SamplerPtr createObject(int32_t argc, ...) const override;
    };

    class D3D11Sampler : public Sampler
    {
    public:
        static D3D11SamplerPtr create(const String &name);

        virtual ~D3D11Sampler();

        ID3D11SamplerState *getD3DSamplerState() { return mD3DSampler; }

    protected:
        D3D11Sampler(const String &name);

        /**
         * @fn  virtual TResult Sampler::load() override;
         * @brief   重寫 Resource::load()
         * @returns 调用成功返回 T3D_OK.
         */
        virtual TResult load() override;

        /**
         * @fn  virtual TResult Sampler::unload() override;
         * @brief   重寫 Resource::unlaod()
         * @returns 调用成功返回 T3D_OK.
         */
        virtual TResult unload() override;

        /**
         * @fn  virtual ResourcePtr Sampler::clone() const override;
         * @brief   重寫 Resource::clone()
         * @returns 调用成功返回一个新的材质对象.
         */
        virtual ResourcePtr clone() const override;

    protected:
        ID3D11SamplerState  *mD3DSampler;
    };
}


#endif  /*__T3D_D3D11_SAMPLER_STATE_H__*/
