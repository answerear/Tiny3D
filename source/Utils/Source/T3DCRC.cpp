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
