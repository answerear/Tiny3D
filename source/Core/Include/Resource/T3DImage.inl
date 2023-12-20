/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
