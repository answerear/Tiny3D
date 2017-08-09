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

#ifndef __T3D_PREREQUISITES_H__
#define __T3D_PREREQUISITES_H__


#include "T3DConfig.h"
#include "T3DMacro.h"
#include <stdarg.h>


#if defined T3DCORE_EXPORT
    #define T3D_ENGINE_API        T3D_EXPORT_API
#else
    #define T3D_ENGINE_API        T3D_IMPORT_API
#endif


#include "T3DPlatform.h"
#include "T3DLog.h"

#include "T3DSphere.h"
#include "T3DAabb.h"
#include "T3DObb.h"
#include "T3DFrustum.h"


#include "T3DRect.h"
#include "T3DMath.h"
#include "T3DMatrix2.h"
#include "T3DMatrix3.h"
#include "T3DMatrix4.h"
#include "T3DQuaternion.h"
#include "T3DVector2.h"
#include "T3DVector3.h"
#include "T3DVector4.h"
#include "T3DPlane.h"



namespace Tiny3D
{
    /// Platform
    class TextCodec;
    class Console;
    class DeviceInfo;
    class DataStream;
    class Dir;
    class FileDataStream;
    class MemoryDataStream;
    class DateTime;
    class RunLoop;
    class RunLoopObserver;
    class Timer;
    class TimerObserver;
    class System;

    /// Log
    class Logger;

    /// Core
    class ApplicationListener;
    class RenderTargetListener;
    class FrameListener;
    class WindowEventListener;
    class TouchEventListener;
    class KeyboardEventListener;
    class JoystickEventListener;

    class WindowEventHandler;

    class Degree;
    class Radian;
    class Math;
    class Matrix2;
    class Matrix3;
    class Matrix4;
    class Quaternion;
    class Vector2;
    class Vector3;
    class Vector4;

    class Transform;
    
    class Entrance;
    class Plugin;

    class Color4;

    class Plane;
    class Sphere;
    class Frustum;
    class Obb;
    class Aabb;

    class Bound;
    class SphereBound;
    class AabbBound;
    class ObbBound;
    class FrustumBound;

    class Viewport;
    class RenderTarget;
    class RenderWindow;
    class Renderer;

    class HardwareBuffer;
    class HardwareVertexBuffer;
    class HardwareIndexBuffer;
    class HardwarePixelBuffer;

    class VertexElement;
    class VertexDeclaration;

    class HardwareBufferManagerBase;
    class HardwareBufferManager;

    class Resource;
    class ResourceManager;
    class Dylib;
    class DylibManager;
    class Material;
    class MaterialManager;
    class Texture;
    class TextureManager;
    class Archive;
    class ArchiveManager;
    class ArchiveCreator;
    class Model;
    class ModelManager;
    class Font;
    class FontManager;

    class FileSystemArchive;
    class FileSystemArchiveCreator;
    class ZipArchive;
    class ZipArchiveCreator;

    class Object;
    class Node;

    class SceneManager;
    class SGNode;
    class SGTransformNode;
    class SGTransform2D;
    class SGBone;
    class SGCamera;
    class SGModel;
    class SGRenderable;
    class SGLight;
    class SGGeometry;
    class SGMesh;
    class SGSkeleton;
    class SGSphere;
    class SGBox;
    class SGAxis;
    class SGQuad;
    class SGSprite;
    class SGText2D;

    class VertexData;
    class IndexData;

    class RenderGroup;
    class RenderQueue;

    class Variant;

    class Image;

    class InputSystem;
    class TouchDevice;
    class KeyDevice;
    class JoystickDevice;
}


#endif    /*__T3D_PREREQUISITES_H__*/
