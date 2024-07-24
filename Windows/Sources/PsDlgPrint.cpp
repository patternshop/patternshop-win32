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
#include "PsDlgPrint.h"
#include "PatternshopView.h"
#include "PsProjectController.h"
#include "PsController.h"

// PsDlgPrint dialog

IMPLEMENT_DYNAMIC(PsDlgPrint, CDialog)

PsDlgPrint::PsDlgPrint(CWnd* pParent /*=NULL*/)
	: CDialog(PsDlgPrint::IDD, pParent)
{

}

PsDlgPrint::~PsDlgPrint()
{
}

void PsDlgPrint::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, Y);
	DDX_Control(pDX, IDC_EDIT2, X);
	DDX_Control(pDX, IDC_EDIT9, Z);
	DDX_Control(pDX, IDC_EDIT7, H);
	DDX_Control(pDX, IDC_EDIT8, W);

	DDX_Control(pDX, IDC_COMBO2, X_combo);
	DDX_Control(pDX, IDC_COMBO1, Y_combo);
	DDX_Control(pDX, IDC_COMBO3, H_combo);
	DDX_Control(pDX, IDC_COMBO4, W_combo);
	DDX_Control(pDX, IDC_CHECK2, AutoFit);
	DDX_Control(pDX, IDC_CHECK1, AutoCenter);
	DDX_Control(pDX, IDC_CHECK3, ShowBox);
}


BEGIN_MESSAGE_MAP(PsDlgPrint, CDialog)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_CBN_SELCHANGE(IDC_COMBO1, &PsDlgPrint::OnCbnSelchangeCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO2, &PsDlgPrint::OnCbnSelchangeCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO3, &PsDlgPrint::OnCbnSelchangeCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO4, &PsDlgPrint::OnCbnSelchangeCombo1)
	ON_EN_CHANGE(IDC_EDIT1, &PsDlgPrint::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT2, &PsDlgPrint::OnEnChangeEdit2)
	ON_EN_CHANGE(IDC_EDIT9, &PsDlgPrint::OnEnChangeEdit9)
	ON_EN_CHANGE(IDC_EDIT7, &PsDlgPrint::OnEnChangeEdit7)
	ON_EN_CHANGE(IDC_EDIT8, &PsDlgPrint::OnEnChangeEdit8)
	ON_BN_CLICKED(IDC_CHECK2, &PsDlgPrint::OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_CHECK1, &PsDlgPrint::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK3, &PsDlgPrint::OnBnClickedCheck3)
END_MESSAGE_MAP()

int PsDlgPrint::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	if (buffer.IsNull())
		buffer.Create(200, 250, 24);
	PrepareMiniImage();
	return 0;
}

BOOL PsDlgPrint::OnInitDialog()
{
	if (!PsController::Instance().project)
		return FALSE;
	PsProjectController* project = PsController::Instance().project;

	CDialog::OnInitDialog();
	SetAvaibleUnits(Y_combo);
	SetAvaibleUnits(X_combo);
	SetAvaibleUnits(H_combo);
	SetAvaibleUnits(W_combo);
	ShowBox.SetCheck(BST_CHECKED);

	x = 0;
	y = 0;
	w = project->GetWidth();
	h = project->GetHeight();
	z = 100;

	// Format A4 vertical (FIXME)
	format_w = 8.2666667 * project->GetDpi();
	format_h = 11.693333 * project->GetDpi();

	// Auto fit
	//---------
	AutoFit.SetCheck(BST_CHECKED);
	OnBnClickedCheck2();

	Update();

	return TRUE;
}

void PsDlgPrint::SetAvaibleUnits(CComboBox& c)
{
	c.AddString("pouces");
	c.AddString("cm");
	c.AddString("mm");
	c.AddString("points");
	c.AddString("picas");
	c.SetCurSel(1);
}

void PsDlgPrint::Update()
{
	UpdateValue(x, X, X_combo);
	UpdateValue(y, Y, Y_combo);
	UpdateValue((int)(w * z / 100.f), W, W_combo);
	UpdateValue((int)(h * z / 100.f), H, H_combo);
	if (GetFocus() != &Z)
	{
		char buffer[1024];
		sprintf(buffer, "%.0f%%", z);
		Z.SetWindowText(buffer);
	}
	UpdateMiniImage();
	Invalidate();
}

void PsDlgPrint::OnPaint()
{
	CPaintDC dc(this);
	buffer.BitBlt(dc.GetSafeHdc(), 10, 25);
}


void PsDlgPrint::PrepareMiniImage()
{
	if (!PsController::Instance().project)
		return;

	PsRender& renderer = PsController::Instance().project->renderer;

	int size_x = renderer.size_x;
	int size_y = renderer.size_y;
	float scroll_x = renderer.scroll_x;
	float scroll_y = renderer.scroll_y;
	float x1 = renderer.x1;
	float x2 = renderer.x2;
	float y1 = renderer.y1;
	float y2 = renderer.y2;
	float zoom = renderer.zoom;
	bool showbox = PsController::Instance().GetOption(PsController::OPTION_BOX_SHOW);
	bool showmat = PsController::Instance().GetOption(PsController::OPTION_HIGHLIGHT_SHOW);

	PsController::Instance().SetOption(PsController::OPTION_BOX_SHOW, false);
	PsController::Instance().SetOption(PsController::OPTION_HIGHLIGHT_SHOW, false);
	renderer.SetSize(buffer.GetWidth(), buffer.GetHeight());
	renderer.CenterView();
	PsController::Instance().active->UpdateNow();

	PsRect z; // calcul de la zone de l'image
	z.left = (0 - renderer.x1) / (renderer.x2 - renderer.x1) * buffer.GetWidth();
	z.right = (renderer.doc_x - renderer.x1) / (renderer.x2 - renderer.x1) * buffer.GetWidth();
	z.bottom = buffer.GetHeight() - (renderer.doc_y - renderer.y1) / (renderer.y2 - renderer.y1) * buffer.GetHeight();
	z.top = buffer.GetHeight() - (0 - renderer.y1) / (renderer.y2 - renderer.y1) * buffer.GetHeight();
	int r_size_x = z.right - z.left;
	int r_size_y = z.bottom - z.top;
	r_zoom = renderer.zoom;

	CDC dc;
	dc.CreateCompatibleDC(NULL);
	if (!original.IsNull()) original.Destroy();
	original.Create(r_size_x - 1, r_size_y - 1, 24);
	dc.SelectObject(original);
	CDC dc2;
	CImage& source = PsController::Instance().active->m_buffer.buffer;
	dc2.CreateCompatibleDC(NULL);
	dc2.SelectObject(source);
	dc.BitBlt(0, 0, r_size_x - 1, r_size_y - 1, &dc2, z.left + 1,
		source.GetHeight() - buffer.GetHeight() + z.top + 1, SRCCOPY);
	dc2.DeleteDC();

	PsController::Instance().SetOption(PsController::OPTION_HIGHLIGHT_SHOW, showmat);
	PsController::Instance().SetOption(PsController::OPTION_BOX_SHOW, showbox);
	renderer.SetSize(size_x, size_y);
	renderer.SetZoom(zoom);
	renderer.SetScroll(scroll_x, scroll_y);
	PsController::Instance().active->UpdateNow();
}

void PsDlgPrint::UpdateMiniImage()
{
	CDC dc;
	dc.CreateCompatibleDC(NULL);
	dc.SelectObject(buffer);
	CBrush gray(GetSysColor(COLOR_BTNFACE));
	CPen penGray(PS_SOLID, 1, GetSysColor(COLOR_BTNFACE));
	dc.SelectObject(&gray);
	dc.SelectObject(&penGray);
	dc.Rectangle(0, 0, buffer.GetWidth(), buffer.GetHeight());

	int px = (buffer.GetWidth() - original.GetWidth()) / 2;
	int py = (buffer.GetHeight() - original.GetHeight()) / 2;
	CBrush white(RGB(255, 255, 255));
	CPen penWhite(PS_SOLID, 1, RGB(255, 255, 255));
	dc.SelectObject(&white);
	dc.SelectObject(&penWhite);
	dc.Rectangle(px, py, px + original.GetWidth(), py + original.GetHeight());
	dc.SelectObject(buffer);

	int r_x = 0;
	if (x / r_zoom < 0)
		r_x = FloatToInt(-x / r_zoom);

	int r_y = 0;
	if (y / r_zoom < 0)
		r_y = FloatToInt(-y / r_zoom);

	int l_x = 0;
	if (x > 0)
		l_x = FloatToInt(x / r_zoom * ((float)w / (float)format_w));

	int l_y = 0;
	if (y > 0)
		l_y = FloatToInt(y / r_zoom * ((float)h / (float)format_h));

	int o_width = original.GetWidth();
	int o_height = original.GetHeight();
	int d_width = FloatToInt(o_width * z / 100.f * w / format_w);
	int d_height = FloatToInt(o_height * z / 100.f * h / format_h);
	if (d_width + l_x > original.GetWidth())
	{
		d_width = original.GetWidth() - l_x;
		o_width = d_width / z * 100.f / w * format_w;
	}
	if (d_height + l_y > original.GetHeight())
	{
		d_height = original.GetHeight() - l_y;
		o_height = d_height / z * 100.f / h * format_h;
	}
	original.StretchBlt(dc, px + l_x, py + l_y, d_width, d_height, r_x, r_y, o_width, o_height);

	if (ShowBox.GetCheck() == BST_CHECKED)
	{
		CPen penBlackDash(PS_DASH, 1, RGB(0, 0, 0));
		/*
		LOGBRUSH LogBrush;
		LogBrush.lbColor = RGB(255, 0, 0);
		LogBrush.lbStyle = PS_SOLID;
		penBlackDash.CreatePen( PS_COSMETIC | PS_ALTERNATE , 1, &LogBrush, 0, NULL );*/
		dc.SelectObject(&penBlackDash);
		dc.MoveTo(px + l_x, py + l_y);
		dc.LineTo(px + l_x + d_width, py + l_y);
		dc.LineTo(px + l_x + d_width, py + l_y + d_height);
		dc.LineTo(px + l_x, py + l_y + d_height);
		dc.LineTo(px + l_x, py + l_y);
	}

	PsRect r;
	r.left = px - 1;
	r.top = py - 1;
	r.right = px + original.GetWidth() + 1;
	r.bottom = py + original.GetHeight() + 1;
	dc.DrawEdge(&r, EDGE_SUNKEN, BF_RECT);
}

void PsDlgPrint::UpdateValue(int px, CEdit& e, CComboBox& c)
{
	CWnd* c_ = GetFocus();
	if (c_ == &e) return;
	float v;
	char buffer[1024];
	float dpi = PsController::Instance().project->GetDpi();
	switch (c.GetCurSel())
	{
	case 0:
		v = px / dpi;
		sprintf(buffer, "%.3f", v);
		e.SetWindowText(buffer);
		break;
	case 1:
		v = px / (dpi / 2.54f);
		sprintf(buffer, "%.3f", v);
		e.SetWindowText(buffer);
		break;
	case 2:
		v = px * 10.f / (dpi / 2.54f);
		sprintf(buffer, "%.3f", v);
		e.SetWindowText(buffer);
		break;
	case 3:
		v = px / dpi / 0.0138888f;
		sprintf(buffer, "%.1f", v);
		e.SetWindowText(buffer);
		break;
	case 4:
		v = px / dpi * 6.f;
		sprintf(buffer, "%.1f", v);
		e.SetWindowText(buffer);
		break;
	default:
		e.SetWindowText("?");
		break;
	}
}

int PsDlgPrint::GetValue(CEdit& e, CComboBox& c)
{
	char buffer[1024];
	e.GetWindowText(buffer, 1024);
	float v = atof(buffer);
	float dpi = PsController::Instance().project->renderer.dpi;
	switch (c.GetCurSel())
	{
	case 0: return v * dpi; break;
	case 1: return v * (dpi / 2.54f); break;
	case 2: return v / 10.f * (dpi / 2.54f); break;
	case 3: return v * dpi * 0.0138888f; break;
	case 4: return v * dpi / 6.f; break;
	default: break;
	}
	return -1;
}

void PsDlgPrint::OnCbnSelchangeCombo1()
{
	Update();
}

void PsDlgPrint::OnEnChangeEdit2()
{
	CWnd* c = GetFocus();
	if (c != &X) return;
	x = GetValue(X, X_combo);
	UpdateMiniImage();
	Invalidate();
}

void PsDlgPrint::OnEnChangeEdit1()
{
	CWnd* c = GetFocus();
	if (c != &Y) return;
	y = GetValue(Y, Y_combo);
	UpdateMiniImage();
	Invalidate();
}

void PsDlgPrint::SetZ(float z_)
{
	z = z_;
	if (AutoCenter.GetCheck() == BST_CHECKED) OnBnClickedCheck1();
	else Update();
}

void PsDlgPrint::OnEnChangeEdit9()
{
	CWnd* c = GetFocus();
	if (c != &Z) return;
	char buffer[1024];
	Z.GetWindowText(buffer, 1024);
	float z_ = atol(buffer);
	if (z_ >= 10)
		SetZ(z_);
}

void PsDlgPrint::OnEnChangeEdit7()
{
	CWnd* c = GetFocus();
	if (c != &H) return;
	float h_ = GetValue(H, H_combo);
	if (h_ > 0)
		SetZ(h_ / h * 100.f);
}

void PsDlgPrint::OnEnChangeEdit8()
{
	CWnd* c = GetFocus();
	if (c != &W) return;
	float w_ = GetValue(W, W_combo);
	if (w_ > 0)
		SetZ(w_ / w * 100.f);
}

void PsDlgPrint::OnBnClickedCheck2()
{
	if (AutoFit.GetCheck() == BST_CHECKED)
	{
		Z.EnableWindow(FALSE);
		W.EnableWindow(FALSE);
		H.EnableWindow(FALSE);
		z = (float)format_w / (float)w * 100.f;
		if ((float)format_w / (float)w > (float)format_h / (float)h)
			z = (float)format_h / (float)h * 100.f;
		AutoCenter.SetCheck(BST_CHECKED);
		OnBnClickedCheck1();
	}
	else
	{
		Z.EnableWindow(TRUE);
		W.EnableWindow(TRUE);
		H.EnableWindow(TRUE);
	}
}

void PsDlgPrint::OnBnClickedCheck1()
{
	if (AutoCenter.GetCheck() == BST_CHECKED)
	{
		X.EnableWindow(FALSE);
		Y.EnableWindow(FALSE);
		x = (format_w - w * z / 100.f) / 2;
		y = (format_h - h * z / 100.f) / 2;
		Update();
	}
	else
	{
		X.EnableWindow(TRUE);
		Y.EnableWindow(TRUE);
	}
}

void PsDlgPrint::OnBnClickedCheck3()
{
	UpdateMiniImage();
	Invalidate();
}
