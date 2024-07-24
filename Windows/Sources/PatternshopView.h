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

#include <string>
#include <atlimage.h>

#include "PsTypes.h"
#include "PsHardware.h"

#include "resource.h"

class	PsProjectController;

class	CPatternshopView : public CView
{
public:
	CDocument* GetDocument() const;
	virtual void	OnDraw(CDC* pDC);

	PsProjectController* project;
	std::string		name;

	bool			updated;
	SoftwareBuffer m_buffer;

#ifdef _DEBUG
	virtual void	AssertValid() const;
	virtual void	Dump(CDumpContext& dc) const;
#endif

protected:
	CPatternshopView();
	virtual			~CPatternshopView();

	DECLARE_DYNCREATE(CPatternshopView)

public:
	void  Update() { updated = false; }
	void  SetProjectSize(int, int, int);
	BOOL  MenuFileLoad(const char*);
	void	MenuFileImage();
	void	MenuFileExport();
	void	OnPrint(CDC* pDC, CPrintInfo* pInfo);
	void	MenuFileExportRaccord();
	void	MenuFilePattern();
	void	MenuEditClone();
	void	MenuEditReplace();
	void	MenuImageDel();
	void	MenuLogRedo();
	void	MenuLogUndo();
	void	MenuMatrixClone();
	void	MenuMatrixColor();
	void	MenuMatrixDel();
	void	MenuMatrixNew();
	void	MenuMatrixReset();

	void	MenuOptionDocumentBlend();
	void	MenuOptionDocumentShow();
	void	MenuOptionHighlightShow();

	void SetAutoPosition();
	void SetMouseCursor(HCURSOR mcursor);

public:
	void  UpdateNow();
	void	SetRenderSize(int, int);

private:
	HDC				m_hOldDC;
	HDC				m_hMemDC;
	HGLRC			m_hOldRC;
	HGLRC			m_hMemRC;
	BITMAPINFO		m_bmi;
	LPVOID			m_pBitmapBits;
	HBITMAP			m_hDib;
	HGDIOBJ			m_hOldDib;
	CSize			m_szPage;
	HCURSOR m_hMouseCursor;

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void	OnDestroy();
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void	OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void	OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void	OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void	OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void	OnSetFocus(CWnd* pOldWnd);
	afx_msg void	OnDropFiles(HDROP hDropInfo);
	afx_msg void	OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void	OnTimer(UINT_PTR nIDEvent);
	afx_msg void	OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateEditUndo(CCmdUI* pCmdUI);
	//afx_msg void	OnUpdateOptionBoxMove(CCmdUI *pCmdUI);
	afx_msg void	OnUpdateOptionHighlightShow(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateOptionDocumentShow(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateOptionDocumentBlend(CCmdUI* pCmdUI);
	afx_msg void	OnFichierImporteruneimagelibre();
	afx_msg void  OnSize(UINT nType, int cx, int cy);
	afx_msg	BOOL	OnPreparePrinting(CPrintInfo* pInfo);
	afx_msg BOOL  OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL  OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

public:
	afx_msg void OnProjetTailledelazonedetravail();
};

#ifndef _DEBUG  // version de débogage dans PatternshopView.cpp
inline CDocument* CPatternshopView::GetDocument() const
{
	return reinterpret_cast<CDocument*>(m_pDocument);
}
#endif

