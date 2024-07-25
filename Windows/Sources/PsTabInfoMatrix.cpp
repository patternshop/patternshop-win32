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
#include "PsTabInfoMatrix.h"
#include "PatternshopView.h"
#include "PsAction.h"
#include "PsWin32.h"

// PsTabInfoMatrix dialog
bool OPTION_REFLECT_ALWAYS = false;

IMPLEMENT_DYNAMIC(PsTabInfoMatrix, CPropertyPage)

PsTabInfoMatrix::PsTabInfoMatrix()
	: CPropertyPage(PsTabInfoMatrix::IDD)
{
}

void PsTabInfoMatrix::Disable()
{
	OnShowWindow(false, 0);
}

void PsTabInfoMatrix::Enable()
{
	OnShowWindow(true, 0);
}

PsTabInfoMatrix::~PsTabInfoMatrix()
{
}

void PsTabInfoMatrix::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_X2, X);
	DDX_Control(pDX, IDC_Y2, Y);
	DDX_Control(pDX, IDC_ANGLE2, R);
	DDX_Control(pDX, IDC_I2, Ti);
	DDX_Control(pDX, IDC_J2, Tj);
	DDX_Control(pDX, IDC_W, W);
	DDX_Control(pDX, IDC_H, H);
	DDX_Control(pDX, IDC_BUTTON3, lock);
	DDX_Control(pDX, IDC_W3, dW);
	DDX_Control(pDX, IDC_H3, dH);
	DDX_Control(pDX, IDC_W2, WP);
	DDX_Control(pDX, IDC_H2, HP);
	DDX_Control(pDX, IDC_CHECK1, DivideBox);
	DDX_Control(pDX, IDC_CHECK4, reflectCheckBox);
}

BOOL PsTabInfoMatrix::OnInitDialog()
{
	lock.LoadBitmaps(IDB_NOTLOCK);
	rotationImage.LoadFromResource(AfxGetInstanceHandle(), IDB_ROTATION);
	torsionImage.LoadFromResource(AfxGetInstanceHandle(), IDB_TORSIO);
	return CDialog::OnInitDialog();
}


BEGIN_MESSAGE_MAP(PsTabInfoMatrix, CPropertyPage)
	ON_WM_ERASEBKGND()
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
	ON_EN_CHANGE(IDC_X2, &PsTabInfoMatrix::OnEnChangeX2)
	ON_EN_CHANGE(IDC_Y2, &PsTabInfoMatrix::OnEnChangeY2)
	ON_EN_CHANGE(IDC_ANGLE2, &PsTabInfoMatrix::OnEnChangeAngle2)
	ON_EN_CHANGE(IDC_I2, &PsTabInfoMatrix::OnEnChangeI2)
	ON_EN_CHANGE(IDC_J2, &PsTabInfoMatrix::OnEnChangeJ2)
	ON_EN_CHANGE(IDC_W, &PsTabInfoMatrix::OnEnChangeW)
	ON_EN_CHANGE(IDC_H, &PsTabInfoMatrix::OnEnChangeH)
	ON_BN_CLICKED(IDC_BUTTON3, &PsTabInfoMatrix::OnBnClickedButton3)
	ON_EN_CHANGE(IDC_W3, &PsTabInfoMatrix::OnEnChangeW3)
	ON_EN_CHANGE(IDC_H3, &PsTabInfoMatrix::OnEnChangeH3)
	ON_BN_CLICKED(IDC_CHECK1, &PsTabInfoMatrix::OnBnClickedCheck1)
	ON_EN_CHANGE(IDC_W2, &PsTabInfoMatrix::OnEnChangeW2)
	ON_EN_CHANGE(IDC_H2, &PsTabInfoMatrix::OnEnChangeH2)
	ON_BN_CLICKED(IDC_CHECK4, &PsTabInfoMatrix::OnBnClickedCheck4)
END_MESSAGE_MAP()


void PsTabInfoMatrix::OnEnChangeX2()
{
	CWnd* c = GetFocus();
	if (c != &X) return;
	if (!PsController::Instance().project_controller) return;
	PsProjectController* project_controller = PsController::Instance().project_controller;
	PsMatrix* matrix = project_controller->matrix;
	if (X.IsTopParentActive() && matrix)
	{
		char buffer[1024];
		X.GetWindowTextA(buffer, 1024);
		project_controller->LogAdd(new LogMove(*project_controller, matrix, matrix->x, matrix->y));
		matrix->x = atof(buffer);
		PsController::Instance().UpdateWindow();
	}
}

void PsTabInfoMatrix::OnEnChangeY2()
{
	CWnd* c = GetFocus();
	if (c != &Y) return;
	if (!PsController::Instance().project_controller) return;
	PsProjectController* project_controller = PsController::Instance().project_controller;
	PsMatrix* matrix = project_controller->matrix;
	if (matrix)
	{
		char buffer[1024];
		Y.GetWindowTextA(buffer, 1024);
		project_controller->LogAdd(new LogMove(*project_controller, matrix, matrix->x, matrix->y));
		matrix->y = atof(buffer);
		PsController::Instance().UpdateWindow();
	}
}

void PsTabInfoMatrix::OnEnChangeAngle2()
{
	CWnd* c = GetFocus();
	if (c != &R) return;
	if (!PsController::Instance().project_controller) return;
	PsProjectController* project_controller = PsController::Instance().project_controller;
	PsMatrix* matrix = project_controller->matrix;
	if (matrix)
	{
		char buffer[1024];
		R.GetWindowTextA(buffer, 1024);
		project_controller->LogAdd(new LogRotate(*project_controller, matrix, matrix->r));
		matrix->r = (atof(buffer) * 3.14159265f) / 180.f;
		PsController::Instance().UpdateWindow();
	}
}

void PsTabInfoMatrix::OnEnChangeI2()
{
	CWnd* c = GetFocus();
	if (c != &Ti) return;
	if (PsController::Instance().project_controller
		&& PsController::Instance().project_controller->matrix)
	{
		char buffer[1024];
		Ti.GetWindowTextA(buffer, 1024);
		PsController::Instance().project_controller->matrix->i = atof(buffer);
		PsController::Instance().UpdateWindow();
	}
}

void PsTabInfoMatrix::OnEnChangeJ2()
{
	CWnd* c = GetFocus();
	if (c != &Tj) return;
	if (PsController::Instance().project_controller
		&& PsController::Instance().project_controller->matrix)
	{
		char buffer[1024];
		Tj.GetWindowTextA(buffer, 1024);
		PsController::Instance().project_controller->matrix->j = atof(buffer);
		PsController::Instance().UpdateWindow();
	}
}


void PsTabInfoMatrix::OnEnChangeW()
{
	CWnd* c = GetFocus();
	if (c != &W) return;
	if (!PsController::Instance().project_controller) return;
	PsProjectController* project_controller = PsController::Instance().project_controller;
	PsMatrix* matrix = project_controller->matrix;
	if (matrix)
	{
		char buffer[1024];
		W.GetWindowTextA(buffer, 1024);
		if (atof(buffer) >= PsMatrix::minimum_dim)
		{
			project_controller->LogAdd(new LogResize(*project_controller, matrix, matrix->x, matrix->y, matrix->w, matrix->h));
			if (matrix->constraint)
				matrix->h *= atof(buffer) / matrix->w;
			matrix->w = atof(buffer);
			PsController::Instance().UpdateWindow();
			Update(matrix);
		}
	}
}

void PsTabInfoMatrix::OnEnChangeH()
{
	CWnd* c = GetFocus();
	if (c != &H) return;
	if (!PsController::Instance().project_controller) return;
	PsProjectController* project_controller = PsController::Instance().project_controller;
	PsMatrix* matrix = project_controller->matrix;
	if (matrix)
	{
		char buffer[1024];
		H.GetWindowTextA(buffer, 1024);
		if (atof(buffer) >= PsMatrix::minimum_dim)
		{
			project_controller->LogAdd(new LogResize(*project_controller, matrix, matrix->x, matrix->y, matrix->w, matrix->h));
			if (matrix->constraint)
				matrix->w *= atof(buffer) / matrix->h;
			matrix->h = atof(buffer);
			PsController::Instance().UpdateWindow();
			Update(matrix);
		}
	}
}

void PsTabInfoMatrix::OnEnChangeW2()
{
	CWnd* c = GetFocus();
	if (c != &WP) return;
	if (!PsController::Instance().project_controller) return;
	PsProjectController* project_controller = PsController::Instance().project_controller;
	PsMatrix* matrix = project_controller->matrix;
	if (matrix)
	{
		char buffer[1024];
		WP.GetWindowTextA(buffer, 1024);
		float w = atof(buffer) * PsMatrix::default_w / 100.f;
		if (w >= PsMatrix::minimum_dim)
		{
			project_controller->LogAdd(new LogResize(*project_controller, matrix, matrix->x, matrix->y, matrix->w, matrix->h));
			if (matrix->constraint)
				matrix->h *= w / matrix->w;
			matrix->w = w;
			PsController::Instance().UpdateWindow();
			Update(matrix);
		}
	}
}

void PsTabInfoMatrix::OnEnChangeH2()
{
	CWnd* c = GetFocus();
	if (c != &HP) return;
	if (!PsController::Instance().project_controller) return;
	PsProjectController* project_controller = PsController::Instance().project_controller;
	PsMatrix* matrix = project_controller->matrix;
	if (matrix)
	{
		char buffer[1024];
		HP.GetWindowTextA(buffer, 1024);
		float h = atof(buffer) * PsMatrix::default_h / 100.f;
		if (h >= PsMatrix::minimum_dim)
		{
			project_controller->LogAdd(new LogResize(*project_controller, matrix, matrix->x, matrix->y, matrix->w, matrix->h));
			if (matrix->constraint)
				matrix->w *= h / matrix->h;
			matrix->h = h;
			PsController::Instance().UpdateWindow();
			Update(matrix);
		}
	}
}

void PsTabInfoMatrix::Update(PsMatrix* matrix)
{
	CWnd* c = GetFocus();

	if (matrix)
	{
		Enable();
		float x, y, a, d, f, i, j;
		matrix->GetPosition(x, y);
		char buffer[1024];

		if (c != &X)
		{
			sprintf(buffer, "%.2f", x);
			X.SetWindowText(buffer);
		}

		if (c != &Y)
		{
			sprintf(buffer, "%.2f", y);
			Y.SetWindowText(buffer);
		}

		if (c != &R)
		{
			a = matrix->GetAngle() * 180.0f / 3.14159265f;
			while (a < 0.0f)
				a += 360.0f;
			while (a > 360.0f)
				a -= 360.0f;
			sprintf(buffer, "%.2f", a);
			R.SetWindowText(buffer);
		}

		matrix->GetTorsion(i, j);
		d = i / 3.14159265f;
		if (d < 0) d = -1 * d;
		else d = 1 * d;
		f = j;
		if (f < 0) f = -1 * f;
		else f = 1 * f;

		if (c != &Ti)
		{
			sprintf(buffer, "%.2f", d);
			Ti.SetWindowText(buffer);
		}

		if (c != &Tj)
		{
			sprintf(buffer, "%.2f", f);
			Tj.SetWindowText(buffer);
		}

		if (c != &W)
		{
			sprintf(buffer, "%.2f", matrix->w);
			W.SetWindowText(buffer);
		}

		if (c != &H)
		{
			sprintf(buffer, "%.2f", matrix->h);
			H.SetWindowText(buffer);
		}

		if (c != &WP)
		{
			sprintf(buffer, "%.2f", matrix->w / PsMatrix::default_w * 100.f);
			WP.SetWindowText(buffer);
		}
		if (c != &HP)
		{
			sprintf(buffer, "%.2f", matrix->h / PsMatrix::default_h * 100.f);
			HP.SetWindowText(buffer);
		}

		if (c != &dW)
		{
			sprintf(buffer, "%d", matrix->div_x);
			dW.SetWindowText(buffer);
		}

		if (c != &dH)
		{
			sprintf(buffer, "%d", matrix->div_y);
			dH.SetWindowText(buffer);
		}

		if (matrix->constraint) lock.LoadBitmaps(IDB_LOCK);
		else lock.LoadBitmaps(IDB_NOTLOCK);

		if (matrix->div_is_active)
		{
			DivideBox.SetCheck(BST_CHECKED);
			OnBnClickedCheck1();
		}
		else
		{
			DivideBox.SetCheck(BST_UNCHECKED);
			OnBnClickedCheck1();
		}

		lock.Invalidate(true);
	}
	else
	{
		OnShowWindow(true, 0);
	}
}

void PsTabInfoMatrix::OnBnClickedButton3()
{
	if (PsController::Instance().project_controller)
	{
		PsMatrix* matrix = PsController::Instance().project_controller->matrix;
		if (matrix)
		{
			if (!matrix->constraint)
			{
				lock.LoadBitmaps(IDB_LOCK);
				matrix->constraint = true;
				lock.Invalidate(true);
			}
			else
			{
				lock.LoadBitmaps(IDB_NOTLOCK);
				matrix->constraint = false;
				lock.Invalidate(true);
			}
		}
	}
}

void PsTabInfoMatrix::OnEnChangeW3()
{
	CWnd* c = GetFocus();
	if (c != &dW) return;
	if (PsController::Instance().project_controller
		&& PsController::Instance().project_controller->matrix)
	{
		char buffer[1024];
		dW.GetWindowText(buffer, 1024);
		PsController::Instance().project_controller->matrix->div_x = atof(buffer);
		PsController::Instance().UpdateWindow();
	}
}

void PsTabInfoMatrix::OnEnChangeH3()
{
	CWnd* c = GetFocus();
	if (c != &dH) return;
	if (PsController::Instance().project_controller
		&& PsController::Instance().project_controller->matrix)
	{
		char buffer[1024];
		dH.GetWindowText(buffer, 1024);
		PsController::Instance().project_controller->matrix->div_y = atof(buffer);
		PsController::Instance().UpdateWindow();
	}
}

void PsTabInfoMatrix::OnBnClickedCheck1()
{
	if (PsController::Instance().project_controller
		&& PsController::Instance().project_controller->matrix)
	{
		if (DivideBox.GetCheck() != BST_CHECKED)
		{
			PsController::Instance().project_controller->matrix->div_is_active = false;
			PsController::Instance().UpdateWindow();
			dW.EnableWindow(FALSE);
			dH.EnableWindow(FALSE);
		}
		else
		{
			PsController::Instance().project_controller->matrix->div_is_active = true;
			PsController::Instance().UpdateWindow();
			dW.EnableWindow(TRUE);
			dH.EnableWindow(TRUE);
		}
	}
}

void PsTabInfoMatrix::OnBnClickedCheck4()
{
	if (PsController::Instance().project_controller)
	{
		if (reflectCheckBox.GetCheck() != BST_CHECKED)
		{
			PsController::Instance().SetOption(PsController::OPTION_REFLECT, false);
			OPTION_REFLECT_ALWAYS = false;
		}
		else
		{
			PsController::Instance().SetOption(PsController::OPTION_REFLECT, true);
			OPTION_REFLECT_ALWAYS = true;
		}
	}
}

void PsTabInfoMatrix::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
	if (bShow && (!PsController::Instance().project_controller ||
		(PsController::Instance().project_controller && !PsController::Instance().project_controller->matrix)))
	{
		X.EnableWindow(FALSE);
		Y.EnableWindow(FALSE);
		R.EnableWindow(FALSE);
		Ti.EnableWindow(FALSE);
		Tj.EnableWindow(FALSE);
		W.EnableWindow(FALSE);
		H.EnableWindow(FALSE);
		lock.EnableWindow(FALSE);
		dW.EnableWindow(FALSE);
		dH.EnableWindow(FALSE);
		WP.EnableWindow(FALSE);
		HP.EnableWindow(FALSE);
		DivideBox.EnableWindow(FALSE);
		reflectCheckBox.EnableWindow(FALSE);
		X.SetWindowText("");
		Y.SetWindowText("");
		R.SetWindowText("");
		Ti.SetWindowText("");
		Tj.SetWindowText("");
		W.SetWindowText("");
		H.SetWindowText("");
		dW.SetWindowText("");
		dH.SetWindowText("");
		WP.SetWindowText("");
		HP.SetWindowText("");
	}
	else
	{
		X.EnableWindow(TRUE);
		Y.EnableWindow(TRUE);
		R.EnableWindow(TRUE);
		Ti.EnableWindow(TRUE);
		Tj.EnableWindow(TRUE);
		W.EnableWindow(TRUE);
		H.EnableWindow(TRUE);
		lock.EnableWindow(TRUE);
		dW.EnableWindow(TRUE);
		dH.EnableWindow(TRUE);
		WP.EnableWindow(TRUE);
		HP.EnableWindow(TRUE);
		DivideBox.EnableWindow(TRUE);
		reflectCheckBox.EnableWindow(TRUE);
	}
}

void PsTabInfoMatrix::OnPaint()
{
	CPaintDC dcPaint(this);
	CPropertyPage::OnPaint();
	CopyImageWithTransparency(dcPaint, rotationImage, 5, 71);
	CopyImageWithTransparency(dcPaint, torsionImage, 5, 103);
}
