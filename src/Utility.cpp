//===----------------------------------------------------------------------===//
//====  Copyright (c) 2020 Advanced Micro Devices, Inc.  All rights reserved.
//
//               Developed by: Advanced Micro Devices, Inc.

#include "Utility.hpp"
#include <intrin.h>

/**	@brief	report Error messages back to user
*	@param	userMsg  user specific to be shown on console
*	@param	exitCode   each exitCode can be defined to denote different message to user
*	@param	printErrorMesg  show last error message if this flag is set via GetLastError
*/


/** @brief  To check whether Vendor is AMD or not
*/
BOOL Authentic_AMD()
{
	char CPUString[0x20];
	int CPUInfo[4] = { -1 };
	unsigned nIds;
	char string[] = "AuthenticAMD";

	__cpuid(CPUInfo, 0);
	nIds = CPUInfo[0];
	memset(CPUString, 0, sizeof(CPUString));
	*((int*)CPUString) = CPUInfo[1];
	*((int*)(CPUString + 4)) = CPUInfo[3];
	*((int*)(CPUString + 8)) = CPUInfo[2];

	if (!strcmp(string, CPUString))
		return true;
	else
		return false;

}


/*
*	@brief	Check if driver is installed or not
*/
INT QueryDrvService()
{
	SERVICE_STATUS ServiceStatus;
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCM)
		return -1;

	SC_HANDLE hOpenService = OpenService(hSCM, RM_SER_NAME, SC_MANAGER_ALL_ACCESS);
	if (GetLastError() == ERROR_SERVICE_DOES_NOT_EXIST)
	{
		return   -1;
	}
	QueryServiceStatus(hOpenService, &ServiceStatus);
	if (ServiceStatus.dwCurrentState != SERVICE_RUNNING)
	{
		return -1;
	}

	CloseServiceHandle(hOpenService);
	CloseServiceHandle(hSCM);
	return 0;
}

bool IsSupportedOS()
{
	bool bIsSupported = false;
	DWORD major = 0;
	DWORD minor = 0;
	LPBYTE pinfoRawData;
	if (IsWindowsServer())
	{
		return false;
	}
	if (NERR_Success == NetWkstaGetInfo(NULL, 100, &pinfoRawData))
	{
		WKSTA_INFO_100 * pworkstationInfo = (WKSTA_INFO_100 *)pinfoRawData;
		major = pworkstationInfo->wki100_ver_major;
		minor = pworkstationInfo->wki100_ver_minor;
		::NetApiBufferFree(pinfoRawData);
	}

	if(major == 10)
	{
		bIsSupported = true;
	}
	
	return bIsSupported;
}

bool GetDriverPath(wchar_t* pDriverPath)
{
	wchar_t* pTemp;
	pTemp = _wgetenv(L"AMDRMMONITORSDKPATH");
	LOG_PROCESS_ERROR(pTemp);
	wsprintf(pDriverPath, L"%s%s", pTemp, DRIVER_FILE_PATH_64);
	return true;
Exit0:
	return false;
}

bool InstallDriver(void)
{
	bool bRetCode = false;
	bool bResult = false;
	DWORD dwLastError;
	SC_HANDLE hSCManager = NULL;
	SC_HANDLE hService = NULL;
	wchar_t szDriverPath[256];


	HANDLE m_hDriver = CreateFile(L"\\\\.\\" AOD_DRIVER_NAME,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (m_hDriver == INVALID_HANDLE_VALUE)
	{
		bRetCode = GetDriverPath(szDriverPath);
		LOG_PROCESS_ERROR(bRetCode);

		hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		LOG_PROCESS_ERROR(hSCManager);

		// Install the driver
		hService = CreateService(hSCManager,
			AOD_DRIVER_NAME, AOD_DRIVER_NAME, SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER,
			SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, szDriverPath,
			NULL, NULL, NULL, NULL, NULL);
		if (hService == NULL)
		{
			dwLastError = GetLastError();
			if (dwLastError == ERROR_SERVICE_EXISTS)
				hService = OpenService(hSCManager, AOD_DRIVER_NAME, SERVICE_ALL_ACCESS);
			else if (dwLastError == ERROR_SERVICE_MARKED_FOR_DELETE)
			{
				hService = OpenService(hSCManager, AOD_DRIVER_NAME, SERVICE_ALL_ACCESS);
				SERVICE_STATUS ServiceStatus;
				ControlService(hService, SERVICE_CONTROL_STOP, &ServiceStatus);

				hService = CreateService(hSCManager,
					AOD_DRIVER_NAME, AOD_DRIVER_NAME, SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER,
					SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, szDriverPath,
					NULL, NULL, NULL, NULL, NULL);
			}
		}
		LOG_PROCESS_ERROR(hService);

		// Start the driver
		BOOL bRet = StartService(hService, 0, NULL);
		if (!bRet)
		{
			dwLastError = GetLastError();
			if (dwLastError == ERROR_PATH_NOT_FOUND)
			{
				bRet = DeleteService(hService);
				LOG_PROCESS_ERROR(bRet);

				CloseServiceHandle(hService);

				hService = CreateService(hSCManager,
					AOD_DRIVER_NAME, AOD_DRIVER_NAME, SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER,
					SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, szDriverPath,
					NULL, NULL, NULL, NULL, NULL);
				LOG_PROCESS_ERROR(hService);

				bRet = StartService(hService, 0, NULL);
				LOG_PROCESS_ERROR(bRet);
			}

			if (dwLastError != ERROR_SERVICE_ALREADY_RUNNING)
			{
				LOG_PROCESS_ERROR(bRet);
			}
		}

		// Try to create the file again
		m_hDriver = CreateFile(L"\\\\.\\" AOD_DRIVER_NAME,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		LOG_PROCESS_ERROR(m_hDriver != INVALID_HANDLE_VALUE);
	}

	bResult = true;

Exit0:

	if (hSCManager)
		CloseServiceHandle(hSCManager);
	if (hService)
		CloseServiceHandle(hService);


	return bResult;
}