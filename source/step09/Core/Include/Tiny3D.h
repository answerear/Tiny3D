/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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


#ifndef __TINY3D_H__
#define __TINY3D_H__

// Global
#include <T3DPrerequisites.h>
#include <T3DErrorDef.h>
#include <T3DTypedef.h>

// Kernel
#include <Kernel/T3DAgent.h>
#include <Kernel/T3DConfigFile.h>
#include <Kernel/T3DCreator.h>
#include <Kernel/T3DObject.h>
#include <Kernel/T3DPlugin.h>
#include <Kernel/T3DPixelFormat.h>
#include <Kernel/T3DNode.h>

// Memory
#include <Memory/T3DSmartPtr.h>

// Resource
#include <Resource/T3DArchive.h>
#include <Resource/T3DArchiveCreator.h>
#include <Resource/T3DArchiveManager.h>
#include <Resource/T3DDylib.h>
#include <Resource/T3DDylibManager.h>
#include <Resource/T3DResource.h>
#include <Resource/T3DResourceManager.h>

// DataStruct
#include <DataStruct/T3DVariant.h>
#include <DataStruct/T3DString.h>

// ImageCodec
#include <ImageCodec/T3DImageCodec.h>
#include <ImageCodec/T3DImageCodecBase.h>
#include <ImageCodec/T3DImage.h>

// Render
#include <Render/T3DRenderer.h>
#include <Render/T3DRenderQueue.h>
#include <Render/T3DRenderTarget.h>
#include <Render/T3DRenderWindow.h>
#include <Render/T3DViewport.h>
#include <Render/T3DHardwareBufferManagerBase.h>
#include <Render/T3DHardwareBufferManager.h>
#include <Render/T3DHardwareBuffer.h>
#include <Render/T3DHardwareVertexBuffer.h>
#include <Render/T3DHardwareIndexBuffer.h>
#include <Render/T3DHardwarePixelBuffer.h>
#include <Render/T3DVertexArrayObject.h>

// Bound
#include <Bound/T3DBound.h>
#include <Bound/T3DSphereBound.h>
#include <Bound/T3DAabbBound.h>
#include <Bound/T3DObbBound.h>
#include <Bound/T3DFrustumBound.h>

// Scene Graph
#include <SceneGraph/T3DSGNode.h>
#include <SceneGraph/T3DSceneManager.h>
#include <SceneGraph/T3DSGAxis.h>
#include <SceneGraph/T3DSGBillboard.h>
#include <SceneGraph/T3DSGBox.h>
#include <SceneGraph/T3DSGCamera.h>
#include <SceneGraph/T3DSGLight.h>
#include <SceneGraph/T3DSGMesh.h>
#include <SceneGraph/T3DSGModel.h>
#include <SceneGraph/T3DSGQuad.h>
#include <SceneGraph/T3DSGRenderable.h>
#include <SceneGraph/T3DSGSphere.h>
#include <SceneGraph/T3DSGTransform3D.h>


#endif  /*__TINY3D_H__*/
