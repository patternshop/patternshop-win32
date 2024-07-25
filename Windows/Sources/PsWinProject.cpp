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
#include "PsWinProject.h"
#include "PatternshopView.h"

IMPLEMENT_DYNAMIC(PsWinProject, PsWin32)


PsWinProject* PsWinProject::instance = 0;

PsWinProject::PsWinProject(CWnd* pParent /*=NULL*/) :
	PsWin32(PsWinProject::IDD, pParent), PsWinProjectCx(this)
{
	scrollbar = new PsScrollBarWin32(scrollbarWin32);
}

PsWinProject::~PsWinProject()
{
	delete scrollbar;
	delete dc;
}

PsWinProject& PsWinProject::Instance()
{
	if (!instance) instance = new PsWinProject();
	return *instance;
}

void PsWinProject::Delete()
{
	if (instance)
	{
		delete instance;
		instance = 0;
	}
}

void PsWinProject::DoDataExchange(CDataExchange* pDX)
{
	PsWin32::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCROLLBAR1, scrollbarWin32);
}

BEGIN_MESSAGE_MAP(PsWinProject, CDialog)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_SYSCOMMAND()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
END_MESSAGE_MAP()


int PsWinProject::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// double buffering
	UpdateSize();
	backbuffer.Create(iWidth, iHeight, 24);
	dc = new CDC();
	CClientDC wdc(this);
	dc->CreateCompatibleDC(&wdc);
	dc->SelectObject(backbuffer);

	if (PsWin32::OnCreate(lpCreateStruct) == -1)
		return -1;

	viewImage.buffer.LoadFromResource(AfxGetInstanceHandle(), IDB_VIEW);
	boxImage.buffer.LoadFromResource(AfxGetInstanceHandle(), IDB_BOX);
	directoryImage.buffer.LoadFromResource(AfxGetInstanceHandle(), IDB_DIRECTORY);
	openImage.buffer.LoadFromResource(AfxGetInstanceHandle(), IDB_OPEN);
	closeImage.buffer.LoadFromResource(AfxGetInstanceHandle(), IDB_CLOSE);
	directoryImageB.buffer.LoadFromResource(AfxGetInstanceHandle(), IDB_DIRECTORYB);
	openImageB.buffer.LoadFromResource(AfxGetInstanceHandle(), IDB_OPENB);
	closeImageB.buffer.LoadFromResource(AfxGetInstanceHandle(), IDB_CLOSEB);
	spot.buffer.LoadFromResource(AfxGetInstanceHandle(), IDB_SPOT);
	m_mouseFinger = AfxGetApp()->LoadCursor(IDC_FINGER);
	m_mouseArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	m_mouseMove = AfxGetApp()->LoadCursor(IDC_HAND2);
	m_hMouseCursor = m_mouseArrow;

	return 0;
}

void PsWinProject::OnPaint()
{
	CPaintDC wdc(this);
	wdc.BitBlt(0, 0, iWidth - s.right, backbuffer.GetHeight(), dc, 0, 0, SRCCOPY);
}


void PsWinProject::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int CurPos = scrollbarWin32.GetScrollPos();

	// Determine the new position of scroll box.
	switch (nSBCode)
	{
	case SB_LEFT:      // Scroll to far left.
		CurPos = 0;
		break;

	case SB_RIGHT:      // Scroll to far right.
		CurPos = 122;
		break;

	case SB_ENDSCROLL:   // End scroll.
		break;

	case SB_LINELEFT:      // Scroll left.
		if (CurPos > 0)
			CurPos--;
		break;

	case SB_LINERIGHT:   // Scroll right.
		if (CurPos < 122)
			CurPos++;
		break;

	case SB_PAGELEFT:    // Scroll one page left.
	{
		// Get the page size. 
		SCROLLINFO   info;
		scrollbarWin32.GetScrollInfo(&info, SIF_ALL);

		//if (CurPos > 0)
		//	CurPos = max(0, CurPos - (int) info.nPage);
		CurPos -= item_count_size / 3;
	}
	break;

	case SB_PAGERIGHT:      // Scroll one page right
	{
		// Get the page size. 
		SCROLLINFO   info;
		scrollbarWin32.GetScrollInfo(&info, SIF_ALL);

		//if (CurPos < 122)
		CurPos += item_count_size / 3; //min(122, CurPos + (int) info.nPage);
	}
	break;

	case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
		CurPos = nPos;      // of the scroll box at the end of the drag operation.
		break;

	case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is the
		CurPos = nPos;     // position that the scroll box has been dragged to.
		break;
	}

	// Set the new position of the thumb (scroll box).
	scrollbarWin32.SetScrollPos(CurPos);
	Update();
}

void PsWinProject::UpdateMouseCursor()
{
	switch (mouseCursor)
	{
	case CURSOR_DEFAULT: m_hMouseCursor = m_mouseArrow; break;
	case CURSOR_FINGER: m_hMouseCursor = m_mouseFinger; break;
	case CURSOR_SCROLL2: m_hMouseCursor = m_mouseMove; break;
	}
	OnSetCursor(NULL, 0, 0);
}

BOOL PsWinProject::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	ASSERT(m_hMouseCursor != NULL);
	::SetCursor(m_hMouseCursor);
	return TRUE;
}


void PsWinProject::OnLButtonDown(UINT nFlags, CPoint point)
{
	OnLeftMouseButtonDown(point);
	PsWin32::OnLButtonDown(nFlags, point);
}

void PsWinProject::OnLButtonUp(UINT nFlags, CPoint point)
{
	OnLeftMouseButtonUp(point);
	PsWin32::OnLButtonUp(nFlags, point);
}

void PsWinProject::OnMouseMove(UINT nFlags, CPoint point)
{
	OnMyMouseMove(point);
	PsWin32::OnMouseMove(nFlags, point);
}

BOOL PsWinProject::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void PsWinProject::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_CLOSE) return;
	PsWin32::OnSysCommand(nID, lParam);
}

void PsWinProject::Update()
{
	CBrush fakeBrush;
	CBrush* pOldBrush = dc->SelectObject(&fakeBrush);
	CPen fakePen;
	CPen* pOldPen = dc->SelectObject(&fakePen);

	//-- mise à jour de la taille du buffer en fonction de la scrollbar...
	int iOldWidth = iWidth;
	UpdateSize();
	iWidth -= scrollbar->GetWidth();
	if (iOldWidth != iWidth)
	{
		backbuffer.Destroy();
		backbuffer.Create(iWidth, iHeight, 24);
		dc->SelectObject(backbuffer);
	}
	//--

	GenericUpdate();

	dc->SelectObject(pOldBrush);
	dc->SelectObject(pOldPen);
	Invalidate(true);
}

void PsWinProject::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	PsWin32::OnKeyDown(nChar, nRepCnt, nFlags);
	PsProjectController* project_controller = PsController::Instance().project_controller;
	sKeyList.insert(nChar);
	/*
	char buffer[1024];
	sprintf(buffer, "%d down\n", nChar);
	OutputDebugString(buffer);
	*/
}

void PsWinProject::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	PsWin32::OnKeyDown(nChar, nRepCnt, nFlags);
	PsProjectController* project_controller = PsController::Instance().project_controller;
	CPatternshopView* active = PsController::Instance().active;
	if (!active) return;

	/*
	char buffer[1024];
	sprintf(buffer, "%d up\n", nChar);
	OutputDebugString(buffer);
	*/

	if (sKeyList.size() == 2 && sKeyList.find(VK_CONTROL) != sKeyList.end())
	{
		if (sKeyList.find(77) != sKeyList.end()) // M
			active->MenuMatrixNew();
		if (sKeyList.find(82) != sKeyList.end()) // R
			active->MenuEditClone();
		if (sKeyList.find(68) != sKeyList.end()) // D
			active->MenuFileImage();
	}

	if (sKeyList.size() == 1)
	{
		if (sKeyList.find(46) != sKeyList.end()) // Suppr
			active->MenuImageDel();
	}

	sKeyList.erase(nChar);
}

void PsWinProject::Show()
{
	ShowWindow(TRUE);
}

PsScrollBarWin32::PsScrollBarWin32(CScrollBar& scrollbar)
{
	this->scrollbar = &scrollbar;
}

int PsScrollBarWin32::GetWidth()
{
	PsRect k;
	scrollbar->GetClientRect(&k);
	return k.right;
}

void PsScrollBarWin32::SetSize(int iSize)
{
	scrollbar->SetScrollRange(0, iSize);
}

int PsScrollBarWin32::GetPos()
{
	return scrollbar->GetScrollPos();
}

void PsScrollBarWin32::Enable()
{
	scrollbar->EnableScrollBar(ESB_ENABLE_BOTH);
}

void PsScrollBarWin32::Disable()
{
	scrollbar->EnableScrollBar(ESB_DISABLE_BOTH);
}

