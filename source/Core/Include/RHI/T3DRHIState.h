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


#ifndef __T3D_RHI_STATE_H__
#define __T3D_RHI_STATE_H__


#include "RHI/T3DRHIResource.h"


namespace Tiny3D
{
    template <typename T>
    uint8_t *getCRCData(const T &state, uint32_t &dataSize)
    {
        dataSize = sizeof(T);
        return (uint8_t*)(&state);
    }

    template <typename T>
    uint32_t calcCRC(const T &state)
    {
        uint32_t dataSize = 0;
        uint8_t *data = getCRCData(state, dataSize);
        return CRC::crc32(data, dataSize);
    }
    
    /**
     * \brief 渲染硬件层的状态
     */
    class T3D_ENGINE_API RHIState : public RHIResource
    {
    public:
        uint32_t hash() const
        {
            return mHash;
        }
        
    protected:
        uint32_t    mHash = 0;
    };
}


#endif  /*__T3D_RHI_STATE_H__*/
