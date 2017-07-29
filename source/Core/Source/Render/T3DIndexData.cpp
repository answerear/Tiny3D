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

#include "Render/T3DIndexData.h"
#include "Render/T3DHardwareBufferManager.h"


namespace Tiny3D
{
    IndexDataPtr IndexData::create(HardwareIndexBuffer *buffer)
    {
        IndexData *data = new IndexData(buffer);
        IndexDataPtr ptr(data);
        data->release();
        return ptr;
    }

    IndexData::IndexData(HardwareIndexBuffer *buffer)
        : mIndexBuffer(buffer)
    {

    }

    IndexData::~IndexData()
    {
        mIndexBuffer = nullptr;
    }

    IndexDataPtr IndexData::clone(bool copyData /* = true */) const
    {
        IndexDataPtr ptr;

        if (copyData)
        {
            HardwareIndexBufferPtr indexBuffer = T3D_HARDWARE_BUFFER_MGR.createIndexBuffer(
                mIndexBuffer->getIndexType(), mIndexBuffer->getIndexCount(), 
                mIndexBuffer->getUsage(), mIndexBuffer->hasShadowBuffer());

            indexBuffer->copyData((HardwareBufferPtr)mIndexBuffer);
            
            IndexData *data = new IndexData(indexBuffer);
            ptr = data;
        }
        else
        {
            IndexData *data = new IndexData(mIndexBuffer);
            ptr = data;
        }
        
        return ptr;
    }
}
