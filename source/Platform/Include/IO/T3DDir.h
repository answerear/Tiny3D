

#ifndef __T3D_DIR_H__
#define __T3D_DIR_H__


#include "T3DType.h"
#include "T3DMacro.h"
#include "T3DPlatformMacro.h"


namespace Tiny3D
{
    class DirInterface;

    class T3D_PLATFORM_API Dir
    {
        T3D_DISABLE_COPY(Dir);

    public:
        static char NATIVE_SEPARATOR;

    public:
        /**
         * @brief Constructor for T3DDir.
         */
        Dir();

        /**
         * @brief Destructor for T3DDir.
         */
        ~Dir();

        /**
         * @brief 开始枚举搜索指定目录下的文件.
         * @param [in] strPath : 指定目录文件格式字符串
         * @return 调用成功返回true，失败返回false.
         */
        bool findFile(const TString &strPath);

        /**
         * @brief 搜索下一个文件.
         * @return 枚举没结束返回true，否则返回false表示枚举结束.
         */
        bool findNextFile();

        /**
         * @brief 结束当前枚举搜索.
         * @return void
         */
        void close();

        /**
         * @brief 获取枚举目录的完整路径.
         * @return 返回枚举目录的路径，返回类型std::string.
         */
        TString getRoot() const;

        /**
         * @brief 获取当前枚举到的完整文件名.
         * @return 返回当前枚举到的完整文件名，类型std::string.
         */
        TString getFileName() const;

        /**
         * @brief 获取当前枚举到的文件的全路径名，包括完整文件名.
         * @return 返回当前枚举到的文件的全路径名，类型std::string.
         */
        TString getFilePath() const;

        /**
         * @brief 获取当前枚举到的文件文件名，不包括扩展名.
         * @return 返回当前枚举到的文件文件名，类型std::string.
         */
        TString getFileTitle() const;

        /**
         * @brief 获取当前枚举到的文件大小.
         * @return 返回当前枚举到的文件大小.
         */
        uint32_t getFileSize() const;

        /**
         * @brief 判断当前枚举到的文件是否点目录，包括'.'和'..' .
         * @return 点目录返回true，否则返回false.
         */
        bool isDots() const;

        /**
         * @brief 判断当前枚举到的文件是否文件夹.
         * @return 文件返回true，否则返回false.
         */
        bool isDirectory() const;

        /**
         * @brief 获取当前枚举到的文件的创建时间戳.
         * @return 返回文件创建时间戳，该时间是从1970年1月1日到当前的秒数
         */
        long_t getCreationTime() const;

        /**
         * @brief 获取当前枚举到的文件的最后访问时间戳.
         * @return 返回文件最后访问时间戳，该时间是从1970年1月1日到当前的秒数
         */
        long_t getLastAccessTime() const;

        /**
         * @brief 获取当前枚举到的文件的最后修改时间戳.
         * @return 返回文件最后修改时间戳，该时间是从1970年1月1日到当前的秒数
         */
        long_t getLastWriteTime() const;

        /**
         * @brief 创建文件夹.
         * @note 该接口不能创建多级文件夹.
         * @param [in] strDir : 文件夹路径名称
         * @return 调用成功返回true，否则返回false.
         */
        static bool makeDir(const TString &strDir);

        /**
         * @brief 删除文件夹.
         * @note 该接口不能删除多级文件夹.
         * @param [in] strDir : 文件夹路径名称
         * @return 调用成功返回true，否则返回false.
         */
        static bool removeDir(const TString &strDir);

        /**
         * @brief 删除文件.
         * @param [in] strFileName : 需要删除文件的名称.
         * @return 调用成功返回true，否则返回false.
         */
        static bool remove(const TString &strFileName);

        /**
         * @brief 判断路径对应的文件是否存在.
         * @param [in] strPath : 完整路径名
         * @return 文件存在返回true，否则返回false.
         */
        static bool exists(const TString &strPath);

        /**
         * @brief 获取应用程序缓存数据存储路径，不同平台指定对应的路径，以对上层透明处理.
         * @return 返回应用程序缓存数据存储路径.
         */
        static TString getCachePath();

        /**
         * @brief 获取应用程序路径.
         * @return 返回应用程序路径.
         */
        static TString getAppPath();

        /**
         * @brief 获取系统相关的路径分隔符
         */
        static char getNativeSeparator();

    protected:
        DirInterface    *m_pDirAdpater;

        static DirInterface *s_pDirAdapter;
    };
}


#endif  /*__T3D_DIR_H__*/
