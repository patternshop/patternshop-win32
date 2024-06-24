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
#include <atlimage.h>
#include "afxwin.h"

// PsDlgPrint dialog

class PsDlgPrint : public CDialog
{
	DECLARE_DYNAMIC(PsDlgPrint)

public:
	PsDlgPrint(CWnd* pParent = NULL);   // standard constructor
	virtual ~PsDlgPrint();	

// Dialog Data
	enum { IDD = IDD_PRINT_PREVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	afx_msg BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnCbnSelchangeCombo1();

private:
	CEdit X;
	CEdit Y;
	CEdit Z;
	CEdit H;
	CEdit W;
	CComboBox Y_combo;
	CComboBox X_combo;
	CComboBox H_combo;
	CComboBox W_combo;
	CButton AutoFit;
	CButton AutoCenter;
	CButton ShowBox;
	CImage buffer, original;

public:
	int x, y, h, w;
	float z;
	float r_zoom;
	int format_w, format_h;

private:
	void Update();
	void UpdateValue(int, CEdit&, CComboBox&);
	int GetValue(CEdit &e, CComboBox &c);
	void SetAvaibleUnits(CComboBox&);
	void PrepareMiniImage();
	void UpdateMiniImage();
	void SetZ(float);

private:
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnEnChangeEdit9();
	afx_msg void OnEnChangeEdit7();
	afx_msg void OnEnChangeEdit8();
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedCheck3();
};
