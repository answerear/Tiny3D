

#ifndef __T3D_DEVICE_INFO_H__
#define __T3D_DEVICE_INFO_H__


#include "T3DSingleton.h"
#include "T3DPlatformPrerequisites.h"


namespace Tiny3D
{
    class DeviceInfoInterface;
    class FactoryInterface;

    class T3D_PLATFORM_API DeviceInfo : public Singleton<DeviceInfo>
    {
        T3D_DISABLE_COPY(DeviceInfo);

    public:
        static const uint32_t PLATFORM_UNKNOWN;
        static const uint32_t PLATFORM_WINDOWS;
        static const uint32_t PLATFORM_MACOSX;
        static const uint32_t PLATFORM_LINUX;
        static const uint32_t PLATFORM_IOS;
        static const uint32_t PLATFORM_ANDROID;

        static const char* Unknown;
        static const char* Windows;
        static const char* MacOSX;
        static const char* Linux;
        static const char* iOS;
        static const char* Android;

    public:
        DeviceInfo(FactoryInterface *pAdapterFactory);
        virtual ~DeviceInfo();

        /**
         * @brief 获取平台类型
         */
        uint32_t getPlatform() const;

        /**
         * @brief 获取平台类型字符串
         */
        String getPlatformString() const;

        /**
         * @brief 获取软件版本号字符串
         */
        String getSoftwareVersion() const;

        /**
         * @brief 获取操作系统版本号字符串
         */
        String getOSVersion() const;

        /**
         * @brief 获取设备机型版本信息字符串
         */
        String getDeviceVersion() const;

        /**
         * @brief 获取屏幕宽度.
         */
        int32_t getScreenWidth() const;

        /**
         * @brief 获取屏幕高度.
         */
        int32_t getScreenHeight() const;

        /**
         * @brief 获取屏幕像素密度.
         */
        float getScreenDPI() const;

        /**
         * @brief 获取设备mac地址.
         */
        String getMacAddress() const;

        /**
         * @brief 获取CPU类型信息.
         */
        String getCPUType() const;

        /**
         * @brief 获取CPU核数
         */
        int32_t getNumberOfProcessors() const;

        /**
         * @brief 获取内存信息.
         */
        uint32_t getMemoryCapacity() const;

        /**
         * @brief 获取设备ID.
         */
        String getDeviceID() const;

    protected:
        DeviceInfoInterface *m_pDevInfoAdapter;
    };

#define T3D_DEVICE_INFO     DeviceInfo::getInstance()
}


#endif  /*__T3D_DEVICE_INFO_H__*/
