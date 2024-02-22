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
    using Keys = TSet<String>;
    
    /**
     * \brief 着色器宏
     * \remarks
     *      Example :
     *      Shader 中代码有如下宏定义
     *          #multi_compile A1 B1 C1 
     *          #multi_compile A2 B2
     *      则产生的 ShaderKey 有：
     *          A1_A2、A1_B2、B1_A2、B1_B2、C1_A2、C1_B2
     *      一共六个 ShaderKey 对象
     */
    TSTRUCT()
    struct T3D_ENGINE_API ShaderKeyword
    {
        TRTTI_FRIEND

    public:
        static ShaderKeyword INVALID;
     
        /**
         * \brief Constructor
         */
        ShaderKeyword();

        /**
         * \brief Copy constructor
         */
        ShaderKeyword(const ShaderKeyword &other);

        /**
         * \brief Move constructor
         */
        ShaderKeyword(ShaderKeyword &&other) noexcept;

        /**
         * \brief Destructor
         */
        virtual ~ShaderKeyword() = default;

        /**
         * \brief 是否有对应的宏
         * \param [in] keyword : 宏定义名称
         * \return 如果有对应的宏定义，则返回 true
         */
        bool hasKey(const String &keyword) const;

        TPROPERTY(RTTRFuncName = "HashCode", RTTRFuncType = "getter")
        uint32_t getHashCode() const { return mHashCode; }

        TPROPERTY(RTTRFuncName = "Keys", RTTRFuncType = "getter")
        const Keys &getKeys() const { return mKeys; }

        /**
         * \brief 设置宏
         * \param [in] keyword : 宏定义名称
         * \return 调用成功返回 T3D_OK
         */
        TResult addKeyword(const String &keyword);

        TResult removeKeyword(const String &keyword);

        /**
         * \brief 当宏集合有变化时，要调用本接口，生成唯一 hash 值，方便后续快速比较
         * \return 调用成功返回 T3D_OK
         */
        TResult generate();

        bool operator !=(const ShaderKeyword &other) const;
     
        bool operator ==(const ShaderKeyword &other) const;
        
        bool operator <(const ShaderKeyword &other) const;
        
        bool operator >(const ShaderKeyword &other) const;

        /**
         * \brief Copy assignment
         */
        ShaderKeyword &operator =(const ShaderKeyword &other);

        /**
         * \brief Move assignment
         */
        ShaderKeyword &operator =(ShaderKeyword &&other) noexcept;

    private:
        TPROPERTY(RTTRFuncName="HashCode", RTTRFuncType="setter")
        void setHashCode(uint32_t hash)
        {
            mHashCode = hash;
        }

        TPROPERTY(RTTRFuncName="Keys", RTTRFuncType="setter")
        void setKeys(const Keys &keys)
        {
            mKeys = keys;
        }
        
    protected:
        void copy(const ShaderKeyword &other);
        
        void move(ShaderKeyword &&other);
        
        uint32_t    mHashCode {0};
        Keys        mKeys {};
    };
}


#endif    /*__T3D_SHADER_KEYWORD_H__*/
