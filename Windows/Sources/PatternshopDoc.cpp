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
#include "PatternshopDoc.h"
#include "PatternshopView.h"
#include "PsProject.h"
#include "PsDlgDocument.h"
#include "PsWinSplash.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPatternshopDoc

IMPLEMENT_DYNCREATE(CPatternshopDoc, CDocument)

BEGIN_MESSAGE_MAP(CPatternshopDoc, CDocument)
END_MESSAGE_MAP()

// construction ou destruction de CPatternshopDoc

CPatternshopDoc::CPatternshopDoc()
{
}

CPatternshopDoc::~CPatternshopDoc()
{
}

extern  bool CPatternshopAppIsLoad;

BOOL CPatternshopDoc::OnNewDocument()
{
   if(!CPatternshopAppIsLoad)
      return FALSE;

   PsDlgDocument dlg;
   if(dlg.DoModal() == IDOK)
   {
      if(!CDocument::OnNewDocument())
         return FALSE;
      POSITION pos = GetFirstViewPosition();
      CPatternshopView *pView =(CPatternshopView *)GetNextView(pos);
      pView->SetProjectSize(dlg.w, dlg.h, dlg.dpi);
      pView->SetAutoPosition();
      PsController::Instance().SetActive(pView);
      PsController::Instance().project = pView->project;
      PsController::Instance().UpdateDialogProject(); 
      return TRUE;
   }
   return FALSE;
}

BOOL CPatternshopDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
   PsWinSplash::Instance().DestroyWindow();
   if(PsController::Instance().active)
      return PsController::Instance().active->MenuFileLoad(lpszPathName);
   return FALSE;
}

BOOL CPatternshopDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
   if(PsController::Instance().active && PsController::Instance().active->project)
      GetError(PsController::Instance().project->FileSave(lpszPathName));
   return TRUE;
}

void CPatternshopDoc::Serialize(CArchive& ar)
{
   ASSERT(1 == 2);
}

