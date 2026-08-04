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
    /**
     * \brief Technique 的运行时实例，按 LightMode 索引 PassInstance 并转发参数设置
     */
    class T3D_ENGINE_API TechniqueInstance : public Object
    {
    public:
        /**
         * \brief 创建 TechniqueInstance 并为 Technique 的每个 Pass 创建 PassInstance
         * \param [in] parent : 所属的 Material
         * \param [in] tech : 对应的 Technique 定义
         * \return 新建的 TechniqueInstance 智能指针
         */
        static TechniqueInstancePtr create(Material *parent, TechniquePtr tech);

        /// 析构函数，清空 PassInstance 映射
        ~TechniqueInstance() override;

        /**
         * \brief 克隆本实例的属性到新的 TechniqueInstance
         * \param [in] material : 新实例所属的 Material
         * \return 克隆成功返回新实例；cloneProperties 失败时返回 nullptr
         */
        TechniqueInstancePtr clone(Material *material) const;

        /// 获取所属的 Material
        Material *getMaterial() const { return mMaterial; }
        
        /// 获取对应的 Technique 定义
        TechniquePtr getTechnique() const { return mTechnique; }

        /// 获取 PassInstance 映射（key 为 LightMode）
        const PassInstances &getPassInstances() const { return mPassInstances; }

        /**
         * \brief 向全部 PassInstance 传递 enable/disable 关键字列表并切换变体
         * \param [in] enableKeys : 要启用的宏名称列表
         * \param [in] disableKeys : 要禁用的宏名称列表
         * \return 调用成功返回 T3D_OK
         */
        TResult switchKeywords(const StringArray &enableKeys, const StringArray &disableKeys);

        /**
         * \brief 向全部 PassInstance 传递指定 ShaderKeyword 并切换变体
         * \param [in] keywrod : 目标 shader 关键字
         * \return 调用成功返回 T3D_OK
         */
        TResult switchKeywords(const ShaderKeyword &keywrod);

        /// 获取最近一次 switchKeywords 传入的 enable 列表
        const StringArray &getEnabledKeywords() const { return mEnabledKeywords; }

        /// 获取 disabled 关键字列表（当前实现未写入）
        const StringArray &getDisabledKeywords() const { return mDisabledKeywords; }

        /**
         * \brief 向全部 PassInstance 设置 bool 常量
         * \param [in] name : 常量名称
         * \param [in] value : 常量值
         */
        void setBool(const String &name, bool value);

        /**
         * \brief 向全部 PassInstance 设置 bool 数组常量
         * \param [in] name : 常量名称
         * \param [in] values : 常量值数组
         */
        void setBoolArray(const String &name, const BoolArray &values);

        /**
         * \brief 向全部 PassInstance 设置 float 常量
         * \param [in] name : 常量名称
         * \param [in] value : 常量值
         */
        void setFloat(const String &name, float32_t value);

        /**
         * \brief 向全部 PassInstance 设置 float 数组常量
         * \param [in] name : 常量名称
         * \param [in] values : 常量值数组
         */
        void setFloatArray(const String &name, const FloatArray &values);

        /**
         * \brief 向全部 PassInstance 设置 int 常量
         * \param [in] name : 常量名称
         * \param [in] value : 常量值
         */
        void setInteger(const String &name, int32_t value);

        /**
         * \brief 向全部 PassInstance 设置 int 数组常量
         * \param [in] name : 常量名称
         * \param [in] values : 常量值数组
         */
        void setIntArray(const String &name, const IntArray &values);

        /**
         * \brief 向全部 PassInstance 设置颜色常量
         * \param [in] name : 常量名称
         * \param [in] value : 颜色值
         */
        void setColor(const String &name, const ColorRGBA &value);

        /**
         * \brief 向全部 PassInstance 设置颜色数组常量
         * \param [in] name : 常量名称
         * \param [in] values : 颜色数组
         */
        void setColorArray(const String &name, const ColorArray &values);
        
        /**
         * \brief 向全部 PassInstance 设置 Vector4 常量
         * \param [in] name : 常量名称
         * \param [in] value : 向量值
         */
        void setVector(const String &name, const Vector4 &value);

        /**
         * \brief 向全部 PassInstance 设置 Vector4 数组常量
         * \param [in] name : 常量名称
         * \param [in] values : 向量数组
         */
        void setVectorArray(const String &name, const Vector4Array &values);

        /**
         * \brief 向全部 PassInstance 设置 Matrix4 常量
         * \param [in] name : 常量名称
         * \param [in] value : 矩阵值
         */
        void setMatrix(const String &name, const Matrix4 &value);

        /**
         * \brief 向全部 PassInstance 设置 Matrix4 数组常量
         * \param [in] name : 常量名称
         * \param [in] values : 矩阵数组
         */
        void setMatrixArray(const String &name, const Matrix4Array &values);

        /**
         * \brief 向全部 PassInstance 写入原始常量数据
         * \param [in] name : 常量名称
         * \param [in] data : 数据指针
         * \param [in] dataSize : 数据字节数
         */
        void setData(const String &name, const void *data, uint32_t dataSize);

        /**
         * \brief 向全部 PassInstance 设置纹理
         * \param [in] name : 采样器/纹理名称
         * \param [in] uuid : 纹理资源 UUID
         */
        void setTexture(const String &name, const UUID &uuid);
        
    protected:
        /// 默认构造，供 clone 使用
        TechniqueInstance() = default;
        
        /**
         * \brief 构造 TechniqueInstance 并为每个 Pass 创建 PassInstance（按 LightMode 索引）
         * \param [in] parent : 所属的 Material
         * \param [in] tech : 对应的 Technique 定义
         */
        TechniqueInstance(Material *parent, TechniquePtr tech);

        /**
         * \brief 从源实例复制属性，包括各 PassInstance 的克隆
         * \param [in] material : 新实例所属的 Material；为 nullptr 时使用源实例的 Material
         * \param [in] src : 源 TechniqueInstance
         * \return 调用成功返回 T3D_OK
         */
        TResult cloneProperties(Material *material, const TechniqueInstance * const src);
        
    protected:
        /// TechniqueInstance 所属的 Material
        Material        *mMaterial {nullptr};
        
        /// 对应的 Technique 定义
        TechniquePtr    mTechnique {nullptr};
        /// PassInstance 映射（key 为 LightMode）
        PassInstances   mPassInstances {};
        /// 最近一次 switchKeywords 传入的 enable 列表
        StringArray     mEnabledKeywords {};
        /// disabled 关键字列表
        StringArray     mDisabledKeywords {};
    };
}


#endif  /*__T3D_TECHNIQUE_INSTANCE_H__*/
