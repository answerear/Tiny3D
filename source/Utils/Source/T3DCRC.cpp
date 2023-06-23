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

#include "T3DCRC.h"

namespace Tiny3D
{
    //--------------------------------------------------------------------------

    uint32_t CRC::msCRCTable[256] = {0};
    
    //--------------------------------------------------------------------------

    void CRC::initCRCTable()
    {
        #define POLYNOMIAL 0x04c11db7L
        int32_t i, j;  
        uint32_t crc_accum;
        
        for (i = 0; i < 256; i++)
        { 
            crc_accum = ((uint32_t)i << 24);
            
            for ( j = 0;  j < 8;  j++ )
            {
                if (crc_accum & 0x80000000L)
                {
                    crc_accum = ( crc_accum << 1 ) ^ POLYNOMIAL;
                }
                else
                {
                    crc_accum = ( crc_accum << 1 ); 
                }
            }
            
            msCRCTable[i] = crc_accum; 
        }
    }
    
    //--------------------------------------------------------------------------

    uint32_t CRC::crc32(const uint8_t *data, uint32_t dataSize)
    {
        uint32_t result;

        if (dataSize == 0)
            return 0;

        result  = *data++ << 24;
        if( dataSize > 1 )
        {
            result |= *data++ << 16;
            if( dataSize > 2 )
            {
                result |= *data++ << 8;
                if( dataSize > 3 ) result |= *data++;
            }
        }
        result = ~ result;

        for( unsigned int i=4; i< dataSize; i++ )
        {
            result = (result << 8 | *data++) ^ msCRCTable[result >> 24];
        }

        return ~result;
    }

    //--------------------------------------------------------------------------
}
