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

RomInformation::RomInformation (const char * RomFile):
	m_DeleteRomInfo(true),
	m_FileName(RomFile? RomFile : ""),
	m_pRomInfo(NULL)
{
	if (m_FileName.length() == 0)  { return; }
	m_pRomInfo = new CN64Rom;
	if (!m_pRomInfo->LoadN64Image(m_FileName.c_str())) {
		delete m_pRomInfo;
		m_pRomInfo = NULL;
		return;
	}
}

RomInformation::RomInformation (CN64Rom * RomInfo) :
	m_DeleteRomInfo(false),
	m_FileName(RomInfo ? RomInfo->GetFileName().c_str() : ""),
	m_pRomInfo(RomInfo)
{
}

RomInformation::~RomInformation(void) {
	if (m_DeleteRomInfo) {
		delete m_pRomInfo;
	}
}

#include <windows.h>
void RomInformation::DisplayInformation(WND_HANDLE hParent) const {
	if (m_FileName.length() == 0) { return; }
	
	DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_Rom_Information), 
		(HWND)hParent, (DLGPROC)RomInfoProc, (DWORD)this);
}

DWORD CALLBACK RomInfoProc (WND_HANDLE hDlg, DWORD uMsg, DWORD wParam, DWORD lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:
		{
			//record class for future usage	
			SetProp((HWND)hDlg,"this",(RomInformation *)lParam);
			RomInformation * _this = (RomInformation *)lParam;
			
			SetWindowText((HWND)hDlg, GS(INFO_TITLE));
			SetDlgItemText((HWND)hDlg, IDC_ROM_NAME, GS(INFO_ROM_NAME_TEXT));
			SetDlgItemText((HWND)hDlg, IDC_FILE_NAME, GS(INFO_FILE_NAME_TEXT));
			SetDlgItemText((HWND)hDlg, IDC_LOCATION, GS(INFO_LOCATION_TEXT));
			SetDlgItemText((HWND)hDlg, IDC_ROM_MD5, GS(INFO_MD5_TEXT));
			SetDlgItemText((HWND)hDlg, IDC_ROM_SIZE, GS(INFO_SIZE_TEXT));
			SetDlgItemText((HWND)hDlg, IDC_CART_ID, GS(INFO_CART_ID_TEXT));
			SetDlgItemText((HWND)hDlg, IDC_MANUFACTURER, GS(INFO_MANUFACTURER_TEXT));
			SetDlgItemText((HWND)hDlg, IDC_COUNTRY, GS(INFO_COUNTRY_TEXT));
			SetDlgItemText((HWND)hDlg, IDC_CRC1, GS(INFO_CRC1_TEXT));
			SetDlgItemText((HWND)hDlg, IDC_CRC2, GS(INFO_CRC2_TEXT));
			SetDlgItemText((HWND)hDlg, IDC_CIC_CHIP, GS(INFO_CIC_CHIP_TEXT));

			SetDlgItemText((HWND)hDlg,IDC_INFO_ROMNAME,_this->m_pRomInfo->GetRomName().c_str());
			
			char path[_MAX_PATH], drive[_MAX_DRIVE],dir[_MAX_DIR], fname[_MAX_FNAME],ext[_MAX_EXT];
			_splitpath(_this->m_pRomInfo->GetFileName().c_str(), drive, dir, fname, ext);
			_makepath(path, drive, dir, "", "");

			SetDlgItemText((HWND)hDlg,IDC_INFO_FILENAME,fname);
			SetDlgItemText((HWND)hDlg,IDC_INFO_LOCATION,path);
			
			SetDlgItemText((HWND)hDlg,IDC_INFO_MD5,_this->m_pRomInfo->GetRomMD5().c_str());

			char String[255] = " ";
			sprintf(&String[1],"%.1f MBit",(float)_this->m_pRomInfo->GetRomSize()/0x20000);
			SetDlgItemText((HWND)hDlg,IDC_INFO_ROMSIZE,String);

			BYTE * RomHeader = _this->m_pRomInfo->GetRomAddress();
			String[1] = RomHeader[0x3F];
			String[2] = RomHeader[0x3E];
			String[3] = '\0';
			SetDlgItemText((HWND)hDlg,IDC_INFO_CARTID,String);
			
			switch (RomHeader[0x38]) {
			case 'N': SetDlgItemText((HWND)hDlg,IDC_INFO_MANUFACTURER," Nintendo"); break;
			case 0:   SetDlgItemText((HWND)hDlg,IDC_INFO_MANUFACTURER," None"); break;
			default:  SetDlgItemText((HWND)hDlg,IDC_INFO_MANUFACTURER," (Unknown)"); break;
			}

			switch (RomHeader[0x3D]) {
			case NTSC_BETA: SetDlgItemText((HWND)hDlg,IDC_INFO_COUNTRY," Beta"); break;
			case X_NTSC:    SetDlgItemText((HWND)hDlg,IDC_INFO_COUNTRY," NTSC"); break;
			case Germany:   SetDlgItemText((HWND)hDlg,IDC_INFO_COUNTRY," Germany"); break;
			case USA:       SetDlgItemText((HWND)hDlg,IDC_INFO_COUNTRY," America"); break;
			case french:    SetDlgItemText((HWND)hDlg,IDC_INFO_COUNTRY," France"); break;
			case Italian:   SetDlgItemText((HWND)hDlg,IDC_INFO_COUNTRY," Italy"); break;
			case Japan:     SetDlgItemText((HWND)hDlg,IDC_INFO_COUNTRY," Japan"); break;
			case Europe:    SetDlgItemText((HWND)hDlg,IDC_INFO_COUNTRY," Europe"); break;
			case Spanish:   SetDlgItemText((HWND)hDlg,IDC_INFO_COUNTRY," Spain"); break;
			case Australia: SetDlgItemText((HWND)hDlg,IDC_INFO_COUNTRY," Australia"); break;
			case X_PAL:     SetDlgItemText((HWND)hDlg,IDC_INFO_COUNTRY," PAL"); break;
			case Y_PAL:     SetDlgItemText((HWND)hDlg,IDC_INFO_COUNTRY," PAL"); break;
			case 0: SetDlgItemText((HWND)hDlg,IDC_INFO_COUNTRY," None"); break;
			default:
				sprintf(&String[1]," Unknown %c (%02X)",RomHeader[0x3D],RomHeader[0x3D]);
				SetDlgItemText((HWND)hDlg,IDC_INFO_COUNTRY,String);
			}
			
			sprintf(&String[1],"0x%08X",*(DWORD *)(RomHeader + 0x10));
			SetDlgItemText((HWND)hDlg,IDC_INFO_CRC1,String);

			sprintf(&String[1],"0x%08X",*(DWORD *)(RomHeader + 0x14));
			SetDlgItemText((HWND)hDlg,IDC_INFO_CRC2,String);

			if (_this->m_pRomInfo->CicChipID() == CIC_UNKNOWN) { 
				sprintf(&String[1],"Unknown");
			} else {
				sprintf(&String[1],"CIC-NUS-610%d",_this->m_pRomInfo->CicChipID());
			}
			SetDlgItemText((HWND)hDlg,IDC_INFO_CIC,String);
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDCANCEL:
			RemoveProp((HWND)hDlg,"this");
			EndDialog((HWND)hDlg,0);
			break;
		case IDC_CLOSE_BUTTON:
			RemoveProp((HWND)hDlg,"this");
			EndDialog((HWND)hDlg,0);
			break;
		}
	default:
		return false;
	}
	return true;
}