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

#ifndef __T3D_MATERIAL_MANAGER_H__
#define __T3D_MATERIAL_MANAGER_H__


#include "Resource/T3DResourceManager.h"


namespace Tiny3D
{
    class T3D_ENGINE_API MaterialManager
        : public Singleton<MaterialManager>
        , public ResourceManager
    {
    public:
        /**
         * @brief 创建材质资源管理器对象
         */
        static MaterialManagerPtr create();

        /**
         * @brief 创建材质资源对象
         * @param [in] name : 材质名称
         * @param [in] shader : 材质对应的着色器对象 
         * @return 调用成功返回材质对象，否则返回 nullptr
         */
        MaterialPtr createMaterial(const String &name, Shader *shader);

        /**
         * @brief 根据文件名，加载材质对象
         * @param [in] archive : 从该档案对象中加载材质对象
         * @param [in] filename : 材质文件名
         * @return 调用成功返回材质对象，否则返回 nullptr
         */
        MaterialPtr loadMaterial(Archive *archive, const String &filename);

        /**
         * @brief 根据 UUID 加载材质对象
         * @param [in] archive : 从该档案对象中加载材质对象
         * @param [in] uuid : 资源的 UUID
         * @return 调用成功返回材质对象，否则返回 nullptr
         */
        MaterialPtr loadMaterial(Archive *archive, const UUID &uuid);

        /**
         * @brief 把材质对象写到对应文件名的材质文件中
         * @param [in] archive : 保存材质文件到该档案对象中
         * @param [in] filename : 材质文件名
         * @param [in] material : 要保存的材质对象
         * @return 调用成功返回 T3D_OK
         */
        TResult saveMaterial(Archive *archive, const String &filename, Material *material);

        /**
         * @brief 把材质对象写到 UUID 对应的材质文件中
         * @param [in] archive : 保存材质文件到该档案对象中
         * @param [in] material : 要保存的材质对象
         * @return 调用成功返回 T3D_OK
         */
        TResult saveMaterial(Archive *archive, Material *material);

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

    #define T3D_MATERIAL_MGR    (MaterialManager::getInstance()) 
}


#endif    /*__T3D_MATERIAL_MANAGER_H__*/
