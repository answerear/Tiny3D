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

#ifndef __T3D_ARCHIVE_H__
#define __T3D_ARCHIVE_H__


#include "Resource/T3DResource.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Archive : public Resource
    {
    public:
        virtual ~Archive();

        virtual Type getType() const override;

        virtual String getArchiveType() const = 0;

        virtual String getLocation() const = 0;
        virtual bool exists(const String &name) const = 0;
        virtual bool read(const String &name, MemoryDataStream &stream) = 0;
        virtual bool write(const String &name, const MemoryDataStream &stream) = 0;

    protected:
        Archive(const String &name);
    };
}


#endif  /*__T3D_ARCHIVE_H__*/
