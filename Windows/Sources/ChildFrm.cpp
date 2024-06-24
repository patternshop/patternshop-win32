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
#include "ChildFrm.h"
#include "PsDlgConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

// construction ou destruction de CChildFrame

CChildFrame::CChildFrame()
{
}

CChildFrame::~CChildFrame()
{
}

// diagnostics pour CChildFrame

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump (CDumpContext& dc) const
{
	CMDIChildWnd::Dump (dc);
}

#endif //_DEBUG

// gestionnaires de messages pour CChildFrame

void CChildFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
	if (resolution_max)
	{
		lpMMI->ptMaxTrackSize.x = resolution_max;
		lpMMI->ptMaxTrackSize.y = resolution_max;
	}
	CMDIChildWnd::OnGetMinMaxInfo(lpMMI);
}
