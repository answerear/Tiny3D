

#ifndef __T3D_DIR_INTERFACE_H__
#define __T3D_DIR_INTERFACE_H__


#include "T3DType.h"
#include "T3DMacro.h"


namespace Tiny3D
{
	/**
	 * @class IDirAdapter
	 * @brief 操作系统相关平台层的目录树搜索接口类.
	 * @note 相关操作系统平台需要继承该接口类实现具体平台相关功能.
	 */
	class DirInterface
	{
		T3D_DECLARE_INTERFACE(DirInterface);

	public:
		/**
		 * @brief 开始枚举搜索指定目录下的文件.
		 * @param [in] strPath : 指定目录文件格式字符串
		 * @return 调用成功返回true，失败返回false.  
		 */
		virtual bool findFile(const TString &strPath) = 0;

		/**
		 * @brief 搜索下一个文件.
		 * @return 枚举没结束返回true，否则返回false表示枚举结束.  
		 */
		virtual bool findNextFile() = 0;

		/**
		 * @brief 结束当前枚举搜索.
		 * @return void  
		 */
		virtual void close() = 0;

		/**
		 * @brief 获取枚举目录的完整路径.
		 * @return 返回枚举目录的路径，返回类型std::string.
		 */
		virtual TString getRoot() const = 0;

		/**
		 * @brief 获取当前枚举到的完整文件名.
		 * @return 返回当前枚举到的完整文件名，类型std::string.
		 */
		virtual TString getFileName() const = 0;

		/**
		 * @brief 获取当前枚举到的文件的全路径名，包括完整文件名.
		 * @return 返回当前枚举到的文件的全路径名，类型std::string.
		 */
		virtual TString getFilePath() const = 0;

		/**
		 * @brief 获取当前枚举到的文件文件名，不包括扩展名.
		 * @return 返回当前枚举到的文件文件名，类型std::string.  
		 */
		virtual TString getFileTitle() const = 0;

		/**
		 * @brief 获取当前枚举到的文件大小.
		 * @return 返回当前枚举到的文件大小.  
		 */
		virtual uint32_t getLength() const = 0;

		/**
		 * @brief 判断当前枚举到的文件是否点目录，包括'.'和'..' .
		 * @return 点目录返回true，否则返回false.  
		 */
		virtual bool isDots() const = 0;

		/**
		 * @brief 判断当前枚举到的文件是否文件夹.
		 * @return 文件返回true，否则返回false.  
		 */
		virtual bool isDirectory() const = 0;

		/**
		 * @brief 获取当前枚举到的文件的创建时间戳.
		 * @return 返回文件创建时间戳，该时间是从1970年1月1日到当前的秒数
		 */
		virtual long_t getCreationTime() const = 0;

		/**
		 * @brief 获取当前枚举到的文件的最后访问时间戳.
		 * @return 返回文件最后访问时间戳，该时间是从1970年1月1日到当前的秒数
		 */
		virtual long_t getLastAccessTime() const = 0;

		/**
		 * @brief 获取当前枚举到的文件的最后修改时间戳.
		 * @return 返回文件最后修改时间戳，该时间是从1970年1月1日到当前的秒数  
		 */
		virtual long_t getLastWriteTime() const = 0;

		/**
		 * @brief 创建文件夹.
		 * @note 该接口不能创建多级文件夹.
		 * @param [in] strDir : 文件夹路径名称
		 * @return 调用成功返回true，否则返回false.  
		 */
		virtual bool makeDir(const TString &strDir) = 0;
        
		/**
		 * @brief 删除文件夹. 
		 * @note 该接口不能删除多级文件夹.
		 * @param [in] strDir : 文件夹路径名称
		 * @return 调用成功返回true，否则返回false.   
		 */
		virtual bool removeDir(const TString &strDir) = 0;

		/**
		 * @brief 删除文件.
		 * @param [in] strFileName : 需要删除文件的名称.
		 * @return 调用成功返回true，否则返回false.  
		 */
		virtual bool remove(const TString &strFileName) = 0;

		/**
		 * @brief 判断路径对应的文件是否存在.
		 * @param [in] strPath : 完整路径名
		 * @return 文件存在返回true，否则返回false.  
		 */
        virtual bool exists(const TString &strPath) const = 0;

		/**
		 * @brief 获取应用程序缓存数据存储路径，不同平台指定对应的路径，以对上层透明处理.
		 * @return 返回应用程序缓存数据存储路径.  
		 */
		virtual TString getCachePath() const = 0;

		/**
		 * @brief 获取应用程序路径
		 * @return 返回应用程序路径
		 */
		virtual TString getAppPath() const = 0;

		virtual char getNativeSeparator() const = 0;
	};
}


#endif	/*__T3D_DIR_INTERFACE_H__*/
