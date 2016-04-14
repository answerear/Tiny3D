
#include "Render/T3DHardwareVertexBuffer.h"
#include "Render/T3DHardwareBufferManager.h"


namespace Tiny3D
{
    HardwareVertexBuffer::HardwareVertexBuffer(size_t vertexSize, size_t vertexCount, Usage usage, bool useSystemMemory, bool useShadowBuffer)
        : HardwareBuffer(usage, useSystemMemory, useShadowBuffer)
        , mVertexCount(vertexCount)
        , mVertexSize(vertexSize)
    {
        mBufferSize = mVertexSize * mVertexCount;
    }

    HardwareVertexBuffer::~HardwareVertexBuffer()
    {

    }

    ////////////////////////////////////////////////////////////////////////////

    VertexElement::VertexElement()
        : mType(E_VET_FLOAT1)
        , mSemantic(E_VES_POSITION)
        , mOffset(0)
    {

    }

    VertexElement::VertexElement(size_t offset, Type type, Semantic semantic)
        : mType(type)
        , mSemantic(semantic)
        , mOffset(offset)
    {

    }

    VertexElement::~VertexElement()
    {

    }

    size_t VertexElement::getSize() const
    {
        size_t s = 0;
        switch (mType)
        {
        case E_VET_FLOAT1:
            s = sizeof(float);
            break;
        case E_VET_FLOAT2:
            s = sizeof(float) * 2;
            break;
        case E_VET_FLOAT3:
            s = sizeof(float) * 3;
            break;
        case E_VET_FLOAT4:
            s = sizeof(float) * 4;
            break;
        case E_VET_COLOR:
            s = sizeof(uint32_t);
            break;
        case E_VET_BYTE4:
        case E_VET_BYTE4_NORM:
            s = sizeof(int8_t);
            break;
        case E_VET_UBYTE4:
        case E_VET_UBYTE4_NORM:
            s = sizeof(uint8_t) * 4;
            break;
        case E_VET_SHORT2:
        case E_VET_SHORT2_NORM:
            s = sizeof(int16_t) * 2;
            break;
        case E_VET_SHORT4:
        case E_VET_SHORT4_NORM:
            s = sizeof(int16_t) * 4;
            break;
        case E_VET_USHORT2:
        case E_VET_USHORT2_NORM:
            s = sizeof(uint16_t) * 2;
            break;
        case E_VET_USHORT4:
        case E_VET_USHORT4_NORM:
            s = sizeof(uint16_t) * 4;
            break;
        case E_VET_DOUBLE1:
            s = sizeof(double);
            break;
        case E_VET_DOUBLE2:
            s = sizeof(double) * 2;
            break;
        case E_VET_DOUBLE3:
            s = sizeof(double) * 3;
            break;
        case E_VET_DOUBLE4:
            s = sizeof(double) * 4;
            break;
        case E_VET_INT1:
            s = sizeof(int32_t);
            break;
        case E_VET_INT2:
            s = sizeof(int32_t) * 2;
            break;
        case E_VET_INT3:
            s = sizeof(int32_t) * 3;
            break;
        case E_VET_INT4:
            s = sizeof(int32_t) * 4;
            break;
        case E_VET_UINT1:
            s = sizeof(uint32_t);
            break;
        case E_VET_UINT2:
            s = sizeof(uint32_t) * 2;
            break;
        case E_VET_UINT3:
            s = sizeof(uint32_t) * 3;
            break;
        case E_VET_UINT4:
            s = sizeof(uint32_t) * 4;
            break;
        case E_VET_FLOAT16_2:
            s = sizeof(float);
            break;
        case E_VET_FLOAT16_4:
            s = sizeof(float) * 2;
            break;
        }

        return s;
    }

    ////////////////////////////////////////////////////////////////////////////

    VertexDeclaration::VertexDeclaration()
    {

    }

    VertexDeclaration::~VertexDeclaration()
    {

    }

    const VertexElement &VertexDeclaration::getElement(size_t index) const
    {
        T3D_ASSERT(index < mVertexElements.size());

        VertexElementListConstItr itr = mVertexElements.begin();

        size_t i = 0;
        while (itr != mVertexElements.end())
        {
            if (i == index)
                break;
            ++itr;
            ++i;
        }

        return *itr;
    }

    const VertexElement &VertexDeclaration::addElement(size_t offset, VertexElement::Type type, VertexElement::Semantic semantic)
    {
        VertexElement element(offset, type, semantic);

        mVertexElements.push_back(element);
        return mVertexElements.back();
    }

    const VertexElement &VertexDeclaration::insertElement(size_t pos, size_t offset, VertexElement::Type type, VertexElement::Semantic semantic)
    {
        if (pos >= mVertexElements.size())
        {
            return addElement(offset, type, semantic);
        }

        auto itr = mVertexElements.begin();
        size_t i = 0;
        for (i = 0; i < pos; ++i)
        {
            ++itr;
        }

        itr = mVertexElements.insert(itr, VertexElement(offset, type, semantic));
        return *itr;
    }

    bool VertexDeclaration::addElement(const VertexElement &vertexElement)
    {
        mVertexElements.push_back(vertexElement);
        return true;
    }

    bool VertexDeclaration::insertElement(size_t pos, const VertexElement &vertexElement)
    {
        if (pos >= mVertexElements.size())
        {
            return addElement(vertexElement);
        }

        auto itr = mVertexElements.begin();
        size_t i = 0;
        for (i = 0; i < pos; ++i)
        {
            ++itr;
        }

        itr = mVertexElements.insert(itr, vertexElement);

        return true;
    }

    void VertexDeclaration::removeElement(size_t pos)
    {
        if (pos >= mVertexElements.size())
            return;

        VertexElementListItr itr = mVertexElements.begin();
        size_t i = 0;
        for (i = 0; i < pos; ++i)
        {
            ++itr;
        }

        mVertexElements.erase(itr);
    }

    void VertexDeclaration::removeElement(VertexElement::Semantic semantic)
    {
        VertexElementListItr itr = mVertexElements.begin();
        while (itr != mVertexElements.end())
        {
            if (itr->getSemantic() == semantic)
            {
                mVertexElements.erase(itr);
                break;
            }
            ++itr;
        }
    }

    void VertexDeclaration::removeAllElements()
    {
        mVertexElements.clear();
    }

    void VertexDeclaration::updateElement(size_t pos, size_t offset, VertexElement::Type type, VertexElement::Semantic semantic)
    {
        if (pos >= mVertexElements.size())
            return;

        VertexElementListItr itr = mVertexElements.begin();
        size_t i = 0;
        for (i = 0; i < pos; ++i)
        {
            ++itr;
        }

        *itr = VertexElement(offset, type, semantic);
    }

    const VertexElement *VertexDeclaration::findElementBySemantic(VertexElement::Semantic semantic) const
    {
        VertexElementListConstItr itr = mVertexElements.begin();
        while (itr != mVertexElements.end())
        {
            if (itr->getSemantic() == semantic)
            {
                return &(*itr);
                break;
            }
            ++itr;
        }

        return nullptr;
    }

    size_t VertexDeclaration::getVertexSize() const
    {
        size_t s = 0;
        VertexElementListConstItr itr = mVertexElements.begin();
        while (itr != mVertexElements.end())
        {
            s += itr->getSize();
            ++itr;
        }

        return s;
    }

    VertexDeclarationPtr VertexDeclaration::clone() const
    {
        VertexDeclarationPtr decl = T3D_HARDWARE_BUFFER_MGR.createVertexDeclaration();
        VertexElementListConstItr itr = mVertexElements.begin();

        while (itr != mVertexElements.end())
        {
            decl->addElement(itr->getOffset(), itr->getType(), itr->getSemantic());
            ++itr;
        }

        return decl;
    }
}
