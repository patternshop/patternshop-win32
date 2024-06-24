/**
 * This file is part of Patternshop Project.
 * 
 * Patternshop is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Patternshop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Patternshop.  If not, see <http://www.gnu.org/licenses/>
*/
#pragma once

#include <sys/stat.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions (including VB)
#include <afxcmn.h>         // MFC common controls
#include <shlobj.h>			 // for IShellFolder and shell stuff
#include <atlbase.h>			 // for CComQIPtr smart pointers
#include <dlgs.h>				 // for common dialog control IDs

class PsDlgOpen : public CFileDialog {
public:
	PsDlgOpen( BOOL bOpenFileDialog,
	   LPCTSTR lpszDefExt = NULL,
	   LPCTSTR lpszFileName = NULL,
	   DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	   LPCTSTR lpszFilter = NULL,
	   CWnd* pParentWnd = NULL,
	   DWORD dwSize = 0);

protected:
	CListCtrl m_wndList;

	enum { ID_LISTVIEW = lst2 };

	// reverse-engineered command codes for SHELLDLL_DefView
	enum LISTVIEWCMD
	{	ODM_VIEW_ICONS = 0x7029,
		ODM_VIEW_LIST  = 0x702b,
		ODM_VIEW_DETAIL= 0x702c,
		ODM_VIEW_THUMBS= 0x702d,
		ODM_VIEW_TILES = 0x702e,
	};

	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnPostInit(WPARAM wp, LPARAM lp);

	BOOL SetListView(LISTVIEWCMD cmd);

	DECLARE_DYNAMIC(PsDlgOpen)
	DECLARE_MESSAGE_MAP()
};
