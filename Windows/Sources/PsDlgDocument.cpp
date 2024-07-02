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
#include "PsDlgDocument.h"


// PsDlgDocument dialog

IMPLEMENT_DYNAMIC(PsDlgDocument, CDialog)

PsDlgDocument::PsDlgDocument(CWnd* pParent /*=NULL*/)
	: CDialog(PsDlgDocument::IDD, pParent)
{
	bNoDefault = false;
}

PsDlgDocument::~PsDlgDocument()
{
}

void PsDlgDocument::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, W);
	DDX_Control(pDX, IDC_COMBO1, W_combo);
	DDX_Control(pDX, IDC_EDIT2, H);
	DDX_Control(pDX, IDC_COMBO2, H_combo);
	DDX_Control(pDX, IDC_EDIT10, D);
	DDX_Control(pDX, IDC_COMBO5, D_combo);
	DDX_Control(pDX, IDC_COMBO6, F_combo);
}


BEGIN_MESSAGE_MAP(PsDlgDocument, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO6, &PsDlgDocument::OnCbnSelchangeCombo6)
	ON_CBN_SELCHANGE(IDC_COMBO1, &PsDlgDocument::OnCbnSelchangeCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO2, &PsDlgDocument::OnCbnSelchangeCombo2)
	ON_CBN_SELCHANGE(IDC_COMBO5, &PsDlgDocument::OnCbnSelchangeCombo5)
	ON_EN_CHANGE(IDC_EDIT1, &PsDlgDocument::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT2, &PsDlgDocument::OnEnChangeEdit2)
	ON_EN_CHANGE(IDC_EDIT10, &PsDlgDocument::OnEnChangeEdit10)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL PsDlgDocument::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetAvaibleUnits(W_combo);
	SetAvaibleUnits(H_combo);

	switch (getLanguage())
	{
	case French:
		D_combo.AddString("pixels/pouce");
		D_combo.AddString("pixels/cm");
		F_combo.AddString("Personnalisé");
		break;
	default:
		D_combo.AddString("pixels/inch");
		D_combo.AddString("pixels/cm");
		F_combo.AddString("Customized");
		break;
	}

	D_combo.SetCurSel(0);
	F_combo.AddString("A5");
	F_combo.AddString("A4");
	F_combo.AddString("A3");
	F_combo.AddString("A2");
	F_combo.AddString("A1");
	if (!bNoDefault)
	{
		F_combo.SetCurSel(2);
		OnCbnSelchangeCombo6();
	}
	else
	{
		F_combo.SetCurSel(0);
		Update();
	}
	return TRUE;
}

void PsDlgDocument::SetAvaibleUnits(CComboBox& c)
{
	c.AddString("pixels");
	switch (getLanguage())
	{
	case French:
		c.AddString("pouces");
		break;
	default:
		c.AddString("inchs");
		break;
	}
	c.AddString("cm");
	c.AddString("mm");
	c.AddString("points");
	c.AddString("picas");
	c.SetCurSel(3);
}


void PsDlgDocument::UpdateValue(int px, CEdit& e, CComboBox& c)
{
	CWnd* c_ = GetFocus();
	if (c_ == &e) return;
	float v;
	char buffer[1024];
	float dpi = this->dpi;
	switch (c.GetCurSel())
	{
	case 0:
		sprintf(buffer, "%d", px);
		e.SetWindowText(buffer);
		break;
	case 1:
		v = px / dpi;
		sprintf(buffer, "%.3f", v);
		e.SetWindowText(buffer);
		break;
	case 2:
		v = px / (dpi / 2.54f);
		sprintf(buffer, "%.1f", v);
		e.SetWindowText(buffer);
		break;
	case 3:
		v = px * 10.f / (dpi / 2.54f);
		sprintf(buffer, "%.3f", v);
		e.SetWindowText(buffer);
		break;
	case 4:
		v = px / dpi / 0.0138888f;
		sprintf(buffer, "%.1f", v);
		e.SetWindowText(buffer);
		break;
	case 5:
		v = px / dpi * 6.f;
		sprintf(buffer, "%.1f", v);
		e.SetWindowText(buffer);
		break;
	default:
		e.SetWindowText("?");
		break;
	}
}

int PsDlgDocument::GetValue(CEdit& e, CComboBox& c)
{
	char buffer[1024];
	e.GetWindowText(buffer, 1024);
	float v = atof(buffer);
	float dpi = this->dpi;
	switch (c.GetCurSel())
	{
	case 0: return v; break;
	case 1: return v * dpi; break;
	case 2: return v * (dpi / 2.54f); break;
	case 3: return v / 10.f * (dpi / 2.54f); break;
	case 4: return v * dpi * 0.0138888f; break;
	case 5: return v * dpi / 6.f; break;
	default: break;
	}
	return -1;
}

void PsDlgDocument::Update()
{
	OnCbnSelchangeCombo1();
	OnCbnSelchangeCombo2();
	OnCbnSelchangeCombo5();
}

void PsDlgDocument::OnCbnSelchangeCombo6()
{
	switch (F_combo.GetCurSel())
	{
	case 1:
		w = 1748;
		h = 2480;
		break;
	case 2:
		w = 2480;
		h = 3508;
		break;
	case 3:
		w = 3508;
		h = 4961;
		break;
	case 4:
		w = 4961;
		h = 7016;
		break;
	case 5:
		w = 7016;
		h = 9921;
		break;
	default:
		break;
	}
	dpi = 300;
	Update();
}

void PsDlgDocument::OnCbnSelchangeCombo1()
{
	UpdateValue(w, W, W_combo);
}

void PsDlgDocument::OnCbnSelchangeCombo2()
{
	UpdateValue(h, H, H_combo);
}

void PsDlgDocument::OnCbnSelchangeCombo5()
{
	char buffer[1024];
	switch (D_combo.GetCurSel())
	{
	case 0: sprintf(buffer, "%d", dpi); break;
	case 1:
		float v = dpi / 2.54f;
		sprintf(buffer, "%.2f", v);
		break;
	}
	D.SetWindowText(buffer);
}

void PsDlgDocument::OnEnChangeEdit1()
{
	if (GetFocus() != &W)
		return;
	w = GetValue(W, W_combo);
	F_combo.SetCurSel(0);
}

void PsDlgDocument::OnEnChangeEdit2()
{
	if (GetFocus() != &H)
		return;
	h = GetValue(H, H_combo);
	F_combo.SetCurSel(0);
}

void PsDlgDocument::OnEnChangeEdit10()
{
	if (GetFocus() != &D)
		return;

	char buffer[1024];
	D.GetWindowText(buffer, 1024);
	switch (D_combo.GetCurSel())
	{
	case 0: dpi = atof(buffer); break;
	case 1: dpi = atof(buffer) * 2.54f; break;
	}
	F_combo.SetCurSel(0);
}

void PsDlgDocument::OnClose()
{
	Update();
	CDialog::OnClose();
}
