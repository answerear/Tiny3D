/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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


#ifndef __T3D_D3D11_GPU_PROGRAM_H__
#define __T3D_D3D11_GPU_PROGRAM_H__


#include "T3DD3D11Prerequisites.h"
#include "T3DD3D11Error.h"


namespace Tiny3D
{
    /**
     * @brief DirectX 11 着色器
     */
    class D3D11Shader : public Shader
    {
    public:
        /**
         * @brief 创建 D3D11Shader 对象
         */
        static D3D11ShaderPtr create(const String &name);

        /**
         * @brief 析构函数
         */
        virtual ~D3D11Shader();

        /**
         * @brief 重写 Shader::compile() 接口
         */
        virtual TResult compile() override;

        /**
         * @brief 重写 Shader::hasCompiled() 接口
         */
        virtual bool hasCompiled() const override;

        /**
         * @brief 获取编译后的字节码
         * @param [in][out] bytecode : 字节码二进制数据
         * @param [in][out] bytecodeLength : 字节码长度
         * @return 调用成功返回T3D_OK
         */
        TResult getBytecode(const void *&bytecode, size_t &bytecodeLength)
        {
            if (!mHasCompiled)
                return T3D_ERR_SHADER_NOT_COMPILED;
            bytecode = mBytecode;
            bytecodeLength = mBytecodeLength;
            return T3D_OK;
        }

    protected:
        /**
         * @brief 构造函数
         */
        D3D11Shader(const String &name);

        /**
         * @brief 重写 Resource::load() 接口
         */
        virtual TResult load() override;

        /**
         * @brief 重写 Resource::unload() 接口
         */
        virtual TResult unload() override;

        /**
         * @brief 重写 Resource::clone() 接口
         */
        virtual ResourcePtr clone() const override;

    protected:
        uint8_t    *mBytecode;          /**< 编译后的字节码 */
        size_t  mBytecodeLength;    /**< 编译后的字节码长度 */
        bool    mHasCompiled;       /**< 是否编译 */
    };

    /**
     * @brief DirectX 11 GPU程序
     */
    class D3D11GPUProgram : public GPUProgram
    {
    public:
        /**
         * @brief 创建 D3D11GPUProgram 对象
         */
        static D3D11GPUProgramPtr create(const String &name);

        /**
         * @brief 析构函数
         */
        virtual ~D3D11GPUProgram();

        /**
         * @brief 重写 GPUProgram::link() 接口
         */
        virtual TResult link(ShaderPtr vertexShader, 
            ShaderPtr pixelShader) override;

        /**
         * @brief 重写 GPUProgram::hasLinked() 接口
         */
        virtual bool hasLinked() const override;

        /**
         * @brief 获取顶点着色器对象
         */
        D3D11ShaderPtr getVertexShader() const
        {
            return mVertexShader;
        }

        /**
         * @brief 获取像素着色器对象
         */
        D3D11ShaderPtr getPixelShader() const
        {
            return mPixelShader;
        }

    protected:
        /**
         * @brief 构造函数
         */
        D3D11GPUProgram(const String &name);

        /**
         * @brief 重写 Resource::load() 接口
         */
        virtual TResult load() override;

        /**
         * @brief 重写 Resource::unload() 接口
         */
        virtual TResult unload() override;

        /**
         * @brief 重写 Resource::clone() 接口
         */
        virtual ResourcePtr clone() const override;

    protected:
        D3D11ShaderPtr  mVertexShader;  /**< 顶点着色器对象 */
        D3D11ShaderPtr  mPixelShader;   /**< 像素着色器对象 */
        bool            mHasLinked;     /**< 是否链接标记 */
    };
}


#endif  /*__T3D_D3D11_GPU_PROGRAM_H__*/