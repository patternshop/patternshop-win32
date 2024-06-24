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
#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// PsDlgConfig dialog

class PsDlgConfig : public CDialog
{
	DECLARE_DYNAMIC(PsDlgConfig)

public:
	PsDlgConfig(CWnd* pParent = NULL);   // standard constructor
	virtual ~PsDlgConfig();

// Dialog Data
	enum { IDD = IDD_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	CSliderCtrl power_slider;
	CEdit L;
	CEdit H;
	CEdit L_min;
	CEdit H_min;
	CComboBox langageBox;

public:
	afx_msg void OnBnClickedButton1();
};

extern bool do_not_save_options;
extern int resolution_max;
