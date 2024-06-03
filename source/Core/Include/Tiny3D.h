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
#include <Kernel/T3DGameObject.h>
#include <Kernel/T3DTransform.h>

// Resource
#include <Resource/T3DResource.h>
#include <Resource/T3DResourceManager.h>
#include <Resource/T3DDylib.h>
#include <Resource/T3DMaterial.h>
#include <Resource/T3DMaterialManager.h>
#include <Resource/T3DShader.h>
#include <Resource/T3DShaderManager.h>
#include <Resource/T3DTexture.h>
#include <Resource/T3DTextureManager.h>
#include <Resource/T3DPrefab.h>
#include <Resource/T3DPrefabManager.h>
#include <Resource/T3DImage.h>
#include <Resource/T3DImageManager.h>
#include <Resource/T3DScene.h>
#include <Resource/T3DSceneManager.h>
#include <Resource/T3DMesh.h>
#include <Resource/T3DMeshManager.h>
#include <Resource/T3DSubMesh.h>

// Material
#include <Material/T3DPass.h>
#include <Material/T3DTechnique.h>
#include <Material/T3DShaderConstantParam.h>
#include <Material/T3DShaderSamplerParam.h>
#include <Material/T3DShaderKeyword.h>
#include <Material/T3DShaderVariant.h>
#include <Material/T3DShaderBinding.h>

// Component
#include <Component/T3DComponent.h>
#include <Component/T3DCamera.h>
#include <Component/T3DTransformNode.h>
#include <Component/T3DTransform3D.h>
#include <Component/T3DRenderable.h>
#include <Component/T3DGeometry.h>

// Bound
#include <Bound/T3DBound.h>
#include <Bound/T3DSphereBound.h>
#include <Bound/T3DAabbBound.h>
#include <Bound/T3DObbBound.h>
#include <Bound/T3DFrustumBound.h>

// Serialize
#include <Serializer/T3DSerializer.h>
#include <Serializer/T3DBinSerializer.h>
#include <Serializer/T3DJsonSerializer.h>
#include <Serializer/T3DSerializerManager.h>

// ImageCodec
#include <ImageCodec/T3DImageCodec.h>
#include <ImageCodec/T3DImageCodecBase.h>

// Render
#include <Render/T3DRenderConstant.h>
#include <Render/T3DVertexAttribute.h>
#include <Render/T3DRenderResource.h>
#include <Render/T3DBlendState.h>
#include <Render/T3DDepthStencilState.h>
#include <Render/T3DRasterizerState.h>
#include <Render/T3DSamplerState.h>
#include <Render/T3DRenderState.h>
#include <Render/T3DVertexDeclaration.h>
#include <Render/T3DVertexBuffer.h>
#include <Render/T3DIndexBuffer.h>
#include <Render/T3DPixelBuffer.h>
#include <Render/T3DConstantBuffer.h>
#include <Render/T3DRenderResourceManager.h>
#include <Render/T3DRenderTarget.h>
#include <Render/T3DRenderTexture.h>
#include <Render/T3DRenderWindow.h>
#include <Render/T3DViewport.h>

// RHI
#include <RHI/T3DRHIRenderer.h>
#include <RHI/T3DRHIContext.h>
#include <RHI/T3DRHIResource.h>
#include <RHI/T3DRHIBlendState.h>
#include <RHI/T3DRHIDepthStencilState.h>
#include <RHI/T3DRHIRasterizerState.h>
#include <RHI/T3DRHISamplerState.h>
#include <RHI/T3DRHIVertexDeclaration.h>
#include <RHI/T3DRHIVertexBuffer.h>
#include <RHI/T3DRHIIndexBuffer.h>
#include <RHI/T3DRHIPixelBuffer.h>
#include <RHI/T3DRHIConstantBuffer.h>
#include <RHI/T3DRHIRenderTarget.h>
#include <RHI/T3DRHIRenderWindow.h>
#include <RHI/T3DRHIRenderTexture.h>
#include <RHI/T3DRHIShader.h>
#include <RHI/T3DRHIThread.h>
#include <RHI/T3DRHICommand.h>

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
