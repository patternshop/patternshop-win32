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

#include "PsProject.h"
#include "Patternshop.h"
#include "PsWinTools.h"
#include "PsWinOverview.h"
#include "PatternshopView.h"

PsWinTools*	PsWinTools::instance = 0;

IMPLEMENT_DYNAMIC(PsWinTools, CDialog)
PsWinTools::PsWinTools(CWnd* pParent /*=NULL*/)
	: CDialog(PsWinTools::IDD, pParent)
{
  instance = NULL;
}

PsWinTools::~PsWinTools()
{
}

void PsWinTools::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

PsWinTools&	PsWinTools::Instance()
{
	if(!instance)
		instance = new PsWinTools();
	return *instance;
}

void	PsWinTools::Delete()
{
	if(instance)
	{
		delete instance;
		instance = 0;
	}
}

BEGIN_MESSAGE_MAP(PsWinTools, CDialog)
	ON_BN_CLICKED(ID_TOOL1, OnBnClickedTool1)
	ON_BN_CLICKED(ID_TOOL2, OnBnClickedTool2)
	ON_BN_CLICKED(ID_TOOL3, OnBnClickedTool3)
	ON_BN_DOUBLECLICKED(ID_TOOL3, OnBnDoubleClickedTool3)
	ON_WM_ERASEBKGND()
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()

BOOL	PsWinTools::OnInitDialog()
{
	btn_modify.SubclassDlgItem (ID_TOOL1, this);
	btn_modify.SetWindowPos(NULL, 0, 0, 25, 21, SWP_NOMOVE);
	btn_scroll.SubclassDlgItem (ID_TOOL2, this);
	btn_scroll.SetWindowPos(NULL, 0, 0, 25, 21, SWP_NOMOVE);
	btn_magnify.SubclassDlgItem (ID_TOOL3, this);
	btn_magnify.SetWindowPos(NULL, 0, 0, 25, 21, SWP_NOMOVE);
	OnBnClickedTool1();

	return CDialog::OnInitDialog();
}

void	PsWinTools::OnBnClickedTool1()
{
	PsController::Instance().SetTool(PsController::TOOL_MODIFY);
	releaseToolsIcons();
	btn_modify.LoadBitmaps (IDB_TOOL_MODIFY3);
	this->Invalidate(true);
}

void	PsWinTools::OnBnClickedTool2()
{
	PsController::Instance().SetTool (PsController::TOOL_SCROLL);
	releaseToolsIcons();
	btn_scroll.LoadBitmaps (IDB_TOOL_SCROLL3);
	this->Invalidate(true); 
}

void	PsWinTools::OnBnClickedTool3()
{
	PsController::Instance().SetTool (PsController::TOOL_MAGNIFY);
	releaseToolsIcons();
	btn_magnify.LoadBitmaps (IDB_TOOL_MAGNIFY3);
	this->Invalidate(true);
}

void	PsWinTools::OnBnDoubleClickedTool3()
{
	if (PsController::Instance().project)
		PsController::Instance().project->renderer.zoom = 1.0f;

	if (PsController::Instance().active)
		PsController::Instance().active->Update();
}

void	PsWinTools::releaseToolsIcons()
{
	btn_modify.LoadBitmaps (IDB_TOOL_MODIFY1);
	btn_scroll.LoadBitmaps (IDB_TOOL_SCROLL1);
	btn_magnify.LoadBitmaps (IDB_TOOL_MAGNIFY1);
}

BOOL PsWinTools::OnEraseBkgnd(CDC* pDC)
{
     CBrush BkClrBrush(RGB(212, 208, 200));
     CBrush* pOldBrush = pDC->SelectObject(&BkClrBrush);

     CRect rect;
     pDC->GetClipBox(&rect);
     pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
     pDC->SelectObject(pOldBrush);
     return TRUE;

}

void PsWinTools::OnSysCommand(UINT nID, LPARAM lParam)
{
	if(nID == SC_CLOSE) return;
	CDialog::OnSysCommand(nID, lParam);
}
