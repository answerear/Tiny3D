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

#ifndef __T3D_MD5_H__
#define __T3D_MD5_H__


#include "T3DUtilsPrerequisites.h"
#include <fstream>


namespace Tiny3D
{
    class T3D_UTILS_API MD5
    {
    public:
        MD5();
        MD5(const void *input, size_t length);
        MD5(const String &str);
        MD5(std::ifstream &in);
        void update(const void *input, size_t length);
        void update(const String &str);
        void update(std::ifstream &in);
        const uint8_t *digest();
        String toString();
        void reset();
    private:
        void update(const uint8_t *input, size_t length);
        void final();
        void transform(const uint8_t block[64]);
        void encode(const uint32_t *input, uint8_t *output, size_t length);
        void decode(const uint8_t *input, uint32_t *output, size_t length);
        String bytesToHexString(const uint8_t *input, size_t length);

        /* class uncopyable */
        MD5(const MD5&);
        MD5& operator=(const MD5&);
    private:
        uint32_t _state[4];	/* state (ABCD) */
        uint32_t _count[2];	/* number of bits, modulo 2^64 (low-order word first) */
        uint8_t _buffer[64];	/* input buffer */
        uint8_t _digest[16];	/* message digest */
        bool _finished;		/* calculate finished ? */

        static const uint8_t PADDING[64];	/* padding for calculate */
        static const char HEX[16];
        static const size_t BUFFER_SIZE = 1024;
    };
}


#endif  /*__T3D_UTILS_PREREQUISITES_H__*/
