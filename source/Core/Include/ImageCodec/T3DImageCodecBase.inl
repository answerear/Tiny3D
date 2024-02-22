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


namespace Tiny3D
{
    //--------------------------------------------------------------------------
    
    inline void ImageCodecBase::setImageData(Image &image, uint8_t *data, 
        size_t size)
    {
        image.mData = data;
        image.mDataSize = size;
    }

    //--------------------------------------------------------------------------
    
    inline void ImageCodecBase::setImageDimension(Image &image, int32_t width, 
        int32_t height, int32_t pitch)
    {
        image.mWidth = width;
        image.mHeight = height;
        image.mPitch = pitch;
    }

    //--------------------------------------------------------------------------
    
    inline void ImageCodecBase::setImageInfo(Image &image, Image::FileFormat fileFormat,
        int32_t bpp, bool hasAlpha, bool isPreMulti, PixelFormat ePixelFormat)
    {
        image.mBPP = bpp;
        image.mFileFormat = fileFormat;
        image.mHasAlpha = hasAlpha;
        image.mIsPreMulti = isPreMulti;
        image.mPixelFormat = ePixelFormat;
    }

    //--------------------------------------------------------------------------
}
