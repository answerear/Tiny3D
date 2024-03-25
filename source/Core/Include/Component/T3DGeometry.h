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

#ifndef __T3D_GEOMETRY_H__
#define __T3D_GEOMETRY_H__


#include "Component/T3DRenderable.h"


namespace Tiny3D
{
    TCLASS()
    class T3D_ENGINE_API Geometry : public Renderable
    {
        TRTTI_ENABLE(Renderable)
        TRTTI_FRIEND
        
    public:
        ~Geometry() override = default;

        ComponentPtr clone() const override;

        Material *getMaterial() override;

        PrimitiveType getPrimitiveType() const override;

        VertexDeclaration *getVertexDeclaration() const override;
        
        const VertexBuffers &getVertexBuffers() const override;

        IndexBuffer *getIndexBuffer() const override;

        const VertexStrides &getVertexStrides() const override;

        const VertexOffsets &getVertexOffsets() const override;

        Mesh *getMeshObject() const { return mMesh; }

        void setMeshObject(Mesh *mesh, SubMesh *submesh);
        
        SubMesh *getSubMeshObject() const { return mSubMesh; }
        
        TPROPERTY(RTTRFuncName="Mesh", RTTRFuncType="getter")
        const String &getMesh() const { return mMeshName; }

        TPROPERTY(RTTRFuncName="Mesh", RTTRFuncType="setter")
        void setMesh(const String &name) { mMeshName = name; }

        TPROPERTY(RTTRFuncName="SubMesh", RTTRFuncType="getter")
        const String &getSubMesh() const { return mSubMeshName; }

        TPROPERTY(RTTRFuncName="SubMesh", RTTRFuncType="setter")
        void setSubMesh(const String &name) { mSubMeshName = name; }

    protected:
        Geometry() = default;

        TResult cloneProperties(const Component * const src) override;

        bool onPostLoad() override;
        
    protected:
        String  mMeshName {};
        String  mSubMeshName {};

        MeshPtr mMesh {nullptr};
        SubMesh *mSubMesh {nullptr};
    };
}


#endif  /*__T3D_GEOMETRY_H__*/
