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
    T3D_DECLARE_SMART_PTR(Material);
    T3D_DECLARE_SMART_PTR(MaterialManager);
    T3D_DECLARE_SMART_PTR(Texture);
    T3D_DECLARE_SMART_PTR(TextureManager);
    T3D_DECLARE_SMART_PTR(Shader);
    T3D_DECLARE_SMART_PTR(ShaderManager);
    T3D_DECLARE_SMART_PTR(GPUProgram);
    T3D_DECLARE_SMART_PTR(GPUProgramManager);
    T3D_DECLARE_SMART_PTR(GPUConstBuffer);
    T3D_DECLARE_SMART_PTR(GPUConstBufferManager);

    T3D_DECLARE_SMART_PTR(ScriptParser);
    T3D_DECLARE_SMART_PTR(ScriptParserMaterial);
    T3D_DECLARE_SMART_PTR(ScriptParserTechnique);
    T3D_DECLARE_SMART_PTR(ScriptParserPass);
    T3D_DECLARE_SMART_PTR(ScriptParserTextureUnit);
    T3D_DECLARE_SMART_PTR(ScriptParserSampler);
    T3D_DECLARE_SMART_PTR(ScriptParserGPUProgram);
    T3D_DECLARE_SMART_PTR(ScriptParserGPUProgramRef);
    T3D_DECLARE_SMART_PTR(ScriptParserShader);
    T3D_DECLARE_SMART_PTR(ScriptParserGPUCBuffer);
    T3D_DECLARE_SMART_PTR(ScriptParserGPUCBufferRef);

    T3D_DECLARE_SMART_PTR(HardwareBuffer);
    T3D_DECLARE_SMART_PTR(HardwareIndexBuffer);
    T3D_DECLARE_SMART_PTR(HardwarePixelBuffer);
    T3D_DECLARE_SMART_PTR(HardwareVertexBuffer);
    T3D_DECLARE_SMART_PTR(HardwareConstantBuffer);
    T3D_DECLARE_SMART_PTR(VertexAttribute);
    T3D_DECLARE_SMART_PTR(VertexDeclaration);

    T3D_DECLARE_SMART_PTR(VertexArrayObject);

    T3D_DECLARE_SMART_PTR(Shader);
    T3D_DECLARE_SMART_PTR(GPUProgramRef);
    T3D_DECLARE_SMART_PTR(GPUConstBufferRef);

    T3D_DECLARE_SMART_PTR(Technique);
    T3D_DECLARE_SMART_PTR(Pass);
    T3D_DECLARE_SMART_PTR(TextureUnit);

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

    T3D_DECLARE_SMART_PTR(Bound);
    T3D_DECLARE_SMART_PTR(SphereBound);
    T3D_DECLARE_SMART_PTR(AabbBound);
    T3D_DECLARE_SMART_PTR(ObbBound);
    T3D_DECLARE_SMART_PTR(FrustumBound);

    T3D_DECLARE_SMART_PTR(SceneManagerBase);
    T3D_DECLARE_SMART_PTR(SceneManager);

    T3D_DECLARE_SMART_PTR(DefaultSceneMgr);

    T3D_DECLARE_SMART_PTR(SGNode);

    T3D_DECLARE_SMART_PTR(SGTransform3D);
    T3D_DECLARE_SMART_PTR(SGBone);

    T3D_DECLARE_SMART_PTR(SGCamera);
    T3D_DECLARE_SMART_PTR(SGLight);
    T3D_DECLARE_SMART_PTR(SGModel);

    T3D_DECLARE_SMART_PTR(SGRenderable);
    T3D_DECLARE_SMART_PTR(SGMesh);
    T3D_DECLARE_SMART_PTR(SGTerrain);
    T3D_DECLARE_SMART_PTR(SGBillboard);
    T3D_DECLARE_SMART_PTR(SGQuad);
    T3D_DECLARE_SMART_PTR(SGBox);
    T3D_DECLARE_SMART_PTR(SGAxis);
    T3D_DECLARE_SMART_PTR(SGSphere);

    T3D_DECLARE_SMART_PTR(SGTransform2D);
    T3D_DECLARE_SMART_PTR(SGSprite);
    T3D_DECLARE_SMART_PTR(SGText2D);

    typedef VariantMap                      Settings;

    typedef TMap<String, RendererPtr>       Renderers;
    typedef Renderers::iterator             RenderersItr;
    typedef Renderers::const_iterator       RenderersConstItr;
    typedef Renderers::value_type           RenderersValue;

    typedef TList<NodePtr>                  Children;
    typedef Children::iterator              ChildrenItr;
    typedef Children::const_iterator        ChildrenConstItr;
}


#endif  /*__T3D_TYPEDEF_H__*/
