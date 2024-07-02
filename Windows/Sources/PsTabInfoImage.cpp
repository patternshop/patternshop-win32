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
#include "PsTabInfoImage.h"
#include "PatternshopView.h"
#include "PsWinImage.h"
#include "PsAction.h"
#include "PsWin32.h"

// PsTabInfoImage dialog

IMPLEMENT_DYNAMIC(PsTabInfoImage, CPropertyPage)

PsTabInfoImage::PsTabInfoImage()
	: CPropertyPage(PsTabInfoImage::IDD)
{
}

PsTabInfoImage::~PsTabInfoImage()
{
}

BOOL PsTabInfoImage::OnInitDialog()
{
	lock.LoadBitmaps(IDB_NOTLOCK);
	rotationImage.LoadFromResource(AfxGetInstanceHandle(), IDB_ROTATION);
	return CDialog::OnInitDialog();
}

void PsTabInfoImage::Disable()
{
	X.Clear();
	Y.Clear();
	R.Clear();
	W.Clear();
	H.Clear();
	W2.Clear();
	H2.Clear();
	X.EnableWindow(FALSE);
	Y.EnableWindow(FALSE);
	R.EnableWindow(FALSE);
	W.EnableWindow(FALSE);
	H.EnableWindow(FALSE);
	W2.EnableWindow(FALSE);
	H2.EnableWindow(FALSE);
	lock.EnableWindow(FALSE);
	ButtonShow.EnableWindow(FALSE);
	loadButton.EnableWindow(FALSE);
	X.SetWindowText("");
	Y.SetWindowText("");
	R.SetWindowText("");
	W.SetWindowText("");
	H.SetWindowText("");
	W2.SetWindowText("");
	H2.SetWindowText("");
}

void PsTabInfoImage::Enable()
{
	X.EnableWindow(TRUE);
	Y.EnableWindow(TRUE);
	R.EnableWindow(TRUE);
	W.EnableWindow(TRUE);
	H.EnableWindow(TRUE);
	W2.EnableWindow(TRUE);
	H2.EnableWindow(TRUE);
	lock.EnableWindow(TRUE);
	ButtonShow.EnableWindow(TRUE);
	loadButton.EnableWindow(TRUE);
}

void PsTabInfoImage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_X2, X);
	DDX_Control(pDX, IDC_Y2, Y);
	DDX_Control(pDX, IDC_ANGLE2, R);
	DDX_Control(pDX, IDC_W, W);
	DDX_Control(pDX, IDC_H, H);
	DDX_Control(pDX, IDC_W2, W2);
	DDX_Control(pDX, IDC_H2, H2);
	DDX_Control(pDX, IDC_BUTTON3, lock);
	DDX_Control(pDX, IDC_BUTTON1, ButtonShow);
	DDX_Control(pDX, IDC_BUTTON2, loadButton);
}

void PsTabInfoImage::Update(PsImage* image)
{
	char buffer[1024];

	if (!X) return;

	PsProject* project = PsController::Instance().project;

	if (!project) return;

	if (image)
	{
		Enable();

		float	angle;
		angle = image->GetAngle() * 180.0f / 3.14159265f;
		while (angle < 0.0f)
			angle += 360.0f;
		while (angle > 360.0f)
			angle -= 360.0f;
		if (image->parent)
		{
			sprintf(buffer, "%.2f", image->x / (SHAPE_SIZE * 2)
				* project->matrix->w
				+ project->matrix->w / 2);
			X.SetWindowTextA(buffer);
			sprintf(buffer, "%.2f", image->y / (SHAPE_SIZE * 2)
				* project->matrix->h
				+ project->matrix->h / 2);
			Y.SetWindowTextA(buffer);
		}
		else
		{
			sprintf(buffer, "%.2f", image->x);
			X.SetWindowTextA(buffer);
			sprintf(buffer, "%.2f", image->y);
			Y.SetWindowTextA(buffer);
		}

		sprintf(buffer, "%.2f", image->w);
		W.SetWindowTextA(buffer);
		sprintf(buffer, "%.2f", image->h);
		H.SetWindowTextA(buffer);
		sprintf(buffer, "%.2f", angle);
		R.SetWindowTextA(buffer);
		sprintf(buffer, "%.2f", image->w / image->GetTexture().width * 100.0f);
		W2.SetWindowTextA(buffer);
		sprintf(buffer, "%.2f", image->h / image->GetTexture().height * 100.0f);
		H2.SetWindowTextA(buffer);
		if (image->constraint) lock.LoadBitmaps(IDB_LOCK);
		else lock.LoadBitmaps(IDB_NOTLOCK);
		lock.Invalidate(true);
	}
	else
	{
		OnShowWindow(true, 0);
	}
}

BEGIN_MESSAGE_MAP(PsTabInfoImage, CPropertyPage)
	ON_WM_ERASEBKGND()
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
	ON_EN_CHANGE(IDC_X2, &PsTabInfoImage::OnEnChangeX2)
	ON_EN_CHANGE(IDC_Y2, &PsTabInfoImage::OnEnChangeY2)
	ON_EN_CHANGE(IDC_ANGLE2, &PsTabInfoImage::OnEnChangeAngle2)
	ON_EN_CHANGE(IDC_W, &PsTabInfoImage::OnEnChangeW)
	ON_EN_CHANGE(IDC_H, &PsTabInfoImage::OnEnChangeH)
	ON_BN_CLICKED(IDC_BUTTON1, &PsTabInfoImage::OnBnClickedButton1)
	ON_EN_CHANGE(IDC_W2, &PsTabInfoImage::OnEnChangeW2)
	ON_EN_CHANGE(IDC_H2, &PsTabInfoImage::OnEnChangeH2)
	ON_BN_CLICKED(IDC_BUTTON3, &PsTabInfoImage::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, &PsTabInfoImage::OnBnClickedButton2)
END_MESSAGE_MAP()


// PsTabInfoImage message handlers

void PsTabInfoImage::OnEnChangeX2()
{
	CWnd* c = GetFocus();
	if (c != &X) return;
	PsProject* project = PsController::Instance().project;
	if (X.IsTopParentActive() && project && project->image)
	{
		char buffer[1024];
		X.GetWindowTextA(buffer, 1024);
		float value = atof(buffer);
		if (project->image->parent)
			if (value < 0.f) value = 0.f;
			else if (value > project->matrix->w) value = project->matrix->w;
		float x, y;
		project->image->GetPosition(x, y);
		project->LogAdd(new LogMove(*project, project->image, x, y));
		if (project->image->parent) project->image->x = (value - project->matrix->w / 2) / project->matrix->w * (SHAPE_SIZE * 2);
		else project->image->x = value;
		PsController::Instance().UpdateWindow();
	}
}

void PsTabInfoImage::OnEnChangeY2()
{
	CWnd* c = GetFocus();
	if (c != &Y) return;
	PsProject* project = PsController::Instance().project;
	if (project && project->image)
	{
		char buffer[1024];
		Y.GetWindowTextA(buffer, 1024);
		float value = atof(buffer);
		if (project->image->parent)
			if (value < 0.f) value = 0.f;
			else if (value > project->matrix->h) value = project->matrix->h;
		float x, y;
		project->image->GetPosition(x, y);
		project->LogAdd(new LogMove(*project, project->image, x, y));
		if (project->image->parent) project->image->y = (value - project->matrix->h / 2) / project->matrix->h * (SHAPE_SIZE * 2);
		else project->image->y = value;
		PsController::Instance().UpdateWindow();
	}
}

void PsTabInfoImage::OnEnChangeAngle2()
{
	CWnd* c = GetFocus();
	if (c != &R) return;
	PsProject* project = PsController::Instance().project;
	if (project && project->image)
	{
		char buffer[1024];
		R.GetWindowTextA(buffer, 1024);
		project->LogAdd(new LogRotate(*project, project->image, project->image->r));
		project->image->r = (atof(buffer) * 3.14159265) / 180.0;
		PsController::Instance().UpdateWindow();
	}
}

void PsTabInfoImage::OnEnChangeW()
{
	CWnd* c = GetFocus();
	if (c != &W) return;
	PsProject* project = PsController::Instance().project;
	if (project && project->image)
	{
		PsImage* image = project->image;
		float x, y;
		image->GetPosition(x, y);
		project->LogAdd(new LogResize(*project, image, x, y, image->w, image->h));
		char buffer[1024];
		W.GetWindowTextA(buffer, 1024);
		image->w = atof(buffer);
		sprintf(buffer, "%.2f", image->w * 100 / image->GetTexture().width);
		W2.SetWindowTextA(buffer);
		if (image->constraint)
		{
			H2.SetWindowTextA(buffer);
			image->h = atof(buffer) * image->GetTexture().height / 100;
			sprintf(buffer, "%.2f", image->h);
			H.SetWindowTextA(buffer);
		}
		PsController::Instance().UpdateWindow();
	}
}

void PsTabInfoImage::OnEnChangeH()
{
	CWnd* c = GetFocus();
	if (c != &H) return;
	PsProject* project = PsController::Instance().project;
	if (project && project->image)
	{
		PsImage* image = project->image;
		float x, y;
		image->GetPosition(x, y);
		project->LogAdd(new LogResize(*project, image, x, y, image->w, image->h));
		char buffer[1024];
		H.GetWindowTextA(buffer, 1024);
		image->h = atof(buffer);
		sprintf(buffer, "%.2f", image->h * 100 / image->GetTexture().height);
		H2.SetWindowTextA(buffer);
		if (image->constraint)
		{
			W2.SetWindowTextA(buffer);
			image->w = atof(buffer) * image->GetTexture().width / 100;
			sprintf(buffer, "%.2f", image->w);
			W.SetWindowTextA(buffer);
		}
		PsController::Instance().UpdateWindow();
	}
}

void PsTabInfoImage::OnEnChangeW2()
{
	CWnd* c = GetFocus();
	if (c != &W2) return;
	if (PsController::Instance().project
		&& PsController::Instance().project->image)
	{
		char buffer[1024];
		W2.GetWindowTextA(buffer, 1024);
		PsImage* image = PsController::Instance().project->image;
		image->w = atof(buffer) * image->GetTexture().width / 100;
		sprintf(buffer, "%.2f", image->w);
		W.SetWindowTextA(buffer);
		if (image->constraint)
		{
			W2.GetWindowTextA(buffer, 1024);
			H2.SetWindowTextA(buffer);
			image->h = atof(buffer) * image->GetTexture().height / 100;
			sprintf(buffer, "%.2f", image->h);
			H.SetWindowTextA(buffer);
		}
		PsController::Instance().UpdateWindow();
	}
}

void PsTabInfoImage::OnEnChangeH2()
{
	CWnd* c = GetFocus();
	if (c != &H2) return;
	if (PsController::Instance().project
		&& PsController::Instance().project->image)
	{
		char buffer[1024];
		H2.GetWindowTextA(buffer, 1024);
		PsImage* image = PsController::Instance().project->image;
		image->h = atof(buffer) * image->GetTexture().height / 100;
		sprintf(buffer, "%.2f", image->h);
		H.SetWindowTextA(buffer);
		if (image->constraint)
		{
			H2.GetWindowTextA(buffer, 1024);
			W2.SetWindowTextA(buffer);
			image->w = atof(buffer) * image->GetTexture().width / 100;
			sprintf(buffer, "%.2f", image->w);
			W.SetWindowTextA(buffer);
		}
		PsController::Instance().UpdateWindow();
	}
}

void PsTabInfoImage::OnBnClickedButton1()
{
	PsWinImage::Instance().ShowWindow(SW_SHOW);
}



void PsTabInfoImage::OnBnClickedButton3()
{
	if (PsController::Instance().project)
	{
		PsImage* image = PsController::Instance().project->image;
		if (image)
		{
			if (!image->constraint)
			{
				lock.LoadBitmaps(IDB_LOCK);
				image->constraint = true;
				lock.Invalidate(true);
				char w[1024], h[1024];
				W2.GetWindowText(w, 1024);
				H2.GetWindowText(h, 1024);
				if (atof(w) != atof(h))
				{
					if (atof(w) > atof(h))
					{
						H2.SetFocus();
						H2.SetWindowText(w);
						OnEnChangeH2();
					}
					else
					{
						W2.SetFocus();
						W2.SetWindowText(h);
						OnEnChangeW2();
					}
				}
			}
			else
			{
				lock.LoadBitmaps(IDB_NOTLOCK);
				lock.GetBitmap();
				image->constraint = false;
				lock.Invalidate(true);
			}
		}
	}
}

void PsTabInfoImage::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
	if (bShow && (!PsController::Instance().project ||
		(PsController::Instance().project && !PsController::Instance().project->image)))
	{
		Disable();
	}
	else
	{
		Enable();
		if (PsController::Instance().project)
			Update(PsController::Instance().project->image);
	}
}

void PsTabInfoImage::OnBnClickedButton2()
{
	if (PsController::Instance().active)
		PsController::Instance().active->MenuEditReplace();
}

void PsTabInfoImage::OnPaint()
{
	CPaintDC dcPaint(this);
	CPropertyPage::OnPaint();
	CopyImageWithTransparency(dcPaint, rotationImage, 3, 71);
}
