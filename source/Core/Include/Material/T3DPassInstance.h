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

#ifndef __T3D_PASS_INSTANCE_H__
#define __T3D_PASS_INSTANCE_H__


#include "T3DTypedef.h"
#include "Material/T3DShaderKeyword.h"
#include "Material/T3DShaderVariantInstance.h"


namespace Tiny3D
{
    /**
     * \brief Pass 的运行时实例，持有当前各 shader stage 的 ShaderVariantInstance 并转发参数设置
     */
    class T3D_ENGINE_API PassInstance : public Object
    {
    public:
        /**
         * \brief 创建 PassInstance 并切换到 pass 的当前关键字
         * \param [in] parent : 所属的 TechniqueInstance
         * \param [in] pass : 对应的 Pass 定义
         * \return 新建的 PassInstance 智能指针
         */
        static PassInstancePtr create(TechniqueInstance *parent, PassPtr pass);

        /// 析构函数
        virtual ~PassInstance() = default;

        /**
         * \brief 克隆本实例的属性到新的 PassInstance
         * \param [in] parent : 新实例所属的 TechniqueInstance
         * \return 克隆成功返回新实例；cloneProperties 失败时返回 nullptr
         */
        PassInstancePtr clone(TechniqueInstance *parent) const;

        /// 获取所属的 TechniqueInstance
        TechniqueInstance *getTechInstance() const { return mTechInstance; }

        /// 获取对应的 Pass 定义
        PassPtr getPass() const { return mPass; }

        /// 获取当前生效的 shader 关键字
        const ShaderKeyword &getCurrentKeyword() const { return mCurrentKeyword; }

        /**
         * \brief 基于 enable/disable 列表修改关键字并切换变体；与当前关键字相同时不切换
         * \param [in] enableKeys : 要启用的宏名称列表
         * \param [in] disableKeys : 要禁用的宏名称列表
         * \return 调用成功返回 T3D_OK
         */
        TResult switchKeywords(const StringArray &enableKeys, const StringArray &disableKeys);

        /**
         * \brief 切换到指定关键字对应的各 stage 变体；全部 stage 成功后统一更新当前变体
         * \param [in] keyword : 目标 shader 关键字
         * \return 调用成功返回 T3D_OK
         */
        TResult switchKeyword(const ShaderKeyword &keyword);

        /// 获取当前 Vertex Shader 变体实例，无匹配时为 nullptr
        ShaderVariantInstance *getCurrentVertexShader() const { return mCurrentVS; }

        /// 获取当前 Hull Shader 变体实例
        ShaderVariantInstance *getCurrentHullShader() const { return mCurrentHS; }

        /// 获取当前 Domain Shader 变体实例
        ShaderVariantInstance *getCurrentDomainShader() const { return mCurrentDS; }

        /// 获取当前 Geometry Shader 变体实例
        ShaderVariantInstance *getCurrentGeometryShader() const { return mCurrentGS; }

        /// 获取当前 Pixel Shader 变体实例
        ShaderVariantInstance *getCurrentPixelShader() const { return mCurrentPS; }

        /**
         * \brief 向当前全部 stage 变体实例设置 bool 常量
         * \param [in] name : 常量名称
         * \param [in] value : 常量值
         */
        void setBool(const String &name, bool value);

        /**
         * \brief 向当前全部 stage 变体实例设置 bool 数组常量
         * \param [in] name : 常量名称
         * \param [in] values : 常量值数组
         */
        void setBoolArray(const String &name, const BoolArray &values);

        /**
         * \brief 向当前全部 stage 变体实例设置 float 常量
         * \param [in] name : 常量名称
         * \param [in] value : 常量值
         */
        void setFloat(const String &name, float32_t value);

        /**
         * \brief 向当前全部 stage 变体实例设置 float 数组常量
         * \param [in] name : 常量名称
         * \param [in] values : 常量值数组
         */
        void setFloatArray(const String &name, const FloatArray &values);

        /**
         * \brief 向当前全部 stage 变体实例设置 int 常量
         * \param [in] name : 常量名称
         * \param [in] value : 常量值
         */
        void setInteger(const String &name, int32_t value);

        /**
         * \brief 向当前全部 stage 变体实例设置 int 数组常量
         * \param [in] name : 常量名称
         * \param [in] values : 常量值数组
         */
        void setIntArray(const String &name, const IntArray &values);

        /**
         * \brief 向当前全部 stage 变体实例设置颜色常量
         * \param [in] name : 常量名称
         * \param [in] value : 颜色值
         */
        void setColor(const String &name, const ColorRGBA &value);

        /**
         * \brief 向当前全部 stage 变体实例设置颜色数组常量
         * \param [in] name : 常量名称
         * \param [in] values : 颜色数组
         */
        void setColorArray(const String &name, const ColorArray &values);
        
        /**
         * \brief 向当前全部 stage 变体实例设置 Vector4 常量
         * \param [in] name : 常量名称
         * \param [in] value : 向量值
         */
        void setVector(const String &name, const Vector4 &value);

        /**
         * \brief 向当前全部 stage 变体实例设置 Vector4 数组常量
         * \param [in] name : 常量名称
         * \param [in] values : 向量数组
         */
        void setVectorArray(const String &name, const Vector4Array &values);

        /**
         * \brief 向当前全部 stage 变体实例设置 Matrix4 常量
         * \param [in] name : 常量名称
         * \param [in] value : 矩阵值
         */
        void setMatrix(const String &name, const Matrix4 &value);

        /**
         * \brief 向当前全部 stage 变体实例设置 Matrix4 数组常量
         * \param [in] name : 常量名称
         * \param [in] values : 矩阵数组
         */
        void setMatrixArray(const String &name, const Matrix4Array &values);

        /**
         * \brief 向当前全部 stage 变体实例写入原始常量数据
         * \param [in] name : 常量名称
         * \param [in] data : 数据指针
         * \param [in] dataSize : 数据字节数
         */
        void setData(const String &name, const void *data, uint32_t dataSize);

        /**
         * \brief 向当前全部 stage 变体实例设置纹理
         * \param [in] name : 采样器/纹理名称
         * \param [in] uuid : 纹理资源 UUID
         */
        void setTexture(const String &name, const UUID &uuid);
        
    protected:
        /// 默认构造，供 clone 使用
        PassInstance() = default;
        
        /**
         * \brief 构造 PassInstance 并按 pass 的当前关键字初始化各 stage 变体
         * \param [in] parent : 所属的 TechniqueInstance
         * \param [in] pass : 对应的 Pass 定义
         */
        PassInstance(TechniqueInstance *parent, PassPtr pass);

        /**
         * \brief 从源实例复制属性，包括各 stage 的 ShaderVariantInstance 克隆
         * \param [in] parent : 新实例所属的 TechniqueInstance；为 nullptr 时使用源实例的 parent
         * \param [in] src : 源 PassInstance
         * \return 调用成功返回 T3D_OK
         */
        TResult cloneProperties(TechniqueInstance *parent, const PassInstance * const src);

        /**
         * \brief 在指定 stage 的变体集合中查找 keyword 并创建新的 ShaderVariantInstance
         * \param [in] keyword : 目标关键字
         * \param [in] shaderVariants : 该 stage 的变体集合
         * \param [in,out] currentVariant : 输出新的变体实例；集合为空或未找到 keyword 时设为 nullptr
         * \return 调用成功返回 T3D_OK
         */
        TResult switchShaderVariants(const ShaderKeyword &keyword, const ShaderVariantSets &shaderVariants, ShaderVariantInstancePtr &currentVariant);

        /**
         * \brief 若 shader 非 nullptr，则调用其对应的 set 方法写入常量值
         * \tparam Value_t : 常量值类型
         * \tparam ShaderSetValue_t : ShaderVariantInstance 成员函数指针类型
         */
        template <typename Value_t, typename ShaderSetValue_t>
        void setValue(ShaderVariantInstance *shader, const String &name, const Value_t &value, ShaderSetValue_t shaderSetValue)
        {
            if (shader != nullptr)
            {
                (shader->*shaderSetValue)(name, value);
            }
        }

        /**
         * \brief 若 shader 非 nullptr，则向其写入原始常量数据
         * \param [in] shader : 目标变体实例
         * \param [in] name : 常量名称
         * \param [in] data : 数据指针
         * \param [in] size : 数据字节数
         */
        void setData(ShaderVariantInstance *shader, const String &name, const void *data, size_t size)
        {
            if (shader != nullptr)
            {
                shader->setData(name, data, size);
            }
        }

        /**
         * \brief 若 shader 非 nullptr，则向其设置纹理
         * \param [in] shader : 目标变体实例
         * \param [in] name : 采样器/纹理名称
         * \param [in] uuid : 纹理资源 UUID
         */
        void setTexture(ShaderVariantInstance *shader, const String &name, const UUID &uuid)
        {
            if (shader != nullptr)
            {
                shader->setTexture(name, uuid);
            }
        }
        
    protected:
        /// PassInstance 所属的 TechniqueInstance
        TechniqueInstance   *mTechInstance {nullptr};
        
        /// 对应的 Pass 定义
        PassPtr             mPass {nullptr};
        
        /// 当前生效的 shader 关键字
        ShaderKeyword       mCurrentKeyword {};
        
        /// 当前生效的 Vertex Shader 变体实例
        ShaderVariantInstancePtr    mCurrentVS {nullptr};
        /// 当前生效的 Pixel Shader 变体实例
        ShaderVariantInstancePtr    mCurrentPS {nullptr};
        /// 当前生效的 Geometry Shader 变体实例
        ShaderVariantInstancePtr    mCurrentGS {nullptr};
        /// 当前生效的 Hull Shader 变体实例
        ShaderVariantInstancePtr    mCurrentHS {nullptr};
        /// 当前生效的 Domain Shader 变体实例
        ShaderVariantInstancePtr    mCurrentDS {nullptr};
    };

    //--------------------------------------------------------------------------

    inline void PassInstance::setBool(const String &name, bool value)
    {
        setValue(mCurrentVS, name, value, &ShaderVariantInstance::setBool);
        setValue(mCurrentPS, name, value, &ShaderVariantInstance::setBool);
        setValue(mCurrentGS, name, value, &ShaderVariantInstance::setBool);
        setValue(mCurrentHS, name, value, &ShaderVariantInstance::setBool);
        setValue(mCurrentDS, name, value, &ShaderVariantInstance::setBool);
    }

    //--------------------------------------------------------------------------
    
    inline void PassInstance::setBoolArray(const String &name, const BoolArray &values)
    {
        setValue(mCurrentVS, name, values, &ShaderVariantInstance::setBoolArray);
        setValue(mCurrentPS, name, values, &ShaderVariantInstance::setBoolArray);
        setValue(mCurrentGS, name, values, &ShaderVariantInstance::setBoolArray);
        setValue(mCurrentHS, name, values, &ShaderVariantInstance::setBoolArray);
        setValue(mCurrentDS, name, values, &ShaderVariantInstance::setBoolArray);
    }

    //--------------------------------------------------------------------------
    
    inline void PassInstance::setFloat(const String &name, float32_t value)
    {
        setValue(mCurrentVS, name, value, &ShaderVariantInstance::setFloat);
        setValue(mCurrentPS, name, value, &ShaderVariantInstance::setFloat);
        setValue(mCurrentGS, name, value, &ShaderVariantInstance::setFloat);
        setValue(mCurrentHS, name, value, &ShaderVariantInstance::setFloat);
        setValue(mCurrentDS, name, value, &ShaderVariantInstance::setFloat);
    }

    //--------------------------------------------------------------------------
    
    inline void PassInstance::setFloatArray(const String &name, const FloatArray &values)
    {
        setValue(mCurrentVS, name, values, &ShaderVariantInstance::setFloatArray);
        setValue(mCurrentPS, name, values, &ShaderVariantInstance::setFloatArray);
        setValue(mCurrentGS, name, values, &ShaderVariantInstance::setFloatArray);
        setValue(mCurrentHS, name, values, &ShaderVariantInstance::setFloatArray);
        setValue(mCurrentDS, name, values, &ShaderVariantInstance::setFloatArray);
    }

    //--------------------------------------------------------------------------
    
    inline void PassInstance::setInteger(const String &name, int32_t value)
    {
        setValue(mCurrentVS, name, value, &ShaderVariantInstance::setInteger);
        setValue(mCurrentPS, name, value, &ShaderVariantInstance::setInteger);
        setValue(mCurrentGS, name, value, &ShaderVariantInstance::setInteger);
        setValue(mCurrentHS, name, value, &ShaderVariantInstance::setInteger);
        setValue(mCurrentDS, name, value, &ShaderVariantInstance::setInteger);
    }

    //--------------------------------------------------------------------------
    
    inline void PassInstance::setIntArray(const String &name, const IntArray &values)
    {
        setValue(mCurrentVS, name, values, &ShaderVariantInstance::setIntArray);
        setValue(mCurrentPS, name, values, &ShaderVariantInstance::setIntArray);
        setValue(mCurrentGS, name, values, &ShaderVariantInstance::setIntArray);
        setValue(mCurrentHS, name, values, &ShaderVariantInstance::setIntArray);
        setValue(mCurrentDS, name, values, &ShaderVariantInstance::setIntArray);
    }

    //--------------------------------------------------------------------------
    
    inline void PassInstance::setColor(const String &name, const ColorRGBA &value)
    {
        setValue(mCurrentVS, name, value, &ShaderVariantInstance::setColor);
        setValue(mCurrentPS, name, value, &ShaderVariantInstance::setColor);
        setValue(mCurrentGS, name, value, &ShaderVariantInstance::setColor);
        setValue(mCurrentHS, name, value, &ShaderVariantInstance::setColor);
        setValue(mCurrentDS, name, value, &ShaderVariantInstance::setColor);
    }

    //--------------------------------------------------------------------------
    
    inline void PassInstance::setColorArray(const String &name, const ColorArray &values)
    {
        setValue(mCurrentVS, name, values, &ShaderVariantInstance::setColorArray);
        setValue(mCurrentPS, name, values, &ShaderVariantInstance::setColorArray);
        setValue(mCurrentGS, name, values, &ShaderVariantInstance::setColorArray);
        setValue(mCurrentHS, name, values, &ShaderVariantInstance::setColorArray);
        setValue(mCurrentDS, name, values, &ShaderVariantInstance::setColorArray);
    }

    //--------------------------------------------------------------------------
    
    inline void PassInstance::setVector(const String &name, const Vector4 &value)
    {
        setValue(mCurrentVS, name, value, &ShaderVariantInstance::setVector);
        setValue(mCurrentPS, name, value, &ShaderVariantInstance::setVector);
        setValue(mCurrentGS, name, value, &ShaderVariantInstance::setVector);
        setValue(mCurrentHS, name, value, &ShaderVariantInstance::setVector);
        setValue(mCurrentDS, name, value, &ShaderVariantInstance::setVector);
    }

    //--------------------------------------------------------------------------
    
    inline void PassInstance::setVectorArray(const String &name, const Vector4Array &values)
    {
        setValue(mCurrentVS, name, values, &ShaderVariantInstance::setVectorArray);
        setValue(mCurrentPS, name, values, &ShaderVariantInstance::setVectorArray);
        setValue(mCurrentGS, name, values, &ShaderVariantInstance::setVectorArray);
        setValue(mCurrentHS, name, values, &ShaderVariantInstance::setVectorArray);
        setValue(mCurrentDS, name, values, &ShaderVariantInstance::setVectorArray);
    }

    //--------------------------------------------------------------------------
    
    inline void PassInstance::setMatrix(const String &name, const Matrix4 &value)
    {
        setValue(mCurrentVS, name, value, &ShaderVariantInstance::setMatrix);
        setValue(mCurrentPS, name, value, &ShaderVariantInstance::setMatrix);
        setValue(mCurrentGS, name, value, &ShaderVariantInstance::setMatrix);
        setValue(mCurrentHS, name, value, &ShaderVariantInstance::setMatrix);
        setValue(mCurrentDS, name, value, &ShaderVariantInstance::setMatrix);
    }

    //--------------------------------------------------------------------------
    
    inline void PassInstance::setMatrixArray(const String &name, const Matrix4Array &values)
    {
        setValue(mCurrentVS, name, values, &ShaderVariantInstance::setMatrixArray);
        setValue(mCurrentPS, name, values, &ShaderVariantInstance::setMatrixArray);
        setValue(mCurrentGS, name, values, &ShaderVariantInstance::setMatrixArray);
        setValue(mCurrentHS, name, values, &ShaderVariantInstance::setMatrixArray);
        setValue(mCurrentDS, name, values, &ShaderVariantInstance::setMatrixArray);
    }
    
    //--------------------------------------------------------------------------

    inline void PassInstance::setData(const String &name, const void *data, uint32_t dataSize)
    {
        setData(mCurrentVS, name, data, dataSize);
        setData(mCurrentPS, name, data, dataSize);
        setData(mCurrentGS, name, data, dataSize);
        setData(mCurrentHS, name, data, dataSize);
        setData(mCurrentDS, name, data, dataSize);
    }

    //--------------------------------------------------------------------------

    inline void PassInstance::setTexture(const String &name, const UUID &uuid)
    {
        setTexture(mCurrentVS, name, uuid);
        setTexture(mCurrentPS, name, uuid);
        setTexture(mCurrentGS, name, uuid);
        setTexture(mCurrentHS, name, uuid);
        setTexture(mCurrentDS, name, uuid);
    }

    //--------------------------------------------------------------------------
}


#endif  /*__T3D_PASS_INSTANCE_H__*/
