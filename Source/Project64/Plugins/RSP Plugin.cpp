/****************************************************************************
*                                                                           *
* Project 64 - A Nintendo 64 emulator.                                      *
* http://www.pj64-emu.com/                                                  *
* Copyright (C) 2012 Project64. All rights reserved.                        *
*                                                                           *
* License:                                                                  *
* GNU/GPLv2 http://www.gnu.org/licenses/gpl-2.0.html                        *
*                                                                           *
****************************************************************************/
#include "stdafx.h"

void DummyFunc1 ( BOOL /*a*/) {}

CRSP_Plugin::CRSP_Plugin (const char * FileName) :
	CPlugin(),
	InitiateRSP(NULL),
	DoRspCycles(NULL),
	EnableDebugging(NULL),
	GetDebugInfo(NULL),
	InitiateDebugger(NULL),
	m_CycleCount(0)
{
	memset(&m_RSPDebug, 0, sizeof(m_RSPDebug));
	Init(FileName);
}

bool CRSP_Plugin::Init(const char * FileName)
{
	if (!CPlugin::Init(FileName))
	{
		UnloadPlugin();
		return false;
	}

	// Find entries for functions in DLL
	LoadFunction(InitiateRSP);
	LoadFunction(DoRspCycles);
	_LoadFunction("GetRspDebugInfo", GetDebugInfo);
	_LoadFunction("InitiateRSPDebugger", InitiateDebugger);
	LoadFunction(EnableDebugging);
	if (EnableDebugging == NULL) { EnableDebugging = DummyFunc1; }

	//Make sure dll had all needed functions
	if (DoRspCycles == NULL
		|| InitiateRSP == NULL
		|| (m_PluginInfo.Version >= 0x0102
			&& PluginLoaded == NULL))
	{
		UnloadPlugin(); 
		return false; 
	}

	if (PluginLoaded != NULL)
		PluginLoaded();

	// Get debug info if able
	if (GetDebugInfo != NULL)
		GetDebugInfo(&m_RSPDebug);

	return true;
}

bool CRSP_Plugin::Initiate ( CPlugins * Plugins, CN64System * System ) 
{
	if (m_PluginInfo.Version == 1 
		|| m_PluginInfo.Version == 0x100
		|| InitiateRSP == NULL)
		return false;

	RSP_INFO_1_1 Info = {0};

	Info.hInst             = GetModuleHandle(NULL);
	Info.CheckInterrupts   = DummyCheckInterrupts;
	Info.MemoryBswaped     = (System == NULL); // only true when the system's not yet loaded

	// We are initializing the plugin before any rom is loaded so we do not have any correct
	// parameters here.. just needed to we can config the DLL.
	if (System == NULL) 
	{
		BYTE Buffer[100];
		DWORD Value = 0;

		Info.ProcessDlist      = DummyCheckInterrupts;
		Info.ProcessRdpList    = DummyCheckInterrupts;
		Info.ShowCFB           = DummyCheckInterrupts;
		Info.ProcessAlist      = DummyCheckInterrupts;

		Info.RDRAM             = Buffer;
		Info.DMEM              = Buffer;
		Info.IMEM              = Buffer;
		
		Info.MI__INTR_REG      = &Value;
			
		Info.SP__MEM_ADDR_REG  = &Value;
		Info.SP__DRAM_ADDR_REG = &Value;
		Info.SP__RD_LEN_REG    = &Value;
		Info.SP__WR_LEN_REG    = &Value;
		Info.SP__STATUS_REG    = &Value;
		Info.SP__DMA_FULL_REG  = &Value;
		Info.SP__DMA_BUSY_REG  = &Value;
		Info.SP__PC_REG        = &Value;
		Info.SP__SEMAPHORE_REG = &Value;
			
		Info.DPC__START_REG    = &Value;
		Info.DPC__END_REG      = &Value;
		Info.DPC__CURRENT_REG  = &Value;
		Info.DPC__STATUS_REG   = &Value;
		Info.DPC__CLOCK_REG    = &Value;
		Info.DPC__BUFBUSY_REG  = &Value;
		Info.DPC__PIPEBUSY_REG = &Value;
		Info.DPC__TMEM_REG     = &Value;
	}
	// Send initialization information to the DLL
	else
	{
		Info.ProcessDlist      = Plugins->Gfx()->ProcessDList;
		Info.ProcessRdpList    = Plugins->Gfx()->ProcessRDPList;
		Info.ShowCFB           = Plugins->Gfx()->ShowCFB;
		Info.ProcessAlist      = Plugins->Audio()->ProcessAList;

		Info.RDRAM             = g_MMU->Rdram();
		Info.DMEM              = g_MMU->Dmem();
		Info.IMEM              = g_MMU->Imem();

		Info.MI__INTR_REG      = &g_Reg->m_RspIntrReg;
		
		Info.SP__MEM_ADDR_REG  = &g_Reg->SP_MEM_ADDR_REG;
		Info.SP__DRAM_ADDR_REG = &g_Reg->SP_DRAM_ADDR_REG;
		Info.SP__RD_LEN_REG    = &g_Reg->SP_RD_LEN_REG;
		Info.SP__WR_LEN_REG    = &g_Reg->SP_WR_LEN_REG;
		Info.SP__STATUS_REG    = &g_Reg->SP_STATUS_REG;
		Info.SP__DMA_FULL_REG  = &g_Reg->SP_DMA_FULL_REG;
		Info.SP__DMA_BUSY_REG  = &g_Reg->SP_DMA_BUSY_REG;
		Info.SP__PC_REG        = &g_Reg->SP_PC_REG;
		Info.SP__SEMAPHORE_REG = &g_Reg->SP_SEMAPHORE_REG;
		
		Info.DPC__START_REG    = &g_Reg->DPC_START_REG;
		Info.DPC__END_REG      = &g_Reg->DPC_END_REG;
		Info.DPC__CURRENT_REG  = &g_Reg->DPC_CURRENT_REG;
		Info.DPC__STATUS_REG   = &g_Reg->DPC_STATUS_REG;
		Info.DPC__CLOCK_REG    = &g_Reg->DPC_CLOCK_REG;
		Info.DPC__BUFBUSY_REG  = &g_Reg->DPC_BUFBUSY_REG;
		Info.DPC__PIPEBUSY_REG = &g_Reg->DPC_PIPEBUSY_REG;
		Info.DPC__TMEM_REG     = &g_Reg->DPC_TMEM_REG;
	}

	// NOTE: Sleep(100) call removed after InitiateRSP() in the System == NULL condition.
	InitiateRSP(Info, &m_CycleCount);
	m_Initilized = true;

	//jabo had a bug so I call CreateThread so his dllmain gets called again
	DWORD ThreadID;
	HANDLE hthread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)DummyFunction,NULL,0, &ThreadID);	
	CloseHandle(hthread);
	return m_Initilized;
}

void CRSP_Plugin::UnloadPlugin()
{
	CPlugin::UnloadPlugin();

	memset(&m_RSPDebug, 0, sizeof(m_RSPDebug));
	DoRspCycles      = NULL;
	EnableDebugging  = NULL;
	GetDebugInfo     = NULL;
	InitiateDebugger = NULL;
}

void CRSP_Plugin::ProcessMenuItem (int id )
{
	if (m_RSPDebug.ProcessMenuItem)
	{
		m_RSPDebug.ProcessMenuItem(id); 
	}
}

CRSP_Plugin::~CRSP_Plugin()
{
	Close();
	UnloadPlugin();
}