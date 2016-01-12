
#ifndef __T3D_HARDWARE_VERTEX_BUFFER_H__
#define __T3D_HARDWARE_VERTEX_BUFFER_H__


#include "Render/T3DHardwareBuffer.h"


namespace Tiny3D
{
    class T3D_ENGINE_API HardwareVertexBuffer : public HardwareBuffer
    {
    public:
        virtual ~HardwareVertexBuffer();

        size_t getVertexSize() const    { return mVertexSize; }
        size_t getVertexCount() const   { return mVertexCount; }

    protected:
        HardwareVertexBuffer(size_t vertexSize, size_t vertexCount, Usage usage, bool useSystemMemory, bool useShadowBuffer);

        size_t  mVertexCount;
        size_t  mVertexSize;
    };

    class T3D_ENGINE_API VertexElement : public Object
    {
    public:
        enum Type
        {
            E_VET_FLOAT1 = 0,
            E_VET_FLOAT2 = 1,
            E_VET_FLOAT3 = 2,
            E_VET_FLOAT4 = 3,
            E_VET_COLOR = 4,
            E_VET_BYTE4 = 5,
            E_VET_BYTE4_NORM = 6,
            E_VET_UBYTE4 = 7,
            E_VET_UBYTE4_NORM = 8,
            E_VET_SHORT2 = 9,
            E_VET_SHORT4 = 10,
            E_VET_SHORT2_NORM = 11,
            E_VET_SHORT4_NORM = 12,
            E_VET_USHORT2 = 13,
            E_VET_USHORT4 = 14,
            E_VET_USHORT2_NORM = 15,
            E_VET_USHORT4_NORM = 16,
            E_VET_DOUBLE1 = 17,
            E_VET_DOUBLE2 = 18,
            E_VET_DOUBLE3 = 19,
            E_VET_DOUBLE4 = 20,
            E_VET_INT1 = 21,
            E_VET_INT2 = 22,
            E_VET_INT3 = 23,
            E_VET_INT4 = 24,
            E_VET_UINT1 = 25,
            E_VET_UINT2 = 26,
            E_VET_UINT3 = 27,
            E_VET_UINT4 = 28,
            E_VET_FLOAT16_2 = 29,
            E_VET_FLOAT16_4 = 30,
        };

        enum Semantic
        {
            E_VES_POSITION = 0,
            E_VES_BLENDWEIGHT = 1,
            E_VES_BLENDINDICES = 2,
            E_VES_NORMAL = 3,
            E_VES_DIFFUSE = 4,
            E_VES_SPECULAR = 5,
            E_VES_TEXCOORD = 6,
            E_VES_TENGENT = 7,
            E_VES_BINORMAL = 8,
        };

        VertexElement();
        VertexElement(size_t offset, Type type, Semantic semantic);
        virtual ~VertexElement();

        size_t getOffset() const    { return mOffset; }

        Type getType() const        { return mType; }

        Semantic getSemantic() const    { return mSemantic; }

        size_t getSize() const;

    protected:
        Type        mType;          /// 顶点元素数据类型
        Semantic    mSemantic;      /// 顶点元素语义
        size_t      mOffset;        /// 这个顶点元素定义的在顶点缓存中的起始点偏移值
    };

    class T3D_ENGINE_API VertexDeclaration : public Object
    {
    public:
        typedef std::list<VertexElement>            VertexElementList;
        typedef VertexElementList::iterator         VertexElementListItr;
        typedef VertexElementList::const_iterator   VertexElementListConstItr;

        VertexDeclaration();
        virtual ~VertexDeclaration();

        size_t getElementCount() const  { return mVertexElements.size(); }
        const VertexElementList &getElements() const    { return mVertexElements; }

        const VertexElement &getElement(size_t index) const;

        virtual const VertexElement &addElement(size_t offset, VertexElement::Type type, VertexElement::Semantic semantic);
        virtual const VertexElement &insertElement(size_t pos, size_t offset, VertexElement::Type type, VertexElement::Semantic semantic);

        virtual void removeElement(size_t pos);
        virtual void removeElement(VertexElement::Semantic semantic);
        virtual void removeAllElements();

        virtual void updateElement(size_t pos, size_t offset, VertexElement::Type type, VertexElement::Semantic semantic);

        virtual const VertexElement *findElementBySemantic(VertexElement::Semantic semantic) const;

        virtual size_t getVertexSize() const;

        virtual VertexDeclarationPtr clone() const;

    protected:
        VertexElementList   mVertexElements;
    };
}


#endif  /*__T3D_HARDWARE_VERTEX_BUFFER_H__*/
