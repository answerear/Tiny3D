

#ifndef __T3D_D3D9_VERTEX_DECLARATION_H__
#define __T3D_D3D9_VERTEX_DECLARATION_H__


#include "T3DD3D9Prerequisites.h"
#include "Render/T3DHardwareVertexBuffer.h"


namespace Tiny3D
{
    class D3D9VertexDeclaration : public VertexDeclaration
    {
    public:
        D3D9VertexDeclaration();
        virtual ~D3D9VertexDeclaration();

        virtual const VertexElement &addElement(size_t stream, size_t offset, VertexElement::Type type, VertexElement::Semantic semantic) override;
        virtual const VertexElement &insertElement(size_t pos, size_t stream, size_t offset, VertexElement::Type type, VertexElement::Semantic semantic) override;
        virtual bool addElement(const VertexElement &vertexElement) override;
        virtual bool insertElement(size_t pos, const VertexElement &vertexElement) override;
        virtual void removeElement(VertexElement::Semantic semantic) override;
        virtual void removeElement(size_t pos) override;
        virtual void removeAllElements() override;

        virtual void updateElement(size_t pos, size_t stream, size_t offset, VertexElement::Type type, VertexElement::Semantic semantic) override;

        virtual VertexDeclarationPtr clone() const override;

        LPDIRECT3DVERTEXDECLARATION9 getD3D9VertexDeclaration();

    protected:
        LPDIRECT3DVERTEXDECLARATION9    mD3DVertexDecl;
        bool        mIsDirty;
    };
}


#endif  /*__T3D_D3D9_VERTEX_DECLARATION_H__*/
