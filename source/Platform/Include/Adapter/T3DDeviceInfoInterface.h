

#ifndef __T3D_DEVICE_INFO_INTERFACE_H__
#define __T3D_DEVICE_INFO_INTERFACE_H__


#include "T3DType.h"
#include "T3DMacro.h"


namespace Tiny3D
{
	class DeviceInfoInterface
	{
		T3D_DECLARE_INTERFACE(DeviceInfoInterface);

	public:
		/**
		 * @brief ��ȡƽ̨����
		 */
		virtual uint32_t getPlatform() const = 0;
		
		/**
		 * @brief ��ȡ�����汾���ַ���
		 */
		virtual TString getSoftwareVersion() const = 0;

		/**
		 * @brief ��ȡ����ϵͳ�汾���ַ���
		 */
		virtual TString getOSVersion() const = 0;

		/**
		 * @brief ��ȡ�豸���Ͱ汾��Ϣ�ַ���
		 */
		virtual TString getDeviceVersion() const = 0;

		/**
		 * @brief ��ȡ��Ļ����.  
		 */
		virtual int32_t getScreenWidth() const = 0;

		/**
		 * @brief ��ȡ��Ļ�߶�.  
		 */
		virtual int32_t getScreenHeight() const = 0;

		/**
		 * @brief ��ȡ��Ļ�����ܶ�.
		 */
		virtual float getScreenDPI() const = 0;

		/**
		 * @brief ��ȡ�豸mac��ַ.  
		 */
		virtual TString getMacAddress() const = 0;

		/**
		 * @brief ��ȡCPU������Ϣ.  
		 */
		virtual TString getCPUType() const = 0;

		/**
		 * @brief ��ȡCPU����.
		 */
		virtual int32_t getNumberOfProcessors() const = 0;

		/**
		 * @brief ��ȡ�ڴ���Ϣ.  
		 */
		virtual uint32_t getMemoryCapacity() const = 0;

		/**
		 * @brief ��ȡ�豸ID.  
		 */
		virtual TString getDeviceID() const = 0;
	};
}


#endif	/*__T3D_DEVICE_INFO_INTERFACE_H__*/