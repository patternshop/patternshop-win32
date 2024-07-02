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
#include "PatternshopView.h"
#include "PsWinOverview.h"
#include "PsDlgDocument.h"
#include "PsProject.h"
#include "PsController.h"

PsWinOverview* PsWinOverview::instance = 0;

IMPLEMENT_DYNAMIC(PsWinOverview, CDialog)

PsWinOverview::PsWinOverview(CWnd* pParent /*=NULL*/)
	: PsWin32(PsWinOverview::IDD, pParent), DialogOverviewCx()
{
}

PsWinOverview::~PsWinOverview()
{
}

PsWinOverview& PsWinOverview::Instance()
{
	if (!instance)
		instance = new PsWinOverview();
	return *instance;
}

void	PsWinOverview::Delete()
{
	if (instance)
	{
		delete instance;
		instance = 0;
	}
}

int PsWinOverview::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	PsRect r;
	GetClientRect(&r);

	m_RenduImage.Create(r.right, r.bottom, 24);
	window_buffer2.Create(r.right, r.bottom, 24);
	window_buffer.Create(r.right, r.bottom, 24);

	SetTarget(&window_buffer);
	CleanBackground();

	SetTimer(0, 100, NULL);

	return 0;
}

BEGIN_MESSAGE_MAP(PsWinOverview, CDialog)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()

void PsWinOverview::DrawRedSelection()
{
	if (!PsController::Instance().project)
		return;

	PsRect z =
		GetSelectionRectangle(window_buffer2.GetWidth(),
			window_buffer2.GetHeight());

	CPen red(PS_SOLID, 2, RGB(255, 0, 0));
	dc->SelectObject(&red);

	MovePenTo(z.left, z.top);
	DrawLineTo(z.right, z.top);
	DrawLineTo(z.right, z.bottom);
	DrawLineTo(z.left, z.bottom);
	DrawLineTo(z.left, z.top - 1);
}

void PsWinOverview::UpdateNow()
{
	if (PsController::Instance().active &&
		PsController::Instance().project)
	{
		UpdateMiniImage();
		Invalidate(true);
	}
}

void PsWinOverview::OnPaint()
{
	FastUpdate();
	CPaintDC dc(this);
	window_buffer.buffer.BitBlt(dc.GetSafeHdc(), 0, 0);
}

BOOL PsWinOverview::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void PsWinOverview::OnLButtonDown(UINT nFlags, CPoint point)
{
	OnLeftMouseButtonDown(point);
	Invalidate();
	CDialog::OnLButtonDown(nFlags, point);
}

void PsWinOverview::OnMouseMove(UINT nFlags, CPoint point)
{
	if (bDragging || zooming)
		OnLButtonDown(nFlags, point);
	CDialog::OnMouseMove(nFlags, point);
}

void PsWinOverview::OnLButtonUp(UINT nFlags, CPoint point)
{
	OnLeftMouseButtonUp(point);
	CDialog::OnLButtonUp(nFlags, point);
}

void PsWinOverview::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_CLOSE) return;
	CDialog::OnSysCommand(nID, lParam);
}

void PsWinOverview::OnTimer(UINT_PTR nIDEvent)
{
	if (!bUpdated && !PsController::Instance().bMouseButtonIsDown && !zooming && !bDragging)
	{
		UpdateNow();
		bUpdated = true;
	}
}
