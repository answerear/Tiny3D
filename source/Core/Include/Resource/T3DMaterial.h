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

#ifndef __T3D_MATERIAL_H__
#define __T3D_MATERIAL_H__


#include "Resource/T3DResource.h"
#include "Material/T3DShaderConstantParam.h"
#include "Material/T3DShaderSamplerParam.h"


/**
 * Shader "Name"
 * {
 *      Properties
 *      {
 *          // [optional: attribute] name("display text in Inspector", type name) = default value
 *          _IntegerName ("Integer display name", Integer) = 1
 *          _FloatName ("Float display name", Float) = 0.5
 *          _RangeName ("Float with range", Range(0.0, 1.0)) = 0.5
 *          _TextureName ("Texture2D display name", 2D) = "" {}
 *          _RedTextureName ("Texture2D display name", 2D) = "red" {}
 *          _Texture2DArrayName ("Texture2DArray display name", 2DArray) = ""
 *          _Texture2DName ("Texture3D", 3D) = "" {}
 *          _CubemapName ("Cubemap", Cube) = "" {}
 *          _CubemapArrayName ("CubemapArray", CubeArray) = "" {}
 *          _ColorName("Example color", Color) = (.25, .5, .5, 1)
 *          _VectorName ("Example vector", Vector) = (.25, .5, .5, 1)
 *      }
 *
 *      SubShader
 *      {
 *          LOD 100
 *          Tags { "ExampleSubShaderTagKey" = "ExampleSubShaderTagValue" }
 *          
 *          AlphaToMask On      // AlphaToMask <state>
 *
 *          // render target    Integer             range 0 through 7
 *          // state            Off
 *          // factor           One	                The value of this input is one. Use this to use the value of the source or the destination color.
 *          //                  Zero                The value of this input is zero. Use this to remove either the source or the destination values.
 *          //                  SrcColor            The GPU multiplies the value of this input by the source color value.
 *          //                  SrcAlpha            The GPU multiplies the value of this input by the source alpha value.
 *          //                  SrcAlphaSaturate    The GPU multiplies the value of this input by the minimum value of source alpha and (1 - destination alpha)
 *          //                  DstColor            The GPU multiplies the value of this input by the frame buffer source color value.
 *          //                  DstAlpha            The GPU multiplies the value of this input by the frame buffer source alpha value.
 *          //                  OneMinusSrcColor    The GPU multiplies the value of this input by (1 - source color).
 *          //                  OneMinusSrcAlpha    The GPU multiplies the value of this input by (1 - source alpha).
 *          //                  OneMinusDstColor    The GPU multiplies the value of this input by (1 - destination color).
 *          //                  OneMinusDstAlpha    The GPU multiplies the value of this input by (1 - destination alpha).
 *          Blend Off                   // Blend <state>
 *          Blend 1 Off                 // Blend <render target> <state>
 *          Blend One Zero              // Blend <source factor> <destination factor>
 *          Blend 1 One Zero            // Blend <render target> <source factor> <destination factor>
 *          Blend One Zero, Zero One    // Blend <source factor RGB> <destination factor RGB>, <source factor alpha> <destination factor alpha>
 *          Blend 1 One Zero, Zero One  // Blend <render target> <source factor RGB> <destination factor RGB>, <source factor alpha> <destination factor alpha>
 *
 *          // operation    Add	                Add source and destination together.
 *          //              Sub	                Subtract destination from source.
 *          //              RevSub              Subtract source from destination.
 *          //              Min	                Use the smaller of source and destination. (See note 1)
 *          //              Max	                Use the larger of source and destination. (See note 1)
 *          //              LogicalClear        Logical operation: Clear (0) (See note 2)
 *          //              LogicalSet          Logical operation: Set (1) (See note 2)
 *          //              LogicalCopy         Logical operation: Copy (s) (See note 2)
 *          //              LogicalCopyInverted	Logical operation: Copy inverted (!s) 2
 *          //              LogicalNoop	        Logical operation: Noop (d) (See note 2)
 *          //              LogicalInvert       Logical operation: Invert (!d) (See note 2)
 *          //              LogicalAnd          Logical operation: And (s & d) (See note 2)
 *          //              LogicalNand	        Logical operation: Nand !(s & d) (See note 2)
 *          //              LogicalOr           Logical operation: Or (s | d) (See note 2)
 *          //              LogicalNor          Logical operation: Nor !(s | d) (See note 2)
 *          //              LogicalXor          Logical operation: Xor (s ^ d) (See note 2)
 *          //              LogicalEquiv        Logical operation: Equivalence !(s ^ d) (See note 2)
 *          //              LogicalAndReverse   Logical operation: Reverse And (s & !d) (See note 2)
 *          //              LogicalAndInverted  Logical operation: Inverted And (!s & d) (See note 2)
 *          //              LogicalOrReverse    Logical operation: Reverse Or (s | !d) (See note 2)
 *          //              LogicalOrInverted   Logical operation: Inverted Or (!s | d) (See note 2)
 *          //              Multiply            Advanced OpenGL blending operation: Multiply (See note 3)
 *          //              Screen              Advanced OpenGL blending operation: Screen (See note 3)
 *          //              Overlay             Advanced OpenGL blending operation: Overlay (See note 3)
 *          //              Darken              Advanced OpenGL blending operation: Darken (See note 3)
 *          //              Lighten             Advanced OpenGL blending operation: Lighten (See note 3)
 *          //              ColorDodge          Advanced OpenGL blending operation: ColorDodge (See note 3)
 *          //              ColorBurn           Advanced OpenGL blending operation: ColorBurn (See note 3)
 *          //              HardLight           Advanced OpenGL blending operation: HardLight (See note 3)
 *          //              SoftLight           Advanced OpenGL blending operation: SoftLight (See note 3)
 *          //              Difference          Advanced OpenGL blending operation: Difference (See note 3)
 *          //              Exclusion           Advanced OpenGL blending operation: Exclusion (See note 3)
 *          //              HSLHue              Advanced OpenGL blending operation: HSLHue (See note 3)
 *          //              HSLSaturation       Advanced OpenGL blending operation: HSLSaturation (See note 3)
 *          //              HSLColor            Advanced OpenGL blending operation: HSLColor (See note 3)
 *          //              HSLLuminosity       Advanced OpenGL blending operation: HSLLuminosity (See note 3)
 *          BlendOp Sub         // BlendOp <operation>
 *
 *          // render target    Integer, 0 through 7.                                               The render target index.
 *          // channels	        0                                                                   Enables color writes to the R, G, B, and A channels.
 *          //                  R                                                                   Enables color writes to the red channel.
 *          //                  G                                                                   Enables color writes to the green channel.
 *          //                  B                                                                   Enables color writes to the blue channel.
 *          //                  A                                                                   Enables color writes to the alpha channel.
 *          //                  Any combination of R, G, B, and A without spaces. For example: RB   Enables color writes to the given channels.
 *          ColorMask RGB       // ColorMask <channels>
 *          ColorMask RGB 2     // ColorMask <channels> <render target>
 *
 *          Conservative True   // Conservative <enabled>, <enable> - True or False
 *
 *          Cull Back       // Cull <state> , <state> - Back, Front, Off
 *
 *          // factor   Float, range –1 through 1.      Scales the maximum Z slope, also called the depth slope, to produce a variable depth offset for each polygon.
 *          //
 *          //                                          Polygons that are not parallel to the near and far clip planes have Z slope. Adjust this value to avoid visual artifacts on such polygons.
 *          // units    Float, range –1 through 1.      Scales the minimum resolvable depth buffer value, to produce a constant depth offset. The minimum depth resolvable depth buffer value (one unit) varies by device.
 *          //
 *          //                                          A negative value means that the GPU draws the polygon closer to the camera. A positive value means that the GPU draws the polygon further away from the camera.
 *          Offset 1, 1     // Offset <factor>, <units>
 *
 *          // Stencil
 *          // {
 *          //     Ref <ref>
 *          //     ReadMask <readMask>
 *          //     WriteMask <writeMask>
 *          //     Comp <comparisonOperation>
 *          //     Pass <passOperation>
 *          //     Fail <failOperation>
 *          //     ZFail <zFailOperation>
 *          //     CompBack <comparisonOperationBack>
 *          //     PassBack <passOperationBack>
 *          //     FailBack <failOperationBack>
 *          //     ZFailBack <zFailOperationBack>
 *          //     CompFront <comparisonOperationFront>
 *          //     PassFront <passOperationFront>
 *          //     FailFront <failOperationFront>
 *          //     ZFailFront <zFailOperationFront>
 *          // }
 *          //
 *          // Comparision operation :
 *          // Never    1   Never render pixels.
 *          // Less     2   Render pixels when their reference value is less than the current value in the stencil buffer.
 *          // Equal    3   Render pixels when their reference value is equal to the current value in the stencil buffer.
 *          // LEqual   4   Render pixels when their reference value is less than or equal to the current value in the stencil buffer.
 *          // Greater  5   Render pixels when their reference value is greater than the current value in the stencil buffer.
 *          // NotEqual 6   Render pixels when their reference value differs from the current value in the stencil buffer.
 *          // GEqual   7   Render pixels when their reference value is greater than or equal to the current value in the stencil buffer.
 *          // Always   8   Always render pixels.
 *          //
 *          // Stencil operation :
 *          // Keep	    0   Keep the current contents of the stencil buffer.
 *          // Zero     1   Write 0 into the stencil buffer.
 *          // Replace  2   Write the reference value into the buffer.
 *          // IncrSat  3   Increment the current value in the buffer. If the value is 255 already, it stays at 255.
 *          // DecrSat  4   Decrement the current value in the buffer. If the value is 0 already, it stays at 0.
 *          // Invert   5   Negate all the bits of the current value in the buffer.
 *          // IncrWrap 6   Increment the current value in the buffer. If the value is 255 already, it becomes 0.
 *          // DecrWrap 7   Decrement the current value in the buffer. If the value is 0 already, it becomes 255.
 *          Stencil
 *          {
 *              Ref 2
 *              Comp equal
 *              Pass keep
 *              ZFail decrWrap
 *          }
 *
 *          ZClip True  // ZClip [enabled]
 *
 *          // operation    Less        Draw geometry that is in front of existing geometry. Do not draw geometry that is at the same distance as or behind existing geometry.
 *          //              LEqual      Draw geometry that is in front of or at the same distance as existing geometry. Do not draw geometry that is behind existing geometry.
 *          //                          This is the default value.
 *          //              Equal       Draw geometry that is at the same distance as existing geometry. Do not draw geometry that is in front of or behind existing geometry.
 *          //              GEqual      Draw geometry that is behind or at the same distance as existing geometry. Do not draw geometry that is in front of existing geometry.
 *          //              Greater     Draw geometry that is behind existing geometry. Do not draw geometry that is at the same distance as or in front of existing geometry.
 *          //              NotEqual    Draw geometry that is not at the same distance as existing geometry. Do not draw geometry that is at the same distance as existing geometry.
 *          //              Always      No depth testing occurs. Draw all geometry, regardless of distance.
 *          ZTest Less  // ZTest [operation]
 *
 *          ZWrite Off  // ZWrite [state]
 *
 *          UsePass "Shader object name/PASS NAME IN UPPERCASE"
 *          GrabPass { "ExampleTextureName" }
 *
 *          Pass
 *          {
 *              Name "ExamplePassName"                          // <optional: name>
 *              Tags { "ExampleTagKey" = "ExampleTagValue" }    // <optional: tags>
 *              // <optional: commands>
 *              // <optional: shader code>
 *          }
 *      }
 *
 *      Fallback "fallback"
 * }
 */ 


namespace Tiny3D
{
    /**
     * \brief 材质
     */
    TCLASS()
    class T3D_ENGINE_API Material : public Resource
    {
        TRTTI_ENABLE(Resource)
        TRTTI_FRIEND
        
    public:
        static MaterialPtr create(const String &name);

        ~Material() override;

        Type getType() const override;

        TResult switchKeywords(const StringArray &enableKeys, const StringArray &disableKey);

        bool isKeywordEnable(const String &keyword) const;

        /**
         * \brief 设置整型数据
         * \param name 
         * \param value 
         */
        void setInteger(const String &name, int32_t value);

        int32_t getInteger(const String &name) const;

        bool hasInteger(const String &name) const;

        void setFloat(const String &name, float32_t value);

        float32_t getFloat(const String &name) const;

        bool hasFloat(const String &name) const;

        void setFloatArray(const String &name, const FloatArray &values);

        FloatArray getFloatArray(const String &name) const;

        void setColor(const String &name, const ColorRGBA &value);

        ColorRGBA getColor(const String &name) const;

        bool hasColor(const String &name) const;

        void setColorArray(const String &name, const ColorArray &values);

        ColorArray getColorArray(const String &name) const;

        void setVector(const String &name, const Vector4 &value);

        Vector4 getVector(const String &name) const;

        bool hasVector(const String &name) const;

        void setVectorArray(const String &name, const Vector4Array &values);

        Vector4Array getVectorArray(const String &name) const;

        void setMatrix(const String &name, const Matrix4 &value);

        Matrix4 getMatrix(const String &name) const;

        bool hasMatrix(const String &name) const;

        void setMatrixArray(const String &name, const Matrix4Array &values);

        Matrix4Array getMatrixArray(const String &name) const;

        void setTexture(const String &name, Texture *value);

        Texture *getTexture(const String &name) const;

        bool hasTexture(const String &name) const;

        TPROPERTY(RTTRFuncName="Shader", RTTRFuncType="getter")
        const String &getShaderName() const { return mShaderName; }

        TPROPERTY(RTTRFuncName="Constants", RTTRFuncType="getter")
        const ShaderConstantParams &getConstantParams() const { return mConstants; }

        TPROPERTY(RTTRFuncName="Samplers", RTTRFuncType="getter")
        const ShaderSamplerParams &getSamplerParams() const { return mSamplers; }

        ShaderPtr getShader() const { return mShader; }

        TechniqueInstancePtr getCurrentTechnique() const { return mCurTechnique; }
        
    private:
        Material() : Material("") {}

        TPROPERTY(RTTRFuncName="Shader", RTTRFuncType="setter")
        void setShaderName(const String &name) { mShaderName = name; }

        TPROPERTY(RTTRFuncName="Constants", RTTRFuncType="setter")
        void setConstantParams(const ShaderConstantParams &params) { mConstants = params; }

        TPROPERTY(RTTRFuncName="Samplers", RTTRFuncType="setter")
        void setSamplerParams(const ShaderSamplerParams &params) { mSamplers = params; }
        
        template <typename T>
        void setValue(const String &name, const void *data)
        {
            auto itr = mConstants.find(name);
            if (itr != mConstants.end())
            {
                itr->second->setData(&data);
            }
        }
        
        template <typename T>
        T getValue(const String &name) const
        {
            T value {};
            auto itr = mConstants.find(name);
            if (itr != mConstants.end() && itr->second->getDataSize() == sizeof(int32_t))
            {
                memcpy(&value, itr->second->getData(), itr->second->getDataSize());
            }
            return value;
        }

        template <typename T>
        TArray<T> getValues(const String &name) const
        {
            auto itr = mConstants.find(name);
            if (itr != mConstants.end())
            {
                return TArray<T>((T*)itr->second->getData(), (T*)(itr->second->getData()) + itr->second->getDataSize());
            }

            return TArray<T>();
        }

        template <typename T>
        bool hasValue(const String &name) const
        {
            auto itr = mConstants.find(name);
            return (itr != mConstants.end() && itr->second->getDataSize() == sizeof(T));
        }
        
    protected:
        Material(const String &name);
        
        ResourcePtr clone() const override;

        void cloneProperties(const Resource * const src) override;

        bool onPostLoad() override;

    protected:
        /// shader 常量
        ShaderConstantParams    mConstants {};
        /// shader 纹理采样器
        ShaderSamplerParams     mSamplers {};
        /// 着色器资源名字
        String                  mShaderName {};
        /// 材质对应的着色器对象
        ShaderPtr               mShader {nullptr};
        /// 当前可用的技术对象
        TechniqueInstancePtr    mCurTechnique {nullptr};
    };
}

#include "T3DMaterial.inl"

#endif    /*__T3D_MATERIAL_H__*/
