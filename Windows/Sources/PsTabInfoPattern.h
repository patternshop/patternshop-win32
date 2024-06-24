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

#include "DialogPatternCx.h"
#include "PsHardware.h"
#include "PsPAttern.h"

#include <atlimage.h>
#include "afxwin.h"
#include "afxcmn.h"

class PsTabInfoPattern : public CPropertyPage
{
	DECLARE_DYNAMIC(PsTabInfoPattern)

public:
	PsTabInfoPattern();   // standard constructor
	virtual ~PsTabInfoPattern();

// Dialog Data
	enum { IDD = IDD_PATTERN };

protected:
	virtual void DoDataExchange (CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
  void Update();
	void UpdateMiniImage();
	void FastUpdate();
	void CleanBackground();
  PsLayer *GetCurrentLayer();
  void UpdateNow();
  void UpdateText();
  void Enable();
  void Disable();

private:
  DialogPatternCx dpCore;

public:
	afx_msg void OnPaint();
  afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult);
 
public:
  afx_msg void OnChangeTX();
  afx_msg void OnChangeTY();
  afx_msg void OnChangeTZ();
  afx_msg void OnChangeRX();
  afx_msg void OnChangeRY();
  afx_msg void OnChangeRZ();
  afx_msg void OnChangeZ();

private:
  CSliderCtrl m_SliderQuality;
  CImage window_buffer, window_buffer2;
  CEdit tX, tY, tZ;
  CEdit rX, rY, rZ;
  CEdit Z;
};
