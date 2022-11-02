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


#include "T3DUUID.h"
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

        return uuid;
    }

    String UUID::toString() const
    {
        std::stringstream ss;

        for (uint32_t i = 0; i < kDefaultLength; i++)
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
        for (uint32_t i = 0; i < kDefaultLength; i++)
        {
            std::stringstream ss;
            //ss.put('0');
            //ss.put('x');
            ss.put(str[i<<1]);
            ss.put(str[(i<<1)+1]);
            uint32_t val;
            ss >> std::hex >> val;
            bytes[i] = val;
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
//     registration::class_<Tiny3D::UUID>("Tiny3D::UUID")
//         .constructor<>()(policy::ctor::as_object)
//         .method("toString", &Tiny3D::UUID::toString)
//         .method("fromString", &Tiny3D::UUID::fromString)
//         .property("value", &Tiny3D::UUID::getValue, &Tiny3D::UUID::setValue);
// }

