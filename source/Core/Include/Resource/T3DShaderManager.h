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
    class T3D_ENGINE_API ShaderManager
        : public Singleton<ShaderManager>
        , public ResourceManager
    {
    public:
        /**
         * @brief 创建 shader 管理器对象
         */
        static ShaderManagerPtr create();

        /**
         * @brief 创建 shader 对象
         * @param [in] name : shader 名称
         * @return 调用成功返回 shader 对象，否则返回 nullptr
         */
        ShaderPtr createShader(const String &name);

        /**
         * @brief 根据文件名，加载 shader 对象
         * @param [in] archive : 从该档案对象中加载 shader 对象
         * @param [in] filename : shader 文件名
         * @return 调用成功返回 shader 对象，否则返回 nullptr
         */
        ShaderPtr loadShader(Archive *archive, const String &filename);

        /**
         * @brief 根据 UUID 加载 shader 对象
         * @param [in] archive : 从该档案对象中加载 shader 对象
         * @param [in] uuid : 资源的 UUID
         * @return 调用成功返回 shader 对象，否则返回 nullptr
         */
        ShaderPtr loadShader(Archive *archive, const UUID &uuid);

        /**
         * @brief 把 mesh 对象写到对应文件名的 shader 文件中
         * @param [in] archive : 保存 shader 文件到该档案对象中
         * @param [in] filename : shader 文件名
         * @param [in] shader : 要保存的 shader 对象
         * @return 调用成功返回 T3D_OK
         */
        TResult saveShader(Archive *archive, const String &filename, Shader *shader);

        /**
         * @brief 把 mesh 对象写到 UUID 对应的 shader 文件中
         * @param [in] archive : 保存 shader 文件到该档案对象中
         * @param [in] shader : 要保存的 shader 对象
         * @return 调用成功返回 T3D_OK
         */
        TResult saveShader(Archive *archive, Shader *shader);

    protected:
        /**
         * 重写基类接口
         */
        ResourcePtr newResource(const String &name, int32_t argc, va_list args) override;

        /**
         * 重写基类接口
         */
        ResourcePtr loadResource(const String &name, DataStream &stream) override;

        /**
         * 重写基类接口
         */
        ResourcePtr loadResource(DataStream &stream) override;

        /**
         * 重写基类接口
         */
        TResult saveResource(DataStream &stream, Resource *res) override;
    };

    #define T3D_SHADER_MGR    (ShaderManager::getInstance()) 
}


#endif    /*__T3D_SHADER_MANAGER_H__*/
