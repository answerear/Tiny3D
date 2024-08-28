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


#include "T3DMetaFSArchivePlugin.h"
#include "T3DMetaFSArchive.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    MetaFSArchivePlugin::MetaFSArchivePlugin()
        : mName("MetaFSArchive")
    {

    }

    //--------------------------------------------------------------------------

    MetaFSArchivePlugin::~MetaFSArchivePlugin()
    {

    }

    //--------------------------------------------------------------------------

    const String &MetaFSArchivePlugin::getName() const
    {
        return mName;
    }

    //--------------------------------------------------------------------------

    TResult MetaFSArchivePlugin::install()
    {
        return T3D_ARCHIVE_MGR.addArchiveCreator(
            MetaFSArchive::ARCHIVE_TYPE,
            [](const String &name, Archive::AccessMode mode)
            {
                MetaFSArchivePtr archive = MetaFSArchive::create(name, mode);
                return archive;
            });
    }

    //--------------------------------------------------------------------------

    TResult MetaFSArchivePlugin::startup()
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MetaFSArchivePlugin::shutdown()
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MetaFSArchivePlugin::uninstall()
    {
        return T3D_ARCHIVE_MGR.removeArchiveCreator(MetaFSArchive::ARCHIVE_TYPE);
    }
}

