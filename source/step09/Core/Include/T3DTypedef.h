/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Agent)
 * Copyright (C) 2015-2019  Answer Wong
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


#ifndef __T3D_TYPEDEF_H__
#define __T3D_TYPEDEF_H__

#include "Kernel/T3DObject.h"
#include "Memory/T3DSmartPtr.h"

namespace Tiny3D
{
    T3D_DECLARE_SMART_PTR(Object);

    T3D_DECLARE_SMART_PTR(Resource);
    T3D_DECLARE_SMART_PTR(ResourceManager);
    T3D_DECLARE_SMART_PTR(Dylib);
    T3D_DECLARE_SMART_PTR(DylibManager);
    T3D_DECLARE_SMART_PTR(Archive);
    T3D_DECLARE_SMART_PTR(ArchiveManager);

    T3D_DECLARE_SMART_PTR(HardwareBuffer);
    T3D_DECLARE_SMART_PTR(HardwareIndexBuffer);
    T3D_DECLARE_SMART_PTR(HardwarePixelBuffer);
    T3D_DECLARE_SMART_PTR(HardwareVertexBuffer);
    T3D_DECLARE_SMART_PTR(VertexAttribute);
    T3D_DECLARE_SMART_PTR(VertexDeclaration);
    T3D_DECLARE_SMART_PTR(VertexArray);
    T3D_DECLARE_SMART_PTR(HardwareBufferManagerBase);
    T3D_DECLARE_SMART_PTR(HardwareBufferManager);

    T3D_DECLARE_SMART_PTR(RenderTarget);
    T3D_DECLARE_SMART_PTR(RenderWindow);
    T3D_DECLARE_SMART_PTR(Viewport);

    T3D_DECLARE_SMART_PTR(Renderer);

    T3D_DECLARE_SMART_PTR(RenderGroup);
    T3D_DECLARE_SMART_PTR(RenderQueue);

    T3D_DECLARE_SMART_PTR(ImageCodecBase);
    T3D_DECLARE_SMART_PTR(ImageCodec);

    T3D_DECLARE_SMART_PTR(Node);


    typedef TArray<Variant>                 VariantArray;
    typedef VariantArray::iterator          VariantArrayItr;
    typedef VariantArray::const_iterator    VariantArrayConstItr;

    typedef TList<Variant>                  VariantList;
    typedef VariantList::iterator           VariantListItr;
    typedef VariantList::const_iterator     VariantListConstItr;

    typedef TMap<Variant, Variant>          VariantMap;
    typedef VariantMap::iterator            VariantMapItr;
    typedef VariantMap::const_iterator      VariantMapConstItr;

    typedef TPair<Variant, Variant>         VariantMapValue;

    typedef VariantMap                      Settings;

    typedef TMap<String, RendererPtr>       Renderers;
    typedef Renderers::iterator             RenderersItr;
    typedef Renderers::const_iterator       RenderersConstItr;
    typedef Renderers::value_type           RenderersValue;
}


#endif  /*__T3D_TYPEDEF_H__*/
