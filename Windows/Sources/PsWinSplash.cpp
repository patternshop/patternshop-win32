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
#include "PsWinSplash.h"

PsWinSplash*	PsWinSplash::instance = 0;

IMPLEMENT_DYNAMIC(PsWinSplash, CDialog)

PsWinSplash::PsWinSplash(CWnd* pParent /*=NULL*/)
	: CDialog(PsWinSplash::IDD, pParent)
{
  m_CreationTime = GetTickCount();
}

PsWinSplash::~PsWinSplash()
{
}

PsWinSplash&	PsWinSplash::Instance()
{
	if(!instance)
		instance = new PsWinSplash();
	return *instance;
}

void	PsWinSplash::Delete()
{
	if(instance)
	{
		delete instance;
		instance = 0;
	}
}

void PsWinSplash::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(PsWinSplash, CDialog)
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_CREATE()
END_MESSAGE_MAP()


extern bool CPatternshopAppIsLoad;

void PsWinSplash::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (CPatternshopAppIsLoad)
		DestroyWindow();
	CDialog::OnLButtonDown(nFlags, point);
}

void PsWinSplash::OnTimer(UINT_PTR nIDEvent)
{
	if (CPatternshopAppIsLoad || m_CreationTime + 10000 < GetTickCount())
	{
		DestroyWindow();
	}
}

int PsWinSplash::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	SetTimer(0, 2000, NULL);
	return 0;
}
