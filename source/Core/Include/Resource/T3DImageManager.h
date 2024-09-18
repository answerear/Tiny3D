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

#ifndef __T3D_IMAGE_MANAGER_H__
#define __T3D_IMAGE_MANAGER_H__


#include "Kernel/T3DConstant.h"
#include "Resource/T3DResourceManager.h"


namespace Tiny3D
{
    class T3D_ENGINE_API ImageManager
        : public Singleton<ImageManager>
        , public ResourceManager
    {
    public:
        /**
         * @brief 创建图像资源管理器对象
         */
        static ImageManagerPtr create();

        /**
         * @brief 创建图像资源对象
         * @param [in] name : 图像资源的名称
         * @param [in] width : 图像宽度 
         * @param [in] height : 图像高度 
         * @param [in] format : 图像像素格式
         * @return 调用成功返回图像对象，否则返回 nullptr
         */
        ImagePtr createImage(const String &name, uint32_t width, uint32_t height, PixelFormat format);

        /**
         * @brief 根据文件名，加载图像对象
         * @param [in] archive : 从该档案对象中加载图像对象
         * @param [in] filename : 图像文件名
         * @return 调用成功返回图像对象，否则返回 nullptr
         */
        ImagePtr loadImage(Archive *archive, const String &filename);

        /**
         * @brief 根据 UUID 加载图像对象
         * @param [in] archive : 从该档案对象中加载图像对象
         * @param [in] uuid : 资源的 UUID
         * @return 调用成功返回图像对象，否则返回 nullptr
         */
        ImagePtr loadImage(Archive *archive, const UUID &uuid);

        /**
         * @brief 把图像对象写到对应文件名的图像文件中
         * @param [in] archive : 保存图像文件到该档案对象中
         * @param [in] filename : 图像文件名
         * @param [in] image : 要保存的图像对象 
         * @return 调用成功返回 T3D_OK
         */
        TResult saveImage(Archive *archive, const String &filename, Image *image);

        /**
         * @brief 把图像对象写到 UUID 对应的图像文件中
         * @param [in] archive : 保存图像文件到该档案对象中
         * @param [in] image : 要保存的图像对象
         * @return 调用成功返回 T3D_OK
         */
        TResult saveImage(Archive *archive, Image *image);

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
        TResult saveResource(DataStream &stream, Resource *res) override;
    };

    #define T3D_IMAGE_MGR    (ImageManager::getInstance()) 
}


#endif    /*__T3D_IMAGE_MANAGER_H__*/
