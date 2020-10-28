//===----------------------------------------------------------------------===//
//====  Copyright (c) 2020 Advanced Micro Devices, Inc.  All rights reserved.
//
//               Developed by: Advanced Micro Devices, Inc.

#pragma once
#include <windows.h>
#include <Shlobj.h>
#include <iostream>
#include <VersionHelpers.h>
#include "ICPUEx.h"
#include "IPlatform.h"
#include "IDeviceManager.h"
#include "IBIOSEx.h"
#include <intrin.h>

#include "Utility.hpp"
#define MAX_LENGTH 50

wchar_t eString[] = { L"" };

IPlatform* rPlatform_;
IDeviceManager* rDeviceManager_;
ICPUEx* obj_;
IBIOSEx* objB_;

enum CPU_PackageType
{
	cptFP5 = 0,
	cptAM,
	cptAM4,
	cptSP3r2 = 7,
	cptUnknown = 0xF
};

struct CPUShort
{
	int Init{ 0 };
	float  fPPTValue{ -1 };
	float  fEDCValue_VDD{ -1 };
	float  fTDCValue_VDD{ -1 };
	double dTemperature{ -1 };
	double dPeakCoreVoltage{ -1 };
	double dAvgCoreVoltage{ -1 };
};

struct CPULong
{
	int Init{ 0 };
	int uCoreCount{ -1 };
	int uCorePark{ -1 };
	int L1DataCache{ -1 };
	int L1InstructionCache{ -1 };
	int L3Cache{ -1 };
	float  fPPTLimit{ -1 };
	float  fPPTValue{ -1 };
	float  fEDCLimit_VDD{ -1 };
	float  fEDCValue_VDD{ -1 };
	float  fTDCLimit_VDD{ -1 };
	float  fTDCValue_VDD{ -1 };
	float  fEDCLimit_SOC{ -1 };
	float  fEDCValue_SOC{ -1 };
	float  fTDCLimit_SOC{ -1 };
	float  fTDCValue_SOC{ -1 };
	float  fcHTCLimit{ -1 };
	float  fFCLKP0Freq{ -1 };
	float  fVDDCR_VDD_Power{ -1 };
	float  fVDDCR_SOC_Power{ -1 };
	float  fCCLK_Fmax{ -1 };
	int eMode{ -1 };
	double dPeakSpeed{ -1 };
	double dPeakCoreVoltage{ -1 };
	double dAvgCoreVoltage{ -1 };
	double dSocVoltage{ -1 };
	double dTemperature{ -1 };
	int uVDDIO{ -1 };
	int uMemClock{ -1 };
	int uTcl{ -1 };
	int uTcdrd{ -1 };
	int uTras{ -1 };
	int uTrp{ -1 };

};

struct SystemInfo
{
	int Privileges{ 0 };
	int SupportedOS{ 0 };
	int AuthenticAMD{ 0 };
	int DriverService{ 0 };
	int SupportedProc{ 0 };

};


BOOL IsSupportedProcessor(VOID)
{
	bool retBool = false;
	int CPUInfo[4] = { -1 };
	__cpuid(CPUInfo, 0x80000001);
	unsigned long uCPUID = CPUInfo[0];
	CPU_PackageType pkgType = (CPU_PackageType)(CPUInfo[1] >> 28);
	switch (pkgType)
	{
	case cptFP5:
		switch (uCPUID)
		{
		case 0x00810F80:
		case 0x00810F81:
		case 0x00860F00:
		case 0x00860F01:
			retBool = true;
			break;
		default:
			break;
		}
	case cptAM4:
		switch (uCPUID)
		{
		case 0x00800F00:
		case 0x00800F10:
		case 0x00800F11:
		case 0x00800F12:

		case 0x00810F10:
		case 0x00810F11:

		case 0x00800F82:
		case 0x00800F83:

		case 0x00870F00:
		case 0x00870F10:

		case 0x00810F80:
		case 0x00810F81:

		case 0x00860F00:
		case 0x00860F01:
			retBool = true;
			break;
		default:
			break;
		}
		break;
	case cptSP3r2:
		switch (uCPUID)
		{
		case 0x00800F10:
		case 0x00800F11:
		case 0x00800F12:

		case 0x00800F82:
		case 0x00800F83:
		case 0x00830F00:
		case 0x00830F10:
			retBool = true;
			break;
		default:
			break;
		}
		break;
	}
	return retBool;
}


extern "C"
{
	__declspec(dllexport) SystemInfo __stdcall IsSupported(void)
	{
		SystemInfo SIRes;

		if (IsUserAnAdmin())
		{
			SIRes.Privileges = 1;
		}
		//Check for OS support
		if (IsSupportedOS())
		{
			SIRes.SupportedOS = 1;
		}
		//Check if we have AMD processor
		if (Authentic_AMD())
			SIRes.AuthenticAMD = 1;

		//Check if driver is installed or not
		if (QueryDrvService() < 0)
		{
			if (false == InstallDriver())
				SIRes.DriverService = 0;
		}
		else {
			SIRes.DriverService = 1;
		}

		//Check if application is running on supported Processor?
		if (IsSupportedProcessor())
		{
			SIRes.SupportedProc = 1;
		}
		return SIRes;

	}

}
/*
void DllAttach(void)
{
	bool bRetCode2 = false;

	IPlatform& rPlatform2 = GetPlatform();
	bRetCode2 = rPlatform2.Init();

}

*/

extern "C" BOOL WINAPI DllMain(
	HINSTANCE const instance,  // handle to DLL module
	DWORD     const reason,    // reason for calling function
	LPVOID    const reserved)  // reserved
{
	// Perform actions based on the reason for calling.

	switch (reason)
	{

	case DLL_PROCESS_ATTACH:
		// Initialize once for each new process.
		// Return FALSE to fail DLL load.
		break;

	case DLL_THREAD_ATTACH:
		// Do thread-specific initialization.
		break;

	case DLL_THREAD_DETACH:
		// Do thread-specific cleanup.
		break;

	case DLL_PROCESS_DETACH:
		// Perform any necessary cleanup.
		//PlatformUninit();
		break;
	}
	return TRUE;  // Successful DLL_PROCESS_ATTACH.
}


void shortGrab(ICPUEx* &obj, CPUShort &QueryRes) {

	CPUParameters stData;
	int iRet = 0;

	iRet = obj->GetCPUParameters(stData);

	if (!iRet)
	{
		QueryRes.fPPTValue = stData.fPPTValue;
		QueryRes.fEDCValue_VDD = stData.fEDCValue_VDD;
		QueryRes.fTDCValue_VDD = stData.fTDCValue_VDD;
		QueryRes.dTemperature = stData.dTemperature;
		QueryRes.dPeakCoreVoltage = stData.dPeakCoreVoltage;
		QueryRes.dAvgCoreVoltage = stData.dAvgCoreVoltage;
	}

}

void longGrab(ICPUEx* &obj, IBIOSEx* &objB, CPULong& QueryRes) {

	CACHE_INFO result;
	unsigned int uCorePark = 0;
	unsigned int uCoreCount = 0;
	CPUParameters stData;
	int iRet = 0;
	unsigned short uVDDIO = 0, uMemClock = 0;
	unsigned char uTcl = 0, uTcdrd = 0, uTras = 0, uTrp = 0;

	
	iRet = objB->GetMemVDDIO(uVDDIO);
	if (!iRet)
	{
		QueryRes.uVDDIO = uVDDIO;
	}

	iRet = objB->GetCurrentMemClock(uMemClock);
	if (!iRet)
	{
		QueryRes.uMemClock = uMemClock;
	}

	iRet = objB->GetMemCtrlTcl(uTcl);
	if (!iRet)
	{
		QueryRes.uTcl = uTcl;
	}

	iRet = objB->GetMemCtrlTrcdrd(uTcdrd);
	if (!iRet)
	{
		QueryRes.uTcdrd = uTcdrd;
	}

	iRet = objB->GetMemCtrlTras(uTras);
	if (!iRet)
	{
		QueryRes.uTras = uTras;
	}

	iRet = objB->GetMemCtrlTrp(uTrp);
	if (!iRet)
	{
		QueryRes.uTrp = uTrp;
	}


	iRet = obj->GetCPUParameters(stData);

	if (!iRet)
	{
		QueryRes.fPPTValue = stData.fPPTValue;
		QueryRes.fPPTLimit = stData.fPPTLimit;
		QueryRes.fEDCLimit_VDD = stData.fEDCLimit_VDD;
		QueryRes.fEDCValue_VDD = stData.fEDCValue_VDD;
		QueryRes.fTDCLimit_VDD = stData.fTDCLimit_VDD;
		QueryRes.fTDCValue_VDD = stData.fTDCValue_VDD;
		QueryRes.fEDCLimit_SOC = stData.fEDCLimit_SOC;
		QueryRes.fEDCValue_SOC = stData.fEDCValue_SOC;
		QueryRes.fTDCLimit_SOC = stData.fTDCLimit_SOC;
		QueryRes.fTDCValue_SOC = stData.fTDCValue_SOC;
		QueryRes.fcHTCLimit = stData.fcHTCLimit;
		QueryRes.fFCLKP0Freq = stData.fFCLKP0Freq;
		QueryRes.fVDDCR_VDD_Power = stData.fVDDCR_VDD_Power;
		QueryRes.fVDDCR_SOC_Power = stData.fVDDCR_SOC_Power;
		QueryRes.fCCLK_Fmax = stData.fCCLK_Fmax;

		if (stData.eMode.uManual)
		{
			QueryRes.eMode = 0;
		}
		else if (stData.eMode.uPBOMode)
		{
			QueryRes.eMode = 1;
		}
		else if (stData.eMode.uAutoOverclocking)
		{
			QueryRes.eMode = 2;
		}
		else
		{
			QueryRes.eMode = 3;
		}

		QueryRes.dPeakSpeed = stData.dPeakSpeed;
		QueryRes.dPeakCoreVoltage = stData.dPeakCoreVoltage;
		QueryRes.dAvgCoreVoltage = stData.dAvgCoreVoltage;
		QueryRes.dSocVoltage = stData.dSocVoltage;
		QueryRes.dTemperature = stData.dTemperature;

	}
	
    iRet = obj->GetCoreCount(uCoreCount);
	if (!iRet)
	{
		QueryRes.uCoreCount = uCoreCount;
	}


	iRet = obj->GetCorePark(uCorePark);
	if (!iRet)
	{
		QueryRes.uCorePark = uCorePark;
	}

	iRet = obj->GetL1DataCache(result);
	if (!iRet)
	{
		QueryRes.L1DataCache = int(result.fSize);
	}

	result = { NULL };
	iRet = obj->GetL1InstructionCache(result);
	if (!iRet)
	{
		QueryRes.L1InstructionCache = int(result.fSize);
	}
	
	result = { NULL };
	iRet = obj->GetL3Cache(result);
	if (!iRet)
	{
		QueryRes.L3Cache = int(result.fSize);
	}
	
}


extern "C"
{
	__declspec(dllexport) CPULong __stdcall LongQuery(void)
	{

		CPULong QueryRes;

		ICPUEx* obj = obj_;
		IBIOSEx* objB = objB_;

		if (obj && objB) {

			QueryRes.Init = 1;

			//shortGrab(obj, QueryRes2);

			longGrab(obj, objB, QueryRes);

			obj_ = obj;
			objB_ = objB;

		} else {

			QueryRes.Init = 0;

		}
		return QueryRes;

	}
}


extern "C"
{
	__declspec(dllexport) CPUShort __stdcall ShortQuery(void)
	{

		CPUShort QueryRes;

		ICPUEx* obj = obj_;

		if (obj) {

			QueryRes.Init = 1;

			shortGrab(obj, QueryRes);

			obj_ = obj;

		} else {

			QueryRes.Init = 0;

		}
		return QueryRes;

	}
}


extern "C"
{
	__declspec(dllexport) bool __stdcall PlatformInit(void)
	{

		bool bRetCode = false;

		IPlatform& rPlatform = GetPlatform();
		bRetCode = rPlatform.Init();

		if (bRetCode)
		{
			IDeviceManager& rDeviceManager = rPlatform.GetIDeviceManager();

			ICPUEx* obj = (ICPUEx*)rDeviceManager.GetDevice(dtCPU, 0);
			IBIOSEx* objB = (IBIOSEx*)rDeviceManager.GetDevice(dtBIOS, 0);

			rDeviceManager_ = &rDeviceManager;

			obj_ = obj;
			objB_ = objB;

		}

		rPlatform_ = &rPlatform;

		return bRetCode;

	}
}

extern "C"
{
	__declspec(dllexport) bool __stdcall PlatformUninit(void)
	{

		bool bRetCode = false;

		IPlatform& rPlatform = *rPlatform_;
		bRetCode = rPlatform.UnInit();

		return bRetCode;

	}
}

ULONGLONG SubtractTimes(const FILETIME one, const FILETIME two)
{
	LARGE_INTEGER a, b;
	a.LowPart = one.dwLowDateTime;
	a.HighPart = one.dwHighDateTime;

	b.LowPart = two.dwLowDateTime;
	b.HighPart = two.dwHighDateTime;

	return a.QuadPart - b.QuadPart;
}


extern "C"
{
	__declspec(dllexport) double __stdcall CPUUsage(void)
	{
		FILETIME prevSysIdle, prevSysKernel, prevSysUser;
		FILETIME sysIdle, sysKernel, sysUser;

		if (GetSystemTimes(&sysIdle, &sysKernel, &sysUser) == 0)
			return 0;

		prevSysIdle = sysIdle;
		prevSysKernel = sysKernel;
		prevSysUser = sysUser;


		Sleep(100);

		if (GetSystemTimes(&sysIdle, &sysKernel, &sysUser) == 0)
			return 0;


		if (prevSysIdle.dwLowDateTime != 0 && prevSysIdle.dwHighDateTime != 0)
		{
			ULONGLONG sysIdleDiff, sysKernelDiff, sysUserDiff;
			sysIdleDiff = SubtractTimes(sysIdle, prevSysIdle);
			sysKernelDiff = SubtractTimes(sysKernel, prevSysKernel);
			sysUserDiff = SubtractTimes(sysUser, prevSysUser);

			ULONGLONG sysTotal = sysKernelDiff + sysUserDiff;
			ULONGLONG kernelTotal = sysKernelDiff - sysIdleDiff;

			if (sysTotal > 0)
				return (double)(((kernelTotal + sysUserDiff) * 100.0) / sysTotal);
		}
		return 0;

	}
}
