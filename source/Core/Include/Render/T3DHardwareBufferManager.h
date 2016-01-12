

#ifndef __T3D_HARDWARE_BUFFER_MANAGER_H__
#define __T3D_HARDWARE_BUFFER_MANAGER_H__


#include "Misc/T3DObject.h"
#include "Render/T3DHardwareVertexBuffer.h"
#include "Render/T3DHardwareIndexBuffer.h"


namespace Tiny3D
{
    class T3D_ENGINE_API HardwareBufferManagerBase : public Object
    {
    public:
        virtual ~HardwareBufferManagerBase();

        virtual HardwareVertexBufferPtr createVertexBuffer(size_t vertexSize, size_t vertexCount, HardwareBuffer::Usage usage, bool useShadowBuffer) = 0;
        virtual HardwareIndexBufferPtr createIndexBuffer(HardwareIndexBuffer::Type indexType, size_t indexCount, HardwareBuffer::Usage usage, bool useShadowBuffer) = 0;

        virtual VertexDeclarationPtr createVertexDeclaration();

    protected:
        HardwareBufferManagerBase();

        typedef std::set<HardwareVertexBufferPtr>   VertexBufferList;
        typedef VertexBufferList::iterator          VertexBufferListItr;
        typedef VertexBufferList::const_iterator    VertexBufferListConstItr;

        typedef std::set<HardwareIndexBufferPtr>    IndexBufferList;
        typedef IndexBufferList::iterator           IndexBufferListItr;
        typedef IndexBufferList::const_iterator     IndexBufferListConstItr;

        VertexBufferList    mVertexBuffers;
        IndexBufferList     mIndexBuffers;
    };

    class T3D_ENGINE_API HardwareBufferManager 
        : public HardwareBufferManagerBase
        , public Singleton<HardwareBufferManager>
    {
    public:
        HardwareBufferManager(HardwareBufferManagerBase *impl);
        ~HardwareBufferManager();

        virtual HardwareVertexBufferPtr createVertexBuffer(size_t vertexSize, size_t vertexCount, HardwareBuffer::Usage usage, bool useShadowBuffer) override;
        virtual HardwareIndexBufferPtr createIndexBuffer(HardwareIndexBuffer::Type indexType, size_t indexCount, HardwareBuffer::Usage usage, bool useShadowBuffer) override;
        virtual VertexDeclarationPtr createVertexDeclaration() override;

    protected:
        HardwareBufferManagerBasePtr   mImpl;
    };

    #define T3D_HARDWARE_BUFFER_MGR     (HardwareBufferManager::getInstance())
}


#endif  /*__T3D_HARDWARE_BUFFER_MANAGER_H__*/
