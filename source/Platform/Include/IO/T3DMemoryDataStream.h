

#ifndef __T3D_MEMORY_DATA_STREAM_H__
#define __T3D_MEMORY_DATA_STREAM_H__


#include "T3DDataStream.h"


namespace Tiny3D
{
	/**
	 * @class T3DMemoryDataStream
	 * @brief 内存数据流类.
	 * @note 该类提供基于内存数据流的存储访问.
	 */
	class T3D_PLATFORM_API MemoryDataStream : public DataStream
	{
	public:
		static const uint32_t MEMORY_BUFFER_SIZE;	/**< 默认内存流大小 */

		/**
		 * @brief Constructor for T3DMemoryDataStream.
		 * @note 该构造函数内部不自动非配空间，直接对传入的缓冲区操作
		 * @param [in] pBuffer : 数据缓冲
		 * @param [in] unSize : 缓冲大小
		 */
		MemoryDataStream(uchar_t *pBuffer, size_t unSize);

		/**
		 * @brief Constructor for T3DMemoryDataStream.
		 * @note 内部自动分配空间.
		 * @param [in] unSize : 分配空间大小，默认为V_MEMORY_DATA_STREAM_BUFFER_SIZE.
		 * @see V_MEMORY_DATA_STREAM_BUFFER_SIZE
		 */
		MemoryDataStream(size_t unSize = MEMORY_BUFFER_SIZE);

		/**
		 * @brief Constructor for T3DMemoryDataStream.
		 * @note 拷贝构造.
		 * @param [in] other : 源对象
		 */
		MemoryDataStream(const MemoryDataStream &other);

		/**
		 * @brief Destructor for T3DMemoryDataStream.
		 */
		virtual ~MemoryDataStream();

		/**
		 * @brief 重载赋值操作符.
		 */
		MemoryDataStream &operator =(const MemoryDataStream &other);

		/**
		 * @brief 读取内存数据流.
		 * @param [in] pBuffer : 数据缓冲区
		 * @param [in] nSize : 数据缓冲区大小
		 * @return 读取到的数据大小  
		 */
		size_t read(void *pBuffer, size_t nSize);

		/**
		 * @brief 写入内存数据流.
		 * @param [in] pBuffer : 数据缓冲区
		 * @param [in] nSize : 数据缓冲区大小 
		 * @return 写入数据大小  
		 */
		size_t write(void *pBuffer, size_t nSize);

		/**
		 * @brief 设置内存数据流读写偏移位置.
		 * @param [in] unPos : 偏移位置
		 * @return void
		 */
		void seek(long_t lPos);

		/**
		 * @brief 获取当前内存数据流读写偏移位置.
		 * @return 返回读写偏移位置  
		 */
		long_t tell() const;

		/**
		 * @brief 获取内存数据流大小.
		 * @return 返回内存数据流大小  
		 */
		long_t size() const;

		/**
		 * @brief 是否到达内存数据流末尾.
		 * @return 到达末尾返回true，否则返回false.  
		 */
		bool eof() const;

	protected:
		void copy(const MemoryDataStream &other);

	protected:
		uchar_t		*m_pBuffer;		/**< 数据缓冲区 */
		long_t		m_lSize;		/**< 数据缓冲区大小 */
		long_t		m_lCurPos;		/**< 当前读写位置 */

		bool		m_bCreated;		/**< 是否内存创建标记 */
	};
}


#endif	/*__T3D_MEMORY_DATA_STREAM_H__*/
