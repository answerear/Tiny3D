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
    TCLASS()
    class T3D_ENGINE_API Image : public Resource
    {
        TRTTI_ENABLE(Resource)
        TRTTI_FRIEND
        
        friend class ImageCodecBase;
    
    public:
        TENUM()
        enum class FileFormat : uint32_t
        {
            kUnknown = 0,
            /// The filetype raw
            kRAW,
            /// The filetype PNG
            kPNG,
            /// The filetype JPEG
            kJPG,
            /// The filetype bitmap
            kBMP,
            /// The filetype tga
            kTGA,
            /// The filetype DDS
            kDDS,
            /// The filetype pvrtc
            kPVRTC,
            /// The filetype astc
            kASTC,
            /// The first filetype etc
            kETC1,
            /// The second filetype etc
            kETC2
        };

        /**
         * @enum    Filter
         * @brief   Values that represent filters
         */
        TENUM()
        enum class Filter : uint32_t
        {
            /**< Box, pulse, Fourier window, 1st order (constant) b-spline */
            E_FILTER_BOX = 0,
            /**< Mitchell & Netravali's two-param cubic filter */
            E_FILTER_BICUBIC = 1,
            /**< Bilinear filter */
            E_FILTER_BILINEAR = 2,
            /**< 4th order (cubic) b-spline */
            E_FILTER_BSPLINE = 3,
            /**< Catmull-Rom spline, Overhauser spline */
            E_FILTER_CATMULLROM = 4,
            /**< Lanczos3 filter */
            E_FILTER_LANCZOS3 = 5
        };
        
        static ImagePtr create(const String &name);

        static ImagePtr create(const String &name, uint32_t width, uint32_t height, PixelFormat pixelFormat);

        virtual ~Image();

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

        TResult destroy();

        TResult flip();

        TResult mirror();

        TResult fill(const Color4 &rkColor);

        TResult copy(const Image &image, const Rect *srcRect = nullptr, const Rect *dstRect = nullptr, Filter filter = Filter::E_FILTER_BILINEAR);

        TResult convert(PixelFormat format);

        TResult convert(Image &image, PixelFormat format) const;

        bool compare(const Image &other, bool compareAlpha = true) const;

        FileFormat getFileFormat() const;

        uint8_t *getData();

        const uint8_t * const getData() const;

        size_t getSize() const;

        uint32_t getWidth() const;

        uint32_t getHeight() const;

        uint32_t getPitch() const;

        uint32_t getBPP() const;

        PixelFormat getFormat() const;

        bool hasAlpha() const;

        bool isPremultipliedAlpha() const;

        bool isEmpty() const;

        void getColorMask(uint32_t &redMask, uint32_t &greenMask, uint32_t &blueMask, uint32_t &alphaMask) const;

        static uint32_t calcPitch(uint32_t width, uint32_t bpp);

        static uint32_t getBPP(PixelFormat format);

        uint32_t getBytesPerPixel() const;

    protected:
        Image(const String &name);
        
        Image(const String &name, uint32_t width, uint32_t height, PixelFormat pixelFormat);
        
        ResourcePtr clone() const override;

        void cloneProperties(const Resource * const src) override;

        TResult onCreate() override;

        TResult onLoad() override;

        TResult onUnload() override;
        
        uint32_t calcPitch() const;

    private:
        /// 图像数据源类型
        FileFormat  mFileFormat {FileFormat::kUnknown};
        /// 图像宽度
        uint32_t    mWidth {0};
        /// 图像高度
        uint32_t    mHeight {0};
        /// 图像色深
        uint32_t    mBPP {0};
        /// 图像行跨度
        uint32_t    mPitch {0};
        /// 图像像素格式
        PixelFormat mPixelFormat {PixelFormat::E_PF_UNKNOWN};
        /// 是否带alpha通道
        bool        mHasAlpha {false};
        /// 是否预乘alpha
        bool        mIsPreMulti {false};
        ///空图像（即没有任何数据的图像）
        bool        mIsEmpty {true};
        /// 图像数据
        uint8_t     *mData {nullptr};
        /// 图像数据大小
        size_t      mDataSize {0};
    };
}


#include "T3DImage.inl"


#endif  /*__T3D_IMAGE_H__*/
