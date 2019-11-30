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


#ifndef __T3D_GPU_PROGRAM_MANAGER_H__
#define __T3D_GPU_PROGRAM_MANAGER_H__


#include "Resource/T3DResourceManager.h"
#include "Resource/T3DGPUProgram.h"


namespace Tiny3D
{
    class ShaderCreator;
    class GPUProgramCreator;

    //--------------------------------------------------------------------------

    /**
     * @brief 着色器资源管理器
     */
    class T3D_ENGINE_API ShaderManager
        : public Singleton<ShaderManager>
        , public ResourceManager
    {
    public:
        /**
         * @brief 创建着色器管理器对象
         */
        static ShaderManagerPtr create();

        /**
         * @brief 析构函数
         */
        virtual ~ShaderManager();

        /**
         * @brief 设置着色器创建器
         */
        void setShaderCreator(ShaderCreator *creator);

        /**
         * @brief 加载着色器资源
         * @param [in] shaderType : 着色器类型
         * @param [in] name : 着色器资源名称
         * @return 返回一个着色器对象
         */
        virtual ShaderPtr loadShader(Shader::ShaderType shaderType, 
            const String &name);

        /**
         * @brief 加载着色器资源
         * @param [in] shaderType : 着色器类型
         * @param [in] name : 着色器名称
         * @param [in] content : 着色器程序内容
         * @return 返回一个着色器对象
         */
        virtual ShaderPtr loadShader(Shader::ShaderType shaderType, 
            const String &name, const String &content);

        /**
         * @brief 卸载着色器资源
         * @param [in] shader : 需要卸载的着色器对象
         */
        virtual TResult unloadShader(ShaderPtr shader);

    protected:
        /**
         * @brief 构造函数
         */
        ShaderManager();

        /**
         * @brief 重写 Resource::create() 接口
         */
        virtual ResourcePtr create(const String &name, int32_t argc,
            va_list args) override;

    protected:
        ShaderCreator   *mCreator;      /**< GPU程序创建器对象 */
    };

    #define T3D_SHADER_MGR      (ShaderManager::getInstance())

    //--------------------------------------------------------------------------

    /**
     * @brief GPU程序资源管理器
     */
    class T3D_ENGINE_API GPUProgramManager 
        : public Singleton<GPUProgramManager>
        , public ResourceManager
    {
    public:
        /**
         * @brief 创建 GPU 程序资源管理器对象
         */
        static GPUProgramManagerPtr create();

        /**
         * @brief 析构函数
         */
        virtual ~GPUProgramManager();

        /**
         * @brief 设置GPU程序创建器
         */
        void setGPUProgramCreator(GPUProgramCreator *creator);

        /**
         * @brief 加载GPU程序
         * @param [in] name : GPU程序名称
         * @return 返回一个新建的GPU程序对象
         */
        virtual GPUProgramPtr loadGPUProgram(const String &name);

        /**
         * @brief 卸载GPU程序
         * @param [in] program : 需要卸载的GPU程序对象
         */
        virtual TResult unloadGPUProgram(GPUProgramPtr program);

        /**
         * @fn  TResult loadBuiltInResources();
         * @brief   Loads built in resources
         * @returns The built in resources.
         */
        TResult loadBuiltInResources();

    protected:
        /**
         * @brief 构造函数
         */
        GPUProgramManager();

        /**
         * @brief 重写 Resource::create() 接口
         */
        virtual ResourcePtr create(const String &name, int32_t argc,
            va_list args) override;

    protected:
        GPUProgramCreator   *mCreator;      /**< GPU程序创建器对象 */
    };

    #define T3D_GPU_PROGRAM_MGR     (GPUProgramManager::getInstance())
}


#endif  /*__T3D_GPU_PROGRAM_MANAGER_H__*/
