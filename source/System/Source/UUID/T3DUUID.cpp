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


#include "UUID/T3DUUID.h"
#include <sstream>
#include <random>
#include <string>


namespace Tiny3D
{
    static uint32_t random_char() 
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);
        return dis(gen);
    }

    const UUID UUID::INVALID;

    //String UUID::generate(uint32_t len)
    //{
    //    std::stringstream ss;

    //    for (uint32_t i = 0; i < len; i++)
    //    {
    //        const uint32_t rc = random_char();
    //        std::stringstream hexstream;
    //        hexstream << std::hex << rc;
    //        auto hex = hexstream.str();
    //        ss << (hex.length() < 2 ? '0' + hex : hex);
    //    }

    //    return ss.str();
    //}

    //String UUID::generate()
    //{
    //    return generate(kDefaultLength);
    //}

    UUID UUID::generate()
    {
        UUID uuid;

        for (uint32_t i = 0; i < kDefaultLength; i++)
        {
            uint32_t rc = random_char();
            uuid.bytes[i] = (uint8_t)rc;
        }

        // uuid.TestObj = TestObject::create();
        return uuid;
    }

    String UUID::toString() const
    {
        std::stringstream ss;

        for (int32_t i = kDefaultLength - 1; i >= 0; i--)
        {
            std::stringstream hexstream;
            hexstream << std::hex << (uint32_t)bytes[i];
            auto hex = hexstream.str();
            ss << (hex.length() < 2 ? '0' + hex : hex);
        }

        return ss.str();
    }

    void UUID::fromString(const String& str)
    {
        for (int32_t i = 0; i < kDefaultLength; i++)
        {
            std::stringstream ss;
            //ss.put('0');
            //ss.put('x');
            ss.put(str[i<<1]);
            ss.put(str[(i<<1)+1]);
            uint32_t val;
            ss >> std::hex >> val;
            bytes[kDefaultLength-i-1] = val;
            ss.str().clear();
        }
    }
}



//------------------------------------------------------------------------------
//                                  RTTR
//------------------------------------------------------------------------------

// RTTR_REGISTRATION
// {
//     using namespace rttr;
//
//     registration::class_<std::vector<int>>("std::vector<int>")
//         .constructor<>()
//         (
//             policy::ctor::as_object
//         );
// }

