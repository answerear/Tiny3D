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


#include "Resource/T3DZipArchieve.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    const char * const ZipArchive::ARCHIVE_TYPE = "Zip";

    //--------------------------------------------------------------------------

    String ZipArchiveCreator::getType() const
    {
        return ZipArchive::ARCHIVE_TYPE;
    }

    ArchivePtr ZipArchiveCreator::createObject(int32_t argc, ...) const
    {
        va_list params;
        va_start(params, argc);
        String name = va_arg(params, char *);
        va_end(params);
        return ZipArchive::create(name);
    }

    //--------------------------------------------------------------------------

    ZipArchivePtr ZipArchive::create(const String &name)
    {
        ZipArchivePtr archive = new ZipArchive(name);
        archive->release();
        return archive;
    }

    //--------------------------------------------------------------------------

    ZipArchive::ZipArchive(const String &name)
        : Archive(name)
    {

    }

    ZipArchive::~ZipArchive()
    {

    }

    //--------------------------------------------------------------------------

    TResult ZipArchive::load()
    {
        return T3D_ERR_OK;
    }

    TResult ZipArchive::unload()
    {
        return T3D_ERR_OK;
    }

    ResourcePtr ZipArchive::clone() const
    {
        ArchivePtr archive = create(mName);
        return archive;
    }

    //--------------------------------------------------------------------------

    String ZipArchive::getArchiveType() const
    {
        return ARCHIVE_TYPE;
    }

    String ZipArchive::getLocation() const
    {
        return mName;
    }

    bool ZipArchive::exists(const String &name) const
    {
        return false;
    }

    TResult ZipArchive::read(const String &name, MemoryDataStream &stream)
    {
        return T3D_ERR_OK;
    }

    TResult ZipArchive::write(const String &name, const MemoryDataStream &stream)
    {
        return T3D_ERR_OK;
    }
}

