

#ifndef __T3D_DATA_STREAM_H__
#define __T3D_DATA_STREAM_H__


#include "T3DType.h"
#include "T3DMacro.h"
#include "T3DPlatformPrerequisites.h"


namespace Tiny3D
{
    class T3D_PLATFORM_API DataStream
    {
        T3D_DECLARE_INTERFACE(DataStream);

    public:
        /**
         * @brief 从数据流中读数据到缓冲区.
         * @param [in] pBuffer : 数据缓冲区
         * @param [in] nSize : 数据缓冲区大小
         * @return 返回读取到的数据大小.
         */
        virtual size_t read(void *pBuffer, size_t nSize) = 0;

        /**
         * @brief 写缓冲区数据到数据流中.
         * @param [in] pBuffer : 数据缓冲区
         * @param [in] nSize : 数据缓冲区大小
         * @return 返回写入的数据大小.
         */
        virtual size_t write(void *pBuffer, size_t nSize) = 0;

        /**
         * @brief 把数据流指针移动到指定位置.
         * @param [in] llPos : 指定偏移位置
         * @param [in] relative : 是否相对位置
         * @return 当调用成功时，返回true
         */
        virtual bool seek(long_t lPos, bool relative) = 0;

        /**
         * @brief 获取当前数据流读写位置.
         * @return 返回当前数据流读写位置.
         */
        virtual long_t tell() const = 0;

        /**
         * @brief 获取数据流的大小.
         * @return 返回当前数据流的大小
         */
        virtual long_t size() const = 0;

        /**
         * @brief 是否到数据流末尾.
         * @return 到末尾返回true，否则返回false.
         */
        virtual bool eof() const = 0;

        /**
         * @brief 输出基本类型值到数据流
         */
        DataStream &operator<<(bool val);
        DataStream &operator<<(int8_t val);
        DataStream &operator<<(uint8_t val);
        DataStream &operator<<(int16_t val);
        DataStream &operator<<(uint16_t val);
        DataStream &operator<<(int32_t val);
        DataStream &operator<<(uint32_t val);
        DataStream &operator<<(int64_t val);
        DataStream &operator<<(uint64_t val);
        DataStream &operator<<(float val);
        DataStream &operator<<(double val);
        DataStream &operator<<(const char *s);
        DataStream &operator<<(const String &s);

        /**
         * @brief 输入数据流到基本类型值
         */
        DataStream &operator>>(bool &val);
        DataStream &operator>>(int8_t &val);
        DataStream &operator>>(uint8_t &val);
        DataStream &operator>>(int16_t &val);
        DataStream &operator>>(uint16_t &val);
        DataStream &operator>>(int32_t &val);
        DataStream &operator>>(uint32_t &val);
        DataStream &operator>>(int64_t &val);
        DataStream &operator>>(uint64_t &val);
        DataStream &operator>>(float &val);
        DataStream &operator>>(double &val);
        DataStream &operator>>(char *&s);
        DataStream &operator>>(String &s);
    };
}

#endif  /*__T3D_DATA_STREAM_H__*/
