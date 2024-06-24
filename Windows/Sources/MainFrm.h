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

#include "PsWinProperties.h"

class CMainFrame : public CMDIFrameWnd
{
   DECLARE_DYNAMIC(CMainFrame)
public:
   CMainFrame();

   // Attributs
public:
   CReBar        m_wndReBar;  // la rebar
   CDialogBar      m_wndDlgBar;  // la barre de dialogue
   CStatusBar      m_wndStatusBar;
   CProgressCtrl    m_wndProgress;

   // Opérations
public:
   void RebuildMenuResource( CDocTemplate* pTemplate );
   void DisplaySubWindow();
   // Substitutions

public:
   virtual BOOL PreCreateWindow (CREATESTRUCT& cs);

   // Implémentation
public:
   virtual ~CMainFrame();
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump (CDumpContext& dc) const;
#endif

public:
   virtual void  MenuOptionAutomatic();
   virtual void  MenuOptionBoxShow();

   virtual void  ProgressBar (int);

   DECLARE_MESSAGE_MAP()

protected:
   afx_msg void  OnClose();
   afx_msg int    OnCreate (LPCREATESTRUCT lpCreateStruct);
   afx_msg void  OnMenuSelect( UINT, UINT, HMENU );
   afx_msg void  OnSize(UINT nType, int cx, int cy);
   afx_msg void  OnMove(int, int);
   afx_msg void  OnFichierPr32850();
   afx_msg LRESULT OnReceiveUnique (WPARAM, LPARAM);
   afx_msg void  OnHelp();
   afx_msg void  OnUpdateOptionBoxShow(CCmdUI *pCmdUI);
   afx_msg void  OnUpdateOptionAutomatic(CCmdUI *pCmdUI);
   afx_msg void  OnAideSignalerunbogue();
   afx_msg BOOL   OnEraseBkgnd(CDC* pDC);
};
