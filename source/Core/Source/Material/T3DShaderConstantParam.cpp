/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/


#include "Material/T3DShaderConstantParam.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ShaderConstantParamPtr ShaderConstantParam::create(const String &cbufferName, const String &name, uint32_t bindingPoint, uint32_t dataSize, uint32_t dataOffset, DATA_TYPE dataType)
    {
        return T3D_NEW ShaderConstantParam(cbufferName, name, bindingPoint, dataSize, dataOffset, dataType);
    }
    
    //--------------------------------------------------------------------------

    ShaderConstantParam::ShaderConstantParam(const String &cbufferName, const String &name, uint32_t bindingPoint, uint32_t dataSize, uint32_t dataOffset, DATA_TYPE dataType)
        : mDataType(dataType)
        , mDataSize(dataSize)
        , mDataOffset(dataOffset)
        , mBindingPoint(bindingPoint)
        , mName(name)
        , mCBufferName(cbufferName)
    {
    }
    
    //--------------------------------------------------------------------------

    ShaderConstantParam::~ShaderConstantParam()
    {
    }

    //--------------------------------------------------------------------------

    ShaderConstantParamPtr ShaderConstantParam::clone() const
    {
        return create(mCBufferName, mName, mBindingPoint, mDataSize, mDataOffset, mDataType);
    }

    //--------------------------------------------------------------------------

    ShaderConstantValuePtr ShaderConstantValue::create(const String &name, ShaderConstantParam::DATA_TYPE dataType, size_t dataSize)
    {
        return T3D_NEW ShaderConstantValue(name, dataType, dataSize);
    }

    //--------------------------------------------------------------------------

    ShaderConstantValue::ShaderConstantValue(const String &name, ShaderConstantParam::DATA_TYPE type, size_t dataSize)
        : mName(name)
        , mDataType(type)
    {
        mValue.DataSize = dataSize;
        mValue.Data = T3D_POD_NEW_ARRAY(uint8_t, mValue.DataSize);
    }

    //--------------------------------------------------------------------------

    ShaderConstantValue::~ShaderConstantValue()
    {
        mValue.release();
    }

    //--------------------------------------------------------------------------

    ShaderConstantValuePtr ShaderConstantValue::clone() const
    {
        ShaderConstantValuePtr newValue = create(getName(), getDataType(), mValue.DataSize);

        if (newValue != nullptr)
        {
            memcpy(newValue->mValue.Data, mValue.Data, mValue.DataSize);
        }
        
        return newValue;
    }
    
    //--------------------------------------------------------------------------
}


