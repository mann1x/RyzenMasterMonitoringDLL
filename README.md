# RyzenMasterMonitoringDLL
Ryzen Master SDK Monitoring DLL used in OCMaestro

# How to use it in C#

Except CPUUsage always use a Mutex or a syncPoint.
Don't query in parallel from different threads or when programming the SMU!
CPUUsage is a diff of CPU usage in a 100ms delta; don't call it more frequently than 125ms. 

If the following driver files are present in the same directory, the DLL will auto-install the RM driver:

AMDRyzenMasterDriver.inf
AMDRyzenMasterDriver.cat
AMDRyzenMasterDriver.sys

# Methods

RMSystemInfo QuerySystem = IsSupported();

bool Init = PlatformInit();

bool unInit = PlatformUninit();

RMQuickStats quick = ShortQuery();

RMFullStats full = LongQuery();

double CPUUsage = CPUUsage();

# DllImports:

		[DllImport("AMDRyzenMasterMonitoringDLL.dll")]
		private static extern RMSystemInfo IsSupported();

		[DllImport("AMDRyzenMasterMonitoringDLL.dll")]
		private static extern RMQuickStats ShortQuery();

		[DllImport("AMDRyzenMasterMonitoringDLL.dll")]
		private static extern RMFullStats LongQuery();

		[DllImport("AMDRyzenMasterMonitoringDLL.dll")]
		private static extern bool PlatformInit();

		[DllImport("AMDRyzenMasterMonitoringDLL.dll")]
		private static extern bool PlatformUninit();

		[DllImport("AMDRyzenMasterMonitoringDLL.dll")]
		private static extern double CPUUsage();


# Structs:

	public struct RMQuickStats
	{
		public int Init;
		public float fPPTValue;
		public float fEDCValue_VDD;
		public float fTDCValue_VDD;
		public double dTemperature;
		public double dPeakCoreVoltage;
		public double dAvgCoreVoltage;
	}
	public struct RMSystemInfo
	{
		public int Privileges;
		public int SupportedOS;
		public int DriverService;
		public int SupportedProc;
		public int AuthenticAMD;
	}
	public enum eMode
	{
		MANUAL = 0,
		PBO = 1,
		AUTOOC = 2,
		RESERVED = 3,
	}

	public struct RMFullStats
	{
		public int Init;
		public int uCoreCount;
		public int uCorePark;
		public int L1DataCache;
		public int L1InstructionCache;
		public int L3Cache;
		public float fPPTLimit;
		public float fPPTValue;
		public float fEDCLimit_VDD;
		public float fEDCValue_VDD;
		public float fTDCLimit_VDD;
		public float fTDCValue_VDD;
		public float fEDCLimit_SOC;
		public float fEDCValue_SOC;
		public float fTDCLimit_SOC;
		public float fTDCValue_SOC;
		public float fcHTCLimit;
		public float fFCLKP0Freq;
		public float fVDDCR_VDD_Power;
		public float fVDDCR_SOC_Power;
		public float fCCLK_Fmax;
		public int eMode;
		public double dPeakSpeed;
		public double dPeakCoreVoltage;
		public double dAvgCoreVoltage;
		public double dSocVoltage;
		public double dTemperature;
		public int uVDDIO;
		public int uMemClock;
		public int uTcl;
		public int uTcdrd;
		public int uTras;
		public int uTrp;
	}

