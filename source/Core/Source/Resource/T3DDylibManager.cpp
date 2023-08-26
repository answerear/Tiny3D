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


#include "Resource/T3DDylibManager.h"

#include <stdbool.h>
#include <stdbool.h>

#include "Resource/T3DDylib.h"
#include "Kernel/T3DArchive.h"

namespace Tiny3D
{
    //--------------------------------------------------------------------------

    DylibManagerPtr DylibManager::create()
    {
        DylibManagerPtr mgr = new DylibManager();
        return mgr;
    }

    //--------------------------------------------------------------------------

    DylibPtr DylibManager::loadDylib(const String &name)
    {
        Archive *archive = nullptr;
        return smart_pointer_cast<Dylib>(load(archive, archive, name, 0));
    }

    //--------------------------------------------------------------------------

    TResult DylibManager::unloadDylib(DylibPtr dylib)
    {
        return unload(dylib);
    }

    //--------------------------------------------------------------------------

    TResult DylibManager::saveMeta(DataStream &stream, Meta *meta)
    {
        T3D_ASSERT(false, "Dylib resource could not save meta !");
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult DylibManager::saveResource(DataStream &stream, Resource *res)
    {
        T3D_ASSERT(false, "Dylib resource could not save resource !");
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}
