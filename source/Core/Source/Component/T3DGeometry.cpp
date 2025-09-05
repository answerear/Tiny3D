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


#include "Component/T3DGeometry.h"
#include "Kernel/T3DArchive.h"
#include "Resource/T3DMaterialManager.h"
#include "Resource/T3DMesh.h"
#include "Resource/T3DSubMesh.h"
#include "Resource/T3DMeshManager.h"
#include "Resource/T3DMaterial.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    Geometry::Geometry(const UUID &uuid)
        : Renderable(uuid)
    {
        
    }

    //--------------------------------------------------------------------------

    Geometry::~Geometry()
    {
        
    }

    //--------------------------------------------------------------------------

    void Geometry::onDestroy()
    {
        if (!mIsDynamicBatch && mMaterial != nullptr)
        {
            // 不是动态合批，要自己卸载掉
            T3D_MATERIAL_MGR.unload(mMaterial);
            mMaterial = nullptr;
        }
        else
        {
            mMaterial = nullptr;
        }

        Renderable::onDestroy();
    }

    //--------------------------------------------------------------------------

    ComponentPtr Geometry::clone() const
    {
        GeometryPtr geometry = T3D_NEW Geometry();
        TResult ret = geometry->cloneProperties(this);
        if (T3D_FAILED(ret))
        {
            geometry = nullptr;
        }
        return geometry;
    }

    //--------------------------------------------------------------------------

    TResult Geometry::cloneProperties(const Component *const src)
    {
        TResult ret;

        do
        {
            ret = Component::cloneProperties(src);
            if (T3D_FAILED(ret))
            {
                break;
            }

            const Geometry * const other = static_cast<const Geometry * const>(src);
            mMeshUUID = other->mMeshUUID;
            mSubMeshName = other->mSubMeshName;

            mMesh = other->mMesh;
            mSubMesh = other->mSubMesh;
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    Material *Geometry::getMaterial()
    {
        return mMaterial;
    }

    //--------------------------------------------------------------------------

    PrimitiveType Geometry::getPrimitiveType() const
    {
        if (mMesh != nullptr && mSubMesh != nullptr)
        {
            return mSubMesh->getPrimitiveType();
        }
        return PrimitiveType::kTriangleList;
    }

    //--------------------------------------------------------------------------

    VertexDeclaration *Geometry::getVertexDeclaration() const
    {
        if (mMesh != nullptr)
        {
            return mMesh->getVertexDeclaration();
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    const VertexBuffers &Geometry::getVertexBuffers() const
    {
        if (mMesh != nullptr)
        {
            return mMesh->getVertexBuffers();
        }
        static VertexBuffers vbuffers;
        return vbuffers;
    }

    //--------------------------------------------------------------------------

    IndexBuffer *Geometry::getIndexBuffer() const
    {
        if (mMesh != nullptr && mSubMesh != nullptr)
        {
            return mSubMesh->getIndexBuffer();
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------

    const VertexStrides &Geometry::getVertexStrides() const
    {
        if (mMesh != nullptr)
        {
            return mMesh->getVertexStrides();
        }
        static VertexStrides strides;
        return strides;
    }

    //--------------------------------------------------------------------------

    const VertexOffsets &Geometry::getVertexOffsets() const
    {
        if (mMesh != nullptr)
        {
            return mMesh->getVertexOffsets();
        }
        static VertexOffsets offsets;
        return offsets;
    }

    //--------------------------------------------------------------------------

    void Geometry::setMeshObject(Mesh *mesh, SubMesh *submesh)
    {
        if (mesh == nullptr || submesh == nullptr)
        {
            mMesh = nullptr;
            mSubMesh = nullptr;
            mMeshUUID = UUID::INVALID;
            mSubMeshName = "";

            if (!mIsDynamicBatch && mMaterial != nullptr)
            {
                T3D_MATERIAL_MGR.unload(mMaterial);
                mMaterial = nullptr;
            }
        }
        else
        {
            const auto &submeshes = mesh->getSubMeshes();
            const auto &itr = submeshes.find(submesh->getName());
            if (itr != submeshes.end())
            {
                mMesh = mesh;
                mSubMesh = submesh;
                mMeshUUID = mMesh->getUUID();
                mSubMeshName = mSubMesh->getName();

                generateRenderMaterial();
            }
        }
    }
    
    //--------------------------------------------------------------------------

    void Geometry::onPostLoad()
    {
        Renderable::onPostLoad();
        // Archive *archive = nullptr;
        // mMesh = T3D_MESH_MGR.loadMesh(archive, mMeshName);
        // if (mMesh != nullptr)
        // {
        //     mSubMesh = mMesh->getSubMesh(mSubMeshName);
        // }
    }

    //--------------------------------------------------------------------------

    void Geometry::onLoadResource(Archive *archive)
    {
        if (mMesh != nullptr)
        {
            if (mMesh->getUUID() != mMeshUUID)
            {
                if (!mIsDynamicBatch && mMaterial != nullptr)
                {
                    T3D_MATERIAL_MGR.unload(mMaterial);
                    mMaterial = nullptr;
                }
                
                mMesh = nullptr;
            }
        }

        if (mMesh == nullptr)
        {
            mMesh = T3D_MESH_MGR.loadMesh(archive, mMeshUUID);

            if (mMesh != nullptr)
            {
                mSubMesh = mMesh->getSubMesh(mSubMeshName);

                if (mSubMesh != nullptr && archive != nullptr)
                {
                    // 加载出来的，那就顺便把材质加载一下
                    T3D_MATERIAL_MGR.loadMaterial(archive, mSubMesh->getMaterialUUID());
                }

                generateRenderMaterial();
            }
            else
            {
                T3D_LOG_ERROR(LOG_TAG_COMPONENT, "Failed to load mesh (%s) !", mMeshUUID.toString().c_str());
            }
        }
    }

    //--------------------------------------------------------------------------

    void Geometry::generateRenderMaterial()
    {
        Material *material = static_cast<Material *>(T3D_MATERIAL_MGR.getResource(mSubMesh->getMaterialUUID()));
        T3D_ASSERT(material != nullptr);
        
        mIsDynamicBatch = false;

        if (mIsDynamicBatch)
        {
            // 动态合批，直接使用 submesh 的材质即可
            mMaterial = material;
        }
        else
        {
            // 不合批，生成一个新材质
            mMaterial = T3D_MATERIAL_MGR.clone(material);
        }
    }

    //--------------------------------------------------------------------------
}
