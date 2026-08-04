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
    /**
     * \brief Image 资源管理器，负责创建、加载与保存 Image
     */
    class T3D_ENGINE_API ImageManager
        : public Singleton<ImageManager>
        , public ResourceManager
    {
    public:
        /**
         * \brief 创建 ImageManager 单例对象
         * \return 新建的 ImageManager 智能指针
         */
        static ImageManagerPtr create();

        /**
         * \brief 创建指定尺寸的 Image 资源
         * \param [in] name : 资源名称
         * \param [in] width : 图像宽度
         * \param [in] height : 图像高度
         * \param [in] format : 像素格式
         * \return 成功返回 Image 智能指针；createResource 失败时返回 nullptr
         */
        ImagePtr createImage(const String &name, uint32_t width, uint32_t height, PixelFormat format);

        /**
         * \brief 按文件名从档案加载 Image
         * \param [in] archive : 来源档案
         * \param [in] filename : 图像文件名
         * \return 成功返回 Image 智能指针；加载失败时返回 nullptr
         */
        ImagePtr loadImage(Archive *archive, const String &filename);

        /**
         * \brief 按 UUID 从档案加载 Image
         * \param [in] archive : 来源档案
         * \param [in] uuid : 资源 UUID
         * \return 成功返回 Image 智能指针；加载失败时返回 nullptr
         */
        ImagePtr loadImage(Archive *archive, const UUID &uuid);

        /**
         * \brief 将 Image 保存到指定文件名
         * \param [in] archive : 目标档案
         * \param [in] filename : 保存文件名
         * \param [in] image : 要保存的 Image 对象
         * \return 转调 ResourceManager::save 的返回值
         */
        TResult saveImage(Archive *archive, const String &filename, Image *image);

        /**
         * \brief 将 Image 保存到以其 UUID 为键的路径
         * \param [in] archive : 目标档案
         * \param [in] image : 要保存的 Image 对象
         * \return 转调 ResourceManager::save 的返回值
         */
        TResult saveImage(Archive *archive, Image *image);

    protected:
        /**
         * \brief 创建 Image 实例（argc 须为 3：width、height、format）
         * \param [in] name : 资源名称
         * \param [in] argc : 可变参数个数
         * \param [in] args : 可变参数列表
         * \return Image::create 的结果
         */
        ResourcePtr newResource(const String &name, int32_t argc, va_list args) override;

        /**
         * \brief 从数据流解码 Image
         * \param [in] name : 资源名称
         * \param [in,out] stream : 输入数据流
         * \return 解码成功返回 Image；失败返回 nullptr
         */
        ResourcePtr loadResource(const String &name, DataStream &stream) override;

        /**
         * \brief 将 Image 序列化到数据流（未实现）
         * \param [in,out] stream : 输出数据流
         * \param [in] res : 资源对象
         * \return 返回 T3D_ERR_NOT_IMPLEMENT
         */
        TResult saveResource(DataStream &stream, Resource *res) override;
    };

    #define T3D_IMAGE_MGR    (ImageManager::getInstance()) 

    T3D_EXTERN_SINGLETON(ImageManager)
}


#endif    /*__T3D_IMAGE_MANAGER_H__*/
