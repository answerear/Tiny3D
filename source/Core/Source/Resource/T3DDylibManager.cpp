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
        return smart_pointer_cast<Dylib>(load(archive, name));
    }

    //--------------------------------------------------------------------------

    TResult DylibManager::unloadDylib(DylibPtr dylib)
    {
        return unload(dylib);
    }

    //--------------------------------------------------------------------------

    ResourcePtr DylibManager::newResource(const String &name, int32_t argc, va_list args)
    {
        return Dylib::create(name);
    }

    //--------------------------------------------------------------------------

    ResourcePtr DylibManager::loadResource(Archive *archive, const String &name)
    {
        MemoryDataStream stream;
        return loadResource(name, stream);
    }

    //--------------------------------------------------------------------------

    ResourcePtr DylibManager::loadResource(const String &name, DataStream &stream)
    {
        return Dylib::create(name);
    }

    //--------------------------------------------------------------------------

    TResult DylibManager::saveResource(DataStream &stream, Resource *res)
    {
        T3D_ASSERT(false, "Dylib resource could not save resource !");
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}
