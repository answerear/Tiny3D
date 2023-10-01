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


#ifndef __TINY3D_H__
#define __TINY3D_H__

// Global
#include <T3DPrerequisites.h>
#include <T3DErrorDef.h>
#include <T3DTypedef.h>
#include <T3DConfig.h>

// Kernel
#include <Kernel/T3DAgent.h>
#include <Kernel/T3DArchive.h>
#include <Kernel/T3DArchiveManager.h>
#include <Kernel/T3DPlugin.h>
#include <Kernel/T3DSettings.h>

// Resource
#include <Resource/T3DResource.h>
#include <Resource/T3DResourceManager.h>
#include <Resource/T3DMeta.h>
#include <Resource/T3DDylib.h>
#include <Resource/T3DMaterial.h>
#include <Resource/T3DMaterialManager.h>
#include <Resource/T3DShader.h>
#include <Resource/T3DShaderManager.h>
#include <Resource/T3DTexture.h>
#include <Resource/T3DTextureManager.h>
#include <Resource/T3DPrefab.h>
#include <Resource/T3DPrefabManager.h>

// Material
#include <Material/T3DPass.h>
#include <Material/T3DTechnique.h>
#include <Material/T3DShaderConstantParam.h>
#include <Material/T3DShaderSamplerParam.h>
#include <Material/T3DShaderKeyword.h>
#include <Material/T3DShaderVariant.h>

// Serialize
#include <Serializer/T3DSerializer.h>
#include <Serializer/T3DSerializerManager.h>

// ImageCodec
#include <ImageCodec/T3DImageCodec.h>
#include <ImageCodec/T3DImageCodecBase.h>
#include <ImageCodec/T3DImage.h>

// Render
#include <Render/T3DRenderResource.h>
#include <Render/T3DBlendState.h>
#include <Render/T3DDepthStencilState.h>
#include <Render/T3DRasterizerState.h>
#include <Render/T3DSamplerState.h>
#include <Render/T3DRenderState.h>
#include <Render/T3DTextureState.h>
#include <Render/T3DVertexBuffer.h>
#include <Render/T3DIndexBuffer.h>
#include <Render/T3DPixelBuffer.h>
#include <Render/T3DDataBuffer.h>
#include <Render/T3DRenderResourceManager.h>

// RHI
#include <Render/T3DRenderConstant.h>
#include <RHI/T3DRHIRenderer.h>
#include <RHI/T3DRHIContext.h>
#include <RHI/T3DRHIRenderTarget.h>
#include <RHI/T3DRHIRenderWindow.h>
#include <RHI/T3DRHIViewport.h>
#include <RHI/T3DRHIBlendState.h>
#include <RHI/T3DRHIDepthStencilState.h>
#include <RHI/T3DRHIRasterizerState.h>
#include <RHI/T3DRHISamplerState.h>
#include <RHI/T3DRHIVertexAttribute.h>
#include <RHI/T3DRHIVertexDeclaration.h>
#include <RHI/T3DRHIBuffer.h>
#include <RHI/T3DRHIDataBuffer.h>
#include <RHI/T3DRHIVertexBuffer.h>
#include <RHI/T3DRHIIndexBuffer.h>
#include <RHI/T3DRHIPixelBuffer.h>
#include <RHI/T3DRHIBufferManager.h>
#include <RHI/T3DRHIBufferManagerBase.h>

// #include <Render/T3DHardwareBufferManagerBase.h>
// #include <Render/T3DHardwareBufferManager.h>
// #include <Render/T3DHardwareBuffer.h>
// #include <Render/T3DHardwareVertexBuffer.h>
// #include <Render/T3DHardwareIndexBuffer.h>
// #include <Render/T3DHardwarePixelBuffer.h>
// #include <Render/T3DHardwareConstantBuffer.h>
// #include <Render/T3DVertexArrayObject.h>

// // Bound
// #include <Bound/T3DBound.h>
// #include <Bound/T3DSphereBound.h>
// #include <Bound/T3DAabbBound.h>
// #include <Bound/T3DObbBound.h>
// #include <Bound/T3DFrustumBound.h>
//
// // Scene Graph
// #include <Scene/T3DSceneNode.h>
// #include <Scene/T3DSceneManager.h>
//
// // Component
// #include <Component/T3DComponent.h>
// #include <Component/T3DComponentCreator.h>
// #include <Component/T3DAxis.h>
// #include <Component/T3DBillboard.h>
// #include <Component/T3DCube.h>
// #include <Component/T3DCamera.h>
// #include <Component/T3DLight.h>
// #include <Component/T3DMesh.h>
// #include <Component/T3DQuad.h>
// #include <Component/T3DRenderable.h>
// #include <Component/T3DGlobe.h>
// #include <Component/T3DTransform3D.h>


#endif  /*__TINY3D_H__*/
