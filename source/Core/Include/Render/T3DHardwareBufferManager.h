/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
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

#ifndef __T3D_HARDWARE_BUFFER_MANAGER_H__
#define __T3D_HARDWARE_BUFFER_MANAGER_H__


#include "Misc/T3DObject.h"
#include "Misc/T3DCommon.h"
#include "Render/T3DHardwareVertexBuffer.h"
#include "Render/T3DHardwareIndexBuffer.h"


namespace Tiny3D
{
    class T3D_ENGINE_API HardwareBufferManagerBase : public Object
    {
    public:
        virtual ~HardwareBufferManagerBase();

        virtual HardwareVertexBufferPtr createVertexBuffer(size_t vertexSize, size_t vertexCount, HardwareBuffer::Usage usage, bool useShadowBuffer) = 0;
        virtual HardwareIndexBufferPtr createIndexBuffer(HardwareIndexBuffer::Type indexType, size_t indexCount, HardwareBuffer::Usage usage, bool useShadowBuffer) = 0;
        virtual HardwarePixelBufferPtr createPixelBuffer(uint32_t width, uint32_t height, PixelFormat format, HardwareBuffer::Usage usage, bool useShadowBuffer) = 0;
        virtual VertexDeclarationPtr createVertexDeclaration();

    protected:
        HardwareBufferManagerBase();

        typedef std::set<HardwareVertexBufferPtr>   VertexBufferList;
        typedef VertexBufferList::iterator          VertexBufferListItr;
        typedef VertexBufferList::const_iterator    VertexBufferListConstItr;

        typedef std::set<HardwareIndexBufferPtr>    IndexBufferList;
        typedef IndexBufferList::iterator           IndexBufferListItr;
        typedef IndexBufferList::const_iterator     IndexBufferListConstItr;

        typedef std::set<HardwarePixelBufferPtr>    PixelBufferList;
        typedef PixelBufferList::iterator           PixelBufferListItr;
        typedef PixelBufferList::const_iterator     PixelBufferListConstItr;

        VertexBufferList    mVertexBuffers;
        IndexBufferList     mIndexBuffers;
        PixelBufferList     mPixelBuffers;
    };

    class T3D_ENGINE_API HardwareBufferManager 
        : public HardwareBufferManagerBase
        , public Singleton<HardwareBufferManager>
    {
    public:
        HardwareBufferManager(HardwareBufferManagerBase *impl);
        ~HardwareBufferManager();

        virtual HardwareVertexBufferPtr createVertexBuffer(size_t vertexSize, size_t vertexCount, HardwareBuffer::Usage usage, bool useShadowBuffer) override;
        virtual HardwareIndexBufferPtr createIndexBuffer(HardwareIndexBuffer::Type indexType, size_t indexCount, HardwareBuffer::Usage usage, bool useShadowBuffer) override;
        virtual HardwarePixelBufferPtr createPixelBuffer(uint32_t width, uint32_t height, PixelFormat format, HardwareBuffer::Usage usage, bool useShadowBuffer) override;
        virtual VertexDeclarationPtr createVertexDeclaration() override;

    protected:
        HardwareBufferManagerBasePtr   mImpl;
    };

    #define T3D_HARDWARE_BUFFER_MGR     (HardwareBufferManager::getInstance())
}


#endif  /*__T3D_HARDWARE_BUFFER_MANAGER_H__*/
