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

#ifndef __T3D_SHADER_KEYWORD_H__
#define __T3D_SHADER_KEYWORD_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    /// shader 关键字集合中使用的字符串集合类型
    using Keys = TSet<String>;
    
    /**
     * \brief 着色器宏关键字组合，由多个宏名构成并生成唯一 hash 用于快速比较
     * \remarks 例如 #multi_compile A1 B1 C1 与 #multi_compile A2 B2 产生 A1_A2、A1_B2 等六个组合
     */
    TSTRUCT()
    struct T3D_ENGINE_API ShaderKeyword
    {
        TRTTI_FRIEND

    public:
        /// 无效/空关键字常量
        static ShaderKeyword INVALID;
     
        /// 默认构造空关键字
        ShaderKeyword();

        /// 拷贝构造
        ShaderKeyword(const ShaderKeyword &other);

        /// 移动构造
        ShaderKeyword(ShaderKeyword &&other) noexcept;

        /// 析构函数
        virtual ~ShaderKeyword() = default;

        /// 获取由 generate() 生成的可读名称字符串
        const String &getName() const { return mName; }

        /**
         * \brief 判断关键字集合中是否包含指定宏名
         * \param [in] keyword : 宏名称
         * \return 包含时返回 true
         */
        bool hasKey(const String &keyword) const;

        /// 获取 hash 码（须先调用 generate()）
        TPROPERTY(RTTRFuncName = "HashCode", RTTRFuncType = "getter")
        uint32_t getHashCode() const { return mHashCode; }

        /// 获取宏名集合
        TPROPERTY(RTTRFuncName = "Keys", RTTRFuncType = "getter")
        const Keys &getKeys() const { return mKeys; }

        /**
         * \brief 向集合插入宏名
         * \param [in] keyword : 宏名称
         * \return 调用成功返回 T3D_OK
         */
        TResult addKeyword(const String &keyword);

        /**
         * \brief 从集合移除宏名
         * \param [in] keyword : 宏名称
         * \return 调用成功返回 T3D_OK
         */
        TResult removeKeyword(const String &keyword);

        /**
         * \brief 根据当前宏集合生成可读名称与 hash 码
         * \return 调用成功返回 T3D_OK
         * \note 宏集合变更后须调用本方法才能正确比较
         */
        TResult generate();

        /// 按 hash 码比较不等
        bool operator !=(const ShaderKeyword &other) const;
     
        /// 按 hash 码比较相等
        bool operator ==(const ShaderKeyword &other) const;
        
        /// 按 hash 码小于比较
        bool operator <(const ShaderKeyword &other) const;
        
        /// 按 hash 码大于比较
        bool operator >(const ShaderKeyword &other) const;

        /// 拷贝赋值
        ShaderKeyword &operator =(const ShaderKeyword &other);

        /// 移动赋值
        ShaderKeyword &operator =(ShaderKeyword &&other) noexcept;

    private:
        /// 设置 hash 码（RTTR setter）
        TPROPERTY(RTTRFuncName="HashCode", RTTRFuncType="setter")
        void setHashCode(uint32_t hash)
        {
            mHashCode = hash;
        }

        /// 设置宏名集合（RTTR setter）
        TPROPERTY(RTTRFuncName="Keys", RTTRFuncType="setter")
        void setKeys(const Keys &keys)
        {
            mKeys = keys;
        }
        
    protected:
        /// 从 other 拷贝宏集合与 hash
        void copy(const ShaderKeyword &other);
        
        /// 从 other 移动宏集合与 hash
        void move(ShaderKeyword &&other);
        
        /// 由 generate() 生成的 hash 码
        uint32_t    mHashCode {0};
        /// 宏名集合
        Keys        mKeys {};
        /// 由 generate() 生成的可读名称
        String      mName {};
    };
}


#endif    /*__T3D_SHADER_KEYWORD_H__*/
