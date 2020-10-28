//===----------------------------------------------------------------------===//
//====  Copyright (c) 2020 Advanced Micro Devices, Inc.  All rights reserved.
//
//               Developed by: Advanced Micro Devices, Inc.

#pragma once
#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <Lm.h>
#include <VersionHelpers.h>

#ifdef _MT
#include <process.h>
#endif
#define RM_SER_NAME _T("AMDRyzenMasterDriverV16")
#define AOD_DRIVER_NAME		L"AMDRyzenMasterDriverV16"
#define DRIVER_FILE_PATH_64	L"bin\\AMDRyzenMasterDriver.sys"

BOOL Authentic_AMD();
INT QueryDrvService();
bool IsSupportedOS();
bool InstallDriver(void);

#define LOG_PRINT(dRet, ch) (dRet==-1)?_tprintf(_T(" NA\n")) : _tprintf(_T(" %0.1f %s\n"), dRet, ch);

#define LOG_PROCESS_ERROR(__CONDITION__)		\
	do											\
	{											\
		if (!(__CONDITION__))					\
		{										\
			goto Exit0;							\
		}										\
	} while (false)