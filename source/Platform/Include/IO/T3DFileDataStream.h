

#ifndef __T3D_FILE_DATA_STREAM_H__
#define __T3D_FILE_DATA_STREAM_H__



#include "T3DDataStream.h"
#include <stdio.h>


namespace Tiny3D
{
    /**
     * @class T3DFileDataStream
     * @brief 文件流类.
     * @note 该类提供文件流读写操作.
     */
    class T3D_PLATFORM_API FileDataStream : public DataStream
    {
        T3D_DISABLE_COPY(FileDataStream);

    public:
        /**
         * @brief 文件流打开模式
         */
        enum EOpenMode
        {
            E_MODE_NOT_OPEN = 0x00000000L,      /**< 文件没有打开 */
            E_MODE_READ_ONLY = 0x00000001L,     /**< 只读方式打开 */
            E_MODE_WRITE_ONLY = 0x00000002L,        /**< 只写方式打开 */
            E_MODE_READ_WRITE = E_MODE_READ_ONLY | E_MODE_WRITE_ONLY,   /**< 读写方式打开 */
            E_MODE_APPEND = 0x00000004L,            /**< 追加方式打开 */
            E_MODE_TRUNCATE = 0x00000008L,      /**< 文件已经存在时清空原来内容打开 */
            E_MODE_TEXT = 0x00000010L           /**< 文本文件 */
        };

        /**
         * @brief Constructor for T3DFileDataStream.
         */
        FileDataStream();

        /**
         * @brief Destructor for T3DFileDataStream.
         */
        virtual ~FileDataStream();

        /**
         * @brief 打开文件流.
         * @param [in] szFileName : 文件名
         * @param [in] eMode : 打开方式
         * @return 打开成功返回true，否则返回false.
         * @see enum EOpenMode
         */
        bool open(const char *szFileName, uint32_t unMode);

        /**
         * @brief 关闭文件流.
         * @return void
         */
        void close();

        /**
         * @brief 读取文件流.
         * @param [in] pBuffer : 数据缓冲区
         * @param [in] nSize : 数据缓冲区大小
         * @return 读取到的数据大小.
         */
        size_t read(void *pBuffer, size_t nSize);

        /**
         * @brief 写入文件流.
         * @param [in] pBuffer : 待写入数据缓冲区
         * @param [in] nSize : 数据缓冲区大小
         * @return 写入的数据大小
         */
        size_t write(void *pBuffer, size_t nSize);

        /**
         * @brief 把文件缓存在内存的内容全部强制写到文件中
         */
        void flush();

        /**
         * @brief 设置文件流读写偏移位置.
         * @param [in] llPos : 偏移位置
         * @return void
         */
        void seek(long_t lPos);

        /**
         * @brief 获取当前文件流当前读写位置.
         * @return 返回文件流当前读写位置
         */
        long_t tell() const;

        /**
         * @brief 获取文件大小.
         * @return 返回文件大小
         */
        long_t size() const;

        /**
         * @brief 获取是否到文件流末尾.
         * @return 已到文件流末尾
         */
        bool eof() const;

    protected:
        FILE                *m_pFileHandle;     /**< 文件操作指针 */
        mutable long_t      m_lSize;            /**< 文件大小 */
    };
}


#endif  /*__T3D_FILE_DATA_STREAM_H__*/
