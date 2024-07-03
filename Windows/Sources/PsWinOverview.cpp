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
	: PsWin32(PsWinOverview::IDD, pParent), DialogOverviewCx(this)
{
}

PsWinOverview::~PsWinOverview()
{
}

PsWinOverview& PsWinOverview::Instance()
{
	if (!instance) instance = new PsWinOverview();
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
	bool mouseButtonIsDown = PsController::Instance().bMouseButtonIsDown;
	if (!bUpdated && !mouseButtonIsDown && !zooming && !bDragging)
	{
		UpdateNow();
		bUpdated = true;
	}
}

void PsWinOverview::UpdateMiniImage()
{
	PsRender& renderer = PsController::Instance().project->renderer;

	float size_x = renderer.size_x;
	float size_y = renderer.size_y;
	float scroll_x = renderer.scroll_x;
	float scroll_y = renderer.scroll_y;
	float zoom = renderer.zoom;
	bool showbox = PsController::Instance().GetOption(PsController::OPTION_BOX_SHOW);
	bool showmat = PsController::Instance().GetOption(PsController::OPTION_HIGHLIGHT_SHOW);
	int height = window_buffer2.GetHeight() - 14;

	PsController::Instance().SetOption(PsController::OPTION_BOX_SHOW, false);
	PsController::Instance().SetOption(PsController::OPTION_HIGHLIGHT_SHOW, false);
	renderer.SetSize(window_buffer2.GetWidth(), height);
	renderer.CenterView();

	PsController::Instance().project->RenderToScreen();
	hardwareRenderer.CopyToSoftBuffer(m_RenduImage);

	PsRect z; // calcul de la zone de l'image
	z.left = (0 - renderer.x1) / (renderer.x2 - renderer.x1) * window_buffer2.GetWidth();
	z.right = (renderer.doc_x - renderer.x1) / (renderer.x2 - renderer.x1) * window_buffer2.GetWidth();
	z.bottom = height - (renderer.doc_y - renderer.y1) / (renderer.y2 - renderer.y1) * height;
	z.top = height - (0 - renderer.y1) / (renderer.y2 - renderer.y1) * height;
	r_size_x = z.right - z.left;
	r_size_y = z.bottom - z.top;
	r_zoom = renderer.zoom;

	psWin->SetTarget(&window_buffer2);
	CleanBackground();
	psWin->SetTarget(NULL);

	int x = FloatToInt((m_RenduImage.GetWidth() - r_size_x) / 2);
	int y = FloatToInt((height - r_size_y) / 2);

#ifdef _WINDOWS
	CDC dc;
	dc.CreateCompatibleDC(NULL);
	dc.SelectObject(window_buffer2.buffer);
	CDC dc2;
	dc2.CreateCompatibleDC(NULL);
	dc2.SelectObject(m_RenduImage.buffer);
	dc.BitBlt(x, y, r_size_x, r_size_y, &dc2, z.left, m_RenduImage.buffer.GetHeight() - height + z.top, SRCCOPY);
	dc2.DeleteDC();
#else /* _MACOSX */
	SetTarget(&window_buffer2);
	CGRect subRect = CGRectMake(z.left, m_RenduImage.GetHeight() - height + z.top, r_size_x, r_size_y);
	CGImageRef subImage = CGImageCreateWithImageInRect(m_RenduImage.buffer, subRect);
	SoftwareBuffer sb;
	sb.buffer = subImage;
	DrawSoftwareBuffer(sb, x, y);
	SetTarget(NULL);
#endif

	PsRect r;
	r.left = x - 1;
	r.top = y - 1;
	r.right = x + r_size_x + 1;
	r.bottom = y + r_size_y + 1;

#ifdef _WINDOWS
	dc.DrawEdge(&r, EDGE_SUNKEN, BF_RECT);
#endif

	PsController::Instance().SetOption(PsController::OPTION_HIGHLIGHT_SHOW, showmat);
	PsController::Instance().SetOption(PsController::OPTION_BOX_SHOW, showbox);
	renderer.SetSize(size_x, size_y);
	renderer.SetZoom(zoom);
	renderer.SetScroll(scroll_x, scroll_y);

}