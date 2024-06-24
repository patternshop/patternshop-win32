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
#include "afxwin.h"


// PsDlgDocument dialog

class PsDlgDocument : public CDialog
{
	DECLARE_DYNAMIC(PsDlgDocument)

public:
	PsDlgDocument(CWnd* pParent = NULL);   // standard constructor
	virtual ~PsDlgDocument();

// Dialog Data
	enum { IDD = IDD_NEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	void UpdateValue(int px, CEdit &e, CComboBox &c);
	int GetValue(CEdit &e, CComboBox &c);
	void SetAvaibleUnits(CComboBox &c);

public:
	void Update();
	int w, h, dpi;

public:
	CEdit W;
	CEdit H;
	CEdit D;
	CComboBox W_combo;
	CComboBox H_combo;
	CComboBox D_combo;
	CComboBox F_combo;

  bool bNoDefault;

public:
	afx_msg BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeCombo6();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnCbnSelchangeCombo2();
	afx_msg void OnCbnSelchangeCombo5();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnEnChangeEdit10();
public:
	afx_msg void OnClose();
};
