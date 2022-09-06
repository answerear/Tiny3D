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


#include "T3DBundleArchivePlugin.h"
#include "T3DBundleArchiveCreator.h"
#include "T3DBundleArchiveError.h"

namespace Tiny3D
{
    //--------------------------------------------------------------------------

    BundleArchivePlugin::BundleArchivePlugin()
        : mName("BundleArchive")
        , mZipCreator(nullptr)
    {

    }

    //--------------------------------------------------------------------------

    BundleArchivePlugin::~BundleArchivePlugin()
    {

    }

    //--------------------------------------------------------------------------

    const String &BundleArchivePlugin::getName() const
    {
        return mName;
    }

    //--------------------------------------------------------------------------

    TResult BundleArchivePlugin::install()
    {
        TResult ret = T3D_OK;

        mZipCreator = new BundleArchiveCreator();
        Agent::getInstance().addArchiveCreator(mZipCreator);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult BundleArchivePlugin::startup()
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult BundleArchivePlugin::shutdown()
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult BundleArchivePlugin::uninstall()
    {
        TResult ret = T3D_OK;
        Agent::getInstance().removeArchiveCreator(mZipCreator);
        delete mZipCreator;
        mZipCreator = nullptr;

        return ret;
    }
}

