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

#include "stdafx.h"
#include "resource.h"
#include "afxwin.h"

#include "math.h"

#if	!defined (MIN)
#	define	MIN	min
#endif

#if	!defined (MAX)
#	define	MAX	max
#endif

#define WM_FO_SELECTBULLETTYPEOK				WM_USER+270 // Select arrow ok.

#define	HSV_LOOP_STEPS(w)	((w) - 1)

#define	HSV_0()	RGB ((BYTE) (coef1 >> int_extend),(BYTE) (coef3 >> int_extend),(BYTE) (val >> int_extend))

#define	HSV_HUE_ADV_0() coef1 += coef1_adv,coef3 += coef3_adv

#define	HSV_1()	RGB ((BYTE) (coef1 >> int_extend),(BYTE) (val >> int_extend),(BYTE) (coef2 >> int_extend))

#define	HSV_HUE_ADV_1()	coef1 += coef1_adv,	coef2 += coef2_adv


#define	HSV_2()	RGB ((BYTE) (coef3 >> int_extend),(BYTE) (val >> int_extend),(BYTE) (coef1 >> int_extend))

#define	HSV_HUE_ADV_2()		HSV_HUE_ADV_0()

#define	HSV_3()	RGB ((BYTE) (val >> int_extend),(BYTE) (coef2 >> int_extend),(BYTE) (coef1 >> int_extend))

#define	HSV_HUE_ADV_3()		HSV_HUE_ADV_1()

#define	HSV_4()	RGB ((BYTE) (val >> int_extend),(BYTE) (coef1 >> int_extend),(BYTE) (coef3 >> int_extend))

#define	HSV_HUE_ADV_4()		HSV_HUE_ADV_0()

#define	HSV_5()	 RGB (	(BYTE) (coef2 >> int_extend),(BYTE) (coef1 >> int_extend),(BYTE) (val >> int_extend))

#define	HSV_HUE_ADV_5()		HSV_HUE_ADV_1()

// initialize for HSV colorspace in SAT mode, for HUE between 0 and 1 (0 and 60 deg)
#define	HSV_SAT_INIT_0()	coef3 = coef1,	coef3_adv = (int) ((val - coef3) / HSV_LOOP_STEPS (j))

// advance for HSV colorspace in SAT mode, for HUE between 0 and 1 (0 and 60 deg)
#define	HSV_SAT_ADV_0()	    coef3 += coef3_adv

#define	HSV_SAT_INIT_1()	coef2 = val, coef2_adv = (int) ((val * (1.0 - sat) - coef2) / HSV_LOOP_STEPS (j))

#define	HSV_SAT_ADV_1()	    coef2 += coef2_adv

#define	HSV_SAT_INIT_2()	HSV_SAT_INIT_0()
#define	HSV_SAT_ADV_2()		HSV_SAT_ADV_0()

#define	HSV_SAT_INIT_3()	HSV_SAT_INIT_1()
#define	HSV_SAT_ADV_3()		HSV_SAT_ADV_1()

#define	HSV_SAT_INIT_4()	HSV_SAT_INIT_0()
#define	HSV_SAT_ADV_4()		HSV_SAT_ADV_0()

#define	HSV_SAT_INIT_5()	HSV_SAT_INIT_1()
#define	HSV_SAT_ADV_5()		HSV_SAT_ADV_1()

// for HSV colorspace, VAL mode is calculate in a same manner as SAT mode
//	so all macroses simply maps over SAT mode macroses
#define	HSV_VAL_INIT_0()	HSV_SAT_INIT_0()
#define	HSV_VAL_ADV_0()		HSV_SAT_ADV_0()

#define	HSV_VAL_INIT_1()	HSV_SAT_INIT_1()
#define	HSV_VAL_ADV_1()		HSV_SAT_ADV_1()

#define	HSV_VAL_INIT_2()	HSV_SAT_INIT_2()
#define	HSV_VAL_ADV_2()		HSV_SAT_ADV_2()

#define	HSV_VAL_INIT_3()	HSV_SAT_INIT_3()
#define	HSV_VAL_ADV_3()		HSV_SAT_ADV_3()

#define	HSV_VAL_INIT_4()	HSV_SAT_INIT_4()
#define	HSV_VAL_ADV_4()		HSV_SAT_ADV_4()

const int	int_extend = 20;

enum modes_visual
{
	modes_rgb_red = 0,
	modes_rgb_green = 1,
	modes_rgb_blue = 2,
	modes_hsv_hue = 3,
	modes_hsv_sat = 4,
	modes_hsv_value = 5,
	modes_max,
	modes_mask = 0x7F,
	modes_reverse = 0x80
};

// coefficients to scale color components with to calculate real component value
enum scale_values
{
	scale_rgb = 1,
	scale_rgb_red = scale_rgb,
	scale_rgb_green = scale_rgb,
	scale_rgb_blue = scale_rgb,
	scale_hsv_hue = 10,
	scale_hsv_sat = 1000,
	scale_hsv_value = 1000
};

enum max_values
{
	max_rgb = 255,
	max_rgb_red = max_rgb,
	max_rgb_green = max_rgb,
	max_rgb_blue = max_rgb,
	max_hsv_hue = 3599,
	max_hsv_sat = 1000,
	max_hsv_value = 1000
};

class GTDrawHelper  
{
public:
	GTDrawHelper();
	virtual ~GTDrawHelper();

	// draw_panel flags bit mask
	enum flags
	{
		raised = 1,
		sunken = 2,
		blackbox = 4
	};

	enum types
	{
		left = 1,
		top = 2,
		right = 3,
		bottom = 4,
		left_top = 5,
		left_bottom = 6,
		right_top = 7,
		right_bottom = 8
	};

	// Draw panel border.
	virtual void DrawPanel(HDC hdc,int left,int top,int right,int bottom,int flags);

	// Draw panel border.
	virtual void DrawPanel (HDC hdc,LPRECT rc,int flags)
	{
		DrawPanel(hdc, rc->left, rc->top, rc->right, rc->bottom, flags);
	};

	// Draw triangle.
	virtual void DrawTriangle(HDC hdc,int x,int y,enum types type,int size,COLORREF line_color,COLORREF fill_color);

	// Convert from hsv to rgb value.
	int	HSV2RGB (double hue,double sat,double value,double *red,double *green,double *blue);

	// Convert from rgb to hsv value.
	int	RGB2HSV(double red,double green,double blue,double *hue,double *sat,double *value);

	// Convert from hsv to rgb value.
	COLORREF HSV2RGB(double hue,double sat,double value);

	// Get rgb value.
	void GetRGB(DWORD *buffer,int samples,COLORREF start,COLORREF end);

	// hsv from hue.
	void HSV_HUE(DWORD *buffer,int samples,double sat,double val);

	// hsv from sat.
	void HSV_SAT(DWORD *buffer,int samples,double hue,double val);

	// hsv value.
	void HSV_VAL(DWORD *buffer,int samples,double hue,double sat);

	// Set
	void set(unsigned long *buffer,unsigned long value,size_t count);

	// 
	void set(unsigned long **buffer,unsigned long value,size_t count);

	//
	void copy(unsigned long *target,const unsigned long *source,size_t count);

	// 
	void copy(unsigned long **target,const unsigned long *source,size_t count);

	//
	void copy_reverse(unsigned long *target,const unsigned long *source,size_t count);

	//
	void reverse(unsigned long *buffer,size_t count);

};

inline int scaled_red (COLORREF c)
{
	return (GetRValue (c) << int_extend);
}

inline int scaled_green (COLORREF c)
{
	return (GetGValue (c) << int_extend);
}

inline int scaled_blue (COLORREF c)
{
	return (GetBValue (c) << int_extend);
}

template <class T, class T1> void in_range (T& x,T1 start,T1 end)
{
	if (x < static_cast <T> (start)) x = static_cast <T> (start);
	if (x > static_cast <T> (end)) x = static_cast <T> (end);
}


/////////////////////////////////////////////////////////////////////////////
// GTColorSelectorWnd window
const int CONTRAST_MARKER = 1;
const int MARKER_SIZE = 5;

const int	max_picker [6][3] =
{
	{ max_rgb_red, max_rgb_blue, max_rgb_green },	// max_rgb red: Rx, B>, G^
	{ max_rgb_green, max_rgb_blue, max_rgb_red },	// max_rgb green: Gx, B>, R^
	{ max_rgb_blue, max_rgb_red, max_rgb_green },	// max_rgb blue: Bx, R>, G^
	{ max_hsv_hue, max_hsv_sat, max_hsv_value },	// max_hsv hue: Hx, S>, V^
	{ max_hsv_sat, max_hsv_hue, max_hsv_value },	// max_hsv sat: Sx, H>, V^
	{ max_hsv_value, max_hsv_hue, max_hsv_sat }		// max_hsv value: Vx, H>, S^
};

class GTColorSelectorWnd : public CWnd
{

public:
	// Constroctor
	GTColorSelectorWnd();

// Attributes
public:

	//Create Arrow Type Window
	BOOL Create(DWORD dwStyle,
		CRect rcPos, 
		CWnd* pParent,
		UINT nID,
		int nBulletType,
		BOOL bPopup = FALSE);

// Operations

public:
	int		m_nCurMode;
	int		additional_component;
	int		x_pos;
	int		y_pos;
	int		max_x;
	int		max_y;
	int		old_x, old_y;

	bool	m_bNeedPaint;

	// whether mouse is being tracked (drag)
	bool	m_bTracking;			

	// Save color.
	COLORREF m_crSaveColor;

	// user interaction handlers
	enum mouse_buttons
	{
		button_left,
		button_right,
		button_middle
	};

	// cache DIB bitmap information
	BITMAPINFO	bmp_info;
	HBITMAP		bmp_handle;
	DWORD		*bmp_data;

	CRect	m_rcPanel;
	CRect   m_rcPaint;
	CRect	m_rcInteraction;

	// control's m_rcFrame (bounding box)
	CRect		m_rcFrame;	

	
public:

	// parent notification methods
	virtual LRESULT	SendMessageToParent(UINT code);

	// get/set visual mode
	inline int GetDataValue(void) const { return (m_nCurMode); };
	void SetDataValue(int value);

	// get/set additional component's value
	inline int GetControlValue(void) const { return (additional_component); };
	void SetControlValue(int value);

	// Operations
	void DoCalculateColors(COLORREF &lefttop,COLORREF &righttop,COLORREF &leftbottom,COLORREF &rightbottom);

	// Draw all
	void OnDraw(void);

	// draws blend in rgb mode - includes red, green and blue modes
	void DoDrawRGB(void);

	// draws blend in hsv mode - hue, sat and value modes separately
	void DoDrawHSV_Hue(void);
	void DoDrawHSV_Sat(void);
	void DoDrawHSV_Val(void);

	// get/set current color
	COLORREF GetColor(void) const;
	void SetColor(COLORREF value);

	// get/set current x-axis position
	inline int GetXPos(void) const { return (x_pos); };
	void SetXPos(int value);

	// get/set current y-axis position
	inline int GetYPos(void) const { return (y_pos); };
	void SetYPos(int value);
	
protected:

	HCURSOR	picker_cursor;

protected:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(GTColorSelectorWnd)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:

	// Destructor
	virtual ~GTColorSelectorWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(GTColorSelectorWnd)

	//message map function On Erase Background
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	//message map function On left button double clicked
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	//message map function On left button down
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	//message map function On left button up
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	//message map function On middle button double clicked
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);

	//message map function On middle button down
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);

	//message map function On middle button up
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);

    //message map function On Mouse Move
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	// message map function On keystroke  nonsystem character
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

    //message map function On window Destroyed
	afx_msg void OnDestroy();

   //message map function On window painted
	afx_msg void OnPaint();

	//message map function On right button down
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

	//message map function On right button up
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

	//message map function On Cancel Mode
	afx_msg void OnCancelMode();

	//message map function On Key Down
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//message map function On window size changed
	afx_msg void OnSize(UINT nType, int cx, int cy);
	// System color change.
	afx_msg void OnSysColorChange();
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG
	
	//message map function On Select Day OK
	afx_msg LONG OnSelectBulletOK(UINT wParam, LONG lParam);
	DECLARE_MESSAGE_MAP()
public:

    // The pointer to Notify Window
	CWnd *pNotifyWnd;

	CWnd *pNotifyWellWnd;
};
/////////////////////////////////////////////////////////////////////////////


class PsDlgColor;

/////////////////////////////////////////////////////////////////////////////
// GTColorWellWnd window

class GTColorWellWnd : public CWnd
{

public:
	// Constroctor
	GTColorWellWnd();

// Attributes
public:

	//Create Arrow Type Window
	BOOL Create(DWORD dwStyle,
		CRect rcPos, 
		CWnd* pParent,
		UINT nID,
		int nBulletType,
		BOOL bPopup = FALSE);

// Operations

	COLORREF m_crColor;
	PsDlgColor *parent;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(GTColorWellWnd)
	protected:
		
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:

	// Destructor
	virtual ~GTColorWellWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(GTColorWellWnd)

	//message map function On Erase Background
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	//message map function On left button double clicked
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	//message map function On left button down
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	//message map function On left button up
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	//message map function On middle button double clicked
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);

	//message map function On middle button down
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);

	//message map function On middle button up
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);

    //message map function On Mouse Move
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	// message map function On keystroke  nonsystem character
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

    //message map function On window Destroyed
	afx_msg void OnDestroy();

   //message map function On window painted
	afx_msg void OnPaint();

	//message map function On right button down
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

	//message map function On right button up
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

	//message map function On Cancel Mode
	afx_msg void OnCancelMode();

	//message map function On Key Down
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//message map function On window size changed
	afx_msg void OnSize(UINT nType, int cx, int cy);
	// System color change.
	afx_msg void OnSysColorChange();
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG
	
	//message map function On Select Day OK
	afx_msg LONG OnSelectBulletOK(UINT wParam, LONG lParam);
	DECLARE_MESSAGE_MAP()
public:

    // The pointer to Notify Window
	CWnd *pNotifyWnd;
};


// slider visual modes against max, max_component_1 and max_component_2
const int max_slider [6][3] =
{
	{ max_rgb_red, max_rgb_green, max_rgb_blue },	// max_rgb red: R, G, B
	{ max_rgb_green, max_rgb_red, max_rgb_blue },	// max_rgb green: G, R, B
	{ max_rgb_blue, max_rgb_red, max_rgb_green },	// max_rgb blue: B, R, G
	{ max_hsv_hue, max_hsv_sat, max_hsv_value },	// max_hsv hue: H, S, V
	{ max_hsv_sat, max_hsv_hue, max_hsv_value },	// max_hsv sat: S, H, V
	{ max_hsv_value, max_hsv_hue, max_hsv_sat }		// max_hsv value: V, H, S
};

class GTColorSliderWnd : public CWnd
{

public:
	// Constroctor
	GTColorSliderWnd();

// Attributes
public:

	//Create Arrow Type Window
	BOOL Create(DWORD dwStyle,
		CRect rcPos, 
		CWnd* pParent,
		UINT nID,
		int nBulletType,
		BOOL bPopup = FALSE);

	// get/set layout
	inline int GetLayoutValue(void) const { return (m_nCurLayout); };
	void SetLayoutValue(int value);

	// get/set visual mode
	int GetDataValue(void) const { return (m_nCurMode & modes_mask); };
	void SetDataValue(int value);

	// get/set value at the begining of the slider
	inline int GetStartValue(void) const { return (m_nStartValue); };
	void SetStartValue(int value);

	// get/set value at the end of the slider
	inline int GetEndValue(void) const { return (m_nEndValue); };
	void SetEndValue(IN int value);

	// get/set position of the slider
	inline int GetPosition(void) const { return (m_nCurMode & modes_reverse ? m_nEndValue - m_nCurPosition : m_nCurPosition); };
	void SetPosition(IN int value);

	// get/set one-of slider additional components
	inline int GetControlValue(int index) const { return (additional_components [index]); };
	void SetControlValue(int index,int value);

	// painting methods
	void OnDraw(void);

	// draws blend in rgb mode - includes red, green and blue modes
	void DoDrawRGB(void);

	// draws blend in hsv mode - hue, sat and value modes separately
	void DoDrawHSV_Hue(void);
	void DoDrawHSV_Sat(void);
	void DoDrawHSV_Val(void);

	// parent notification methods
	virtual LRESULT	SendMessageToParent(IN UINT code);

public:

// Operations
	enum layouts
	{
		layout_horizontal = 0x8000,	// horizontal slider
		layout_tr_top = 0x2000,		// triangles
		layout_tr_bottom = 0x1000,

		layout_vertical = 0x4000,	// vertical slider
		layout_tr_left = 0x2000,	// triangles
		layout_tr_right = 0x1000,
	};

protected:

	int		m_nCurMode;	// visual mode (colorspace)
	int		m_nCurLayout;

	int		m_nStartValue;
	int		m_nEndValue;
	int		m_nCurPosition;
	int		m_nPreviousPos;
	int		m_nSteps;
	int		m_nPageSizes;

	CRect	m_rcPanel;
	CRect	m_rcPaint;
	int		m_nTriangle;


	// additional components depends on current visual mode (indexes given below):
	// - rgb red - 0 is green in [0, 255], 1 is blue in [0, 255]
	// - rgb green - 0 is red in [0, 255], 1 is blue in [0, 255]
	// - rgb blue - 0 is red in [0, 255], 1 is green in [0, 255]
	// - hsv hue - 0 is sat in [0, 1000 (1000 is 100.0% or 1.0)], 1 is value in [0, 1000]
	// - hsv sat - 0 is hue in [0, 3599 (3599 is 359.9 degs)], 1 is value in [0, 1000]
	// - hsv value - 0 is hue in [0, 3599], 1 is sat in [0, 1000]

	int		additional_components [4];

	DWORD	*row_buffer;			// 1px-high 32-bit bitmap, having same width as blend
	int		m_nBuffSize;			// size of the row buffer in doublewords

	// cache DIB bitmap information
	BITMAPINFO	bmp_info;
	HBITMAP		bmp_handle;
	DWORD		*bmp_data;

	CRect	m_rcFrame;				// control's m_rcFrame (bounding box)
	CRect	m_rcInteraction;	// area, where mouse-drags will be traced (client area)
	bool	m_bTracking;			// whether mouse is being tracked (drag)

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(GTColorSliderWnd)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:

	// Destructor
	virtual ~GTColorSliderWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(GTColorSliderWnd)

	//message map function On Erase Background
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	//message map function On left button double clicked
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	//message map function On left button down
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	//message map function On left button up
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	//message map function On middle button double clicked
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);

	//message map function On middle button down
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);

	//message map function On middle button up
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);

    //message map function On Mouse Move
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	// message map function On keystroke  nonsystem character
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

    //message map function On window Destroyed
	afx_msg void OnDestroy();

   //message map function On window painted
	afx_msg void OnPaint();

	//message map function On right button down
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

	//message map function On right button up
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

	//message map function On Cancel Mode
	afx_msg void OnCancelMode();

	//message map function On Key Down
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//message map function On window size changed
	afx_msg void OnSize(UINT nType, int cx, int cy);
	// System color change.
	afx_msg void OnSysColorChange();
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG
	
	//message map function On Select Day OK
	afx_msg LONG OnSelectBulletOK(UINT wParam, LONG lParam);
	DECLARE_MESSAGE_MAP()
public:

    // The pointer to Notify Window
	CWnd *pNotifyWnd;
};


class PsDlgColor : public CDialog
{
	DECLARE_DYNAMIC(PsDlgColor)

public:
	PsDlgColor(CWnd* pParent = NULL);   // standard constructor
	virtual ~PsDlgColor();

// Dialog Data
	enum { IDD = IDD_GTCOLORDIALOG_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	afx_msg void OnRadio4();
	afx_msg void OnRadio5();
	afx_msg void OnRadio6();
	afx_msg void OnEnChangeRGB();
	afx_msg void OnEnChangeHVS();
	DECLARE_MESSAGE_MAP()

public:
  bool ShowModal();
	void SetColor(COLORREF crColor);
	void SetColor(int, int, int);
  void UpdateText();
	COLORREF GetColor();
  int GetColorRValue();
	int GetColorGValue();
	int GetColorBValue();
	COLORREF m_crColor;	

public:
	GTColorSelectorWnd m_wndBulleted;
	GTColorWellWnd m_wndWell;
	GTColorSliderWnd m_wndSlider;

public:
	CEdit R;
	CEdit G;
	CEdit B;
	CEdit H;
	CEdit S;
	CEdit V;
};

