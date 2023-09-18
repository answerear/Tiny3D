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

#ifndef __T3D_SHADER_VARIANT_H__
#define __T3D_SHADER_VARIANT_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Material/T3DShaderKeyword.h"


namespace Tiny3D
{
    /**
     * \brief Shader 变体，也是实际 shader 代码和编译后的字节码
     */
    TCLASS()
    class T3D_ENGINE_API ShaderVariant
        : public Object
        , public Noncopyable
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND
        
    public:
        /**
         * \brief 创建 shader 变体对象
         * \param [in] keyword : 变体对应的关键字
         * \param [in] code : 变体对应的源码
         * \return 返回 shader 变体对象
         */
        static ShaderVariantPtr create(ShaderKeyword &&keyword, const String &code);

        /**
         * \brief Destructor
         */
        ~ShaderVariant() override;

        /**
         * \brief 获取变体对应的关键字
         * \return 返回变体对应的关键字对象
         */
        TPROPERTY(RTTRFuncName="ShaderKeyword", RTTRFuncType="getter")
        const ShaderKeyword &getShaderKeyword() const { return *mShaderKeyword; }

        /**
         * \brief 设置 shader 变量值
         * \param [in] name : 变量名
         * \param [in] data : 变量对应的数据 
         * \return 调用成功返回 T3D_OK
         */
        TResult setParam(const String &name, void *data);

        /**
         * \brief 设置 shader 采样器
         * \param [in] name : 采样器名称
         * \param [in] index : 采样器索引
         * \return 调用成功返回 T3D_OK
         */
        TResult setParam(const String &name, int32_t index);
        
        bool hasCompiled() const { return mHasCompiled; }

        char *getBytesCode(size_t &bytesLength) const
        {
            bytesLength = mBytesCodeSize;
            return mBytesCode;
        }

        void setBytesCode(const char *bytes, size_t bytesLength)
        {
            copyCode(bytes, bytesLength);
            mHasCompiled = true;
        }

        void setSourceCode(const char *code, size_t codeLength)
        {
            copyCode(code, codeLength);
            mHasCompiled = false;
        }

    private:
        ShaderVariant() = default;

        TPROPERTY(RTTRFuncName="ShaderKeyword", RTTRFuncType="setter")
        void setShaderKeyword(const ShaderKeyword &keyword)
        {
            if (mShaderKeyword == nullptr)
            {
                mShaderKeyword = new ShaderKeyword(keyword);
            }
            else
            {
                *mShaderKeyword = keyword;
            }
        }

        TPROPERTY(RTTRFuncName="Code", RTTRFuncType="getter")
        String getSourceCode() const
        {
            return String(mBytesCode, mBytesCodeSize);
        }

        TPROPERTY(RTTRFuncName="Code", RTTRFuncType="setter")
        void setSourceCode(String code)
        {
            setSourceCode(&code[0], code.length());
        }
        
    protected:
        ShaderVariant(ShaderKeyword &&key, const String &code);

        void copyCode(const char *code, size_t codeSize)
        {
            if (mBytesCodeCapacity < codeSize || mBytesCodeCapacity == 0)
            {
                T3D_SAFE_DELETE_ARRAY(mBytesCode);
                mBytesCode = new char[codeSize];
                mBytesCodeCapacity = codeSize;
            }
            memcpy(mBytesCode, code, codeSize);
            mBytesCodeSize = codeSize;
        }

        /// shader keyword
        ShaderKeyword   *mShaderKeyword {nullptr};
        
        /// 编译前是源代码 ，编译后是字节码 
        char        *mBytesCode {nullptr};
        /// mBytesCode 的长度
        size_t      mBytesCodeSize {0};
        /// mBytesCode 的空间容量
        size_t      mBytesCodeCapacity {0};
        /// 是否编译
        bool        mHasCompiled {false};
    };
}


#endif    /*__T3D_SHADER_VARIANT_H__*/
