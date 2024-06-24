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

#include "PsDlgExportCx.h"

class PsDlgExport : public CDialog, public PsDlgExportCx
{
	DECLARE_DYNAMIC(PsDlgExport)

public:
	PsDlgExport(CWnd* pParent = NULL);   // standard constructor
	virtual ~PsDlgExport();

// Dialog Data
	enum { IDD = IDD_EXPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	afx_msg BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnCbnSelchangeCombo1();

private:
	void Update();
	void UpdateValue(double, CEdit&, CComboBox&);
	double GetValue(CEdit &e, CComboBox &c);
	void SetAvaibleUnits(CComboBox&);

private:
  CEdit Z;
	CEdit H;
	CEdit W;
	CComboBox H_combo;
	CComboBox W_combo;
  CButton cadriageCheck;
  CEdit DpiEdit;

private:
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnEnChangeEdit9();
	afx_msg void OnEnChangeEdit7();
	afx_msg void OnEnChangeEdit8();
	afx_msg void OnEnChangeEdit11();
  afx_msg void OnBnClickedCheck1();
  afx_msg void OnBnClickedOk();
};
