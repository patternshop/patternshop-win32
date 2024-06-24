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
#include "afxcmn.h"


// PsDlgInfoLight dialog

class PsDlgInfoLight : public CDialog
{
	DECLARE_DYNAMIC(PsDlgInfoLight)

public:
	PsDlgInfoLight(CWnd* pParent = NULL);   // standard constructor
	virtual ~PsDlgInfoLight();

// Dialog Data
	enum { IDD = IDD_YUV };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	static float p, e, r;

public:
	void Update();
  bool ShowModal();

public:
	CSliderCtrl Us;
	CSliderCtrl Vs;
	CEdit Pe;
	CEdit Ee;
	CEdit R;
	CComboBox S;

public:
	afx_msg void OnPaint();
	afx_msg BOOL OnInitDialog();
	afx_msg void OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
};
