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
#include "PsDlgColor.h"

IMPLEMENT_DYNAMIC(PsDlgColor, CDialog)

BEGIN_MESSAGE_MAP(PsDlgColor, CDialog)
	//{{AFX_MSG_MAP(CGTColorDialogDlg)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	ON_BN_CLICKED(IDC_RADIO4, OnRadio4)
	ON_BN_CLICKED(IDC_RADIO5, OnRadio5)
	ON_BN_CLICKED(IDC_RADIO6, OnRadio6)
	ON_EN_CHANGE(1010, &PsDlgColor::OnEnChangeRGB)
	ON_EN_CHANGE(1011, &PsDlgColor::OnEnChangeRGB)
	ON_EN_CHANGE(1012, &PsDlgColor::OnEnChangeRGB)
	ON_EN_CHANGE(1014, &PsDlgColor::OnEnChangeHVS)
	ON_EN_CHANGE(1015, &PsDlgColor::OnEnChangeHVS)
	ON_EN_CHANGE(IDC_EDIT4, &PsDlgColor::OnEnChangeHVS)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

PsDlgColor::PsDlgColor(CWnd* pParent /*=NULL*/)
	: CDialog(PsDlgColor::IDD, pParent)
{

}

PsDlgColor::~PsDlgColor()
{
}

void PsDlgColor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, 1010, R);
	DDX_Control(pDX, 1011, G);
	DDX_Control(pDX, 1012, B);
	DDX_Control(pDX, 1014, H);
	DDX_Control(pDX, IDC_EDIT4, S);
	DDX_Control(pDX, 1015, V);
}


BOOL PsDlgColor::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect theRect;
	GetDlgItem(IDC_COLORPICKER)->GetWindowRect(&theRect);
	ScreenToClient(&theRect);
	m_wndBulleted.Create(WS_CHILD | WS_VISIBLE | WS_EX_CLIENTEDGE, theRect, this, IDC_COLORPICKER, 1);

	theRect;
	GetDlgItem(IDC_COLORWELL)->GetWindowRect(&theRect);
	ScreenToClient(&theRect);
	m_wndWell.Create(WS_CHILD | WS_VISIBLE | WS_EX_CLIENTEDGE, theRect, this, IDC_COLORWELL, 1);
	m_wndWell.parent = this;

	theRect;
	GetDlgItem(IDC_COLORSLIDER)->GetWindowRect(&theRect);
	ScreenToClient(&theRect);
	m_wndSlider.Create(WS_CHILD | WS_VISIBLE | WS_EX_CLIENTEDGE, theRect, this, IDC_COLORSLIDER, 1);

	m_wndBulleted.pNotifyWnd = &m_wndSlider;
	m_wndBulleted.pNotifyWellWnd = &m_wndWell;

	m_wndBulleted.SetDataValue(modes_hsv_hue | modes_reverse);
	m_wndSlider.SetDataValue(modes_hsv_hue | modes_reverse);
	m_wndSlider.pNotifyWnd = &m_wndBulleted;
	m_wndWell.pNotifyWnd = &m_wndBulleted;

	CheckRadioButton(IDC_RADIO1, IDC_RADIO6, IDC_RADIO4);
	m_wndBulleted.SetColor(m_crColor);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

bool PsDlgColor::ShowModal()
{
	return CDialog::DoModal() == IDOK;
}

COLORREF PsDlgColor::GetColor()
{
	return m_wndWell.m_crColor;
}

int PsDlgColor::GetColorRValue()
{
	return GetRValue(m_wndWell.m_crColor);
}

int PsDlgColor::GetColorGValue()
{
	return GetGValue(m_wndWell.m_crColor);
}

int PsDlgColor::GetColorBValue()
{
	return GetBValue(m_wndWell.m_crColor);
}

void PsDlgColor::SetColor(COLORREF crColor)
{
	m_crColor = crColor;
}

void PsDlgColor::SetColor(int r, int g, int b)
{
	m_crColor = RGB(r, g, b);
}

void PsDlgColor::UpdateText()
{
	CWnd* c = GetFocus();
	char buffer[1024];

	float r = GetRValue(m_wndWell.m_crColor);
	float g = GetGValue(m_wndWell.m_crColor);
	float b = GetBValue(m_wndWell.m_crColor);

	if (c != &R)
	{
		sprintf(buffer, "%.0f", r);
		R.SetWindowText(buffer);
		R.Invalidate();
	}

	if (c != &G)
	{
		sprintf(buffer, "%.0f", g);
		G.SetWindowText(buffer);
		G.Invalidate();
	}

	if (c != &B)
	{
		sprintf(buffer, "%.0f", b);
		B.SetWindowText(buffer);
		B.Invalidate();
	}

	float min = 260.f;
	if (r < min) min = r;
	if (g < min) min = g;
	if (b < min) min = b;

	float max = -5.f;
	if (r > max) max = r;
	if (g > max) max = g;
	if (b > max) max = b;

	if (c != &H)
	{
		float Hv = 0.f;
		if (max == r)
		{
			Hv = 60.f * (g - b) / (max - min);
			if (g < b) Hv += 360.f;
		}
		else if (max == g)
			Hv = 60.f * (b - r) / (max - min) + 120.f;
		else if (max == b)
			Hv = 60.f * (r - g) / (max - min) + 240.f;
		sprintf(buffer, "%.0f", Hv);
		H.SetWindowText(buffer);
		H.Invalidate();
	}

	if (c != &S)
	{
		if (max <= 0.f) sprintf(buffer, "0");
		else sprintf(buffer, "%.0f", 100.f * (1.f - min / max));
		S.SetWindowText(buffer);
		S.Invalidate();
	}

	if (c != &V)
	{
		sprintf(buffer, "%.0f", 100.f * max / 255.f);
		V.SetWindowText(buffer);
		V.Invalidate();
	}

}

void PsDlgColor::OnRadio1()
{
	// TODO: Add your control notification handler code here
	COLORREF	old_color;

	old_color = m_wndBulleted.GetColor();
	m_wndBulleted.SetDataValue(modes_rgb_red | modes_reverse);
	m_wndSlider.SetDataValue(modes_rgb_red | modes_reverse);
	m_wndBulleted.SetColor(old_color);
}

void PsDlgColor::OnRadio2()
{
	// TODO: Add your control notification handler code here
	COLORREF	old_color;

	old_color = m_wndBulleted.GetColor();
	m_wndBulleted.SetDataValue(modes_rgb_green | modes_reverse);
	m_wndSlider.SetDataValue(modes_rgb_green | modes_reverse);
	m_wndBulleted.SetColor(old_color);
}

void PsDlgColor::OnRadio3()
{
	// TODO: Add your control notification handler code here
	COLORREF	old_color;

	old_color = m_wndBulleted.GetColor();
	m_wndBulleted.SetDataValue(modes_rgb_blue | modes_reverse);
	m_wndSlider.SetDataValue(modes_rgb_blue | modes_reverse);
	m_wndBulleted.SetColor(old_color);
}

void PsDlgColor::OnRadio4()
{
	// TODO: Add your control notification handler code here
	COLORREF	old_color;

	old_color = m_wndBulleted.GetColor();
	m_wndBulleted.SetDataValue(modes_hsv_hue | modes_reverse);
	m_wndSlider.SetDataValue(modes_hsv_hue | modes_reverse);
	m_wndBulleted.SetColor(old_color);
}

void PsDlgColor::OnRadio5()
{
	// TODO: Add your control notification handler code here
	COLORREF	old_color;

	old_color = m_wndBulleted.GetColor();
	m_wndBulleted.SetDataValue(modes_hsv_sat | modes_reverse);
	m_wndSlider.SetDataValue(modes_hsv_sat | modes_reverse);
	m_wndBulleted.SetColor(old_color);
}

void PsDlgColor::OnRadio6()
{
	COLORREF	old_color;

	old_color = m_wndBulleted.GetColor();
	// TODO: Add your control notification handler code here
	m_wndBulleted.SetDataValue(modes_hsv_value | modes_reverse);
	m_wndSlider.SetDataValue(modes_hsv_value | modes_reverse);
	m_wndBulleted.SetColor(old_color);
}

void PsDlgColor::OnEnChangeRGB()
{
	CWnd* c = GetFocus();
	if (c != &R && c != &G && c != &B) return;
	COLORREF old_color = m_wndWell.m_crColor;
	char buffer[1024];
	R.GetWindowText(buffer, 1024);
	float r = atoi(buffer);
	G.GetWindowText(buffer, 1024);
	float g = atoi(buffer);
	B.GetWindowText(buffer, 1024);
	float b = atoi(buffer);
	m_wndBulleted.SetColor(RGB(r, g, b));
}


void PsDlgColor::OnEnChangeHVS()
{
	CWnd* c = GetFocus();
	if (c != &H && c != &S && c != &V) return;
	char buffer[1024];

	H.GetWindowText(buffer, 1024);
	int h = atoi(buffer);
	S.GetWindowText(buffer, 1024);
	float s = atof(buffer) / 100.f;
	V.GetWindowText(buffer, 1024);
	float v = atof(buffer) / 100.f;

	int Hi = (h / 60) % 6;
	float f = h / 60.f - Hi;
	float p = v * (1.f - s);
	float q = v * (1.f - f * s);
	float t = v * (1.f - (1 - f) * s);

	float r = 0.f, g = 0.f, b = 0.f;
	switch (Hi)
	{
	case 0: r = v; g = t; b = p; break;
	case 1: r = q; g = v; b = p; break;
	case 2: r = p; g = v; b = t; break;
	case 3: r = p; g = q; b = v; break;
	case 4: r = t; g = p; b = v; break;
	case 5: r = v; g = p; b = q; break;
	}
	m_wndBulleted.SetColor(RGB(r * 255.f, g * 255.f, b * 255.f));
}

/////////////////////////////////////////////////////////////////////////////
// GTColorSelectorWnd

GTColorSelectorWnd::GTColorSelectorWnd()
{
	pNotifyWnd = NULL;
	pNotifyWellWnd = NULL;
	m_nCurMode = modes_hsv_hue;
	additional_component = -1;
	x_pos = -1;
	y_pos = -1;
	max_x = -1;
	max_y = -1;
	old_x = -1;
	old_y = -1;
	m_bNeedPaint = true;
	bmp_handle = NULL;
	m_bTracking = false;
	picker_cursor = AfxGetApp()->LoadCursor(IDC_COLOR);
	m_crSaveColor = RGB(0, 0, 0);
}

GTColorSelectorWnd::~GTColorSelectorWnd()
{
	if (bmp_handle)
		DeleteObject(bmp_handle);
}


BEGIN_MESSAGE_MAP(GTColorSelectorWnd, CWnd)
	//{{AFX_MSG_MAP(GTColorSelectorWnd)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CHAR()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_CANCELMODE()
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_FO_SELECTBULLETTYPEOK, OnSelectBulletOK)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// GTColorSelectorWnd message handlers
BOOL GTColorSelectorWnd::Create(DWORD dwStyle, CRect rcPos, CWnd* pParent, UINT nID, int nBulletType, BOOL bPopup)
{
	nBulletType;
	bPopup;
	LPVOID lp = (LPVOID)NULL;
	if (!CreateEx(0,
		AfxRegisterWndClass(CS_DBLCLKS, ::LoadCursor(NULL, IDC_ARROW)),
		_T("GTColorSelectorWnd"),
		dwStyle,
		rcPos.left,
		rcPos.top,
		rcPos.Width(),
		rcPos.Height(),
		pParent->GetSafeHwnd(),
		(HMENU)nID,
		lp))

	{
		// Not a lot we can do.
		TRACE0("Failed to create GTColorSelectorWnd\n");
		return FALSE;
	}
	pNotifyWnd = NULL;
	pNotifyWellWnd = NULL;
	SetDataValue(modes_rgb_red);
	UpdateWindow();
	return TRUE;
}

BOOL GTColorSelectorWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	pDC;
	return TRUE;
}

void GTColorSelectorWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	//	m_pBulletControl->OnLButtonDblClk(nFlags,point);
	CWnd::OnLButtonDblClk(nFlags, point);
}

void GTColorSelectorWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	POINT	pt = point;

	::SetFocus(m_hWnd);
	if (PtInRect(&m_rcInteraction, pt))
	{
		::SetCapture(m_hWnd);
		m_bTracking = true;
		OnMouseMove(nFlags, point);
	}
}

void GTColorSelectorWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bTracking)
	{
		ReleaseCapture();
		m_bTracking = false;
		// notify parent window
		COLORREF crColor = GetColor();
		m_crSaveColor = crColor;
		SendMessageToParent(crColor);
	}
	CWnd::OnLButtonUp(nFlags, point);
}

void GTColorSelectorWnd::OnMButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	CWnd::OnMButtonDblClk(nFlags, point);
}

void GTColorSelectorWnd::SetDataValue(IN int value)
{
	if (m_nCurMode == value)
		return;

	if ((value & modes_mask) < 0 ||
		(value & modes_mask) > modes_max)
		// invalid mode
		return;

	//	debug_state ("mode_set = %d", value);

	// current position is not preserved
	x_pos = 0;
	y_pos = 0;
	additional_component = 0;
	m_nCurMode = value & modes_mask;

	// blend should be drawn, because visual mode have changed
	m_bNeedPaint = true;

	max_x = max_picker[m_nCurMode][1];
	max_y = max_picker[m_nCurMode][2];
	Invalidate(FALSE);
}

void GTColorSelectorWnd::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	POINT	pt = point;

	OnSetFocus(this);
	if (PtInRect(&m_rcInteraction, pt))
	{
		::SetCapture(m_hWnd);
		m_bTracking = true;
		OnMouseMove(nFlags, point);
	}
	CWnd::OnMButtonDown(nFlags, point);
}

void GTColorSelectorWnd::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	CWnd::OnMButtonUp(nFlags, point);
}

void GTColorSelectorWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	nFlags;
	// TODO: Add your message handler code here and/or call default
	if (m_bTracking)
	{
		SetCursor(picker_cursor);

		// make sure x and y are inside m_rcPaint
		in_range(point.x, m_rcPaint.left, m_rcPaint.right);
		in_range(point.y, m_rcPaint.top, m_rcPaint.bottom);

		// scale x and y in [0, max_x] and [0, max_y];
		//	use ceil, otherwise there is a difference between cursor position
		//	and selection circle drawn
		point.x = (short)ceil((point.x - m_rcPaint.left) * (double)max_x / (m_rcPaint.right - m_rcPaint.left));
		point.y = (short)ceil((m_rcPaint.bottom - point.y) * (double)max_y / (m_rcPaint.bottom - m_rcPaint.top));
		if (point.x == x_pos && point.y == y_pos)
			// don't redraw if not necessary
			return;

		x_pos = point.x;
		y_pos = point.y;
		InvalidateRect(NULL, FALSE);
		// color has changed
		COLORREF crColor = GetColor();
		m_crSaveColor = crColor;
		SendMessageToParent(crColor);
	}
	else
	{
		POINT	pt = { point.x, point.y };
		if (PtInRect(&m_rcInteraction, pt))
			SetCursor(picker_cursor);
		else
			SetCursor(LoadCursor(NULL, IDC_ARROW));
	}
}

void GTColorSelectorWnd::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	//	m_pBulletControl->OnChar(nChar,nRepCnt,nFlags);
	CWnd::OnChar(nChar, nRepCnt, nFlags);
}

void GTColorSelectorWnd::OnDestroy()
{
	CWnd::OnDestroy();

	// TODO: Add your message handler code here

}

void GTColorSelectorWnd::OnPaint()
{
	CPaintDC dcPaint(this); // device context for painting
	HDC		hdc;

	// create compatible DC
	hdc = CreateCompatibleDC(dcPaint.m_hDC);
	if (hdc != NULL)
	{
		HBRUSH	bg_fill;

		SelectObject(hdc, bmp_handle);

		if (m_bNeedPaint || CONTRAST_MARKER)
		{
			old_x = -9999;
			old_y = -9999;

			bg_fill = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
			SelectObject(hdc, GetStockObject(NULL_PEN));
			SelectObject(hdc, bg_fill);
			Rectangle(hdc, 0, 0, m_rcFrame.right + 1, m_rcFrame.bottom + 1);
			SelectObject(hdc, GetStockObject(WHITE_BRUSH));
			DeleteObject(bg_fill);

			CRect rcTemp = m_rcFrame;
			rcTemp.DeflateRect(1, 1, 1, 1);
			GTDrawHelper m_Panel;
			m_Panel.DrawPanel(hdc, &rcTemp, GTDrawHelper::sunken | GTDrawHelper::blackbox);

			GdiFlush();
			OnDraw();
		}
		else
		{
			HPEN	pen_m_rcFrame = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNFACE));

			SelectObject(hdc, pen_m_rcFrame);
			SelectObject(hdc, GetStockObject(NULL_BRUSH));
			Rectangle(hdc, m_rcFrame.left, m_rcFrame.top,
				m_rcFrame.right, m_rcFrame.bottom);
			SelectObject(hdc, GetStockObject(BLACK_PEN));
			DeleteObject(pen_m_rcFrame);
		}


		// current position
		if (CONTRAST_MARKER)
		{
			int	x, y, size;
			COLORREF	c;
			int	val;
			HPEN	marker, old;

			// arc size
			size = MARKER_SIZE;

			IntersectClipRect(hdc, m_rcPaint.left, m_rcPaint.top,
				m_rcPaint.right + 1, m_rcPaint.bottom + 1);

			// current position
			x = x_pos * (m_rcPaint.right - m_rcPaint.left) / max_x + m_rcPaint.left;
			y = m_rcPaint.bottom - y_pos * (m_rcPaint.bottom - m_rcPaint.top) / max_y;

			c = GetColor();
			val = max(max(GetRValue(c), GetGValue(c)), GetBValue(c));
			if (val < 128)
				marker = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
			else
				marker = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
			old = (HPEN)SelectObject(hdc, marker);
			Arc(hdc, x - size, y - size, x + size + 1, y + size + 1,
				x, y - size, x, y - size);
			SelectObject(hdc, old);
			DeleteObject(marker);
		}
		else
		{
			int	x, y, size;

			// arc size
			size = MARKER_SIZE;

			SelectObject(hdc, GetStockObject(WHITE_PEN));
			IntersectClipRect(hdc, m_rcPaint.left, m_rcPaint.top,
				m_rcPaint.right + 1, m_rcPaint.bottom + 1);
			SetROP2(hdc, R2_XORPEN);

			// current position
			x = x_pos * (m_rcPaint.right - m_rcPaint.left) / max_x + m_rcPaint.left;
			y = m_rcPaint.bottom - y_pos * (m_rcPaint.bottom - m_rcPaint.top) / max_y;
			Arc(hdc, x - size, y - size, x + size + 1, y + size + 1,
				x, y - size, x, y - size);

			// old position
			x = old_x * (m_rcPaint.right - m_rcPaint.left) / max_x + m_rcPaint.left;
			y = m_rcPaint.bottom - old_y * (m_rcPaint.bottom - m_rcPaint.top) / max_y;
			Arc(hdc, x - size, y - size, x + size + 1, y + size + 1,
				x, y - size, x, y - size);

			old_x = x_pos;
			old_y = y_pos;

			// restore raster ops mode
			SetROP2(hdc, R2_COPYPEN);
		}

		// bitblt and delete cache DC
		GdiFlush();

		BitBlt(dcPaint.m_hDC, 0, 0, m_rcFrame.right, m_rcFrame.bottom, hdc, 0, 0, SRCCOPY);
		DeleteDC(hdc);

		// blend is cached in the bitmap until next mode change or component change
		m_bNeedPaint = false;
	}
}

void GTColorSelectorWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//	m_pBulletControl->OnRButtonDown(nFlags,point);
	CWnd::OnRButtonDown(nFlags, point);
}

void GTColorSelectorWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnRButtonUp(nFlags, point);
}

void GTColorSelectorWnd::OnCancelMode()
{
	CWnd::OnCancelMode();
	//	m_pBulletControl->OnCancelMode();	
}

void GTColorSelectorWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	nRepCnt;
	nFlags;
	int	x, y;

	// don't process if mouse is being tracked
	if (m_bTracking)
		return;

	x = x_pos;
	y = y_pos;
	switch (nChar)
	{
	case	VK_UP:
		y++; break;
	case	VK_LEFT:
		x--; break;
	case	VK_DOWN:
		y--; break;
	case	VK_RIGHT:
		x++; break;
	case	VK_PRIOR:
		x = max_x;
		y = max_y;
		break;
	case	VK_NEXT:
		x = max_x;
		y = 0;
		break;
	case	VK_HOME:
		x = 0;
		y = max_y;
		break;
	case	VK_END:
		x = 0;
		y = 0;
		break;
	default:
		return;
	}

	in_range(x, 0, max_x);
	in_range(y, 0, max_y);
	if (x == x_pos && y == y_pos)
		// don't redraw if not necessary
		return;

	x_pos = x;
	y_pos = y;
	InvalidateRect(NULL, FALSE);
	// color has changed
	COLORREF crColor = GetColor();
	m_crSaveColor = crColor;
	SendMessageToParent(crColor);
}

BOOL GTColorSelectorWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class

	return CWnd::PreCreateWindow(cs);
}

void GTColorSelectorWnd::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	CWnd::PreSubclassWindow();
}

void GTColorSelectorWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	SetRect(&m_rcFrame, 0, 0, cx, cy);
	SetRect(&m_rcInteraction, 1, 1, cx - 1, cy - 3);

	SetRect(&m_rcPanel, 1, 1, cx - 1, cy - 1);
	InflateRect(&m_rcPanel,
		-1,
		-1);

	CopyRect(&m_rcPaint, &m_rcPanel);
	InflateRect(&m_rcPaint, -1, -1);
	CopyRect(&m_rcInteraction, &m_rcPaint);


	// delete cache bitmap if window is resized
	if (bmp_handle != NULL)
		DeleteObject(bmp_handle);

	// create cache bitmap
	bmp_info.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmp_info.bmiHeader.biWidth = cx;
	bmp_info.bmiHeader.biHeight = cy;
	bmp_info.bmiHeader.biPlanes = 1;
	bmp_info.bmiHeader.biBitCount = 32;
	bmp_info.bmiHeader.biCompression = BI_RGB;
	bmp_info.bmiHeader.biSizeImage = cx * cy * 32 / 8;
	bmp_info.bmiHeader.biXPelsPerMeter =
		bmp_info.bmiHeader.biYPelsPerMeter = 72 * 2 * 1000;
	bmp_info.bmiHeader.biClrUsed = 0;
	bmp_info.bmiHeader.biClrImportant = 0;
	bmp_data = NULL;
	bmp_handle = CreateDIBSection(
		NULL, &bmp_info, DIB_RGB_COLORS, (void**)&bmp_data, NULL, 0);
	if (bmp_handle == NULL)
	{
		return;
	}

	// blend should be drawn, because control size has changed
	m_bNeedPaint = true;
}

LONG GTColorSelectorWnd::OnSelectBulletOK(UINT wParam, LONG lParam)
{
	wParam;
	lParam;
	GTColorSliderWnd* slider = (GTColorSliderWnd*)pNotifyWnd;
	SetControlValue(slider->GetPosition());
	return 0L;
}

void GTColorSelectorWnd::OnSysColorChange()
{
	//	gfxData.OnSysColorChange();
	Invalidate(FALSE);
}

void GTColorSelectorWnd::SetXPos(int value)
{
	if (x_pos == value)
		return;
	x_pos = value;
	in_range(x_pos, 0, max_x);
	Invalidate(FALSE);
}

void GTColorSelectorWnd::SetYPos(int value)
{
	if (y_pos == value)
		return;
	y_pos = value;
	in_range(y_pos, 0, max_y);
	Invalidate(FALSE);
}

UINT GTColorSelectorWnd::OnGetDlgCode()
{
	// TODO: Add your message handler code here and/or call default
	ASSERT_VALID(this);

	UINT nDlgCode = DLGC_WANTARROWS | DLGC_WANTCHARS;
	nDlgCode |= DLGC_WANTTAB;

	return nDlgCode;
}

void GTColorSelectorWnd::OnDraw(void)
{
	switch (m_nCurMode)
	{
	case	modes_rgb_red:
	case	modes_rgb_green:
	case	modes_rgb_blue:	DoDrawRGB(); break;
	case	modes_hsv_hue:		DoDrawHSV_Hue(); break;
	case	modes_hsv_sat:		DoDrawHSV_Sat(); break;
	case	modes_hsv_value:	DoDrawHSV_Val(); break;
	}
}


void GTColorSelectorWnd::DoCalculateColors(COLORREF& lefttop, COLORREF& righttop, COLORREF& leftbottom, COLORREF& rightbottom)
{
	BYTE	c = (BYTE)((int)additional_component * 255 / max_rgb);

	switch (m_nCurMode)
	{
	case	modes_rgb_red:
		lefttop = RGB(c, 255, 0);
		righttop = RGB(c, 255, 255);
		leftbottom = RGB(c, 0, 0);
		rightbottom = RGB(c, 0, 255);
		break;

	case	modes_rgb_green:
		lefttop = RGB(255, c, 0);
		righttop = RGB(255, c, 255);
		leftbottom = RGB(0, c, 0);
		rightbottom = RGB(0, c, 255);
		break;

	case	modes_rgb_blue:
		lefttop = RGB(0, 255, c);
		righttop = RGB(255, 255, c);
		leftbottom = RGB(0, 0, c);
		rightbottom = RGB(255, 0, c);
		break;
	}
}


void GTColorSelectorWnd::DoDrawRGB(void)
{
	int		i;		// loop counters
	int		j;

	int		nSkip;	// number of pixels to skip after current row and before next one

	DWORD* p;		// pointer to first pixel from the blend & a loop pointer

	int		width = m_rcFrame.right - m_rcFrame.left;
	int		height = m_rcFrame.bottom - m_rcFrame.top;
	int		blend_width = m_rcPaint.right - m_rcPaint.left + 1;
	int		blend_height = m_rcPaint.bottom - m_rcPaint.top + 1;

	// left side components, extended with int_extend bits
	int		left_red, left_green, left_blue;
	// left side component advance, extended with int_extend bits
	int		left_red_adv, left_green_adv, left_blue_adv;
	int		right_red, right_green, right_blue;
	int		right_red_adv, right_green_adv, right_blue_adv;

	// current components, extended with int_extend bits
	int		red, green, blue;
	// advance, extended with int_extend bits
	int		red_adv, green_adv, blue_adv;

	COLORREF	lefttop, righttop, leftbottom, rightbottom;

	p = bmp_data + (height - m_rcPaint.bottom - 1) * width + m_rcPaint.left;
	nSkip = m_rcPaint.left + width - m_rcPaint.right - 1;

	DoCalculateColors(lefttop, righttop, leftbottom, rightbottom);

	left_red = scaled_red(leftbottom);
	left_green = scaled_green(leftbottom);
	left_blue = scaled_blue(leftbottom);
	left_red_adv = (scaled_red(lefttop) - left_red) / (blend_height - 1);
	left_green_adv = (scaled_green(lefttop) - left_green) / (blend_height - 1);
	left_blue_adv = (scaled_blue(lefttop) - left_blue) / (blend_height - 1);

	right_red = scaled_red(rightbottom);
	right_green = scaled_green(rightbottom);
	right_blue = scaled_blue(rightbottom);
	right_red_adv = (scaled_red(righttop) - right_red) / (blend_height - 1);
	right_green_adv = (scaled_green(righttop) - right_green) / (blend_height - 1);
	right_blue_adv = (scaled_blue(righttop) - right_blue) / (blend_height - 1);

	// outer loop - rows
	i = blend_height;
	while (i--)
	{
		red = left_red;
		green = left_green;
		blue = left_blue;
		red_adv = (right_red - red) / (blend_width - 1);
		green_adv = (right_green - green) / (blend_width - 1);
		blue_adv = (right_blue - blue) / (blend_width - 1);

		// inner loop - pixels @ each row
		j = blend_width;
		while (j--)
		{
			// in DIB bitmap values are 0BGR
			*p++ = RGB(
				blue >> int_extend,
				green >> int_extend,
				red >> int_extend);

			// advance to the next pixel
			red += red_adv; green += green_adv; blue += blue_adv;
		}

		// advance to the next row
		left_red += left_red_adv;
		left_green += left_green_adv;
		left_blue += left_blue_adv;
		right_red += right_red_adv;
		right_green += right_green_adv;
		right_blue += right_blue_adv;
		p += nSkip;
	}
}


void GTColorSelectorWnd::DoDrawHSV_Hue(void)
{
	int		i;		// loop counters

	DWORD* p;		// pointer to first pixel from the blend & a loop pointer
	int		nSkip;	// number of pixels to skip after current row and before next one

	int		width = m_rcFrame.right - m_rcFrame.left;
	int		height = m_rcFrame.bottom - m_rcFrame.top;
	int		blend_width = m_rcPaint.right - m_rcPaint.left + 1;
	int		blend_height = m_rcPaint.bottom - m_rcPaint.top + 1;

	double	hue;
	double	val, val_adv;

	p = bmp_data +
		(height - m_rcPaint.bottom - 1) * width + // top rows
		m_rcPaint.left; // pixels on first row
	nSkip = m_rcPaint.left +
		width - m_rcPaint.right - 1;

	// constant
	hue = additional_component / (double)scale_hsv_hue;

	// outer loop - rows
	i = blend_height;

	// initial and change
	val = 0.0;
	val_adv = 1.0 / HSV_LOOP_STEPS(i);

	while (i--)
	{
		GTDrawHelper m_Panel;
		m_Panel.HSV_SAT(p, blend_width, hue, val);
		val += val_adv;
		p += (blend_width + nSkip);
	}
}


void GTColorSelectorWnd::DoDrawHSV_Sat(void)
{
	int		i;		// loop counters

	int		nSkip;	// number of pixels to skip after current row and before next one

	DWORD* p;		// pointer to first pixel from the blend & a loop pointer


	int		width = m_rcFrame.right - m_rcFrame.left;
	int		height = m_rcFrame.bottom - m_rcFrame.top;
	int		blend_width = m_rcPaint.right - m_rcPaint.left + 1;
	int		blend_height = m_rcPaint.bottom - m_rcPaint.top + 1;

	double	sat;
	double	val, val_adv;

	p = bmp_data +
		(height - m_rcPaint.bottom - 1) * width + // top rows
		m_rcPaint.left; // pixels on first row
	nSkip = m_rcPaint.left +
		width - m_rcPaint.right - 1;

	// constant
	sat = additional_component / (double)scale_hsv_sat;

	// outer loop - rows
	i = blend_height;

	// initial and change
	val = 0.0;
	val_adv = 1.0 / HSV_LOOP_STEPS(i);

	while (i--)
	{
		GTDrawHelper m_Panel;
		m_Panel.HSV_HUE(p, blend_width, sat, val);
		val += val_adv;
		p += (blend_width + nSkip);
	}
}

COLORREF GTColorSelectorWnd::GetColor(void) const
{
	switch (m_nCurMode)
	{
	case	modes_rgb_red:
		return (RGB(
			additional_component * 255 / max_rgb_red,
			y_pos * 255 / max_rgb_green,
			x_pos * 255 / max_rgb_blue));

	case	modes_rgb_green:
		return (RGB(
			y_pos * 255 / max_rgb_red,
			additional_component * 255 / max_rgb_green,
			x_pos * 255 / max_rgb_blue));

	case	modes_rgb_blue:
		return (RGB(
			x_pos * 255 / max_rgb_red,
			y_pos * 255 / max_rgb_green,
			additional_component * 255 / max_rgb_blue));

	case	modes_hsv_hue:
	{
		GTDrawHelper m_Panel;
		return (m_Panel.HSV2RGB(
			additional_component / (double)scale_hsv_hue,
			x_pos / (double)max_x,
			y_pos / (double)max_y));
	}

	case	modes_hsv_sat:
	{
		GTDrawHelper m_Panel;
		return (m_Panel.HSV2RGB(
			x_pos / (double)scale_hsv_hue,
			additional_component / (double)scale_hsv_sat,
			y_pos / (double)max_y));
	}

	case	modes_hsv_value:
	{
		GTDrawHelper m_Panel;
		return (m_Panel.HSV2RGB(
			x_pos / (double)scale_hsv_hue,
			y_pos / (double)max_y,
			additional_component / (double)scale_hsv_value));
	}
	}
	return (RGB(0, 0, 0));
}


LRESULT GTColorSelectorWnd::SendMessageToParent(UINT code)
{
	UINT	control_id;
	HWND	parent_window;
	NMHDR	message1;

	ASSERT(m_hWnd != NULL);

	control_id = GetDlgCtrlID();
	if (control_id == 0)
		// failed to retrieve control's id
		return (-1);

	// setup structure
	message1.code = code;
	message1.hwndFrom = m_hWnd;
	message1.idFrom = control_id;

	if (pNotifyWnd != NULL)
	{
		// send message as notification to the controller window
		::SendMessage(pNotifyWnd->m_hWnd, WM_FO_SELECTBULLETTYPEOK, control_id, (LPARAM)&message1);
	}

	if (pNotifyWellWnd != NULL)
	{
		// send message as notification to the controller window
		::SendMessage(pNotifyWellWnd->m_hWnd, WM_FO_SELECTBULLETTYPEOK, control_id, (LPARAM)&message1);
	}

	parent_window = (HWND)GetParent();
	if (parent_window != NULL)
		// send WM_NOTIFY message to the parent window
		return (::SendMessage(parent_window, WM_FO_SELECTBULLETTYPEOK, control_id, (LPARAM)&message1));

	return (-1);
}

void GTColorSelectorWnd::SetColor(COLORREF value)
{
	int	x, y, comp;
	BYTE	red, green, blue;
	double	hue, sat, val;

	//	debug_state ("color_set = %x", value);

	red = GetRValue(value);
	green = GetGValue(value);
	blue = GetBValue(value);

	switch (m_nCurMode)
	{
	case	modes_rgb_red:
		x = blue * max_rgb_blue / 255;
		y = green * max_rgb_green / 255;
		comp = red * max_rgb_red / 255;
		break;

	case	modes_rgb_green:
		x = blue * max_rgb_blue / 255;
		y = red * max_rgb_red / 255;
		comp = green * max_rgb_green / 255;
		break;

	case	modes_rgb_blue:
		x = red * max_rgb_red / 255;
		y = green * max_rgb_green / 255;
		comp = blue * max_rgb_blue / 255;
		break;

	case	modes_hsv_hue:
	{
		GTDrawHelper m_Panel;
		m_Panel.RGB2HSV(
			red / (double)max_rgb_red,
			green / (double)max_rgb_green,
			blue / (double)max_rgb_blue,
			&hue, &sat, &val);
		x = (int)ceil(sat * scale_hsv_sat);
		y = (int)ceil(val * scale_hsv_value);
		comp = (int)ceil(hue * scale_hsv_hue);
	}
	break;

	case	modes_hsv_sat:
	{
		GTDrawHelper m_Panel;
		m_Panel.RGB2HSV(
			red / (double)max_rgb_red,
			green / (double)max_rgb_green,
			blue / (double)max_rgb_blue,
			&hue, &sat, &val);
		x = (int)ceil(hue * scale_hsv_hue);
		y = (int)ceil(val * scale_hsv_value);
		comp = (int)ceil(sat * scale_hsv_sat);
	}
	break;

	case	modes_hsv_value:
	{
		GTDrawHelper m_Panel;
		m_Panel.RGB2HSV(
			red / (double)max_rgb_red,
			green / (double)max_rgb_green,
			blue / (double)max_rgb_blue,
			&hue, &sat, &val);
		x = (int)ceil(hue * scale_hsv_hue);
		y = (int)ceil(sat * scale_hsv_sat);
		comp = (int)ceil(val * scale_hsv_value);
	}
	break;

	default:
		x = 0;
		y = 0;
		comp = 0;
	}

	if (x_pos == x && y_pos == y && additional_component == comp)
		return;

	ASSERT(x >= 0 && x <= max_x);
	ASSERT(y >= 0 && y <= max_x);
	ASSERT(comp >= 0 && comp <= max_picker[m_nCurMode][0]);

	// set redraw blend flag if component has changed
	m_bNeedPaint |= (additional_component != comp);
	x_pos = x;
	y_pos = y;
	additional_component = comp;
	COLORREF crColor = GetColor();
	m_crSaveColor = crColor;
	SendMessageToParent(crColor);
	//repaint();
	Invalidate(FALSE);
}

void GTColorSelectorWnd::SetControlValue(int value)
{
	if (value < 0 || value > max_picker[m_nCurMode][0])
		// new value is not in range
		return;

	additional_component = value;

	m_bNeedPaint = true;
	COLORREF crColor = GetColor();
	m_crSaveColor = crColor;
	SendMessageToParent(crColor);
	Invalidate(FALSE);
}

void GTColorSelectorWnd::DoDrawHSV_Val(void)
{
	int		i;		// loop counters

	int		nSkip;	// number of pixels to skip after current row and before next one

	DWORD* p;		// pointer to first pixel from the blend & a loop pointer


	int		width = m_rcFrame.right - m_rcFrame.left;
	int		height = m_rcFrame.bottom - m_rcFrame.top;
	int		blend_width = m_rcPaint.right - m_rcPaint.left + 1;
	int		blend_height = m_rcPaint.bottom - m_rcPaint.top + 1;

	double	sat;
	double	sat_adv;
	double	val;

	p = bmp_data +
		(height - m_rcPaint.bottom - 1) * width + // top rows
		m_rcPaint.left; // pixels on first row
	nSkip = m_rcPaint.left +
		width - m_rcPaint.right - 1;

	// constant
	val = additional_component / (double)scale_hsv_value;

	// outer loop - rows
	i = blend_height;
	sat = 0.0;
	sat_adv = 1.0 / HSV_LOOP_STEPS(i);

	while (i--)
	{
		GTDrawHelper m_Panel;
		m_Panel.HSV_HUE(p, blend_width, sat, val);
		sat += sat_adv;
		p += (blend_width + nSkip);
	}
}


BOOL GTColorSelectorWnd::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class

	return CWnd::OnNotify(wParam, lParam, pResult);
}
// GTColorSliderWnd.cpp : implementation file
//


/////////////////////////////////////////////////////////////////////////////
// GTColorSliderWnd

GTColorSliderWnd::GTColorSliderWnd()
{
	pNotifyWnd = NULL;
	// initialize
	m_nCurMode = 0;
	m_nCurLayout = 0x50017280;
	m_nStartValue = -1;
	m_nEndValue = -1;
	m_nCurPosition = -1;
	m_nPreviousPos = -1;
	m_nSteps = -1;
	m_nPageSizes = -1;
	m_nTriangle = 6;
	m_bTracking = false;
	bmp_handle = NULL;
	row_buffer = NULL;
	additional_components[0] =
		additional_components[1] =
		additional_components[2] =
		additional_components[3] = -1;
}

GTColorSliderWnd::~GTColorSliderWnd()
{
	if (bmp_handle)
		DeleteObject(bmp_handle);
	if (row_buffer)
		delete[] row_buffer;
}


BEGIN_MESSAGE_MAP(GTColorSliderWnd, CWnd)
	//{{AFX_MSG_MAP(GTColorSliderWnd)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CHAR()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_CANCELMODE()
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_FO_SELECTBULLETTYPEOK, OnSelectBulletOK)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// GTColorSliderWnd message handlers
BOOL GTColorSliderWnd::Create(DWORD dwStyle, CRect rcPos, CWnd* pParent, UINT nID, int nBulletType, BOOL bPopup)
{
	nBulletType;
	bPopup;
	LPVOID lp = (LPVOID)NULL;
	if (!CreateEx(0,
		AfxRegisterWndClass(CS_DBLCLKS, ::LoadCursor(NULL, IDC_ARROW)),
		_T("GTColorSliderWnd"),
		dwStyle,
		rcPos.left,
		rcPos.top,
		rcPos.Width(),
		rcPos.Height(),
		pParent->GetSafeHwnd(),
		(HMENU)nID,
		lp))

	{
		// Not a lot we can do.
		TRACE0("Failed to create GTColorSliderWnd\n");
		return FALSE;
	}
	pNotifyWnd = NULL;
	// no default mode; should be set using window style
	SetLayoutValue(0x50017280);
	SetDataValue(1);

	UpdateWindow();
	return TRUE;
}

BOOL GTColorSliderWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	pDC;
	return TRUE;
}

void GTColorSliderWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	CWnd::OnLButtonDblClk(nFlags, point);
}

void GTColorSliderWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	// TODO: Add your message handler code here and/or call default
	POINT	pt = point;
	::SetFocus(m_hWnd);
	if (PtInRect(&m_rcInteraction, pt))
	{
		::SetCapture(m_hWnd);
		m_bTracking = true;
		OnMouseMove(nFlags, point);
	}
}

LRESULT GTColorSliderWnd::SendMessageToParent(IN UINT code)
{
	UINT	control_id;
	HWND	parent_window;
	NMHDR	message1;

	ASSERT(m_hWnd != NULL);

	control_id = GetDlgCtrlID();
	if (control_id == 0)
		// failed to retrieve control's id
		return (-1);

	// setup structure
	message1.code = code;
	message1.hwndFrom = m_hWnd;
	message1.idFrom = control_id;

	if (pNotifyWnd != NULL)
	{
		// send message as notification to the controller window
		return (::SendMessage(pNotifyWnd->m_hWnd, WM_FO_SELECTBULLETTYPEOK, control_id, (LPARAM)&message1));
	}

	parent_window = (HWND)GetParent();
	if (parent_window != NULL)
		// send WM_NOTIFY message to the parent window
		return (::SendMessage(parent_window, WM_FO_SELECTBULLETTYPEOK, control_id, (LPARAM)&message1));

	return (-1);
}

void GTColorSliderWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_bTracking)
	{
		ReleaseCapture();
		m_bTracking = false;
		SendMessageToParent(m_nCurPosition);
	}
	CWnd::OnLButtonUp(nFlags, point);
}

void GTColorSliderWnd::OnMButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	CWnd::OnMButtonDblClk(nFlags, point);
}

void GTColorSliderWnd::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	CWnd::OnMButtonDown(nFlags, point);
}

void GTColorSliderWnd::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	CWnd::OnMButtonUp(nFlags, point);
}

void GTColorSliderWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	nFlags;
	// TODO: Add your message handler code here and/or call default
	double	d;
	int		new_pos;

	SetCursor(LoadCursor(NULL, IDC_ARROW));
	if (!m_bTracking)
		return;
	int x;
	int y;
	x = point.x;
	y = point.y;
	if (m_nCurLayout & layout_horizontal)
	{
		// horizontal
		in_range(x, m_rcPaint.left, m_rcPaint.right);

		d = static_cast <double> (x - m_rcPaint.left) /
			static_cast <double> (m_rcPaint.right - m_rcPaint.left);
	}
	else // if (mode & mode_vertical)
	{
		// vertical
		in_range(y, m_rcPaint.top, m_rcPaint.bottom);

		d = static_cast <double> (y - m_rcPaint.top) /
			static_cast <double> (m_rcPaint.bottom - m_rcPaint.top);
	}

	new_pos = static_cast <int> ((m_nEndValue - m_nStartValue) * d +
		(m_nEndValue > m_nStartValue ? 0.5 : -0.5)) + m_nStartValue;
	if (new_pos == m_nCurPosition)
		// don't redraw if not necessary
		return;

	m_nCurPosition = new_pos;
	InvalidateRect(NULL, FALSE);

	//	debug_state ("mouse_move, m_nCurPosition = %d in [%d, %d]", m_nCurPosition, m_nStartValue, m_nEndValue);

	SendMessageToParent(m_nCurPosition);

}

void GTColorSliderWnd::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	CWnd::OnChar(nChar, nRepCnt, nFlags);
}

void GTColorSliderWnd::OnDestroy()
{
	CWnd::OnDestroy();

	// TODO: Add your message handler code here

}

void GTColorSliderWnd::OnPaint()
{
	CPaintDC dcPaint(this); // device context for painting
	HDC		hdc;

	// create compatible DC
	hdc = CreateCompatibleDC(dcPaint.m_hDC);
	if (hdc != NULL)
	{
		HBRUSH	bg_fill;
		double	d;

		SelectObject(hdc, bmp_handle);

		// paint control's background
		bg_fill = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
		SelectObject(hdc, GetStockObject(NULL_PEN));
		SelectObject(hdc, bg_fill);
		Rectangle(hdc, 0, 0, m_rcFrame.right - m_rcFrame.left + 1, m_rcFrame.bottom - m_rcFrame.top + 1);
		SelectObject(hdc, GetStockObject(WHITE_BRUSH));
		DeleteObject(bg_fill);

		// paint blend and m_rcFrame
		GTDrawHelper m_Panel;
		m_Panel.DrawPanel(hdc, &m_rcPanel, GTDrawHelper::sunken | GTDrawHelper::blackbox);
		GdiFlush();
		OnDraw();

		d = (double)(m_nCurPosition - m_nStartValue) / (m_nEndValue - m_nStartValue);

		if (m_nCurLayout & layout_horizontal)
		{
			// horizontal slider
			int		x;

			x = (int)((m_rcPaint.right - m_rcPaint.left) * d);

			if (m_nCurLayout & layout_tr_top)
				GTDrawHelper m_Panel;
			m_Panel.DrawTriangle(hdc, m_rcPaint.left + x, m_rcPanel.top - 2,
				GTDrawHelper::bottom, m_nTriangle,
				GetSysColor(COLOR_WINDOWTEXT), GetSysColor(COLOR_WINDOW));
			if (m_nCurLayout & layout_tr_bottom)
				GTDrawHelper m_Panel;
			m_Panel.DrawTriangle(hdc, m_rcPaint.left + x, m_rcPanel.bottom + 2,
				GTDrawHelper::top, m_nTriangle,
				GetSysColor(COLOR_WINDOWTEXT), GetSysColor(COLOR_WINDOW));
		}
		else // if (m_nCurLayout & layout_vertical)
		{
			// vertical slider
			int	y;

			y = (int)((m_rcPaint.bottom - m_rcPaint.top) * d);

			if (m_nCurLayout & layout_tr_left)
				GTDrawHelper m_Panel;
			m_Panel.DrawTriangle(hdc, m_rcPanel.left - 2, m_rcPaint.top + y,
				GTDrawHelper::right, m_nTriangle,
				GetSysColor(COLOR_WINDOWTEXT), GetSysColor(COLOR_WINDOW));
			if (m_nCurLayout & layout_tr_right)
				GTDrawHelper m_Panel;
			m_Panel.DrawTriangle(hdc, m_rcPanel.right + 2, m_rcPaint.top + y,
				GTDrawHelper::left, m_nTriangle,
				GetSysColor(COLOR_WINDOWTEXT), GetSysColor(COLOR_WINDOW));
		}

		m_nPreviousPos = m_nCurPosition;

		// bitblt and delete cache DC
		GdiFlush();
		BitBlt(dcPaint.m_hDC, 0, 0,
			m_rcFrame.right - m_rcFrame.left, m_rcFrame.bottom - m_rcFrame.top, hdc, 0, 0, SRCCOPY);
		DeleteDC(hdc);
	}
}

void GTColorSliderWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CWnd::OnRButtonDown(nFlags, point);
}

void GTColorSliderWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnRButtonUp(nFlags, point);
}

void GTColorSliderWnd::OnCancelMode()
{
	CWnd::OnCancelMode();
}

void GTColorSliderWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	nFlags;
	nRepCnt;
	int		new_pos;
	int		delta;

	// don't process if mouse is being tracked
	if (m_bTracking)
		return;

	// process scroll operation
	new_pos = m_nCurPosition;
	delta = 0;
	switch (nChar)
	{
	case	VK_UP:
	case	VK_LEFT:
		delta = -m_nSteps; break;
	case	VK_DOWN:
	case	VK_RIGHT:
		delta = m_nSteps; break;
	case	VK_PRIOR:
		delta = -m_nPageSizes; break;
	case	VK_NEXT:
		delta = m_nPageSizes; break;
	case	VK_HOME:
		new_pos = m_nStartValue; break;
	case	VK_END:
		new_pos = m_nEndValue; break;
	default:
		return;
	}

	if (m_nEndValue > m_nStartValue)
	{
		new_pos += delta;
		in_range(new_pos, m_nStartValue, m_nEndValue);
	}
	else
	{
		new_pos -= delta;
		in_range(new_pos, m_nEndValue, m_nStartValue);
	}

	if (new_pos == m_nCurPosition)
		// don't redraw if not necessary
		return;

	m_nCurPosition = new_pos;
	SendMessageToParent(m_nCurPosition);
	InvalidateRect(NULL, FALSE);

	//	debug_state ("key_down, m_nCurPosition = %d in [%d, %d]", m_nCurPosition, m_nStartValue, m_nEndValue);

}

BOOL GTColorSliderWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class

	return CWnd::PreCreateWindow(cs);
}

void GTColorSliderWnd::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	CWnd::PreSubclassWindow();
}

void GTColorSliderWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	SetRect(&m_rcFrame, 0, 0, cx, cy);
	SetRect(&m_rcInteraction, 2, 2, cx - 3, cy - 3);
	CopyRect(&m_rcPanel, &m_rcInteraction);
	if (m_nCurLayout & layout_horizontal)
	{
		// horizontal slider
		if (m_nCurLayout & layout_tr_top)
			m_rcPanel.top += (m_nTriangle + 1);
		if (m_nCurLayout & layout_tr_bottom)
			m_rcPanel.bottom -= (m_nTriangle + 1);
		InflateRect(&m_rcPanel, -(m_nTriangle - 3), 0);
	}
	else // if (m_nCurLayout & layout_vertical)
	{
		// vertical slider
		if (m_nCurLayout & layout_tr_left)
			m_rcPanel.left += (m_nTriangle + 1);
		if (m_nCurLayout & layout_tr_right)
			m_rcPanel.right -= (m_nTriangle + 1);
		InflateRect(&m_rcPanel, 0, -(m_nTriangle - 3));
	}

	CopyRect(&m_rcPaint, &m_rcPanel);
	InflateRect(&m_rcPaint, -2, -2);

	if (m_nCurLayout & layout_horizontal)
		m_nBuffSize = m_rcPaint.right - m_rcPaint.left + 1;
	else
		m_nBuffSize = m_rcPaint.bottom - m_rcPaint.top + 1;
	if (row_buffer)
		delete[] row_buffer;
	row_buffer = new DWORD[m_nBuffSize];
	if (row_buffer == NULL)
		return;

	// create cache bitmap
	bmp_info.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmp_info.bmiHeader.biWidth = cx;
	bmp_info.bmiHeader.biHeight = cy;
	bmp_info.bmiHeader.biPlanes = 1;
	bmp_info.bmiHeader.biBitCount = 32;
	bmp_info.bmiHeader.biCompression = BI_RGB;
	bmp_info.bmiHeader.biSizeImage = cx * cy * 32 / 8;
	bmp_info.bmiHeader.biXPelsPerMeter =
		bmp_info.bmiHeader.biYPelsPerMeter = 72 * 2 * 1000;
	bmp_info.bmiHeader.biClrUsed = 0;
	bmp_info.bmiHeader.biClrImportant = 0;
	bmp_data = NULL;
	bmp_handle = CreateDIBSection(NULL, &bmp_info, DIB_RGB_COLORS, (void**)&bmp_data, NULL, 0);
	if (bmp_handle == NULL)
	{
		return;
	}
	return;

}

LONG GTColorSliderWnd::OnSelectBulletOK(UINT wParam, LONG lParam)
{
	wParam;
	lParam;
	// We receive this on a color change from one of the well buttons
	// or one of the embedded color wells.
	GTColorSelectorWnd* picker = (GTColorSelectorWnd*)pNotifyWnd;
	switch (m_nCurMode & modes_mask)
	{
	case	modes_rgb_red:
	case	modes_rgb_green:
	case	modes_rgb_blue:
		SetControlValue(0, picker->y_pos);
		SetControlValue(1, picker->x_pos);
		SetPosition(picker->GetControlValue());
		break;

	case	modes_hsv_hue:
	case	modes_hsv_sat:
	case	modes_hsv_value:
		SetControlValue(0, picker->x_pos);
		SetControlValue(1, picker->y_pos);
		SetPosition(picker->GetControlValue());
		break;
	}
	return 0L;
}

void GTColorSliderWnd::OnSysColorChange()
{
	//	gfxData.OnSysColorChange();
	Invalidate();
}

UINT GTColorSliderWnd::OnGetDlgCode()
{
	// TODO: Add your message handler code here and/or call default
	ASSERT_VALID(this);

	UINT nDlgCode = DLGC_WANTARROWS | DLGC_WANTCHARS;
	nDlgCode |= DLGC_WANTTAB;

	return nDlgCode;
}

void GTColorSliderWnd::SetLayoutValue(IN int value)
{
	m_nCurLayout = value;
	Invalidate(FALSE);
}

void GTColorSliderWnd::SetDataValue(IN int value)
{
	if ((value & modes_mask) < 0 ||
		(value & modes_mask) > modes_max)
		// invalid mode
		return;

	m_nCurMode = value;
	switch (m_nCurMode & modes_mask)
	{
	case	modes_rgb_red:
	case	modes_rgb_green:
	case	modes_rgb_blue:
		m_nStartValue = 0;
		m_nEndValue = max_rgb;
		m_nSteps = scale_rgb;
		m_nPageSizes = scale_rgb * 10;
		break;

	case	modes_hsv_hue:
		m_nStartValue = 0;
		m_nEndValue = max_hsv_hue;
		m_nSteps = scale_hsv_hue;
		m_nPageSizes = scale_hsv_hue * 10;
		break;

	case	modes_hsv_sat:
		m_nStartValue = 0;
		m_nEndValue = max_hsv_sat;
		m_nSteps = scale_hsv_sat / 100;
		m_nPageSizes = scale_hsv_sat / 10;
		break;

	case	modes_hsv_value:
		m_nStartValue = 0;
		m_nEndValue = max_hsv_value;
		m_nSteps = scale_hsv_value / 100;
		m_nPageSizes = scale_hsv_value / 10;
		break;
	}

	// position is not preserved
	m_nPreviousPos = -9999;
	m_nCurPosition = (m_nCurMode & modes_reverse ? m_nEndValue : m_nStartValue);

	// reinitialize components
	additional_components[0] =
		additional_components[1] =
		additional_components[2] =
		additional_components[3] = 0;

	Invalidate(FALSE);
}

void GTColorSliderWnd::SetStartValue(int value)
{
	if (m_nStartValue == value)
		return;
	m_nStartValue = value;
	if (m_nStartValue < m_nEndValue)
	{
		if (m_nCurPosition < m_nStartValue) m_nCurPosition = m_nStartValue;
		if (m_nCurPosition > m_nEndValue) m_nCurPosition = m_nEndValue;
	}
	else
	{
		if (m_nCurPosition < m_nEndValue) m_nCurPosition = m_nEndValue;
		if (m_nCurPosition > m_nStartValue) m_nCurPosition = m_nStartValue;
	}
	Invalidate(FALSE);
}

void GTColorSliderWnd::SetEndValue(int value)
{
	if (m_nEndValue == value)
		return;
	m_nEndValue = value;
	if (m_nStartValue < m_nEndValue)
	{
		if (m_nCurPosition < m_nStartValue) m_nCurPosition = m_nStartValue;
		if (m_nCurPosition > m_nEndValue) m_nCurPosition = m_nEndValue;
	}
	else
	{
		if (m_nCurPosition < m_nEndValue) m_nCurPosition = m_nEndValue;
		if (m_nCurPosition > m_nStartValue) m_nCurPosition = m_nStartValue;
	}
	Invalidate(FALSE);
}

void GTColorSliderWnd::SetPosition(int value)
{
	if (m_nCurMode & modes_reverse)
		// invert if neccessary
		value = m_nEndValue - value;
	if (m_nCurPosition == value)
		return;
	m_nCurPosition = value;
	if (m_nStartValue < m_nEndValue)
	{
		if (m_nCurPosition < m_nStartValue) m_nCurPosition = m_nStartValue;
		if (m_nCurPosition > m_nEndValue) m_nCurPosition = m_nEndValue;
	}
	else
	{
		if (m_nCurPosition < m_nEndValue) m_nCurPosition = m_nEndValue;
		if (m_nCurPosition > m_nStartValue) m_nCurPosition = m_nStartValue;
	}
	Invalidate(FALSE);
}

void GTColorSliderWnd::SetControlValue(int index, int value)
{
	if (index < 0 || index > 1)
		// invalid index
		return;
	if (additional_components[index] == value)
		return;
	if (value < 0 || value > max_slider[m_nCurMode & modes_mask][index + 1])
		// value out of range
		return;
	additional_components[index] = value;
	Invalidate(FALSE);
}


void GTColorSliderWnd::OnDraw(void)
{
	int	i;

	DWORD* prgb;	// pointer to first pixel from the blend
	int		nSkip;	// number of pixels to skip after current row and before next one

	DWORD* p;		// loop pointers

	int		width = m_rcFrame.right - m_rcFrame.left;
	int		height = m_rcFrame.bottom - m_rcFrame.top;
	int		blend_width = m_rcPaint.right - m_rcPaint.left + 1;
	int		blend_height = m_rcPaint.bottom - m_rcPaint.top + 1;

	//
	//	horizontal blend schema:
	//
	//	+--+--+--+--+--+--+
	//	|**|  |**|  |**|  | - selection m_rcFrame
	//	+--+--+--+--+--+--+
	//	|  |  |  |  |  |  | - blank
	//	+--+--+--+--+--+--+
	//	................... - triangle (triangle-size blend_height)
	//	+--+--+--+--+--+--+
	//	|  |  |  |  |  |  | - blank
	//	+--+--+--+--+--+--+
	//	|**|  |xx|xx|xx|xx| - relief m_rcFrame
	//	+--+--+--+--+--+--+
	//	|  |  |xx|XX|XX|XX| - black m_rcFrame
	//	+--+--+--+--+--+--+
	//	|**|  |xx|XX| X+--+-- blend's 1st row (prgb pointer)
	//	+--+--+--+--+--+--+
	//

	// fill row buffer
	switch (m_nCurMode & modes_mask)
	{
	case	modes_rgb_red:
	case	modes_rgb_green:
	case	modes_rgb_blue:	DoDrawRGB(); break;
	case	modes_hsv_hue:		DoDrawHSV_Hue(); break;
	case	modes_hsv_sat:		DoDrawHSV_Sat(); break;
	case	modes_hsv_value:	DoDrawHSV_Val(); break;
	default:
		// mode not supported?
		ASSERT(FALSE);
		return;
	}

	// reverse if necessary
	//	when slider is horizontal, x = 0 is at the left
	//	when slider is vertical, y = 0 is at the bottom
	if ((m_nCurLayout & layout_vertical) && !(m_nCurMode & modes_reverse) ||
		(m_nCurLayout & layout_horizontal) && (m_nCurMode & modes_reverse))
	{
		GTDrawHelper m_Panel;
		m_Panel.reverse(row_buffer, m_nBuffSize);
	}

	// prepare
	prgb = bmp_data +
		(height - m_rcPaint.bottom - 1) * width + // top rows
		m_rcPaint.left; // pixels on first row
	nSkip = m_rcPaint.left +
		width - m_rcPaint.right - 1;
	p = prgb;

	if (m_nCurLayout & layout_horizontal)
	{
		i = blend_height;
		while (i--)
		{
			GTDrawHelper m_Panel;
			m_Panel.copy(&p, row_buffer, blend_width), p += nSkip;
		}
	}
	else // if (m_nCurLayout & layout_vertical)
	{
		DWORD* source = row_buffer;

		i = blend_height;
		while (i--)
		{
			GTDrawHelper m_Panel;
			m_Panel.set(&p, *source++, blend_width), p += nSkip;
		}
	}
}


void GTColorSliderWnd::DoDrawRGB(void)
{
	COLORREF	start_color, end_color;
	BYTE	c1, c2;

	c1 = (BYTE)((int)additional_components[0] * 255 / max_rgb);
	c2 = (BYTE)((int)additional_components[1] * 255 / max_rgb);

	// setup start and end color depending of current mode
	switch (m_nCurMode & modes_mask)
	{
	case	modes_rgb_red:
		start_color = RGB(0, c1, c2);
		end_color = RGB(255, c1, c2);
		break;

	case	modes_rgb_green:
		start_color = RGB(c1, 0, c2);
		end_color = RGB(c1, 255, c2);
		break;

	case	modes_rgb_blue:
		start_color = RGB(c1, c2, 0);
		end_color = RGB(c1, c2, 255);
		break;

	default:
		ASSERT(FALSE);
		return;
	}

	GTDrawHelper m_Panel;
	m_Panel.GetRGB(row_buffer, m_nBuffSize, start_color, end_color);
}


void GTColorSliderWnd::DoDrawHSV_Hue(void)
{
	GTDrawHelper m_Panel;
	m_Panel.HSV_HUE(row_buffer, m_nBuffSize,
		1.0f,//additional_components [0] / (double) scale_hsv_sat,
		1.0f);//additional_components [1] / (double) scale_hsv_value);
}


void GTColorSliderWnd::DoDrawHSV_Sat(void)
{
	GTDrawHelper m_Panel;
	m_Panel.HSV_SAT(row_buffer, m_nBuffSize,
		additional_components[0] / (double)scale_hsv_hue,
		additional_components[1] / (double)scale_hsv_value);
}


void GTColorSliderWnd::DoDrawHSV_Val(void)
{
	GTDrawHelper m_Panel;
	m_Panel.HSV_VAL(row_buffer, m_nBuffSize,
		additional_components[0] / (double)scale_hsv_hue,
		additional_components[1] / (double)scale_hsv_sat);
}

BOOL GTColorSliderWnd::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class

	return CWnd::OnNotify(wParam, lParam, pResult);
}

// GTColorWellWnd.cpp : implementation file
//

/////////////////////////////////////////////////////////////////////////////
// GTColorWellWnd

GTColorWellWnd::GTColorWellWnd()
{
	pNotifyWnd = NULL;
	m_crColor = RGB(128, 255, 0);
}

GTColorWellWnd::~GTColorWellWnd()
{
}


BEGIN_MESSAGE_MAP(GTColorWellWnd, CWnd)
	//{{AFX_MSG_MAP(GTColorWellWnd)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CHAR()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_CANCELMODE()
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_FO_SELECTBULLETTYPEOK, OnSelectBulletOK)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// GTColorWellWnd message handlers
BOOL GTColorWellWnd::Create(DWORD dwStyle, CRect rcPos, CWnd* pParent, UINT nID, int nBulletType, BOOL bPopup)
{
	nBulletType;
	bPopup;
	LPVOID lp = (LPVOID)NULL;
	if (!CreateEx(0,
		AfxRegisterWndClass(CS_DBLCLKS, ::LoadCursor(NULL, IDC_ARROW)),
		_T("GTColorWellWnd"),
		dwStyle,
		rcPos.left,
		rcPos.top,
		rcPos.Width(),
		rcPos.Height(),
		pParent->GetSafeHwnd(),
		(HMENU)nID,
		lp))

	{
		// Not a lot we can do.
		TRACE0("Failed to create GTColorWellWnd\n");
		return FALSE;
	}
	pNotifyWnd = pParent;

	UpdateWindow();
	return TRUE;
}

BOOL GTColorWellWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	pDC;
	return TRUE;
}

void GTColorWellWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	CWnd::OnLButtonDblClk(nFlags, point);
}

void GTColorWellWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetCapture();
	::SetFocus(m_hWnd);

	OnMouseMove(nFlags, point);
}

void GTColorWellWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (GetCapture() == this)
		::ReleaseCapture();
	CWnd::OnLButtonUp(nFlags, point);
}

void GTColorWellWnd::OnMButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	CWnd::OnMButtonDblClk(nFlags, point);
}

void GTColorWellWnd::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	CWnd::OnMButtonDown(nFlags, point);
}

void GTColorWellWnd::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	CWnd::OnMButtonUp(nFlags, point);
}

void GTColorWellWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (GetCapture() != this)
	{
		return;
	}
	CWnd::OnMouseMove(nFlags, point);
}

void GTColorWellWnd::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	CWnd::OnChar(nChar, nRepCnt, nFlags);
}

void GTColorWellWnd::OnDestroy()
{
	CWnd::OnDestroy();

	// TODO: Add your message handler code here

}

void GTColorWellWnd::OnPaint()
{
	CPaintDC dcPaint(this); // device context for painting
	CRect frame;
	GetClientRect(frame);

	CDC dc;
	dc.CreateCompatibleDC(&dcPaint);
	CBitmap bmpMem;
	bmpMem.CreateCompatibleBitmap(&dcPaint, frame.Width(), frame.Height());
	CBitmap* pBmpOld = dc.SelectObject(&bmpMem);

	HDC		hdc = dc.m_hDC;
	HBRUSH	fill_brush, old_brush;
	COLORREF color;
	color = m_crColor;
	fill_brush = CreateSolidBrush(color);
	old_brush = (HBRUSH)SelectObject(hdc, fill_brush);
	SelectObject(hdc, GetStockObject(NULL_PEN));
	Rectangle(hdc, frame.left + 2, frame.top + 2, frame.right, frame.bottom / 2);

	if (parent)
	{
		DeleteObject(fill_brush);
		fill_brush = CreateSolidBrush(parent->m_crColor);
		SelectObject(hdc, fill_brush);
		Rectangle(hdc, frame.left + 2, frame.top + frame.bottom / 2 - 1, frame.right, frame.bottom);
	}

	SelectObject(hdc, old_brush);
	DeleteObject(fill_brush);

	CRect rcTemp = frame;
	rcTemp.DeflateRect(0, 0, 1, 1);
	GTDrawHelper m_Panel;
	m_Panel.DrawPanel(hdc, &rcTemp, GTDrawHelper::sunken | GTDrawHelper::blackbox);

	dcPaint.BitBlt(frame.left, frame.top, frame.Width(), frame.Height(), &dc, 0, 0, SRCCOPY);

	dc.SelectObject(pBmpOld);
}

void GTColorWellWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CWnd::OnRButtonDown(nFlags, point);
}

void GTColorWellWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnRButtonUp(nFlags, point);
}

void GTColorWellWnd::OnCancelMode()
{
	CWnd::OnCancelMode();
}

void GTColorWellWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL GTColorWellWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class

	return CWnd::PreCreateWindow(cs);
}

void GTColorWellWnd::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	CWnd::PreSubclassWindow();
}

void GTColorWellWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	CRect rc;
	GetClientRect(&rc);

}

LONG GTColorWellWnd::OnSelectBulletOK(UINT wParam, LONG lParam)
{
	wParam;
	lParam;
	// We receive this on a color change from one of the well buttons
	// or one of the embedded color wells.
	GTColorSelectorWnd* picker = (GTColorSelectorWnd*)pNotifyWnd;

	m_crColor = picker->m_crSaveColor;

	if (parent) parent->UpdateText();

	Invalidate();
	return 0L;
}

void GTColorWellWnd::OnSysColorChange()
{
	//	gfxData.OnSysColorChange();
	Invalidate();
}

UINT GTColorWellWnd::OnGetDlgCode()
{
	// TODO: Add your message handler code here and/or call default
	ASSERT_VALID(this);

	UINT nDlgCode = DLGC_WANTARROWS | DLGC_WANTCHARS;
	nDlgCode |= DLGC_WANTTAB;

	return nDlgCode;
}
// GTDrawHelper.cpp: implementation of the GTDrawHelper class.
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GTDrawHelper::GTDrawHelper()
{

}

GTDrawHelper::~GTDrawHelper()
{

}

void GTDrawHelper::DrawPanel(HDC hdc, int left, int top, int right, int bottom, int flags)
{
	HPEN	highlight_pen, shadow_pen, old_pen;
	HBRUSH	old_brush;

	shadow_pen = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));
	highlight_pen = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_BTNHILIGHT));

	// old pen is preserved here
	if (flags & sunken)
		old_pen = (HPEN)SelectObject(hdc, shadow_pen);
	else
		old_pen = (HPEN)SelectObject(hdc, highlight_pen);

	// draw left and top side
	MoveToEx(hdc, left, bottom, NULL);
	LineTo(hdc, left, top);
	LineTo(hdc, right + 1, top);

	if (flags & sunken)
		SelectObject(hdc, highlight_pen);
	else
		SelectObject(hdc, shadow_pen);

	// bottom and right side
	MoveToEx(hdc, left + 1, bottom, NULL);
	LineTo(hdc, right, bottom);
	LineTo(hdc, right, top);

	SelectObject(hdc, GetStockObject(BLACK_PEN));

	if (flags & blackbox)
	{
		old_brush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
		Rectangle(hdc, left + 1, top + 1,
			right, bottom);
		SelectObject(hdc, old_brush);
	}

	// restore the old pen here
	SelectObject(hdc, old_pen);

	DeleteObject(shadow_pen);
	DeleteObject(highlight_pen);
}

void GTDrawHelper::DrawTriangle(HDC hdc, int x, int y, enum types type, int size, COLORREF line_color, COLORREF fill_color)
{
	POINT	pt[3];								// array of points used for polygon
	HPEN	stroke_pen, old_pen;
	HBRUSH	fill_brush, old_brush;

	size--;
	// fill the array with points according to triangle mode
	pt[0].x = x; pt[0].y = y;
	switch (type)
	{
	case	left:
		pt[1].x = x + size; pt[1].y = y + size;
		pt[2].x = x + size; pt[2].y = y - size;
		break;

	case	top:
		pt[1].x = x + size; pt[1].y = y + size;
		pt[2].x = x - size; pt[2].y = y + size;
		break;

	case	right:
		pt[1].x = x - size; pt[1].y = y - size;
		pt[2].x = x - size; pt[2].y = y + size;
		break;

	case	bottom:
		pt[1].x = x - size; pt[1].y = y - size;
		pt[2].x = x + size; pt[2].y = y - size;
		break;

	case	left_top:
		pt[1].x = x + size; pt[1].y = y;
		pt[2].x = x; pt[2].y = y + size;
		break;

	case	left_bottom:
		pt[1].x = x + size; pt[1].y = y;
		pt[2].x = x; pt[2].y = y - size;
		break;

	case	right_top:
		pt[1].x = x - size; pt[1].y = y;
		pt[2].x = x; pt[2].y = y + size;
		break;

	case	right_bottom:
		pt[1].x = x - size; pt[1].y = y;
		pt[2].x = x; pt[2].y = y - size;
		break;

	default:
		// unknown triangle orientation
//		debug_message ("base::draw_triangle: unknown type %d", type);
		ASSERT(FALSE);
		return;
	}

	stroke_pen = CreatePen(PS_SOLID, 0, line_color);
	fill_brush = CreateSolidBrush(fill_color);

	old_pen = (HPEN)SelectObject(hdc, stroke_pen);
	old_brush = (HBRUSH)SelectObject(hdc, fill_brush);

	Polygon(hdc, pt, 3);

	SelectObject(hdc, old_pen);
	SelectObject(hdc, old_brush);

	DeleteObject(stroke_pen);
	DeleteObject(fill_brush);
}

int GTDrawHelper::HSV2RGB(double hue, double sat, double value, double* red, double* green, double* blue)
{
	double	frac, coef1, coef2, coef3;
	double	intp;
	// hsv values valid?
	if (sat < 0.0 || sat > 1.0 || value < 0.0 || value > 1.0) return (-1);
	if (hue < 0.0 || hue > 360.0) return (-1);

	// gray?
	if (sat == 0.0)
		*red = *green = *blue = value;
	else
	{
		// hue (chromatic) 360 == hue 0
		if (hue == 360.0) hue = 0;
		hue = hue / 60; 						// hue in [0, 6)
		frac = modf(hue, &intp);				// split hue to integer and fraction
		coef1 = value * (1 - sat);
		coef2 = value * (1 - sat * frac);
		coef3 = value * (1 - sat * (1 - frac));
		switch ((int)intp)
		{
		case 0:	*red = value; *green = coef3; *blue = coef1; break;
		case 1:	*red = coef2; *green = value; *blue = coef1; break;
		case 2:	*red = coef1; *green = value; *blue = coef3; break;
		case 3:	*red = coef1; *green = coef2; *blue = value; break;
		case 4:	*red = coef3; *green = coef1; *blue = value; break;
		case 5:	*red = value; *green = coef1; *blue = coef2; break;
		}
	}
	return (0);
}

COLORREF GTDrawHelper::HSV2RGB(double hue, double sat, double value)
{
	double	red, green, blue;

	// ceil (a - 0.5) = round (a) = floor (a + 0.5)
	if (HSV2RGB(hue, sat, value, &red, &green, &blue) == 0)
		return (RGB(
			ceil(red * 255 - 0.5),
			ceil(green * 255 - 0.5),
			ceil(blue * 255 - 0.5)));
	return (RGB(0, 0, 0));
}

int GTDrawHelper::RGB2HSV(double red, double green, double blue, double* hue, double* sat, double* value)
{
	double max, min, delta;

	max = MAX(red, MAX(green, blue));
	min = MIN(red, MIN(green, blue));

	// check the rgb values to see if valid
	if (min < 0.0 || max > 1.0) return (-1); 	// out of range

	*value = max;								// calculate the value v

	if (max > 0.0)
		*sat = (max - min) / max;
	else
		// red = green = blue = 0
		*sat = 0.0;

	if (*sat == 0.0)
		*hue = 0.0;
	else
	{
		delta = max - min;
		if (red == max)
			// between yellow and magenta
			*hue = (green - blue) / delta;
		else if (green == max)
			// between cyan and yellow
			*hue = 2 + (blue - red) / delta;
		else
			// between magenta and cyan
			*hue = 4 + (red - green) / delta;

		// hue to degrees
		*hue = *hue * 60;
		if (*hue < 0)
			// make hue > 0
			*hue = *hue + 360;
	}

	ASSERT(*hue >= 0.0 && *hue <= 360.0);
	ASSERT(*sat >= 0.0 && *sat <= 1.0);
	ASSERT(*value >= 0.0 && *value <= 1.0);

	return (0);
}

void GTDrawHelper::GetRGB(DWORD* buffer, int samples, COLORREF start, COLORREF end)
{
	int	red, green, blue;
	int	red_adv, green_adv, blue_adv;

	red = scaled_red(start);
	green = scaled_green(start);
	blue = scaled_blue(start);
	red_adv = (scaled_red(end) - red) / (samples - 1);
	green_adv = (scaled_green(end) - green) / (samples - 1);
	blue_adv = (scaled_blue(end) - blue) / (samples - 1);

	while (samples--)
	{
		// set current pixel (in DIB bitmap format is BGR, not RGB!)
		*buffer++ = RGB(
			(BYTE)(blue >> int_extend),
			(BYTE)(green >> int_extend),
			(BYTE)(red >> int_extend));
		// advance color values to the next pixel
		red += red_adv;
		green += green_adv;
		blue += blue_adv;
	}
}


void GTDrawHelper::HSV_HUE(DWORD* buffer, int samples, double sat, double val_fp)
{
	// value, but as integer in [0, 255 << int_extend]
	int		val;

	// loop counter
	int		j;

	// coefficients and advances
	int		coef1, coef2, coef3;
	int		coef2_adv, coef3_adv;

	// current position and advance to the next one
	double	pos, pos_adv;

	//
	// hue increments in [0, 360); indirectly
	//	intp changes - 0, 1, 2, 3, 4, 5; indirectly (separate loops)
	//	frac increments in [0, 1) six times; indirectly (coefficients)
	// sat - const, in [0, 1]
	// val - const, in [0, (255 << int_extend)]
	//
	// coef1 => val * (1 - sat)              => const, = val * (1 - sat)
	// coef2 => val * (1 - sat * frac)       => changes from val to val * (1 - sat)
	// coef3 => val * (1 - sat * (1 - frac)) => changes from val * (1 - sat) to val
	//

	// constants
	val = (int)(val_fp * 255) << int_extend;
	coef1 = (int)(val * (1 - sat));

	// prepare
	pos = 0;
	pos_adv = (double)samples / 6.0;

	// hue in [0, 60)
	pos += pos_adv;
	j = (int)pos;
	HSV_SAT_INIT_0();
	while (j--) *buffer++ = HSV_0(), HSV_SAT_ADV_0();

	pos += pos_adv;
	j = (int)pos - (int)(1 * pos_adv);
	HSV_SAT_INIT_1();
	while (j--) *buffer++ = HSV_1(), HSV_SAT_ADV_1();

	pos += pos_adv;
	j = (int)pos - (int)(2 * pos_adv);
	HSV_SAT_INIT_2();
	while (j--) *buffer++ = HSV_2(), HSV_SAT_ADV_2();

	pos += pos_adv;
	j = (int)pos - (int)(3 * pos_adv);
	HSV_SAT_INIT_3();
	while (j--) *buffer++ = HSV_3(), HSV_SAT_ADV_3();

	pos += pos_adv;
	j = (int)pos - (int)(4 * pos_adv);
	HSV_SAT_INIT_4();
	while (j--) *buffer++ = HSV_4(), HSV_SAT_ADV_4();

	pos += (pos_adv + 0.1);	// + 0.1 because of floating-point math's rounding errors
	j = (int)pos - (int)(5 * pos_adv);
	HSV_SAT_INIT_5();
	while (j--) *buffer++ = HSV_5(), HSV_SAT_ADV_5();
}


void GTDrawHelper::HSV_SAT(DWORD* buffer, int samples, double hue, double val_fp)
{
	// value, but as integer in [0, 255 << int_extend]
	int		val;

	// loop counter
	int		j;

	// coefficients and advances
	signed int		coef1, coef2, coef3;
	signed int		coef1_adv, coef2_adv, coef3_adv;

	double	intp, frac;

	//
	// hue - const, in [0, 360)
	//	intp - const in 0, 1, 2, 3, 4, 5
	//	frac - const in [0, 1)
	// sat - increments, in [0, 1]; indirectly (coefficients)
	// val - const, in [0, (255 << int_extend)]
	//
	// coef1 => val * (1 - sat)              => changes from val to 0
	// coef2 => val * (1 - sat * frac)       => changes from val to val * (1 - frac)
	// coef3 => val * (1 - sat * (1 - frac)) => changes from val to val * frac
	//

	// constants
	val = (int)(val_fp * 255) << int_extend;
	frac = modf(hue / 60.0, &intp);

	// prepare
	j = samples;

	coef1 = val;
	coef1_adv = -coef1 / samples;
	coef2 = val;
	coef2_adv = (int)((1 - frac) * val - coef2) / samples;
	coef3 = val;
	coef3_adv = (int)(frac * val - coef3) / samples;

	switch ((int)intp)
	{
	case	0:
		while (j--) *buffer++ = HSV_0(), HSV_HUE_ADV_0();
		break;
	case	1:
		while (j--) *buffer++ = HSV_1(), HSV_HUE_ADV_1();
		break;
	case	2:
		while (j--) *buffer++ = HSV_2(), HSV_HUE_ADV_2();
		break;
	case	3:
		while (j--) *buffer++ = HSV_3(), HSV_HUE_ADV_3();
		break;
	case	4:
		while (j--) *buffer++ = HSV_4(), HSV_HUE_ADV_4();
		break;
	case	5:
		while (j--) *buffer++ = HSV_5(), HSV_HUE_ADV_5();
		break;
	}
}

void GTDrawHelper::HSV_VAL(DWORD* buffer, int samples, double hue, double sat)
{
	// loop counter
	int		j;

	// coefficients and advances
	signed int		coef1, coef2, coef3;
	signed int		coef1_adv, coef2_adv, coef3_adv;

	int		val, val_adv;
	int		val_max;

	double	intp, frac;

	//
	// hue - const, in [0, 360)
	//	intp - const in 0, 1, 2, 3, 4, 5
	//	frac - const in [0, 1)
	// sat - const, in [0, 1]
	// val - increments, in [0, (255 << int_extend)]; indirectly (coefficients)
	//
	// coef1 => val * (1 - sat)              => changes from 0 to val * (1 - sat)
	// coef2 => val * (1 - sat * frac)       => changes from 0 to val * (1 - sat * frac)
	// coef3 => val * (1 - sat * (1 - frac)) => changes from 0 to val * (1 - sat * (1 - frac))
	//

	// constants
	frac = modf(hue / 60.0, &intp);
	val_max = 255 << int_extend;

	// prepare
	j = samples;

	coef1 = 0;
	coef1_adv = (int)(val_max * (1 - sat)) / samples;
	coef2 = 0;
	coef2_adv = (int)(val_max * (1 - sat * frac)) / samples;
	coef3 = 0;
	coef3_adv = (int)(val_max * (1 - sat * (1 - frac))) / samples;
	val = 0;
	val_adv = val_max / samples;

	switch ((int)intp)
	{
	case	0:
		while (j--) *buffer++ = HSV_0(), HSV_HUE_ADV_0(), val += val_adv;
		break;
	case	1:
		while (j--) *buffer++ = HSV_1(), HSV_HUE_ADV_1(), val += val_adv;
		break;
	case	2:
		while (j--) *buffer++ = HSV_2(), HSV_HUE_ADV_2(), val += val_adv;
		break;
	case	3:
		while (j--) *buffer++ = HSV_3(), HSV_HUE_ADV_3(), val += val_adv;
		break;
	case	4:
		while (j--) *buffer++ = HSV_4(), HSV_HUE_ADV_4(), val += val_adv;
		break;
	case	5:
		while (j--) *buffer++ = HSV_5(), HSV_HUE_ADV_5(), val += val_adv;
		break;
	}
}

void GTDrawHelper::set(unsigned long* buffer, unsigned long value, size_t count)
{
	while (count--)
		*buffer++ = value;
}

void GTDrawHelper::set(unsigned long** buffer, unsigned long value, size_t count)
{
	unsigned long* p = *buffer;

	while (count--)
		*p++ = value;
	*buffer = p;
}

void GTDrawHelper::copy(unsigned long* target, const unsigned long* source, size_t count)
{
	while (count--)
		*target++ = *source++;
}

void GTDrawHelper::copy(unsigned long** target, const unsigned long* source, size_t count)
{
	unsigned long* p = *target;

	while (count--)
		*p++ = *source++;
	*target = p;
}

void GTDrawHelper::copy_reverse(unsigned long* target, const unsigned long* source, size_t count)
{
	source += (count - 1);
	while (count--)
		*target++ = *source--;
}

void GTDrawHelper::reverse(unsigned long* buffer, size_t count)
{
	unsigned long* p = buffer + count - 1;

	while (buffer < p)
	{
		*buffer ^= *p;
		*p ^= *buffer;
		*buffer++ ^= *p--;
	}
}
