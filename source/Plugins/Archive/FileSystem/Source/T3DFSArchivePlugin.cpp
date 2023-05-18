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


#include "T3DFSArchivePlugin.h"

#include "T3DFSArchive.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    const String &FileSystemArchivePlugin::getName() const
    {
        return mName;
    }

    //--------------------------------------------------------------------------

    TResult FileSystemArchivePlugin::install()
    {
        TResult ret = T3D_OK;

        ret = T3D_ARCHIVE_MGR.addArchiveCreator(
            FileSystemArchive::ARCHIVE_TYPE,
            [](const String &name, Archive::AccessMode mode)
            {
                FileSystemArchivePtr archive = FileSystemArchive::create(name);
                return archive;
            });

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FileSystemArchivePlugin::startup()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult FileSystemArchivePlugin::shutdown()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult FileSystemArchivePlugin::uninstall()
    {
        TResult ret = T3D_OK;
        ret = T3D_ARCHIVE_MGR.removeArchiveCreator(FileSystemArchive::ARCHIVE_TYPE);
        return ret;
    }
}

