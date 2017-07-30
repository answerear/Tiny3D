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

#ifndef __T3D_VERTEX_DATA_H__
#define __T3D_VERTEX_DATA_H__


#include "Misc/T3DObject.h"
#include "Render/T3DHardwareVertexBuffer.h"


namespace Tiny3D
{
    class T3D_ENGINE_API VertexData : public Object
    {
    public:
        static VertexDataPtr create(const VertexDeclarationPtr &decl);

        virtual ~VertexData();

        VertexDataPtr clone(bool copyData = true) const;

        bool addVertexBuffer(const HardwareVertexBufferPtr &buffer);
        bool insertVertexBuffer(const HardwareVertexBufferPtr &buffer, size_t index);
        bool removeVertexBuffer(const HardwareVertexBufferPtr &buffer);
        bool removeVertexBuffer(size_t index);

        const VertexDeclarationPtr &getDeclaration() const
        {
            return mDeclaration;
        }

        size_t getVertexBufferCount() const
        {
            return mBuffers.size();
        }

        const HardwareVertexBufferPtr &getVertexBuffer(size_t index) const
        {
            T3D_ASSERT(index <= mBuffers.size());
            return mBuffers[index];
        }

        HardwareVertexBufferPtr &getVertexBuffer(size_t index)
        {
            T3D_ASSERT(index <= mBuffers.size());
            return mBuffers[index];
        }

    protected:
        VertexData(const VertexDeclarationPtr &decl);

    private:
        VertexData(const VertexData &);
        VertexData &operator =(const VertexData &);

    protected:
        typedef std::vector<HardwareVertexBufferPtr>    VertexBuffers;
        typedef VertexBuffers::iterator                 VertexBuffersItr;
        typedef VertexBuffers::const_iterator           VertexBuffersConstItr;

        VertexDeclarationPtr    mDeclaration;
        VertexBuffers           mBuffers;
    };
}


#endif  /*__T3D_VERTEX_DATA_H__*/
