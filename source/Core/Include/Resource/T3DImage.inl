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

    inline uint32_t Image::getBytesPerPixel() const
    {
        return (mBPP >> 3);
    }
    
    //--------------------------------------------------------------------------

    inline uint32_t Image::calcPitch(uint32_t width, uint32_t bpp)
    {
        return (width * (bpp / 8) + 3) & ~3;
    }

    //--------------------------------------------------------------------------
    
    inline uint32_t Image::calcPitch() const
    {
        return calcPitch(mWidth, mBPP);//(mWidth * (mBPP / 8) + 3) & ~3;
    }
    
    //--------------------------------------------------------------------------

    inline Image::FileFormat Image::getFileFormat() const
    {
        return mFileFormat;
    }

    //--------------------------------------------------------------------------
    
    inline uint8_t *Image::getData()
    {
        return mData;
    }

    //--------------------------------------------------------------------------
    
    inline const uint8_t * const Image::getData() const
    {
        return mData;
    }

    //--------------------------------------------------------------------------
    
    inline size_t Image::getSize() const
    {
        return mHeight * mPitch;
    }

    //--------------------------------------------------------------------------
    
    inline uint32_t Image::getWidth() const
    {
        return mWidth;
    }

    //--------------------------------------------------------------------------
    
    inline uint32_t Image::getHeight() const
    {
        return mHeight;
    }

    //--------------------------------------------------------------------------
    
    inline uint32_t Image::getPitch() const
    {
        return mPitch;
    }

    //--------------------------------------------------------------------------
    
    inline uint32_t Image::getBPP() const
    {
        return mBPP;
    }

    //--------------------------------------------------------------------------
    
    inline PixelFormat Image::getFormat() const
    {
        return mPixelFormat;
    }

    //--------------------------------------------------------------------------
    
    inline bool Image::hasAlpha() const
    {
        return mHasAlpha;
    }

    //--------------------------------------------------------------------------
    
    inline bool Image::isPremultipliedAlpha() const
    {
        return mIsPreMulti;
    }

    //--------------------------------------------------------------------------
    
    inline bool Image::isEmpty() const
    {
        return mIsEmpty;
    }

    //--------------------------------------------------------------------------
}
