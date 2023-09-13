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

#ifndef __T3D_MATERIAL_H__
#define __T3D_MATERIAL_H__


#include "Resource/T3DResource.h"
#include "RHI/T3DRHIBlendState.h"
#include "RHI/T3DRHIRasterizerState.h"

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
    using ShaderLabTags = TMap<String, String>;
    using ShaderLabTagsValue = ShaderLabTags::value_type;

    TSTRUCT()
    struct RenderState
    {
        TPROPERTY(RTTRFuncName="BlendState", RTTRFuncType="getter")
        const BlendState &getBlendState() const
        {
            return mBlendState;
        }

        TPROPERTY(RTTRFuncName="BlendState", RTTRFuncType="setter")
        void setBlendState(const BlendState &state)
        {
            mBlendState = state;
        }

        TPROPERTY(RTTRFuncName="RasterizerState", RTTRFuncType="getter")
        const RasterizerState &getRasterizeState() const
        {
            return mRasterizerState;
        }

        TPROPERTY(RTTRFuncName="RasterizerState", RTTRFuncType="setter")
        void setRasterizerState(const RasterizerState &state)
        {
            mRasterizerState = state;
        }
        
        BlendState      mBlendState {};
        RasterizerState mRasterizerState {};
    };

    /**
     * \brief 渲染 pass
     */
    TCLASS()
    class T3D_ENGINE_API Pass
        : public Object
        , public Noncopyable
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND

    public:
        static PassPtr create(const String &name);

        ~Pass() override = default;

        PassPtr clone() const;

        void addTag(const String &key, const String &value);

        void removeTag(const String &key);
        
        bool getTag(const String &key, String &value) const;

        ShaderPtr getShader() const
        {
            return mShader;
        }

        void setShader(ShaderPtr shader)
        {
            mShader = shader;
        }

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const
        {
            return mName;
        }

        TPROPERTY(RTTRFuncName="Tags", RTTRFuncType="getter")
        const ShaderLabTags &getTags() const
        {
            return mTags;
        }

        TPROPERTY(RTTRFuncName="Shader", RTTRFuncType="getter")
        const String &getShaderName() const
        {
            return mShaderName;
        }

        TPROPERTY(RTTRFuncName="RenderState", RTTRFuncType="getter")
        const RenderState &getRenderState() const
        {
            return mRenderState;
        }

        TPROPERTY(RTTRFuncName="RenderState", RTTRFuncType="setter")
        void setRenderState(const RenderState &state)
        {
            mRenderState = state;
        }
        
    private:
        Pass() : Pass("") {}

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name)
        {
            mName = name;
        }

        TPROPERTY(RTTRFuncName="Tags", RTTRFuncType="setter")
        void setTags(const ShaderLabTags &tags)
        {
            mTags = tags;
        }

        TPROPERTY(RTTRFuncName="Shader", RTTRFuncType="setter")
        void setShaderName(const String &name) 
        {
            mShaderName = name;
        }
        
    protected:
        Pass(const String &name);

        /// Pass 名称
        String          mName {};
        /// Shader 名称
        String          mShaderName {};
        /// Tags
        ShaderLabTags   mTags {};
        /// 渲染状态
        RenderState     mRenderState {};
        /// 着色器对象
        ShaderPtr       mShader {nullptr};
    };

    using Passes = TList<PassPtr>;
    
    /**
     * \brief 渲染技术
     */
    TCLASS()
    class T3D_ENGINE_API Technique
        : public Object
        , public Noncopyable
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND

    public:
        static TechniquePtr create(const String &name);

        ~Technique() override = default;

        TechniquePtr clone() const;

        void addTag(const String &key, const String &value);

        void removeTag(const String &key);
        
        bool getTag(const String &key, String &value) const;

        void addPass(PassPtr pass);

        void removePass(PassPtr pass);

        bool getPass(const String &name) const;

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const
        {
            return mName;
        }
        
        TPROPERTY(RTTRFuncName="LOD", RTTRFuncType="getter")
        uint32_t getLOD() const
        {
            return mLOD;
        }

        TPROPERTY(RTTRFuncName="LOD", RTTRFuncType="setter")
        void setLOD(uint32_t LOD)
        {
            mLOD = LOD;
        }

        TPROPERTY(RTTRFuncName="Tags", RTTRFuncType="getter")
        const ShaderLabTags &getTags() const
        {
            return mTags;
        }

        TPROPERTY(RTTRFuncName="Passes", RTTRFuncType="getter")
        const Passes &getPasses() const
        {
            return mPasses;
        }
        
    private:
        Technique() : Technique("") {}

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name)
        {
            mName = name;
        }

        TPROPERTY(RTTRFuncName="Tags", RTTRFuncType="setter")
        void setTags(const ShaderLabTags &tags)
        {
            mTags = tags;
        }

        TPROPERTY(RTTRFuncName="Passes", RTTRFuncType="setter")
        void setPasses(const Passes &passes)
        {
            mPasses = passes;
        }
        
    protected:
        Technique(const String &name);

        String          mName {};
        uint32_t        mLOD {0};
        ShaderLabTags   mTags {};
        Passes          mPasses {};
    };

    using Techniques = TList<TechniquePtr>;

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

        ~Material() override = default;

        Type getType() const override;

        TPROPERTY(RTTRFuncName="Techniques", RTTRFuncType="getter")
        const Techniques &getTechniques() const
        {
            return mTechniques;
        }
        
    private:
        Material() : Material("") {}

        TPROPERTY(RTTRFuncName="Techniques", RTTRFuncType="setter")
        void setTechniques(const Techniques &techniques)
        {
            mTechniques = techniques;
        }
        
    protected:
        Material(const String &name);
        
        ResourcePtr clone() const override;

        void cloneProperties(const Resource * const src) override;

        Techniques  mTechniques;
    };
}


#endif    /*__T3D_MATERIAL_H__*/
