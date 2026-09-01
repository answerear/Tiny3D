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

#ifndef __T3D_TEXTURE_MANAGER_H__
#define __T3D_TEXTURE_MANAGER_H__


#include "Resource/T3DResourceManager.h"
#include "Kernel/T3DConstant.h"
#include "Render/T3DRenderConstant.h"


namespace Tiny3D
{
    /**
     * \brief 纹理资源单例管理器，负责各类型纹理的创建、加载与保存
     */
    class T3D_ENGINE_API TextureManager
        : public Singleton<TextureManager>
        , public ResourceManager
    {
    public:
        /**
         * \brief 创建 TextureManager 实例
         * \return 新创建的 TextureManager 智能指针
         */
        static TextureManagerPtr create();

        /**
         * \brief 创建渲染目标纹理
         * \param [in] name : 纹理名称
         * \param [in] width : 宽度（像素）
         * \param [in] height : 高度（像素）
         * \param [in] format : 像素格式
         * \param [in] mipmaps : mipmap 层数，默认 1
         * \param [in] MSAACount : MSAA 采样数，默认 1
         * \param [in] MSAAQuality : MSAA 质量等级，默认 0
         * \param [in] shaderReadable : 是否允许着色器读取，默认 false
         * \param [in] accMode : CPU 访问许可，传 kCPURead 才允许 Texture::map 读回
         * \return 成功返回 RenderTexture 智能指针，失败返回 nullptr
         */
        RenderTexturePtr createRenderTexture(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps = 1, uint32_t MSAACount = 1, uint32_t MSAAQuality = 0, bool shaderReadable = false, uint32_t accMode = kCPUNone);

        /**
         * \brief 创建 1D 纹理
         * \param [in] name : 纹理名称
         * \param [in] width : 宽度（像素）
         * \param [in] format : 像素格式
         * \param [in] data : 像素数据缓冲区
         * \param [in] mipmaps : mipmap 层数，默认 1
         * \return 成功返回 Texture1D 智能指针，失败返回 nullptr
         */
        Texture1DPtr createTexture1D(const String &name, uint32_t width, PixelFormat format, const Buffer &data, uint32_t mipmaps = 1);

        /**
         * \brief 用原始像素数据创建 2D 纹理
         * \param [in] name : 纹理名称
         * \param [in] width : 宽度（像素）
         * \param [in] height : 高度（像素）
         * \param [in] format : 像素格式
         * \param [in] data : 像素数据缓冲区
         * \param [in] mipmaps : mipmap 层数，默认 1
         * \param [in] MSAACount : MSAA 采样数，默认 1
         * \param [in] MSAAQuality : MSAA 质量等级，默认 0
         * \param [in] uuid : 资源 UUID；T3D_EDITOR 下可指定以保留已有 guid，默认 UUID::INVALID
         * \param [in] accMode : CPU 访问许可，传 kCPURead 才允许 Texture::map 读回
         * \return 成功返回 Texture2D 智能指针，失败返回 nullptr
         */
        Texture2DPtr createTexture2D(const String &name, uint32_t width, uint32_t height, PixelFormat format, const Buffer &data, uint32_t mipmaps = 1, uint32_t MSAACount = 1, uint32_t MSAAQuality = 0, const UUID &uuid = UUID::INVALID, uint32_t accMode = kCPUNone);

        /**
         * \brief 从 Image 创建 2D 纹理
         * \param [in] name : 纹理名称
         * \param [in] image : 源图像，不可为 nullptr
         * \param [in] mipmaps : mipmap 层数，默认 1
         * \param [in] MSAACount : MSAA 采样数，默认 1
         * \param [in] MSAAQuality : MSAA 质量等级，默认 0
         * \param [in] uuid : 资源 UUID；T3D_EDITOR 下可指定以保留已有 guid，默认 UUID::INVALID
         * \param [in] accMode : CPU 访问许可，传 kCPURead 才允许 Texture::map 读回
         * \return 成功返回 Texture2D 智能指针，失败返回 nullptr
         */
        Texture2DPtr createTexture2D(const String &name, Image *image, uint32_t mipmaps = 1, uint32_t MSAACount = 1, uint32_t MSAAQuality = 0, const UUID &uuid = UUID::INVALID, uint32_t accMode = kCPUNone);

        /**
         * \brief 创建 2D 纹理数组
         * \param [in] name : 纹理名称
         * \param [in] width : 单层宽度（像素）
         * \param [in] height : 单层高度（像素）
         * \param [in] format : 像素格式
         * \param [in] arraySize : 数组层数
         * \param [in] data : 各层拼接的像素数据
         * \param [in] mipmaps : mipmap 层数，默认 1
         * \param [in] MSAACount : MSAA 采样数，默认 1
         * \param [in] MSAAQuality : MSAA 质量等级，默认 0
         * \return 成功返回 Texture2DArray 智能指针，失败返回 nullptr
         */
        Texture2DArrayPtr createTexture2DArray(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize, const Buffer &data, uint32_t mipmaps = 1, uint32_t MSAACount = 1, uint32_t MSAAQuality = 0);
        
        /**
         * \brief 创建 3D 体积纹理
         * \param [in] name : 纹理名称
         * \param [in] width : 宽度（像素）
         * \param [in] height : 高度（像素）
         * \param [in] depth : 深度（像素）
         * \param [in] format : 像素格式
         * \param [in] data : 体素数据缓冲区
         * \param [in] mipmaps : mipmap 层数，默认 1
         * \return 成功返回 Texture3D 智能指针，失败返回 nullptr
         */
        Texture3DPtr createTexture3D(const String &name, uint32_t width, uint32_t height, uint32_t depth, PixelFormat format, const Buffer &data, uint32_t mipmaps = 1);

        /**
         * \brief 用原始像素数据创建 Cubemap
         * \param [in] name : 纹理名称
         * \param [in] width : 单面宽度（像素）
         * \param [in] height : 单面高度（像素）
         * \param [in] format : 像素格式
         * \param [in] data : 六面拼接的像素数据，Cubemap 接管 data.Data 所有权
         * \param [in] mipmaps : mipmap 层数，默认 1
         * \param [in] MSAACount : MSAA 采样数，默认 1
         * \param [in] MSAAQuality : MSAA 质量等级，默认 0
         * \param [in] uuid : 资源 UUID；T3D_EDITOR 下可指定以保留已有 guid，默认 UUID::INVALID
         * \return 成功返回 Cubemap 智能指针，失败返回 nullptr
         */
        CubemapPtr createCubemap(const String &name, uint32_t width, uint32_t height, PixelFormat format, const Buffer &data, uint32_t mipmaps = 1, uint32_t MSAACount = 1, uint32_t MSAAQuality = 0, const UUID &uuid = UUID::INVALID);

        /**
         * \brief 用 6 张面贴图创建 Cubemap
         * \param [in] name : 纹理名称
         * \param [in] faces : 按 +X、-X、+Y、-Y、+Z、-Z 顺序排列的 6 张 Image；faces 或 faces[0] 为 nullptr，或任一面尺寸/格式不一致时返回 nullptr
         * \param [in] mipmaps : mipmap 层数，默认 1
         * \param [in] uuid : 资源 UUID；T3D_EDITOR 下可指定以保留已有 guid，默认 UUID::INVALID
         * \return 成功返回 Cubemap 智能指针，校验失败返回 nullptr
         */
        CubemapPtr createCubemapFromImages(const String &name, Image * const faces[6], uint32_t mipmaps = 1, const UUID &uuid = UUID::INVALID);

        /**
         * \brief 创建 Cubemap 数组
         * \param [in] name : 纹理名称
         * \param [in] width : 单面宽度（像素）
         * \param [in] height : 单面高度（像素）
         * \param [in] format : 像素格式
         * \param [in] arraySize : 数组中 Cubemap 个数
         * \param [in] data : 各 Cubemap 六面拼接的像素数据
         * \param [in] mipmaps : mipmap 层数，默认 1
         * \param [in] MSAACount : MSAA 采样数，默认 1
         * \param [in] MSAAQuality : MSAA 质量等级，默认 0
         * \return 成功返回 CubemapArray 智能指针，失败返回 nullptr
         */
        CubemapArrayPtr createCubemapArray(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize, const Buffer &data, uint32_t mipmaps = 1, uint32_t MSAACount = 1, uint32_t MSAAQuality = 0);

        /**
         * \brief 按文件名从 Archive 加载纹理
         * \param [in] archive : 资源档案
         * \param [in] filename : 纹理文件名
         * \return 成功返回 Texture 智能指针，失败返回 nullptr
         */
        TexturePtr loadTexture(Archive *archive, const String &filename);

        /**
         * \brief 按 UUID 从 Archive 加载纹理
         * \param [in] archive : 资源档案
         * \param [in] uuid : 资源 UUID
         * \return 成功返回 Texture 智能指针，失败返回 nullptr
         */
        TexturePtr loadTexture(Archive *archive, const UUID &uuid);

        /**
         * \brief 将纹理保存到 Archive 指定文件名
         * \param [in] archive : 目标档案
         * \param [in] filename : 保存文件名
         * \param [in] texture : 待保存纹理，不可为 nullptr
         * \return 成功返回 T3D_OK
         */
        TResult saveTexture(Archive *archive, const String &filename, Texture *texture);

        /**
         * \brief 将纹理按自身 UUID 保存到 Archive
         * \param [in] archive : 目标档案
         * \param [in] texture : 待保存纹理，不可为 nullptr
         * \return 成功返回 T3D_OK
         */
        TResult saveTexture(Archive *archive, Texture *texture);

    protected:
        /**
         * \brief 按 TEXTURE_TYPE 分派，创建对应类型的 Texture 资源
         * \param [in] name : 资源名称
         * \param [in] argc : 可变参数个数
         * \param [in] args : 可变参数列表，首参为 TEXTURE_TYPE
         * \return 创建的 Texture 智能指针
         */
        ResourcePtr newResource(const String &name, int32_t argc, va_list args) override;

        /**
         * \brief 从数据流反序列化纹理（忽略 name，委托 loadResource(stream)）
         * \param [in] name : 资源名称（未使用）
         * \param [in,out] stream : 输入数据流
         * \return 反序列化得到的 Texture 智能指针
         */
        ResourcePtr loadResource(const String &name, DataStream &stream) override;

        /**
         * \brief 从数据流反序列化纹理
         * \param [in,out] stream : 输入数据流
         * \return 反序列化得到的 Texture 智能指针
         */
        ResourcePtr loadResource(DataStream &stream) override;
        
        /**
         * \brief 将 Texture 序列化写入数据流
         * \param [in,out] stream : 输出数据流
         * \param [in] res : 待保存的 Texture 资源
         * \return 成功返回 T3D_OK
         */
        TResult saveResource(DataStream &stream, Resource *res) override;
    };

    #define T3D_TEXTURE_MGR    (TextureManager::getInstance()) 

    T3D_EXTERN_SINGLETON(TextureManager)
}


#endif    /*__T3D_TEXTURE_MANAGER_H__*/
