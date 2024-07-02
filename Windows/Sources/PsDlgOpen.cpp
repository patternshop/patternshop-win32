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

#include "Patternshop.h"
#include "PsDlgOpen.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// self-initialization message posted
const int MYWM_POSTINIT = WM_USER + 1;

IMPLEMENT_DYNAMIC(PsDlgOpen, CFileDialog)
BEGIN_MESSAGE_MAP(PsDlgOpen, CFileDialog)
	ON_MESSAGE(MYWM_POSTINIT, OnPostInit)
END_MESSAGE_MAP()

PsDlgOpen::PsDlgOpen(BOOL bOpenFileDialog,
	LPCTSTR lpszDefExt, LPCTSTR lpszFileName, DWORD dwFlags,
	LPCTSTR lpszFilter, CWnd* pParentWnd, DWORD dwSize)
	: CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName,
		dwFlags, lpszFilter, pParentWnd, dwSize)
{
}


BOOL PsDlgOpen::OnInitDialog()
{
	CFileDialog::OnInitDialog();
	PostMessage(MYWM_POSTINIT, 0, 0);
	return TRUE;
}

LRESULT PsDlgOpen::OnPostInit(WPARAM wp, LPARAM lp)
{
	SetListView(ODM_VIEW_THUMBS);
	return 0;
}

BOOL PsDlgOpen::SetListView(LISTVIEWCMD cmd)
{
	// Note real dialog is my parent, not me!
	CWnd* pshell = GetParent()->GetDlgItem(lst2);
	if (pshell) {
		// SHELLDLL_DefView window found: send it the command.
		pshell->SendMessage(WM_COMMAND, cmd);
		return TRUE;
	}
	return FALSE;
}

