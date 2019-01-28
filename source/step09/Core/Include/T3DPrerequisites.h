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

#ifndef __T3D_PREREQUISITES_H__
#define __T3D_PREREQUISITES_H__

#include "T3DMacro.h"

#if defined T3DCORE_EXPORT
    #define T3D_ENGINE_API        T3D_EXPORT_API
#else
    #define T3D_ENGINE_API        T3D_IMPORT_API
#endif


#include <T3DPlatform.h>
#include <T3DMathLib.h>
#include <T3DLog.h>
#include <T3DFramework.h>

#if defined (T3D_OS_IOS) || defined (T3D_OS_OSX)
#include <SDL_main.h>
#else
#endif

namespace Tiny3D
{
    #define LOG_TAG_CONFIG_FILE         "ConfigFile"
    #define LOG_TAG_ENGINE              "Agent"
    #define LOG_TAG_IMAGE               "Image"
    #define LOG_TAG_RENDER              "Render"
    #define LOG_TAG_PLUGIN              "Plugin"
    #define LOG_TAG_IMAGE_CODEC         "ImageCodec"

    class Object;
    class ObjectTracer;

    class Agent;
    class Plugin;

    class Variant;

    class Resource;
    class ResourceManager;
    class Dylib;
    class DylibManager;
    class Archive;
    class ArchiveCreator;
    class ArchiveManager;

    class HardwareBuffer;
    class HardwareIndexBuffer;
    class HardwarePixelBuffer;
    class HardwareVertexBuffer;
    class VertexAttribute;
    class VertexDeclaration;
    class VertexArray;
    class HardwareBufferManagerBase;
    class HardwareBufferManager;
    class RenderTarget;
    class RenderWindow;
    class Viewport;
    class Renderer;
    class RenderGroup;
    class RenderQueue;

    class ImageCodecBase;
    class ImageCodec;
}


#endif    /*__T3D_PREREQUISITES_H__*/
