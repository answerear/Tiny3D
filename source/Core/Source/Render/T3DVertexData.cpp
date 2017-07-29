/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Aaron Wong
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

#include "Render/T3DVertexData.h"
#include "Render/T3DHardwareBufferManager.h"


namespace Tiny3D
{
    VertexDataPtr VertexData::create(const VertexDeclarationPtr &decl)
    {
        VertexData *data = new VertexData(decl);
        VertexDataPtr ptr(data);
        data->release();
        return ptr;
    }

    VertexData::VertexData(const VertexDeclarationPtr &decl)
        : mDeclaration(decl)
    {

    }

    VertexData::~VertexData()
    {

    }

    bool VertexData::addVertexBuffer(const HardwareVertexBufferPtr &buffer)
    {
        mBuffers.push_back(buffer);
        return true;
    }

    bool VertexData::insertVertexBuffer(const HardwareVertexBufferPtr &buffer, size_t index)
    {
        bool found = false;
        size_t i = 0;

        if (index <= mBuffers.size())
        {
            mBuffers.insert(mBuffers.begin()+index, buffer);
        }
        else
        {
            mBuffers.push_back(buffer);
        }

        return true;
    }

    bool VertexData::removeVertexBuffer(const HardwareVertexBufferPtr &buffer)
    {
        bool found = false;

        auto itr = mBuffers.begin();

        while (itr != mBuffers.end())
        {
            if (buffer == *itr)
            {
                found = true;
                mBuffers.erase(itr);
                break;
            }

            ++itr;
        }

        return found;
    }

    bool VertexData::removeVertexBuffer(size_t index)
    {
        bool found = false;

        if (index < mBuffers.size())
        {
            mBuffers.erase(mBuffers.begin()+index);
            found = true;
        }

        return found;
    }
}
