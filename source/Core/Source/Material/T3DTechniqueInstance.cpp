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


#include "Material/T3DTechniqueInstance.h"
#include "Material/T3DPassInstance.h"
#include "Material/T3DTechnique.h"
#include "Material/T3DPass.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    TechniqueInstancePtr TechniqueInstance::create(Material *parent, TechniquePtr tech)
    {
        return new TechniqueInstance(parent, tech);
    }

    //--------------------------------------------------------------------------

    TechniqueInstance::TechniqueInstance(Material *parent, TechniquePtr tech)
        : mMaterial(parent)
        , mTechnique(tech)
    {
        for (const auto &pass : tech->getPasses())
        {
            PassInstancePtr instance = PassInstance::create(this, pass);
            mPassInstances.emplace_back(instance);
        }
    }

    //--------------------------------------------------------------------------
    
    TResult TechniqueInstance::switchKeywords(const StringArray &enableKeys, const StringArray &disableKeys)
    {
        TResult ret = T3D_OK;

        for (auto instance : mPassInstances)
        {
            ret = instance->switchKeywords(enableKeys, disableKeys);
            if (T3D_FAILED(ret))
            {
                break;
            }
        }
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void TechniqueInstance::setBool(const String &name, bool value)
    {
        for (const auto pass : mPassInstances)
        {
            pass->setBool(name, value);
        }
    }

    //--------------------------------------------------------------------------
    
    void TechniqueInstance::setBoolArray(const String &name, const BoolArray &values)
    {
        for (const auto pass : mPassInstances)
        {
            pass->setBoolArray(name, values);
        }
    }

    //--------------------------------------------------------------------------
    
    void TechniqueInstance::setFloat(const String &name, float32_t value)
    {
        for (auto pass : mPassInstances)
        {
            pass->setFloat(name, value);
        }
    }

    //--------------------------------------------------------------------------
    
    void TechniqueInstance::setFloatArray(const String &name, const FloatArray &values)
    {
        for (const auto pass : mPassInstances)
        {
            pass->setFloatArray(name, values);
        }
    }

    //--------------------------------------------------------------------------
    
    void TechniqueInstance::setInteger(const String &name, int32_t value)
    {
        for (const auto pass : mPassInstances)
        {
            pass->setInteger(name, value);
        }
    }

    //--------------------------------------------------------------------------
    
    void TechniqueInstance::setIntArray(const String &name, const IntArray &values)
    {
        for (const auto pass : mPassInstances)
        {
            pass->setIntArray(name, values);
        }
    }

    //--------------------------------------------------------------------------
    
    void TechniqueInstance::setColor(const String &name, const ColorRGBA &value)
    {
        for (const auto pass : mPassInstances)
        {
            pass->setColor(name, value);
        }
    }

    //--------------------------------------------------------------------------
    
    void TechniqueInstance::setColorArray(const String &name, const ColorArray &values)
    {
        for (const auto pass : mPassInstances)
        {
            pass->setColorArray(name, values);
        }
    }

    //--------------------------------------------------------------------------
    
    void TechniqueInstance::setVector(const String &name, const Vector4 &value)
    {
        for (const auto pass : mPassInstances)
        {
            pass->setVector(name, value);
        }
    }

    //--------------------------------------------------------------------------
    
    void TechniqueInstance::setVectorArray(const String &name, const Vector4Array &values)
    {
        for (const auto pass : mPassInstances)
        {
            pass->setVectorArray(name, values);
        }
    }

    //--------------------------------------------------------------------------
    
    void TechniqueInstance::setMatrix(const String &name, const Matrix4 &value)
    {
        for (const auto pass : mPassInstances)
        {
            pass->setMatrix(name, value);
        }
    }

    //--------------------------------------------------------------------------
    
    void TechniqueInstance::setMatrixArray(const String &name, const Matrix4Array &values)
    {
        for (const auto pass : mPassInstances)
        {
            pass->setMatrixArray(name, values);
        }
    }
    
    //--------------------------------------------------------------------------

    void TechniqueInstance::setData(const String &name, const void *data, uint32_t dataSize)
    {
        for (const auto pass : mPassInstances)
        {
            pass->setData(name, data, dataSize);
        }
    }

    //--------------------------------------------------------------------------

    void TechniqueInstance::setTexture(const String &name, const UUID &uuid)
    {
        for (const auto &pass : mPassInstances)
        {
            pass->setTexture(name, uuid);
        }
    }
    
    //--------------------------------------------------------------------------
}


