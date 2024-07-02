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
#include "afxwin.h"

class PsWinTools : public CDialog
{
	DECLARE_DYNAMIC(PsWinTools)

public:
	static PsWinTools& Instance();
	static void Delete();
	virtual ~PsWinTools();

private:
	PsWinTools(CWnd* pParent = NULL);
	enum { IDD = IDD_TOOLS };

private:
	void releaseToolsIcons();

private:
	static PsWinTools* instance; // Singleton
	CBitmapButton	btn_scroll;
	CBitmapButton	btn_magnify;
	CBitmapButton	btn_modify;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	afx_msg BOOL	OnInitDialog();
	afx_msg void	OnBnClickedTool1();
	afx_msg void	OnBnClickedTool2();
	afx_msg void	OnBnClickedTool3();
	afx_msg void	OnBnDoubleClickedTool3();
	afx_msg BOOL   OnEraseBkgnd(CDC* pDC);
	afx_msg void   OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void   OnSysCommand(UINT nID, LPARAM lParam);
};

