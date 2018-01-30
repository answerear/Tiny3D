/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#include "Adapter/Windows/T3DWin32DeviceInfo.h"
#include "Adapter/T3DFactoryInterface.h"
#include <windows.h>
#include <sstream>
#include <IPHlpApi.h>

#pragma comment(lib, "IPHLPAPI.lib")


namespace Tiny3D
{
    Win32DeviceInfo::Win32DeviceInfo()
        : mSWVersion()
        , mOSVersion()
        , mHWVersion()
        , mSystemInfo()
        , mCPUType()
        , mNumberOfProcessor(0)
    {

    }

    Win32DeviceInfo::~Win32DeviceInfo()
    {

    }

    uint32_t Win32DeviceInfo::getPlatform() const
    {
        return E_PLATFORM_WIN32;
    }

    const String &Win32DeviceInfo::getSoftwareVersion() const
    {
        return mSWVersion;
    }

    void Win32DeviceInfo::setSoftwareVersion(const char *version)
    {
        mSWVersion = version;
    }

    const String &Win32DeviceInfo::getOSVersion() const
    {
        if (mOSVersion.empty())
        {
            do 
            {
                PVOID oldWow64State = NULL;

                HKEY hKey;
                LONG lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                    "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
                    0,
                    KEY_ALL_ACCESS|KEY_WOW64_64KEY,
                    &hKey);
                if (lResult != ERROR_SUCCESS)
                    break;

                DWORD dwType;
                DWORD dwSize;
                char szName[64];

                // 获取操作系统名称
                dwSize = 64;
                lResult = RegQueryValueEx(hKey, "ProductName", NULL,
                    &dwType, (BYTE *)szName, &dwSize);
                if (lResult != ERROR_SUCCESS)
                {
                    RegCloseKey(hKey);
                    break;
                }

                // 获取CSDVersion
                dwSize = 64;
                char szCSDVersion[64];
                lResult = RegQueryValueEx(hKey, "CSDVersion", NULL,
                    &dwType, (BYTE *)szCSDVersion, &dwSize);
                if (lResult != ERROR_SUCCESS)
                {
                    RegCloseKey(hKey);
                    break;
                }

                // 获取CurrentVersion
                dwSize = 64;
                char szVersion[64];
                lResult = RegQueryValueEx(hKey, "CurrentVersion", NULL,
                    &dwType, (BYTE *)szVersion, &dwSize);
                if (lResult != ERROR_SUCCESS)
                {
                    RegCloseKey(hKey);
                    break;
                }

                // 获取CurrentBuildNumber
                dwSize = 64;
                char szBuild[64];
                lResult = RegQueryValueEx(hKey, "CurrentBuild", NULL,
                    &dwType, (BYTE *)szBuild, &dwSize);
                if (lResult != ERROR_SUCCESS)
                {
                    RegCloseKey(hKey);
                    break;
                }

                RegCloseKey(hKey);

                mOSVersion = String(szName) + " " + String(szCSDVersion) + " ("
                    + String(szVersion) + ") Build" + String(szBuild);
            } while (0);

            if (mOSVersion.empty())
            {
                mOSVersion = getOSInfo();
            }
        }

        return mOSVersion;
    }

    String Win32DeviceInfo::getOSInfo() const
    {
        SYSTEM_INFO info;
        ::GetSystemInfo(&info);
        OSVERSIONINFOEX os;
        os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
        String version;

        if (::GetVersionEx((OSVERSIONINFO *)&os))
        {
            // 下面根据版本信息判断操作系统名称
            if (os.dwMajorVersion == 10)
            {
                if (os.dwMinorVersion == 0)
                {
                    if (os.wProductType == VER_NT_WORKSTATION)
                    {
                        // Windows 10
                        version = "Windows 10";
                    }
                    else
                    {
                        // Windows Server 2016
                        version = "Windows Server 2016";
                    }
                }
            }
            else if (os.dwMajorVersion == 6)
            {
                if (os.dwMinorVersion == 3)
                {
                    if (os.wProductType == VER_NT_WORKSTATION)
                    {
                        // Windows 8.1
                        version = "Windows 8.1";
                    }
                    else
                    {
                        // Windows Server 2012 R2
                        version = "Windows Server 2012 R2";
                    }
                }
                else if (os.dwMinorVersion == 2)
                {
                    if (os.wProductType == VER_NT_WORKSTATION)
                    {
                        // Windows 8
                        version = "Windows 8";
                    }
                    else
                    {
                        // Windows Server 2012
                        version = "Windows Server 2012";
                    }
                }
                else if (os.dwMinorVersion == 1)
                {
                    if (os.wProductType == VER_NT_WORKSTATION)
                    {
                        // Windows 7
                        version = "Windows 7";
                    }
                    else
                    {
                        // Windows Server 2008 R2
                        version = "Windows Server 2008 R2";
                    }
                }
                else if (os.dwMinorVersion == 0)
                {
                    if (os.wProductType == VER_NT_WORKSTATION)
                    {
                        // Windows Vista
                        version = "Windows Vista";

                        if (os.wSuiteMask == VER_SUITE_PERSONAL)
                        {
                            // Home
                            version = version + " Home";
                        }
                    }
                    else
                    {
                        // Windows Server 2008
                        version = "Windows Server 2008";

                        if (os.wSuiteMask == VER_SUITE_DATACENTER)
                        {
                            // Datacenter Server
                            version = version + " Datacenter Server";
                        }
                        else if (os.wSuiteMask == VER_SUITE_ENTERPRISE)
                        {
                            // Enterprise
                            version = version + "Enterprise";
                        }
                    }
                }
            }
            else if (os.dwMajorVersion == 5)
            {
                if (os.dwMinorVersion == 2)
                {
                    if (GetSystemMetrics(SM_SERVERR2) != 0)
                    {
                        // Windows Server 2003 R2
                        version = "Windows Server 2003 R2";

                        if (os.wSuiteMask == VER_SUITE_STORAGE_SERVER)
                        {
                            // Storage Server
                            version = version + " Storage Server";
                        }
                    }
                    else if (os.wSuiteMask & VER_SUITE_WH_SERVER)
                    {
                        // Windows Home Server
                        version = "Windows Home Server";
                    }
                    else if (GetSystemMetrics(SM_SERVERR2) == 0)
                    {
                        // Windows Server 2003
                        version = "Windows Server 2003";

                        if (os.wSuiteMask == VER_SUITE_BLADE)
                        {
                            // Web Edition
                            version = version + " Web Edition";
                        }
                        else if (os.wSuiteMask == VER_SUITE_COMPUTE_SERVER)
                        {
                            // Compute Cluster Edition
                            version = version + " Compute Cluster Edition";
                        }
                        else if (os.wSuiteMask == VER_SUITE_STORAGE_SERVER)
                        {
                            // Storage Server
                            version = version + " Storage Server";
                        }
                        else if (os.wSuiteMask == VER_SUITE_DATACENTER)
                        {
                            // Datacenter Edition
                            version = version + " Datacenter Edition";
                        }
                        else if (os.wSuiteMask == VER_SUITE_ENTERPRISE)
                        {
                            // Enterprise Edition
                            version = version + " Enterprise Edition";
                        }
                    }
                    else if ((os.wProductType == VER_NT_WORKSTATION)
                        && (info.wProcessorArchitecture
                            == PROCESSOR_ARCHITECTURE_AMD64))
                    {
                        // Windows XP Professional x64 Edition
                        version = "Windows XP Professional x64 Edition";
                    }
                }
                else if (os.dwMinorVersion == 1)
                {
                    // Windows XP
                    version = "Windows XP";

                    if (os.wSuiteMask == VER_SUITE_EMBEDDEDNT)
                        version = version + " Embedded";
                    else if (os.wSuiteMask == VER_SUITE_PERSONAL)
                        version = version + " Home Edition";
                    else
                        version = version + " Professional";
                }
                else if (os.dwMinorVersion == 0)
                {
                    // Windows 2000
                    version = "Windows 2000";

                    if (os.wSuiteMask == VER_SUITE_ENTERPRISE)
                    {
                        // Windows 2000 Advanced Server
                        version = version + " Advanced Server";
                    }
                }
            }

            if (version.empty())
            {
                // 未知版本，直接输出版本号
                std::stringstream ss;
                ss << os.dwMajorVersion << "." << os.dwMinorVersion
                    << "." << os.dwBuildNumber;
                version = ss.str();
            }
            else
            {
                std::stringstream ss;
                ss << version << " Build " << os.dwBuildNumber;
                version = ss.str();
            }
        }

        return version;
    }

    const String &Win32DeviceInfo::getDeviceVersion() const
    {
        if (mHWVersion.empty())
        {
            OSVERSIONINFOEX os;
            os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
            String version;

            if (::GetVersionEx((OSVERSIONINFO *)&os))
            {
                std::stringstream ss;
                ss << os.dwMajorVersion << "." << os.dwMinorVersion 
                    << "." << os.dwBuildNumber;
                mHWVersion = ss.str();
            }
        }

        return mHWVersion;
    }

    const String &Win32DeviceInfo::getSystemInfo() const
    {
        if (mSystemInfo.empty())
        {
            SYSTEM_INFO info;
            ::GetSystemInfo(&info);
            std::stringstream ss;
            // OS Version
            ss << "Operating System : " << getOSVersion() << "\n";
            // CPU Architecture
            if (info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
            {
                ss << "CPU Architecture : x64 (AMD or Intel)\n";
            }
            else if (info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM)
            {
                ss << "CPU Architecture : ARM\n";
            }
            else if (info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
            {
                ss << "CPU Architecture : Intel Itanium\n";
            }
            else if (info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
            {
                ss << "CPU Architecture : Intel\n";
            }
            else
            {
                ss << "Unknown architecture\n";
            }
            // CPU Type
            ss << "CPU Type : " << getCPUType() << "\n";
            // CPU Level
            ss << "CPU Level : " << info.wProcessorLevel << "\n";
            // CPU Revision
            ss << "OS Revision : " << getDeviceVersion() << "\n";
            // Number of CPU Processor
            ss << "Number of CPU Processor : " << info.dwNumberOfProcessors << "\n";
            mSystemInfo = ss.str();
        }

        return mSystemInfo;
    }

    int32_t Win32DeviceInfo::getScreenWidth() const
    {
        return 0;
    }

    int32_t Win32DeviceInfo::getScreenHeight() const
    {
        return 0;
    }

    float Win32DeviceInfo::getScreenDPI() const
    {
        return 0.0f;
    }

    const String &Win32DeviceInfo::getMacAddress() const
    {
        if (mMacAddress.empty())
        {
            do 
            {
                PIP_ADAPTER_INFO pAdapterInfo;
                DWORD AdapterInfoSize;
                char szMac[32] = { 0 };
                DWORD Err;

                AdapterInfoSize = 0;
                Err = GetAdaptersInfo(NULL, &AdapterInfoSize);

                if ((Err != 0) && (Err != ERROR_BUFFER_OVERFLOW))
                {
                    break;
                }

                //   分配网卡信息内存  
                pAdapterInfo = (PIP_ADAPTER_INFO)GlobalAlloc(GPTR, AdapterInfoSize);
                if (pAdapterInfo == NULL)
                {
                    break;
                }

                if (GetAdaptersInfo(pAdapterInfo, &AdapterInfoSize) != 0)
                {
                    GlobalFree(pAdapterInfo);
                    break;
                }

                std::string strMac;
                for (PIP_ADAPTER_INFO pAdapter = pAdapterInfo; pAdapter != NULL; pAdapter = pAdapter->Next)
                {
                    // 确保是以太网 
                    if (pAdapter->Type != MIB_IF_TYPE_ETHERNET)
                        continue;

                    // 确保MAC地址的长度为 00-00-00-00-00-00 
                    if (pAdapter->AddressLength != 6)
                        continue;

                    char acMAC[128] = { 0 };

                    sprintf(acMAC, "%02X-%02X-%02X-%02X-%02X-%02X",
                        (int)(pAdapter->Address[0]),
                        (int)(pAdapter->Address[1]),
                        (int)(pAdapter->Address[2]),
                        (int)(pAdapter->Address[3]),
                        (int)(pAdapter->Address[4]),
                        (int)(pAdapter->Address[5]));

                    mMacAddress = acMAC;
                    break;
                }

                GlobalFree(pAdapterInfo);
            } while (0);
        }

        return mMacAddress;
    }

    const String &Win32DeviceInfo::getCPUType() const
    {
        if (mCPUType.empty())
        {
            do 
            {
                char strBuf[100];
                HKEY hKey;
                DWORD dwSize;
                LONG lReturn = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                    "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
                    0,
                    KEY_READ,
                    &hKey);

                if (lReturn != ERROR_SUCCESS)
                {
                    break;
                }

                lReturn = RegQueryValueEx(hKey, "ProcessorNameString",
                    NULL, NULL, (BYTE *)strBuf, &dwSize);

                if (lReturn != ERROR_SUCCESS)
                {
                    break;
                }

                RegCloseKey(hKey);

                mCPUType = strBuf;
            } while (0);

            if (mCPUType.empty())
            {
                SYSTEM_INFO info;
                ::GetSystemInfo(&info);

                if (info.dwProcessorType == PROCESSOR_INTEL_386)
                {
                    mCPUType = "Intel 386";
                }
                else if (info.dwProcessorType == PROCESSOR_INTEL_486)
                {
                    mCPUType = "Intel 486";
                }
                else if (info.dwProcessorType == PROCESSOR_INTEL_PENTIUM)
                {
                    mCPUType = "Intel Pentium";
                }
                else if (info.dwProcessorType == PROCESSOR_INTEL_IA64)
                {
                    mCPUType = "Intel Itanium";
                }
                else if (info.dwProcessorType == PROCESSOR_AMD_X8664)
                {
                    mCPUType = "AMD x86_64";
                }
                else
                {
                    std::stringstream ss;
                    ss << info.dwProcessorType;
                    mCPUType = ss.str();
                }
            }
        }

        return mCPUType;
    }

    int32_t Win32DeviceInfo::getNumberOfProcessors() const
    {
        if (mNumberOfProcessor == 0)
        {
            SYSTEM_INFO info;
            ::GetSystemInfo(&info);
            mNumberOfProcessor = info.dwNumberOfProcessors;
        }

        return mNumberOfProcessor;
    }

    uint32_t Win32DeviceInfo::getMemoryCapacity() const
    {
        MEMORYSTATUSEX statex;
        statex.dwLength = sizeof(statex);
        GlobalMemoryStatusEx(&statex);
        return statex.ullTotalPhys / (1024 * 1024);
    }

    const String &Win32DeviceInfo::getDeviceID() const
    {
        return getMacAddress();
    }
}
