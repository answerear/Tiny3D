

#include "T3DD3D9Mappings.h"


namespace Tiny3D
{
    D3DPRIMITIVETYPE D3D9Mappings::get(Renderer::PrimitiveType priType)
    {
        D3DPRIMITIVETYPE d3dtype = D3DPT_TRIANGLESTRIP;

        switch (priType)
        {
        case Renderer::E_PT_POINT:
            d3dtype = D3DPT_POINTLIST;
            break;
        case Renderer::E_PT_LINE:
            d3dtype = D3DPT_LINELIST;
            break;
        case Renderer::E_PT_TRIANGLE_LIST:
            d3dtype = D3DPT_TRIANGLELIST;
            break;
        case Renderer::E_PT_TRIANGLE_STRIP:
            d3dtype = D3DPT_TRIANGLESTRIP;
            break;
        case Renderer::E_PT_TRIANGLE_FAN:
            d3dtype = D3DPT_TRIANGLEFAN;
        }

        return d3dtype;
    }

    DWORD D3D9Mappings::get(HardwareBuffer::Usage usage)
    {
        DWORD ret = 0;

        if (usage & HardwareBuffer::E_HBU_DYNAMIC)
        {
            ret |= D3DUSAGE_DYNAMIC;
        }

        if (usage & HardwareBuffer::E_HBU_WRITE_ONLY)
        {
            ret |= D3DUSAGE_WRITEONLY;
        }

        return ret;
    }

    DWORD D3D9Mappings::get(HardwareBuffer::Usage usage, HardwareBuffer::LockOptions options)
    {
        DWORD ret = 0;
        if (options == HardwareBuffer::E_HBL_DISCARD)
        {
            if (usage & HardwareBuffer::E_HBU_DYNAMIC)
                ret |= D3DLOCK_DISCARD;
        }

        if (options == HardwareBuffer::E_HBL_READ_ONLY)
        {
            if (!(usage & HardwareBuffer::E_HBU_WRITE_ONLY))
                ret |= D3DLOCK_READONLY;
        }

        if (options == HardwareBuffer::E_HBL_NO_OVERWRITE)
        {
            if (usage & HardwareBuffer::E_HBU_DYNAMIC)
                ret |= D3DLOCK_NOOVERWRITE;
        }

        return ret;
    }

    D3DFORMAT D3D9Mappings::get(HardwareIndexBuffer::Type type)
    {
        if (type == HardwareIndexBuffer::E_IT_16BITS)
        {
            return D3DFMT_INDEX16;
        }
        else
        {
            return D3DFMT_INDEX32;
        }
    }

    D3DDECLTYPE D3D9Mappings::get(VertexElement::Type type)
    {
        D3DDECLTYPE d3dtype = D3DDECLTYPE_FLOAT3;

        switch (type)
        {
        case VertexElement::E_VET_COLOR:
            d3dtype = D3DDECLTYPE_D3DCOLOR;
            break;
        case VertexElement::E_VET_FLOAT1:
            d3dtype = D3DDECLTYPE_FLOAT1;
            break;
        case VertexElement::E_VET_FLOAT2:
            d3dtype = D3DDECLTYPE_FLOAT2;
            break;
        case VertexElement::E_VET_FLOAT3:
            d3dtype = D3DDECLTYPE_FLOAT3;
            break;
        case VertexElement::E_VET_SHORT2:
            d3dtype = D3DDECLTYPE_SHORT2;
            break;
        case VertexElement::E_VET_SHORT4:
            d3dtype = D3DDECLTYPE_SHORT4;
            break;
        case VertexElement::E_VET_UBYTE4:
            d3dtype = D3DDECLTYPE_UBYTE4;
            break;
        }

        return d3dtype;
    }

    D3DDECLUSAGE D3D9Mappings::get(VertexElement::Semantic semantic)
    {
        D3DDECLUSAGE usage =  D3DDECLUSAGE_POSITION;

        switch (semantic)
        {
        case VertexElement::E_VES_POSITION:
            usage = D3DDECLUSAGE_POSITION;
            break;
        case VertexElement::E_VES_BLENDWEIGHT:
            usage = D3DDECLUSAGE_BLENDWEIGHT;
            break;
        case VertexElement::E_VES_BLENDINDICES:
            usage = D3DDECLUSAGE_BLENDINDICES;
            break;
        case VertexElement::E_VES_NORMAL:
            usage = D3DDECLUSAGE_NORMAL;
            break;
        case VertexElement::E_VES_DIFFUSE:
        case VertexElement::E_VES_SPECULAR:
            usage = D3DDECLUSAGE_COLOR;
            break;
        case VertexElement::E_VES_TEXCOORD:
            usage = D3DDECLUSAGE_TEXCOORD;
            break;
        case VertexElement::E_VES_TANGENT:
            usage = D3DDECLUSAGE_TANGENT;
            break;
        case VertexElement::E_VES_BINORMAL:
            usage = D3DDECLUSAGE_BINORMAL;
            break;
        }

        return usage;
    }
}
