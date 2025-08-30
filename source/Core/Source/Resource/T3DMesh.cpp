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

#include "Resource/T3DMesh.h"
#include "T3DErrorDef.h"
#include "Material/T3DPass.h"
#include "Material/T3DPassInstance.h"
#include "Material/T3DShaderVariantInstance.h"
#include "Material/T3DTechniqueInstance.h"
#include "Resource/T3DSubMesh.h"
#include "Render/T3DRenderResourceManager.h"
#include "Render/T3DVertexBuffer.h"
#include "Render/T3DVertexDeclaration.h"
#include "Resource/T3DMaterial.h"
#include "Resource/T3DMaterialManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    MeshPtr Mesh::create(const String &name)
    {
        return new Mesh(name);
    }

    //--------------------------------------------------------------------------

    MeshPtr Mesh::create(const String &name, VertexAttributes &&attributes, Vertices &&vertices, VertexStrides &&strides, VertexOffsets &&offsets, SubMeshes &&submeshes)
    {
        return new Mesh(name, std::move(attributes), std::move(vertices), std::move(strides), std::move(offsets), std::move(submeshes));
    }
    
    //--------------------------------------------------------------------------

    Mesh::~Mesh()
    {
        T3D_ASSERT(mVertices.size() == mVBuffers.size());
        for (size_t i = 0; i < mVertices.size(); i++)
        {
            auto &vertices = mVertices[i];
            auto &vbo = mVBuffers[i];
            if (vertices.Data != vbo->getBuffer().Data)
            {
                vertices.release();
            }
        }
    }
    
    //--------------------------------------------------------------------------

    Mesh::Mesh(const String &name)
        : Resource(name)
    {
        
    }

    //--------------------------------------------------------------------------

    Mesh::Mesh(const String &name, VertexAttributes &&attributes, Vertices &&vertices, VertexStrides &&strides, VertexOffsets &&offsets, SubMeshes &&submeshes)
        : Resource(name)
        , mVertexAttributes(std::move(attributes))
        , mVertices(std::move(vertices))
        , mVertexStrides(std::move(strides))
        , mVertexOffsets(std::move(offsets))
        , mSubMeshes(std::move(submeshes))
    {
        mCreatedFromData = true;
    }

    //--------------------------------------------------------------------------

    Resource::Type Mesh::getType() const
    {
        return Type::kMesh;
    }

    //--------------------------------------------------------------------------
    
    ResourcePtr Mesh::clone() const
    {
        MeshPtr mesh = create(getName());
        mesh->cloneProperties(this);
        return mesh;
    }
    
    //--------------------------------------------------------------------------

    void Mesh::cloneProperties(const Resource *const src)
    {
        const Mesh *mesh = static_cast<const Mesh*>(src);
    }

    //--------------------------------------------------------------------------

    TResult Mesh::onCreate()
    {
        TResult ret = Resource::onCreate();
        
        if (mCreatedFromData)
        {
            ret = generateRenderResource(nullptr);
        }
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Mesh::onLoad(Archive *archive)
    {
        TResult ret;

        do
        {
            ret = Resource::onLoad(archive);
            if (T3D_FAILED(ret))
            {
                break;
            }
            
            ret = generateRenderResource(archive);
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);
        return ret;
    }

    //--------------------------------------------------------------------------

    SubMesh *Mesh::getSubMesh(const String &name) const
    {
        SubMesh *submesh = nullptr;
        auto itr = mSubMeshes.find(name);
        if (itr != mSubMeshes.end())
        {
            submesh = itr->second;
        }
        return submesh;
    }

    //--------------------------------------------------------------------------

    TResult Mesh::generateRenderResource(Archive *archive)
    {
        TResult ret = T3D_OK;

        do
        {
            ShaderVariantPtr vshader = nullptr;
            
            // 生成子网格的渲染资源
            for (const auto &submesh : mSubMeshes)
            {
                ret = submesh.second->generateRenderResource(archive);
                if (T3D_FAILED(ret))
                {
                    break;
                }
                if (vshader == nullptr)
                {
                    Material *material = nullptr;
                    
                    if (archive != nullptr)
                    {
                        material = T3D_MATERIAL_MGR.loadMaterial(archive, submesh.second->getMaterialUUID());
                    }
                    else
                    {
                        material = static_cast<Material *>(T3D_MATERIAL_MGR.getResource(submesh.second->getMaterialUUID()));
                    }
                    
                    TechniqueInstance *tech = material->getCurrentTechnique();
                    PassInstance *pass = nullptr;
                    
                    // 先找前向渲染的 pass 里面的 shader
                    auto itr = tech->getPassInstances().find(ShaderLab::kBuiltinLightModeForwardBase);
                    if (itr == tech->getPassInstances().end())
                    {
                        // 没有找到，说明不是内置的材质，只能找第0号的 pass
                        itr = tech->getPassInstances().find(0);
                        if (itr == tech->getPassInstances().end())
                        {
                            // 还没找到，只能找 pass instances 中第一个 pass
                            itr = tech->getPassInstances().begin();
                        }
                    }
                    if (itr == tech->getPassInstances().end())
                    {
                        // 还是没有找到 pass，只能跳出了
                        break;
                    }

                    pass = itr->second;
                    // vshader = pass->getCurrentVertexShader()->getShaderVariant();
                    vshader = pass->getPass()->getVertexShaders().empty() ? nullptr : pass->getPass()->getVertexShaders().begin()->second;
                }
            }

            if (vshader == nullptr)
            {
                ret = T3D_ERR_RES_INVALID_SHADER;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Invalid vertex shader to create vertex declaration !");
                break;
            }
            
            // 创建顶点声明
            mVertexDecl = T3D_RENDER_BUFFER_MGR.addVertexDeclaration(mVertexAttributes, vshader);

            // 创建顶点缓冲对象
            mVBuffers.resize(mVertices.size());
            
            for (uint32_t i = 0; i < mVertices.size(); ++i)
            {
                const auto &buffer = mVertices[i];
                const uint32_t vertexSize = mVertexStrides[i];
                const uint32_t vertexCount = buffer.DataSize / vertexSize;
                bool isDynamic = isDynamicVertices();
                Usage usage = isDynamic ? Usage::kDynamic : Usage::kImmutable;
                CPUAccessMode access = isDynamic ? CPUAccessMode::kCPUWrite : CPUAccessMode::kCPUNone;
                MemoryType memType = isDynamic ? MemoryType::kBoth : MemoryType::kVRAM;
                mVBuffers[i] = T3D_RENDER_BUFFER_MGR.loadVertexBuffer(vertexSize, vertexCount, buffer, memType, usage, access);
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    const VertexAttribute &Mesh::getVertexAttribute(size_t index) const
    {
        T3D_ASSERT(index < mVertexAttributes.size());

        auto itr = mVertexAttributes.begin();

        size_t i = 0;
        while (itr != mVertexAttributes.end())
        {
            if (i == index)
                break;
            ++itr;
            ++i;
        }

        return *itr;
    }

    //--------------------------------------------------------------------------

    const VertexAttribute &Mesh::addVertexAttribute(uint32_t slot, uint32_t offset, VertexAttribute::Type type, VertexAttribute::Semantic semantic, uint32_t semanticIndex)
    {
        mIsAttrDirty = true;
        return mVertexAttributes.emplace_back(slot, offset, type, semantic, semanticIndex);
    }

    //--------------------------------------------------------------------------

    const VertexAttribute &Mesh::insertVertexAttribute(uint32_t pos, uint32_t slot, uint32_t offset, VertexAttribute::Type type, VertexAttribute::Semantic semantic, uint32_t semanticIndex)
    {
        if (pos >= mVertexAttributes.size())
        {
            return addVertexAttribute(slot, offset, type, semantic, semanticIndex);
        }

        auto itr = mVertexAttributes.begin();
        size_t i = 0;
        for (i = 0; i < pos; ++i)
        {
            ++itr;
        }

        itr = mVertexAttributes.emplace(itr, slot, offset, type, semantic, semanticIndex);
        mIsAttrDirty = true;
        
        return *itr;
    }

    //--------------------------------------------------------------------------

    TResult Mesh::addVertexAttribute(const VertexAttribute &vertexAttribute)
    {
        mVertexAttributes.emplace_back(vertexAttribute);
        mIsAttrDirty = true;
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Mesh::insertVertexAttribute(uint32_t pos, const VertexAttribute &vertexAttribute)
    {
        if (pos >= mVertexAttributes.size())
        {
            return addVertexAttribute(vertexAttribute);
        }

        auto itr = mVertexAttributes.begin();
        size_t i = 0;
        for (i = 0; i < pos; ++i)
        {
            ++itr;
        }

        itr = mVertexAttributes.insert(itr, vertexAttribute);
        mIsAttrDirty = true;
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Mesh::removeVertexAttribute(uint32_t pos)
    {
        if (pos >= mVertexAttributes.size())
        {
            T3D_LOG_ERROR(LOG_TAG_RENDER,
                "Remove attribute but pos is out of bound !!!");
            return T3D_ERR_OUT_OF_BOUND;
        }

        auto itr = mVertexAttributes.begin();
        size_t i = 0;
        for (i = 0; i < pos; ++i)
        {
            ++itr;
        }

        mVertexAttributes.erase(itr);
        mIsAttrDirty = true;
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Mesh::removeVertexAttribute(VertexAttribute::Semantic semantic, uint32_t sematicIndex)
    {
        TResult ret = T3D_ERR_NOT_FOUND;
        auto itr = mVertexAttributes.begin();
        while (itr != mVertexAttributes.end())
        {
            if (itr->getSemantic() == semantic 
                && itr->getSemanticIndex() == sematicIndex)
            {
                mVertexAttributes.erase(itr);
                mIsAttrDirty = true;
                ret = T3D_OK;
                break;
            }
            ++itr;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Mesh::removeAllVertexAttributes()
    {
        mVertexAttributes.clear();
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Mesh::updateVertexAttribute(uint32_t pos, uint32_t stream, uint32_t offset, VertexAttribute::Type type, VertexAttribute::Semantic semantic, uint32_t semanticIndex)
    {
        if (pos >= mVertexAttributes.size())
        {
            T3D_LOG_ERROR(LOG_TAG_RENDER, "Update attribute but pos is out of bound !!!");
            return T3D_ERR_OUT_OF_BOUND;
        }

        auto itr = mVertexAttributes.begin();
        size_t i = 0;
        for (i = 0; i < pos; ++i)
        {
            ++itr;
        }

        *itr = VertexAttribute(stream, offset, type, semantic, semanticIndex);
        mIsAttrDirty = true;
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    const VertexAttribute *Mesh::findVertexAttributeBySemantic(VertexAttribute::Semantic semantic, uint32_t semanticIndex) const
    {
        auto itr = mVertexAttributes.begin();
        while (itr != mVertexAttributes.end())
        {
            if (itr->getSemantic() == semantic 
                && itr->getSemanticIndex() == semanticIndex)
            {
                return &(*itr);
            }
            ++itr;
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------

    void Mesh::flushVertexAttributes()
    {
        if (mIsAttrDirty)
        {
            // mVertexDecl = T3D_RENDER_BUFFER_MGR.addVertexDeclaration(mVertexAttributes);
            mIsAttrDirty = false;
        }
    }

    //--------------------------------------------------------------------------

#if defined (T3D_DEBUG)
    String Mesh::getDebugInfo() const
    {
        return "";
    }

#endif
    
    //--------------------------------------------------------------------------
}
