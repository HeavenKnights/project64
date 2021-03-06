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
#include "SettingsType-RomDatabase.h"

CIniFile * CSettingTypeRomDatabase::m_SettingsIniFile = NULL;
stdstr   * CSettingTypeRomDatabase::m_SectionIdent    = NULL;

CSettingTypeRomDatabase::CSettingTypeRomDatabase(LPCSTR Name, int DefaultValue, bool DeleteOnDefault ) :
	m_KeyName(Name),
	m_DefaultStr(""),
	m_DefaultValue(DefaultValue),
	m_DefaultSetting(Default_Constant),
	m_DeleteOnDefault(DeleteOnDefault)
{
}

CSettingTypeRomDatabase::CSettingTypeRomDatabase(LPCSTR Name, bool DefaultValue, bool DeleteOnDefault ) :
	m_KeyName(Name),
	m_DefaultStr(""),
	m_DefaultValue(DefaultValue),
	m_DefaultSetting(Default_Constant),
	m_DeleteOnDefault(DeleteOnDefault)
{
}

CSettingTypeRomDatabase::CSettingTypeRomDatabase(LPCSTR Name, LPCSTR DefaultValue, bool DeleteOnDefault ) :
	m_KeyName(Name),
	m_DefaultStr(DefaultValue),
	m_DefaultValue(0),
	m_DefaultSetting(Default_Constant),
	m_DeleteOnDefault(DeleteOnDefault)
{
}

CSettingTypeRomDatabase::CSettingTypeRomDatabase(LPCSTR Name, SettingID DefaultSetting, bool DeleteOnDefault ) :
	m_KeyName(Name),
	m_DefaultStr(""),
	m_DefaultValue(0),
	m_DefaultSetting(DefaultSetting),
	m_DeleteOnDefault(DeleteOnDefault)
{
}

CSettingTypeRomDatabase::~CSettingTypeRomDatabase()
{
}

void CSettingTypeRomDatabase::Initilize( void )
{
	m_SettingsIniFile = new CIniFile(g_Settings->LoadString(SupportFile_RomDatabase).c_str());

	g_Settings->RegisterChangeCB(Game_IniKey,NULL,GameChanged);
	
	m_SectionIdent = new stdstr(g_Settings->LoadString(Game_IniKey));
}

void CSettingTypeRomDatabase::CleanUp( void )
{
	g_Settings->UnregisterChangeCB(Game_IniKey,NULL,GameChanged);
	if (m_SettingsIniFile)
	{
		delete m_SettingsIniFile;
		m_SettingsIniFile = NULL;
	}
	if (m_SectionIdent)
	{
		delete m_SectionIdent;
		m_SectionIdent = NULL;
	}
}

void CSettingTypeRomDatabase::GameChanged ( void * /*Data */ )
{
	if (m_SectionIdent)
	{
		*m_SectionIdent = g_Settings->LoadString(Game_IniKey);
	}
}

bool CSettingTypeRomDatabase::Load ( int Index, bool & Value ) const
{
	DWORD temp_value = Value;
	bool bRes = Load(Index,temp_value);
	Value = temp_value != 0;
	return bRes;
}

bool CSettingTypeRomDatabase::Load ( int Index, ULONG & Value ) const
{
	bool bRes = m_SettingsIniFile->GetNumber(m_SectionIdent->c_str(),m_KeyName.c_str(),Value,Value);
	if (!bRes)
	{
		LoadDefault(Index,Value);
	}
	return bRes;
}

bool CSettingTypeRomDatabase::Load ( int Index, stdstr & Value ) const
{
	stdstr temp_value;
	bool bRes = m_SettingsIniFile->GetString(m_SectionIdent->c_str(),m_KeyName.c_str(),m_DefaultStr,temp_value);
	if (bRes)
	{
		Value = temp_value;
	}
	else 
	{
		LoadDefault(Index,Value);
	}
	return bRes;
}


//return the default values
void CSettingTypeRomDatabase::LoadDefault ( int /*Index*/, bool & Value ) const
{
	if (m_DefaultSetting != Default_None)
	{
		if (m_DefaultSetting == Default_Constant)
		{
			Value = m_DefaultValue != 0;
		} else {
			g_Settings->LoadBool(m_DefaultSetting,Value);
		}
	}
}

void CSettingTypeRomDatabase::LoadDefault ( int /*Index*/, ULONG & Value  ) const
{
	if (m_DefaultSetting != Default_None)
	{
		if (m_DefaultSetting == Default_Constant)
		{
			Value = m_DefaultValue;
		} else {
			g_Settings->LoadDword(m_DefaultSetting,Value);
		}
	}
}

void CSettingTypeRomDatabase::LoadDefault ( int /*Index*/, stdstr & Value ) const
{
	if (m_DefaultSetting != Default_None)
	{
		if (m_DefaultSetting == Default_Constant)
		{
			Value = m_DefaultStr;
		} else {
			g_Settings->LoadString(m_DefaultSetting,Value);
		}
	}
}


//Update the settings
void CSettingTypeRomDatabase::Save ( int /*Index*/, bool Value )
{
	if (!g_Settings->LoadBool(Setting_RdbEditor))
	{
		return;
	}
	if (m_DeleteOnDefault)
	{	
		Notify().BreakPoint(__FILE__,__LINE__); 
	}
	m_SettingsIniFile->SaveNumber(m_SectionIdent->c_str(),m_KeyName.c_str(),Value);
}

void CSettingTypeRomDatabase::Save ( int Index, ULONG Value )
{
	if (!g_Settings->LoadBool(Setting_RdbEditor))
	{
		return;
	}
	if (m_DeleteOnDefault)
	{	
		ULONG defaultValue = 0;
		LoadDefault(Index,defaultValue);
		if (defaultValue == Value)
		{
			Delete(Index);
			return;
		}
	}
	m_SettingsIniFile->SaveNumber(m_SectionIdent->c_str(),m_KeyName.c_str(),Value);
}

void CSettingTypeRomDatabase::Save ( int /*Index*/, const stdstr & Value )
{
	if (!g_Settings->LoadBool(Setting_RdbEditor))
	{
		return;
	}
	m_SettingsIniFile->SaveString(m_SectionIdent->c_str(),m_KeyName.c_str(),Value.c_str());
}

void CSettingTypeRomDatabase::Save ( int /*Index*/, const char * Value )
{
	if (!g_Settings->LoadBool(Setting_RdbEditor))
	{
		return; 
	}
	m_SettingsIniFile->SaveString(m_SectionIdent->c_str(),m_KeyName.c_str(),Value);
}

void CSettingTypeRomDatabase::Delete ( int /*Index*/ )
{
	if (!g_Settings->LoadBool(Setting_RdbEditor))
	{
		return; 
	}
	m_SettingsIniFile->SaveString(m_SectionIdent->c_str(),m_KeyName.c_str(),NULL);
}
