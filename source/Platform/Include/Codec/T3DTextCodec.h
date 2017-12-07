/***************************************************************************************************
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
 **************************************************************************************************/

#ifndef __T3D_TEXT_CODEC_H__
#define __T3D_TEXT_CODEC_H__


#include "T3DSingleton.h"
#include "T3DPlatformPrerequisites.h"


namespace Tiny3D
{
    /**
     * @brief �ı�����ת����
     */
    class T3D_PLATFORM_API TextCodec : public Singleton<TextCodec>
    {
        T3D_DISABLE_COPY(TextCodec);

    public:
        TextCodec(const String &strCodecFile);
        virtual ~TextCodec();

        bool UTF8ToUTF16(const UTF8String &src, UTF16String &dst);

        bool UTF16ToUTF8(const UTF16String &src, UTF8String &dst);
    };

    #define T3D_TEXT_CODEC      TextCodec::getInstance()
}


#endif  /*__T3D_TEXT_CODEC_H__*/
