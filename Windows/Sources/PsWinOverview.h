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
#include "PsWin32.h"
#include "PsHardware.h"
#include "DialogOverviewCx.h"
#include <atlimage.h>

class PsWinOverview : public PsWin32, public DialogOverviewCx
{
	DECLARE_DYNAMIC(PsWinOverview)

private:
	PsWinOverview(CWnd* pParent = NULL);

public:
	virtual ~PsWinOverview();
	static PsWinOverview& Instance();
	static void Delete();

	enum { IDD = IDD_OVERVIEW };

	DECLARE_MESSAGE_MAP()

private:
	void UpdateNow();
	void DrawRedSelection();
	void UpdateMiniImage();

private:
	static PsWinOverview* instance; // Singleton

public:
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
};

