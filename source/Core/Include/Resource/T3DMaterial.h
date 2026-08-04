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


#include "T3DShader.h"
#include "Resource/T3DResource.h"
#include "Material/T3DShaderConstantParam.h"
#include "Material/T3DShaderSamplerParam.h"
#include "Material/T3DTechnique.h"
#include "Material/T3DTechniqueInstance.h"


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
     * \brief 材质资源，绑定 Shader 并持有常量/采样器参数值及当前 TechniqueInstance
     */
    TCLASS()
    class T3D_ENGINE_API Material : public Resource
    {
        TRTTI_ENABLE(Resource)
        TRTTI_FRIEND
        
    public:
        /**
         * \brief 创建空材质（无 Shader）
         * \param [in] name : 材质名称
         * \return 新建的 Material 智能指针
         */
        static MaterialPtr create(const String &name);

        /**
         * \brief 创建绑定指定 Shader 的材质
         * \param [in] name : 材质名称
         * \param [in] shader : 关联的 Shader 对象，不可为 nullptr
         * \return 新建的 Material 智能指针
         */
        static MaterialPtr create(const String &name, Shader *shader);

        /// 析构函数
        ~Material() override;

        /**
         * \brief 获取资源类型
         * \return 固定返回 Type::kMaterial
         */
        Type getType() const override;

        /**
         * \brief 切换 Shader 关键字（启用/禁用宏）
         * \param [in] enableKeys : 要启用的关键字列表
         * \param [in] disableKey : 要禁用的关键字列表
         * \return mCurTechnique 为 nullptr 时返回 T3D_ERR_INVALID_POINTER；否则返回 TechniqueInstance::switchKeywords 的结果
         */
        TResult switchKeywords(const StringArray &enableKeys, const StringArray &disableKey);

        /**
         * \brief 查询关键字是否已启用
         * \param [in] keyword : 关键字名称
         * \return 当前实现始终返回 false
         */
        bool isKeywordEnable(const String &keyword) const;

        /// 获取当前 Technique 上已启用的关键字列表；无 Technique 时返回空数组
        const StringArray &getEnabledKeywords() const
        {
            return mCurTechnique != nullptr ? mCurTechnique->getEnabledKeywords() : StringUtil::kEmptyStringArray;
        }

        /// 获取当前 Technique 上已禁用的关键字列表；无 Technique 时返回空数组
        const StringArray &getDisabledKeywords() const
        {
            return mCurTechnique != nullptr ? mCurTechnique->getDisabledKeywords() : StringUtil::kEmptyStringArray;
        }

        /**
         * \brief 设置整型 Shader 常量
         * \param [in] name : 参数名称
         * \param [in] value : 整型值
         * \remarks 同步更新 mConstantValues 与 mCurTechnique（若存在）
         */
        void setInteger(const String &name, int32_t value);

        /// 获取整型 Shader 常量；不存在时返回默认构造的 int32_t
        int32_t getInteger(const String &name) const;

        /// 是否存在指定名称且数据大小匹配 int32_t 的常量
        bool hasInteger(const String &name) const;

        /// 设置浮点 Shader 常量
        void setFloat(const String &name, float32_t value);

        /// 获取浮点 Shader 常量；不存在时返回 0
        float32_t getFloat(const String &name) const;

        /// 是否存在指定名称且数据大小匹配 float32_t 的常量
        bool hasFloat(const String &name) const;

        /// 设置浮点数组 Shader 常量
        void setFloatArray(const String &name, const FloatArray &values);

        /// 获取浮点数组 Shader 常量
        FloatArray getFloatArray(const String &name) const;

        /// 设置颜色 Shader 常量
        void setColor(const String &name, const ColorRGBA &value);

        /// 获取颜色 Shader 常量
        ColorRGBA getColor(const String &name) const;

        /// 是否存在指定名称且数据大小匹配 ColorRGBA 的常量
        bool hasColor(const String &name) const;

        /// 设置颜色数组 Shader 常量
        void setColorArray(const String &name, const ColorArray &values);

        /// 获取颜色数组 Shader 常量
        ColorArray getColorArray(const String &name) const;

        /// 是否存在指定名称且数据大小为 ColorRGBA 整数倍的常量
        bool hasColorArray(const String &name) const;

        /// 设置 Vector4 Shader 常量
        void setVector(const String &name, const Vector4 &value);

        /// 获取 Vector4 Shader 常量
        Vector4 getVector(const String &name) const;

        /// 是否存在指定名称且数据大小匹配 Vector4 的常量
        bool hasVector(const String &name) const;

        /// 设置 Vector4 数组 Shader 常量
        void setVectorArray(const String &name, const Vector4Array &values);

        /// 获取 Vector4 数组 Shader 常量
        Vector4Array getVectorArray(const String &name) const;

        /// 是否存在指定名称且数据大小为 Vector4 整数倍的常量
        bool hasVectorArray(const String &name) const;
        
        /// 设置 Matrix4 Shader 常量
        void setMatrix(const String &name, const Matrix4 &value);

        /// 获取 Matrix4 Shader 常量
        Matrix4 getMatrix(const String &name) const;

        /// 是否存在指定名称且数据大小匹配 Matrix4 的常量
        bool hasMatrix(const String &name) const;

        /// 设置 Matrix4 数组 Shader 常量
        void setMatrixArray(const String &name, const Matrix4Array &values);

        /// 获取 Matrix4 数组 Shader 常量
        Matrix4Array getMatrixArray(const String &name) const;

        /// 是否存在指定名称且数据大小为 Matrix4 整数倍的常量
        bool hasMatrixArray(const String &name) const;
        
        /**
         * \brief 设置纹理采样器绑定的纹理 UUID
         * \param [in] name : 采样器参数名称
         * \param [in] uuid : 纹理资源 UUID
         * \remarks 同步更新 mSamplerValues 与 mCurTechnique（若存在）
         */
        void setTexture(const String &name, const UUID &uuid);

        /**
         * \brief 获取纹理采样器绑定的纹理 UUID
         * \param [in] name : 采样器参数名称
         * \return 存在时返回对应 UUID；否则返回 UUID::INVALID
         */
        const UUID &getTexture(const String &name) const;

        /// 是否存在指定名称的纹理采样器
        bool hasTexture(const String &name) const;

        /// 获取关联 Shader 的 UUID
        TPROPERTY(RTTRFuncName="Shader", RTTRFuncType="getter")
        const UUID &getShaderUUID() const { return mShaderUUID; }

        /// 获取所有纹理采样器参数值
        TPROPERTY(RTTRFuncName="SamplerValues", RTTRFuncType="getter")
        const ShaderSamplerValues &getSamplerValues() const { return mSamplerValues; }

        /// 获取已加载的 Shader 对象
        ShaderPtr getShader() const { return mShader; }

        /// 获取当前激活的 TechniqueInstance
        TechniqueInstancePtr getCurrentTechnique() const { return mCurTechnique; }
        
    private:
        /// RTTR 默认构造入口
        Material() : Material("") {}

        /// 设置关联 Shader 的 UUID
        TPROPERTY(RTTRFuncName="Shader", RTTRFuncType="setter")
        void setShaderUUID(const UUID &uuid) { mShaderUUID = uuid; }

        /// 设置纹理采样器参数值表
        TPROPERTY(RTTRFuncName="SamplerValues", RTTRFuncType="setter")
        void setSamplerValues(const ShaderSamplerValues &values) { mSamplerValues = values; }

        /// 获取 Shader 常量参数值表
        TPROPERTY(RTTRFuncName="ConstantValues", RTTRFuncType="getter")
        const ShaderConstantValues &getConstantValues() const { return mConstantValues; }

        /// 设置 Shader 常量参数值表
        TPROPERTY(RTTRFuncName="ConstantValues", RTTRFuncType="setter")
        void setConstantValues(const ShaderConstantValues &values) { mConstantValues = values; }
        
    protected:
        /**
         * \brief 以名称构造材质
         * \param [in] name : 材质名称
         */
        Material(const String &name);

        /**
         * \brief 构造绑定 Shader 的材质
         * \param [in] name : 材质名称
         * \param [in] shader : 关联 Shader
         */
        Material(const String &name, Shader *shader);
        
        /**
         * \brief 克隆材质资源
         * \return 新 Material 智能指针
         */
        ResourcePtr clone() const override;

        /**
         * \brief 从源材质拷贝常量/采样器/Shader/Technique 等属性
         * \param [in] src : 源 Material 对象
         */
        void cloneProperties(const Resource * const src) override;

        /**
         * \brief 资源创建回调：编译 Shader 并初始化 Technique
         * \return init(true, nullptr) 的返回值
         */
        TResult onCreate() override;
        
        /**
         * \brief 资源加载回调：加载 Shader 并初始化 Technique
         * \param [in] archive : 来源档案，用于加载采样器引用的纹理
         * \return Shader 加载或 init 失败时返回对应错误码
         */
        TResult onLoad(Archive *archive) override;

        /**
         * \brief 编译 Shader、可选反射参数并创建 TechniqueInstance
         * \param [in] shouldReflect : 为 true 时从 Shader 变体反射并初始化 mConstantValues/mSamplerValues
         * \param [in] archive : 非 nullptr 时为各采样器加载纹理资源
         * \return 编译或反射失败时返回对应错误码；否则返回 T3D_OK
         */
        TResult init(bool shouldRefelect, Archive *archive);

        /**
         * \brief 检查是否存在指定名称且数据大小匹配 T 的常量
         * \tparam [in] T : 标量常量类型
         * \param [in] name : 参数名称
         * \return 存在且 sizeof(T) 与存储数据大小一致时返回 true
         */
        template <typename T>
        bool hasValue(const String &name) const
        {
            const auto itr = mConstantValues.find(name);
            return (itr != mConstantValues.end() && sizeof(T) == itr->second->getDataSize());
        }

        /**
         * \brief ColorArray 特化：检查数据大小是否为 ColorRGBA 整数倍
         */
        template <>
        bool hasValue<ColorArray>(const String &name) const
        {
            const auto itr = mConstantValues.find(name);
            return (itr != mConstantValues.end() && (itr->second->getDataSize() % sizeof(ColorRGBA) == 0));
        }

        /**
         * \brief Vector4Array 特化：检查数据大小是否为 Vector4 整数倍
         */
        template <>
        bool hasValue<Vector4Array>(const String &name) const
        {
            const auto itr = mConstantValues.find(name);
            return (itr != mConstantValues.end() && (itr->second->getDataSize() % sizeof(Vector4) == 0));
        }

        /**
         * \brief Matrix4Array 特化：检查数据大小是否为 Matrix4 整数倍
         */
        template <>
        bool hasValue<Matrix4Array>(const String &name) const
        {
            
            const auto itr = mConstantValues.find(name);
            return (itr != mConstantValues.end() && (itr->second->getDataSize() % sizeof(Matrix4) == 0));
        }
        
        /**
         * \brief 设置常量值并同步到 TechniqueInstance
         * \tparam Value_t : 值类型
         * \tparam ShaderSetValue_t : ShaderConstantValue 成员函数指针
         * \tparam TechniqueSetValue_t : TechniqueInstance 成员函数指针
         */
        template <typename Value_t, typename ShaderSetValue_t, typename TechniqueSetValue_t>
        void setValue(const String &name, const Value_t &value, ShaderSetValue_t setShaderConstantValue, TechniqueSetValue_t setTechniqueConstantValue)
        {
            const auto itr = mConstantValues.find(name);
            if (itr != mConstantValues.end())
            {
                (itr->second->*setShaderConstantValue)(value);
            }

            if (mCurTechnique != nullptr)
            {
                (mCurTechnique->*setTechniqueConstantValue)(name, value);
            }
        }

        /**
         * \brief 从 mConstantValues 读取常量值
         * \tparam Value_t : 值类型
         * \return 存在时返回存储值；否则返回 Value_t 默认构造值
         */
        template <typename Value_t, typename SahderGetValue_t>
        Value_t getValue(const String &name, SahderGetValue_t getShaderContantValue) const
        {
            const auto itr = mConstantValues.find(name);
            if (itr != mConstantValues.end())
            {
                return (itr->second->*getShaderContantValue)();
            }
            return Value_t {};
        }

        /**
         * \brief 将 ShaderConstantValue 同步到当前 TechniqueInstance
         * \param [in] constValue : 常量值对象，不可为 nullptr
         */
        void setShaderConstantValue(ShaderConstantValue *constValue);

        /**
         * \brief 加载采样器纹理并同步到 TechniqueInstance
         * \param [in] archive : 来源档案；为 nullptr 时跳过纹理加载
         * \param [in] samplerValue : 采样器值对象
         * \remarks UUID 为 INVALID 的运行时纹理（如 shadowMap）会跳过加载
         */
        void setShaderSamplerValue(Archive *archive, ShaderSamplerValue *samplerValue);
        
    protected:
        /// shader 纹理采样器的数据
        ShaderSamplerValues     mSamplerValues {};
        /// shader 常量缓冲区的数据
        ShaderConstantValues    mConstantValues {};
        /// 着色器资源的 UUID
        UUID                    mShaderUUID {UUID::INVALID};
        /// 材质对应的着色器对象
        ShaderPtr               mShader {nullptr};
        /// 当前可用的技术对象
        TechniqueInstancePtr    mCurTechnique {nullptr};
    };
}

#include "T3DMaterial.inl"

#endif    /*__T3D_MATERIAL_H__*/
