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
#include "PsProject.h"
#include "Patternshop.h"
#include "PsDlgInfoLight.h"
#include "PsController.h"
#include "PatternshopView.h"

float PsDlgInfoLight::p = 0.0f, PsDlgInfoLight::e = 1.f, PsDlgInfoLight::r = 90.f;
// PsDlgInfoLight dialog

IMPLEMENT_DYNAMIC(PsDlgInfoLight, CDialog)

PsDlgInfoLight::PsDlgInfoLight(CWnd* pParent /*=NULL*/)
	: CDialog(PsDlgInfoLight::IDD, pParent)
{

}

PsDlgInfoLight::~PsDlgInfoLight()
{
}

BOOL PsDlgInfoLight::OnInitDialog()
{
	CDialog::OnInitDialog();
	Us.SetRange(0, 100);
	Vs.SetRange(0, 100);
	Us.SetPos(p * 100.f);
	Vs.SetPos(e * 100.f);
	Update();
	return TRUE;
}

void PsDlgInfoLight::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, Us);
	DDX_Control(pDX, IDC_SLIDER2, Vs);
	DDX_Control(pDX, IDC_EDIT1, Pe);
	DDX_Control(pDX, IDC_EDIT2, Ee);
	DDX_Control(pDX, IDC_ANGLE2, R);
	DDX_Control(pDX, IDC_COMBO1, S);
}


BEGIN_MESSAGE_MAP(PsDlgInfoLight, CDialog)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, &PsDlgInfoLight::OnNMReleasedcaptureSlider1)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER2, &PsDlgInfoLight::OnNMReleasedcaptureSlider2)
END_MESSAGE_MAP()

void PsDlgInfoLight::Update()
{
	CPaintDC dc(this);
	char buffer[1024];
	sprintf(buffer, "%2.f", p * 100.f);
	Pe.SetWindowText(buffer);
	sprintf(buffer, "%2.f", e * 100.f);
	Ee.SetWindowText(buffer);
	sprintf(buffer, "%2.f", r);
	R.SetWindowText(buffer);
	
	PsController::Instance().project->pattern->SetLinearLight(p, e);
	PsController::Instance().UpdateWindow();
}

// PsDlgInfoLight message handlers
void PsDlgInfoLight::OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	p = Us.GetPos() / 100.f;
	Update();
	*pResult = 0;
}

void PsDlgInfoLight::OnNMReleasedcaptureSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	e = Vs.GetPos() / 100.f;
	Update();
	*pResult = 0;
}

bool PsDlgInfoLight::ShowModal()
{
  return CDialog::DoModal() == IDOK;
}
