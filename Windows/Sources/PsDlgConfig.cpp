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
#include "PsDlgConfig.h"
#include "PsMatrix.h"
#include "PsMessage.h"

bool do_not_save_options = false;
int resolution_max = 0;

extern CPatternshopApp theApp;

IMPLEMENT_DYNAMIC(PsDlgConfig, CDialog)

PsDlgConfig::PsDlgConfig(CWnd* pParent /*=NULL*/)
	: CDialog(PsDlgConfig::IDD, pParent)
{
}

PsDlgConfig::~PsDlgConfig()
{
}

void PsDlgConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, power_slider);
	DDX_Control(pDX, IDC_W, L);
	DDX_Control(pDX, IDC_H, H);
	DDX_Control(pDX, IDC_H2, H_min);
	DDX_Control(pDX, IDC_COMBOBOX_LANGUE, langageBox);
}

BOOL PsDlgConfig::OnInitDialog()
{
	CDialog::OnInitDialog();
	char buffer[1024];
	sprintf(buffer, "%d", PsMatrix::default_w);
	L.SetWindowText(buffer);
	sprintf(buffer, "%d", PsMatrix::default_h);
	H.SetWindowText(buffer);
	sprintf(buffer, "%d", PsMatrix::minimum_dim);
	H_min.SetWindowText(buffer);

	power_slider.SetRange(0, 3);
	switch (resolution_max)
	{
	case 1024: power_slider.SetPos(2); break;
	case 800: power_slider.SetPos(1); break;
	case 640: power_slider.SetPos(0); break;
	default: power_slider.SetPos(3); break;
	}

	if (getLanguage() == French)
	{
		langageBox.SetCurSel(1);
	}
	else
	{
		langageBox.SetCurSel(0);
	}

	return TRUE;
}

void PsDlgConfig::OnOK()
{
	char buffer[1024];
	L.GetWindowText(buffer, 1024);
	int w = atoi(buffer);
	H.GetWindowText(buffer, 1024);
	int h = atoi(buffer);
	H_min.GetWindowText(buffer, 1024);
	int min = atoi(buffer);
	if (min > 1) PsMatrix::minimum_dim = min;
	if (w >= PsMatrix::minimum_dim) PsMatrix::default_w = w;
	if (h >= PsMatrix::minimum_dim) PsMatrix::default_h = h;
	switch (power_slider.GetPos())
	{
	case 2: resolution_max = 1024; break;
	case 1: resolution_max = 800; break;
	case 0: resolution_max = 640; break;
	default: resolution_max = 0; break;
	}

	int lengSel = langageBox.GetCurSel();
	switch (lengSel)
	{
	case 1:
		theApp.SetLanguage(French);
		break;
	default:
		theApp.SetLanguage(English);
		break;
	}

	CDialog::OnOK();
}

BEGIN_MESSAGE_MAP(PsDlgConfig, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &PsDlgConfig::OnBnClickedButton1)
END_MESSAGE_MAP()

void PsDlgConfig::OnBnClickedButton1()
{
	if (GetQuestion(QUESTION_RESET_CONFIG_FILE))
	{
		CWinApp* pApp = AfxGetApp();
		pApp->WriteProfileString("Transformation", NULL, NULL);
		pApp->WriteProfileString("Overview", NULL, NULL);
		pApp->WriteProfileString("PsProject", NULL, NULL);
		pApp->WriteProfileString("PsMatrix", NULL, NULL);
		pApp->WriteProfileString("General", NULL, NULL);
		do_not_save_options = true;
	}
}
