

#ifndef __T3D_HARDWARE_BUFFER_H__
#define __T3D_HARDWARE_BUFFER_H__


#include "Misc/T3DObject.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    class T3D_ENGINE_API HardwareBuffer : public Object
    {
    public:
        enum Usage
        {
            /// 静态缓存，数据放置在显存中，对于静态缓存读写数据是很慢的。
            /// 一般用于创建后就不更新的数据
            E_HBU_STATIC = (1 << 0),
            /// 动态缓存，数据存放在AGP内存中，这种数据能很快的更新
            E_HBU_DYNAMIC = (1 << 1),
            /// 定应用程序只能写缓存。它允许驱动程序分配最适合的内存地址作为写缓存。
            /// 注意如果从创建好的这种缓存中读数据，将会返回错误信息。
            E_HBU_WRITE_ONLY = (1 << 2),
            /// 联合E_HBU_STATIC和E_HBU_WRITE_ONLY
            E_HBU_STATIC_WRITE_ONLY = (E_HBU_STATIC | E_HBU_WRITE_ONLY),
            /// 联合E_HBU_DYNAMIC和E_HBU_WRITE_ONLY
            E_HBU_DYNAMIC_WRITE_ONLY = (E_HBU_DYNAMIC | E_HBU_WRITE_ONLY),
        };

        enum LockOptions
        {
            E_HBL_NORMAL = 0,
            E_HBL_DISCARD,
            E_HBL_NO_OVERWRITE,
            E_HBL_READ_ONLY,
            E_HBL_WRITE_ONLY,
        };

        virtual ~HardwareBuffer();

        void *lock(size_t offset, size_t size, LockOptions options);
        void *lock(LockOptions options);
        void unlock();

        virtual bool readData(size_t offset, size_t size, void *dst) = 0;
        virtual bool writeData(size_t offset, size_t size, const void *src, bool discardWholeBuffer = false) = 0;
        
        bool copyData(const HardwareBufferPtr &srcBuffer, size_t srcOffset, size_t dstOffset, size_t size, bool discardWholeBuffer = false);
        bool copyData(const HardwareBufferPtr &srcBuffer);

        size_t getBufferSize() const    { return mBufferSize; }
        Usage getUsage() const          { return mUsage; }
        bool isSystemMemory() const     { return mSystemMemory; }
        bool hasShadowBuffer() const    { return mUseShadowBuffer; }
        bool isLocked() const           { return (mUseShadowBuffer ? mShadowBuffer->isLocked() : mIsLocked); }

    protected:
        HardwareBuffer(Usage usage, bool systemMemory, bool useShadowBuffer);

        virtual void *lockImpl(size_t offset, size_t size, LockOptions options) = 0;
        virtual void unlockImpl() = 0;

        virtual void updateFromShadow();

        size_t  mBufferSize;                /// 硬件缓存大小
        size_t  mLockSize;                  /// 锁缓存大小
        size_t  mLockOffset;                /// 锁缓存起始偏移

        Usage   mUsage;                     /// 缓存类型

        HardwareBufferPtr   mShadowBuffer;  /// 影子缓存，提高读写效率

        bool    mSystemMemory;              /// 是否使用系统内存
        bool    mUseShadowBuffer;           /// 是否使用影子缓存
        bool    mIsLocked;                  /// 硬件缓存是否被锁
        bool    mIsShadowBufferDirty;       /// 影子缓存是否需要更新
    };
}


#endif  /*__T3D_HARDWARE_BUFFER_H__*/
