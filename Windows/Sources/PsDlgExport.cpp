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
#include "PsDlgExport.h"
#include "PatternshopView.h"
#include "PsProjectController.h"
#include "PsController.h"
#include "PsDlgOpen.h"
#include "PsSoftRender.h"
#include "PsMaths.h"

void Get_WRITE_FORMATS(char*);

IMPLEMENT_DYNAMIC(PsDlgExport, CDialog)

PsDlgExport::PsDlgExport(CWnd* pParent /*=NULL*/)
	: CDialog(PsDlgExport::IDD, pParent), PsDlgExportCx()
{
	// dimension des zones
	exportZone.x = 12;
	exportZone.y = 20;
	exportZone.width = 370;
	exportZone.height = 360;
	previewZone.x = 396;
	previewZone.y = 150;
	previewZone.width = 325;
	previewZone.height = 210;
}

PsDlgExport::~PsDlgExport()
{
}

void PsDlgExport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT9, Z);
	DDX_Control(pDX, IDC_EDIT7, H);
	DDX_Control(pDX, IDC_EDIT8, W);
	DDX_Control(pDX, IDC_EDIT11, DpiEdit);
	DDX_Control(pDX, IDC_COMBO3, H_combo);
	DDX_Control(pDX, IDC_COMBO4, W_combo);
	DDX_Control(pDX, IDC_CHECK1, cadriageCheck);
}


BEGIN_MESSAGE_MAP(PsDlgExport, CDialog)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_CBN_SELCHANGE(IDC_COMBO3, &PsDlgExport::OnCbnSelchangeCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO4, &PsDlgExport::OnCbnSelchangeCombo1)
	ON_EN_CHANGE(IDC_EDIT9, &PsDlgExport::OnEnChangeEdit9)
	ON_EN_CHANGE(IDC_EDIT7, &PsDlgExport::OnEnChangeEdit7)
	ON_EN_CHANGE(IDC_EDIT8, &PsDlgExport::OnEnChangeEdit8)
	ON_EN_CHANGE(IDC_EDIT11, &PsDlgExport::OnEnChangeEdit11)
	ON_BN_CLICKED(IDC_CHECK1, &PsDlgExport::OnBnClickedCheck1)
	ON_BN_CLICKED(IDOK, &PsDlgExport::OnBnClickedOk)
END_MESSAGE_MAP()


BOOL PsDlgExport::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetAvaibleUnits(H_combo);
	SetAvaibleUnits(W_combo);

	cadriageCheck.SetCheck(BST_CHECKED);

	Update();

	return TRUE;
}

void PsDlgExport::SetAvaibleUnits(CComboBox& c)
{
	c.AddString("pouces");
	c.AddString("cm");
	c.AddString("mm");
	c.AddString("points");
	c.AddString("picas");
	c.AddString("pixels");
	c.SetCurSel(1);
}

int PsDlgExport::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!PsDlgExportCx::Initialize())
		return -1;

	return 0;
}

void DrawCadre(CPaintDC* dc, int x, int y, int w, int h)
{
	static CPen penBlack(PS_SOLID, 1, RGB(0, 0, 0));
	dc->SelectObject(&penBlack);
	dc->MoveTo(x, y);
	dc->LineTo(x + w, y);
	dc->LineTo(x + w, y + h);
	dc->LineTo(x, y + h);
	dc->LineTo(x, y);
}

void PsDlgExport::OnPaint()
{
	CPaintDC dc(this);

	//-- dessin du rendu de la matrice
	int x = exportZone.x + (exportZone.width - exportImage.buffer.GetWidth()) / 2;
	int y = exportZone.y + (exportZone.height - exportImage.buffer.GetHeight()) / 2;
	exportImage.buffer.BitBlt(dc.GetSafeHdc(), x, y);
	DrawCadre(&dc, x, y, exportImage.buffer.GetWidth(), exportImage.buffer.GetHeight());
	//--

	//-- prévisualisation
	x = previewZone.x + (previewZone.width - previewImage.buffer.GetWidth()) / 2;
	y = previewZone.y + (previewZone.height - previewImage.buffer.GetHeight()) / 2;
	previewImage.buffer.BitBlt(dc.GetSafeHdc(), x, y);
	DrawCadre(&dc, x, y, previewImage.buffer.GetWidth(), previewImage.buffer.GetHeight());
	//--

	//-- affichage du cadriage
	if (cadriageCheck.GetCheck() == BST_CHECKED)
	{
		int delta_h = previewImage.buffer.GetHeight() / 3;
		int delta_w = previewImage.buffer.GetWidth() / 3;
		DrawCadre(&dc, x, y + delta_h, previewImage.buffer.GetWidth(), delta_h);
		DrawCadre(&dc, x + delta_w, y, delta_w, previewImage.buffer.GetHeight());
	}
	//--
}

void PsDlgExport::OnBnClickedOk()
{
	char buffer3[1024];
	Get_WRITE_FORMATS(buffer3);
	PsDlgOpen	dialog(false, ".tif", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T(buffer3));
	if (dialog.DoModal() == IDOK)
		OnValidation(dialog.GetPathName());
	OnOK();
}

void PsDlgExport::Update()
{
	UpdateValue(w * z / 100.f, W, W_combo);
	UpdateValue(h * z / 100.f, H, H_combo);
	if (GetFocus() != &Z)
	{
		char buffer[1024];
		sprintf(buffer, "%.0f%%", z);
		Z.SetWindowText(buffer);
	}
	if (GetFocus() != &DpiEdit)
	{
		char buffer[1024];
		sprintf(buffer, "%.0f", dpi);
		DpiEdit.SetWindowText(buffer);
	}

	W.Invalidate();
	H.Invalidate();
	Z.Invalidate();
}

void PsDlgExport::UpdateValue(double px, CEdit& e, CComboBox& c)
{
	CWnd* c_ = GetFocus();
	if (c_ == &e) return;
	char buffer[1024];
	GetTextValue(px, c.GetCurSel(), buffer);
	e.SetWindowText(buffer);
}

double PsDlgExport::GetValue(CEdit& e, CComboBox& c)
{
	char buffer[1024];
	e.GetWindowText(buffer, 1024);
	return GetDoubleValue(c.GetCurSel(), buffer);
}

void PsDlgExport::OnCbnSelchangeCombo1()
{
	Update();
}

void PsDlgExport::OnEnChangeEdit9()
{
	CWnd* c = GetFocus();
	if (c != &Z) return;
	char buffer[1024];
	Z.GetWindowText(buffer, 1024);
	double z_ = atol(buffer);
	if (z_ >= 10)
		SetZ(z_);
}

void PsDlgExport::OnEnChangeEdit7()
{
	CWnd* c = GetFocus();
	if (c != &H) return;
	double h_ = GetValue(H, H_combo);
	if (h_ > 0)
		SetZ(h_ * 100.f / h);
}

void PsDlgExport::OnEnChangeEdit8()
{
	CWnd* c = GetFocus();
	if (c != &W) return;
	double w_ = GetValue(W, W_combo);
	if (w_ > 0)
		SetZ(w_ * 100.f / w);
}

void PsDlgExport::OnEnChangeEdit11()
{
	CWnd* c = GetFocus();
	if (c != &DpiEdit) return;
	char buffer[1024];
	DpiEdit.GetWindowText(buffer, 1024);
	double v = atof(buffer);
	if (v >= 10)
	{
		dpi = v;
		Update();
	}
}

void PsDlgExport::OnBnClickedCheck1()
{
	Invalidate();
}


