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

#ifndef __T3D_TECHNIQUE_INSTANCE_H__
#define __T3D_TECHNIQUE_INSTANCE_H__


#include "T3DTypedef.h"


namespace Tiny3D
{
    class T3D_ENGINE_API TechniqueInstance : public Object
    {
    public:
        static TechniqueInstancePtr create(Material *parent, TechniquePtr tech);

        ~TechniqueInstance() override;

        TechniqueInstancePtr clone(Material *material) const;

        Material *getMaterial() const { return mMaterial; }
        
        TechniquePtr getTechnique() const { return mTechnique; }

        const PassInstances &getPassInstances() const { return mPassInstances; }

        TResult switchKeywords(const StringArray &enableKeys, const StringArray &disableKeys);

        TResult switchKeywords(const ShaderKeyword &keywrod);

        void setBool(const String &name, bool value);

        void setBoolArray(const String &name, const BoolArray &values);

        void setFloat(const String &name, float32_t value);

        void setFloatArray(const String &name, const FloatArray &values);

        void setInteger(const String &name, int32_t value);

        void setIntArray(const String &name, const IntArray &values);

        void setColor(const String &name, const ColorRGBA &value);

        void setColorArray(const String &name, const ColorArray &values);
        
        void setVector(const String &name, const Vector4 &value);

        void setVectorArray(const String &name, const Vector4Array &values);

        void setMatrix(const String &name, const Matrix4 &value);

        void setMatrixArray(const String &name, const Matrix4Array &values);

        void setData(const String &name, const void *data, uint32_t dataSize);

        void setTexture(const String &name, const UUID &uuid);
        
    protected:
        TechniqueInstance() = default;
        
        TechniqueInstance(Material *parent, TechniquePtr tech);

        TResult cloneProperties(Material *material, const TechniqueInstance * const src);
        
    protected:
        /// Technique instance 所属的 material
        Material        *mMaterial {nullptr};
        
        /// Technique 实例对应的 Technique
        TechniquePtr    mTechnique {nullptr};
        /// pass instance
        PassInstances   mPassInstances {};
    };
}


#endif  /*__T3D_TECHNIQUE_INSTANCE_H__*/
