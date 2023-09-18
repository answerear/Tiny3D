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


#include "Material/T3DShaderConstantParam.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ShaderConstantParamPtr ShaderConstantParam::create(const String &name, const void* data, uint32_t dataSize, uint32_t dataOffset, DATA_TYPE dataType)
    {
        return new ShaderConstantParam(name, data, dataSize, dataOffset, dataType);
    }
    
    //--------------------------------------------------------------------------

    ShaderConstantParam::ShaderConstantParam(const String &name, const void *data, uint32_t dataSize, uint32_t dataOffset, DATA_TYPE dataType)
        : mDataType(dataType)
        , mDataOffset(dataOffset)
        , mName(name)
    {
        if (data != nullptr)
        {
            mData.setData(data, dataSize);
        }
    }

    //--------------------------------------------------------------------------

    ShaderConstantParam::~ShaderConstantParam()
    {
        
    }

    //--------------------------------------------------------------------------

    ShaderConstantParamPtr ShaderConstantParam::clone() const
    {
        return ShaderConstantParam::create(mName, mData.Data, mData.DataSize, mDataOffset, mDataType);
    }

    //--------------------------------------------------------------------------
}


