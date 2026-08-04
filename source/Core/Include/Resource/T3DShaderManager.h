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

#ifndef __T3D_SHADER_MANAGER_H__
#define __T3D_SHADER_MANAGER_H__


#include "Resource/T3DResourceManager.h"


namespace Tiny3D
{
    /**
     * \brief 着色器资源单例管理器，负责 Shader 的创建、加载与保存
     */
    class T3D_ENGINE_API ShaderManager
        : public Singleton<ShaderManager>
        , public ResourceManager
    {
    public:
        /**
         * \brief 创建 ShaderManager 实例
         * \return 新 ShaderManager 智能指针
         */
        static ShaderManagerPtr create();

        /**
         * \brief 创建空白 Shader 资源
         * \param [in] name : 着色器名称
         * \return 新 Shader 智能指针
         */
        ShaderPtr createShader(const String &name);

#if defined (T3D_EDITOR)
        /**
         * \brief 创建 Shader 并指定 UUID（保留已有资源 guid）
         * \param [in] name : 着色器名称
         * \param [in] uuid : 资源 UUID
         * \return 新 Shader 智能指针
         */
        ShaderPtr createShader(const String &name, const UUID &uuid);
#endif

        /**
         * \brief 按文件名从 Archive 加载 Shader
         * \param [in] archive : 资源档案
         * \param [in] filename : 着色器文件名
         * \return 成功返回 Shader 智能指针，失败返回 nullptr
         */
        ShaderPtr loadShader(Archive *archive, const String &filename);

        /**
         * \brief 按 UUID 从 Archive 加载 Shader
         * \param [in] archive : 资源档案
         * \param [in] uuid : 资源 UUID
         * \return 成功返回 Shader 智能指针，失败返回 nullptr
         */
        ShaderPtr loadShader(Archive *archive, const UUID &uuid);

        /**
         * \brief 将 Shader 保存到 Archive 指定文件名
         * \param [in] archive : 目标档案
         * \param [in] filename : 保存文件名
         * \param [in] shader : 待保存 Shader，不可为 nullptr
         * \return 成功返回 T3D_OK
         */
        TResult saveShader(Archive *archive, const String &filename, Shader *shader);

        /**
         * \brief 将 Shader 按自身 UUID 保存到 Archive
         * \param [in] archive : 目标档案
         * \param [in] shader : 待保存 Shader，不可为 nullptr
         * \return 成功返回 T3D_OK
         */
        TResult saveShader(Archive *archive, Shader *shader);

    protected:
        /**
         * \brief 创建 Shader 资源；T3D_EDITOR 且 argc==1 时设置指定 UUID
         * \param [in] name : 资源名称
         * \param [in] argc : 可变参数个数（0 或 1）
         * \param [in] args : T3D_EDITOR 下可为 UUID*
         * \return 新 Shader 智能指针
         */
        ResourcePtr newResource(const String &name, int32_t argc, va_list args) override;

        /**
         * \brief 从数据流反序列化 Shader（忽略 name，委托 loadResource(stream)）
         * \param [in] name : 资源名称（未使用）
         * \param [in,out] stream : 输入数据流
         * \return 反序列化得到的 Shader 智能指针
         */
        ResourcePtr loadResource(const String &name, DataStream &stream) override;

        /**
         * \brief 从数据流反序列化 Shader
         * \param [in,out] stream : 输入数据流
         * \return 反序列化得到的 Shader 智能指针
         */
        ResourcePtr loadResource(DataStream &stream) override;

        /**
         * \brief 将 Shader 序列化写入数据流
         * \param [in,out] stream : 输出数据流
         * \param [in] res : 待保存的 Shader 资源（须为 kShader 类型）
         * \return 序列化结果
         */
        TResult saveResource(DataStream &stream, Resource *res) override;
    };

    #define T3D_SHADER_MGR    (ShaderManager::getInstance()) 

    T3D_EXTERN_SINGLETON(ShaderManager)
}


#endif    /*__T3D_SHADER_MANAGER_H__*/
