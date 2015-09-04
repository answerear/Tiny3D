

#include "T3DDeviceInfo_Windows.h"
#include "T3DFactoryInterface.h"


namespace Tiny3D
{
	DeviceInfo_Windows::DeviceInfo_Windows()
	{

	}

	DeviceInfo_Windows::~DeviceInfo_Windows()
	{

	}

	uint32_t DeviceInfo_Windows::getPlatform() const
	{
		return E_PLATFORM_WIN32;
	}

	TString DeviceInfo_Windows::getSoftwareVersion() const
	{
		return "3.0.0.0";
	}

	TString DeviceInfo_Windows::getOSVersion() const
	{
		return "Windows 7";
	}

	TString DeviceInfo_Windows::getDeviceVersion() const
	{
		return "PC";
	}

	int32_t DeviceInfo_Windows::getScreenWidth() const
	{
		return 0;
	}

	int32_t DeviceInfo_Windows::getScreenHeight() const
	{
		return 0;
	}

	float DeviceInfo_Windows::getScreenDPI() const
	{
		return 0.0f;
	}

	TString DeviceInfo_Windows::getMacAddress() const
	{
		return "12-34-56-78-9A-BC";
	}

	TString DeviceInfo_Windows::getCPUType() const
	{
		return "Intel Core i5";
	}

	int32_t DeviceInfo_Windows::getNumberOfProcessors() const
	{
		return 1;
	}

	uint32_t DeviceInfo_Windows::getMemoryCapacity() const
	{
		return 0x80000000;
	}

	TString DeviceInfo_Windows::getDeviceID() const
	{
		return "12-34-56-78-9A-BC";
	}
}
