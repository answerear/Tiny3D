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


#ifndef __T3D_IMAGE_H__
#define __T3D_IMAGE_H__


#include "Resource/T3DResource.h"
#include "Kernel/T3DConstant.h"


namespace Tiny3D
{
    /**
     * \brief CPU 端图像资源，持有像素缓冲及格式/尺寸等元数据，支持编解码与图像处理
     */
    TCLASS()
    class T3D_ENGINE_API Image : public Resource
    {
        TRTTI_ENABLE(Resource)
        TRTTI_FRIEND
        
        friend class ImageCodecBase;
    
    public:
        /**
         * \brief 图像文件格式
         */
        TENUM()
        enum class FileFormat : uint32_t
        {
            kUnknown = 0,
            /// 原始像素数据
            kRAW,
            /// PNG
            kPNG,
            /// JPEG
            kJPG,
            /// BMP
            kBMP,
            /// TGA
            kTGA,
            /// DDS
            kDDS,
            /// PVRTC 压缩纹理
            kPVRTC,
            /// ASTC 压缩纹理
            kASTC,
            /// ETC1 压缩纹理
            kETC1,
            /// ETC2 压缩纹理
            kETC2
        };

        /**
         * \brief 图像缩放/重采样滤波方式
         */
        TENUM()
        enum class Filter : uint32_t
        {
            /// Box 滤波（一阶 B-Spline）
            E_FILTER_BOX = 0,
            /// Mitchell-Netravali 双三次滤波
            E_FILTER_BICUBIC = 1,
            /// 双线性滤波
            E_FILTER_BILINEAR = 2,
            /// 四阶 B-Spline
            E_FILTER_BSPLINE = 3,
            /// Catmull-Rom / Overhauser 样条
            E_FILTER_CATMULLROM = 4,
            /// Lanczos3 滤波
            E_FILTER_LANCZOS3 = 5
        };
        
        /**
         * \brief 创建空 Image 资源
         * \param [in] name : 资源名称
         * \return 新建的 Image 智能指针
         */
        static ImagePtr create(const String &name);

        /**
         * \brief 创建指定尺寸的 Image 并分配零填充像素缓冲
         * \param [in] name : 资源名称
         * \param [in] width : 图像宽度（像素）
         * \param [in] height : 图像高度（像素）
         * \param [in] pixelFormat : 像素格式
         * \return 新建的 Image 智能指针
         */
        static ImagePtr create(const String &name, uint32_t width, uint32_t height, PixelFormat pixelFormat);

        /// 析构并释放 mData（若非外部持有）
        virtual ~Image();

        /**
         * \brief 获取资源类型
         * \return 固定返回 Type::kImage
         */
        Type getType() const override;

        // TResult load(const String &path);
        //
        // TResult load(DataStream &stream);
        //
        // TResult load(uint8_t *data, size_t width, size_t height, size_t bpp, size_t pitch, PixelFormat format, bool copySource = false);
        //
        // TResult save(const String &path, FileFormat fileFormat = FileFormat::kPNG) const;
        //
        // TResult save(DataStream &stream, FileFormat fileFormat = FileFormat::kPNG) const;

        /**
         * \brief 释放像素数据并重置尺寸元数据
         * \return 调用成功返回 T3D_OK
         * \remarks 非空时 delete[] mData 并将 mIsEmpty 置 true
         */
        TResult destroy();

        /**
         * \brief 垂直翻转图像
         * \return 转调 ImageCodec 的返回值
         */
        TResult flip();

        /**
         * \brief 水平镜像图像
         * \return 转调 ImageCodec 的返回值
         */
        TResult mirror();

        /**
         * \brief 用指定颜色填充整幅图像
         * \param [in] rkColor : 填充颜色
         * \return 转调 ImageCodec 的返回值
         */
        TResult fill(const Color4 &rkColor);

        /**
         * \brief 从另一幅图像复制像素到本图像
         * \param [in] image : 源图像
         * \param [in] srcRect : 源区域，nullptr 表示整幅源图
         * \param [in] dstRect : 目标区域，nullptr 表示整幅目标图
         * \param [in] filter : 缩放滤波方式
         * \return 调用成功返回 T3D_OK
         * \remarks 本图像为空时直接 memcpy 整幅源数据；否则走 ImageCodec::copy
         */
        TResult copy(const Image &image, const Rect *srcRect = nullptr, const Rect *dstRect = nullptr, Filter filter = Filter::E_FILTER_BILINEAR);

        /**
         * \brief 将本图像转换为指定像素格式
         * \param [in] format : 目标像素格式
         * \return 当前实现为空操作，始终返回 T3D_OK
         */
        TResult convert(PixelFormat format);

        /**
         * \brief 将本图像转换并写入目标 Image
         * \param [out] image : 输出图像
         * \param [in] format : 目标像素格式
         * \return 当前实现为空操作，始终返回 T3D_OK
         */
        TResult convert(Image &image, PixelFormat format) const;

        /**
         * \brief 比较两幅图像是否相同
         * \param [in] other : 待比较的图像
         * \param [in] compareAlpha : 是否比较 alpha 通道
         * \return 当前实现始终返回 false
         */
        bool compare(const Image &other, bool compareAlpha = true) const;

        /// 获取源文件格式
        FileFormat getFileFormat() const;

        /// 获取可写像素数据指针
        uint8_t *getData();

        /// 获取只读像素数据指针
        const uint8_t * const getData() const;

        /// 获取像素数据字节数（mHeight * mPitch）
        size_t getSize() const;

        /// 获取图像宽度（像素）
        uint32_t getWidth() const;

        /// 获取图像高度（像素）
        uint32_t getHeight() const;

        /// 获取行跨度（字节）
        uint32_t getPitch() const;

        /// 获取每像素位数
        uint32_t getBPP() const;

        /// 获取像素格式
        PixelFormat getFormat() const;

        /// 是否含 alpha 通道
        bool hasAlpha() const;

        /// 是否为预乘 alpha
        bool isPremultipliedAlpha() const;

        /// 是否为空图像（无有效像素数据）
        bool isEmpty() const;

        /**
         * \brief 按当前像素格式获取 RGBA 分量位掩码
         * \param [out] redMask : 红色分量掩码
         * \param [out] greenMask : 绿色分量掩码
         * \param [out] blueMask : 蓝色分量掩码
         * \param [out] alphaMask : alpha 分量掩码
         * \remarks 仅对 E_PF_B5G5R5A1、E_PF_B5G6R5、E_PF_B4R4G4A4 等格式赋值；其余格式输出未定义
         */
        void getColorMask(uint32_t &redMask, uint32_t &greenMask, uint32_t &blueMask, uint32_t &alphaMask) const;

        /**
         * \brief 计算 4 字节对齐的行跨度
         * \param [in] width : 行宽（像素）
         * \param [in] bpp : 每像素位数
         * \return 对齐后的 pitch（字节）
         */
        static uint32_t calcPitch(uint32_t width, uint32_t bpp);

        /**
         * \brief 根据像素格式查询每像素位数
         * \param [in] format : 像素格式
         * \return 对应 bpp；未知格式返回 0
         */
        static uint32_t getBPP(PixelFormat format);

        /// 获取每像素字节数（mBPP >> 3）
        uint32_t getBytesPerPixel() const;

    protected:
        /**
         * \brief 构造空 Image
         * \param [in] name : 资源名称
         */
        Image(const String &name);
        
        /**
         * \brief 构造指定尺寸 Image 并分配像素缓冲
         * \param [in] name : 资源名称
         * \param [in] width : 宽度
         * \param [in] height : 高度
         * \param [in] pixelFormat : 像素格式
         */
        Image(const String &name, uint32_t width, uint32_t height, PixelFormat pixelFormat);
        
        /**
         * \brief 克隆 Image 资源
         * \return 新 Image 智能指针
         */
        ResourcePtr clone() const override;

        /**
         * \brief 从源 Image 拷贝属性
         * \param [in] src : 源资源对象
         * \remarks 当前实现为空
         */
        void cloneProperties(const Resource * const src) override;

        /**
         * \brief 资源创建回调
         * \return 转调 Resource::onCreate
         */
        TResult onCreate() override;

        /**
         * \brief 资源保存前回调
         * \param [in] archive : 目标档案
         * \return 转调 Resource::onSave
         */
        TResult onSave(Archive *archive) override;

        /**
         * \brief 资源加载后回调
         * \param [in] archive : 来源档案
         * \return 转调 Resource::onLoad
         */
        TResult onLoad(Archive *archive) override;

        /**
         * \brief 资源卸载回调
         * \return 转调 Resource::onUnload
         */
        TResult onUnload() override;
        
        /// 根据当前 mWidth、mBPP 计算 pitch
        uint32_t calcPitch() const;

    private:
        /// 图像数据源类型
        FileFormat  mFileFormat {FileFormat::kUnknown};
        /// 图像宽度
        uint32_t    mWidth {0};
        /// 图像高度
        uint32_t    mHeight {0};
        /// 图像色深（每像素位数）
        uint32_t    mBPP {0};
        /// 图像行跨度
        uint32_t    mPitch {0};
        /// 图像像素格式
        PixelFormat mPixelFormat {PixelFormat::E_PF_UNKNOWN};
        /// 是否带 alpha 通道
        bool        mHasAlpha {false};
        /// 是否预乘 alpha
        bool        mIsPreMulti {false};
        /// 是否为空图像
        bool        mIsEmpty {true};
        /// 图像像素数据
        uint8_t     *mData {nullptr};
        /// 图像数据大小
        size_t      mDataSize {0};
    };
}


#include "T3DImage.inl"


#endif  /*__T3D_IMAGE_H__*/
