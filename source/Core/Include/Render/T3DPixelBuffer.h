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


#ifndef __T3D_PIXEL_BUFFER_H__
#define __T3D_PIXEL_BUFFER_H__


#include "Render/T3DRenderBuffer.h"
#include "Kernel/T3DConstant.h"


namespace Tiny3D
{
    class T3D_ENGINE_API PixelBuffer : public RenderBuffer
    {
    public:
        static PixelBufferPtr create(const Buffer &buffer, size_t width, size_t height, PixelFormat format, size_t mipmaps, MemoryType memType, Usage usage, uint32_t accMode);
        
        ~PixelBuffer() override = default;

        Type getType() const override;
        
    protected:
        PixelBuffer(const Buffer &buffer, size_t width, size_t height, PixelFormat format, size_t mipmaps, MemoryType memType, Usage usage, uint32_t accMode);

        bool onLoad() override;

        bool onUnload() override;

        /**< 像素缓冲区的宽度 */
        size_t mWidth {0};
        /**< 像素缓冲区的高度 */
        size_t mHeight {0};
        /**< 像素缓冲区的行跨度 */
        size_t mPitch {0};
        /**< mipmaps level */
        size_t mMipmaps {0};
        /**< 像素缓冲区的格式 */
        PixelFormat mFormat {PixelFormat::E_PF_UNKNOWN};
    };
}


#endif  /*__T3D_PIXEL_BUFFER_H__*/
