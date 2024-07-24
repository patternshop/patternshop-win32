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

#include "PsProjectController.h"
#include "afxext.h"
#include <atlimage.h>

// PsTabInfoImage dialog

class PsTabInfoImage : public CPropertyPage
{
	DECLARE_DYNAMIC(PsTabInfoImage)

public:
	PsTabInfoImage();
	virtual ~PsTabInfoImage();

public:
	void Update(PsImage*);
	void Disable();
	void Enable();

	// Dialog Data
	enum { IDD = IDD_INFO_IMAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit X;
	CEdit Y;
	CEdit R;
	CEdit W;
	CEdit H;
	CEdit W2;
	CEdit H2;
	CBitmapButton lock;
	CButton ButtonShow;
	CImage rotationImage;
	CButton loadButton;

public:
	afx_msg BOOL OnInitDialog();
	afx_msg void OnEnChangeX2();
	afx_msg void OnEnChangeY2();
	afx_msg void OnEnChangeAngle2();
	afx_msg void OnEnChangeW();
	afx_msg void OnEnChangeH();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnEnChangeW2();
	afx_msg void OnEnChangeH2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButton2();
	afx_msg void OnPaint();

};
