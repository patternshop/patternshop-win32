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

#include "afxcmn.h"
#include <atlimage.h>
#include "afxwin.h"

#include "PsWin32.h"
#include "resource.h"
#include "PsRender.h"

#include <map>
#include <set>

class PsScrollBarWin32 : public PsScrollBar
{
public:
	PsScrollBarWin32(CScrollBar& scrollbar);
	int GetWidth();
	void SetSize(int);
	int GetPos();
	void Enable();
	void Disable();

protected:
	CScrollBar* scrollbar;

};

class PsWinProjectWin32 : public PsWin32, public PsWinProjectModel
{
	DECLARE_DYNAMIC(PsWinProjectWin32)

	enum { IDD = IDD_PROJECT };

public:
	PsWinProjectWin32(CWnd* pParent = NULL);   // standard constructor
	virtual ~PsWinProjectWin32();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	void Show();
	void Update();

protected:
	CImage backbuffer;
	HCURSOR m_hMouseCursor, m_mouseFinger, m_mouseArrow, m_mouseMove;
	CScrollBar scrollbarWin32;

protected:
	void UpdateMouseCursor();

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

private:
	std::set<UINT> sKeyList;
};
