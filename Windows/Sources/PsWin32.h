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

#include <map>

#include "PsWin.h"
#include "PsHardware.h"

class PsWin32 : public CDialog, public PsWin
{
	DECLARE_DYNAMIC(PsWin32)

public:
	PsWin32(UINT nIDTemplate, CWnd* pParent = NULL);   // standard constructor
	virtual ~PsWin32();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	static CPen penBlackDash;
	static CFont nFont;
	static CFont bFont;

public:
	CDC* windowDc;
	CDC* dc; // pointeur sur le backbuffer

protected:
	static bool staticLoaded;
	std::map<int, CPen*> m_PenMap;
	std::map<int, CBrush*> m_BrushMap;

public:
	void SetDashBlackPen();
	void SetPenColor(int, int, int);
	void SetBrushColor(int, int, int);
	void SetTextColor(int, int, int);
	void DrawRectangle(int, int, int, int);
	void MovePenTo(int, int);
	void DrawLineTo(int, int);
	void DrawSoftwareBuffer(SoftwareBuffer&, int, int);
	void DrawText(const char*, PsRect&, PsFont);
	void DrawPolygon(PsPoint*, int);
	void SetTarget(SoftwareBuffer*);
	void DrawImage(SoftwareBuffer&, int, int);

protected:
	void UpdateSize();

public:
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnTimer(UINT_PTR nIDEvent);
	afx_msg int   OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void  OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void  OnPaint();
};

void CopyImageWithTransparency(CPaintDC&, CImage&, int, int);
