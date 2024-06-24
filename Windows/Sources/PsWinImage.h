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

#include "PsImage.h"
#include "PsWin32.h"
#include "afxcmn.h"
#include <atlimage.h>
#include <map>


// PsWinImage dialog

class PsWinImage : public PsWin32
{
	DECLARE_DYNAMIC(PsWinImage)

public:
	PsWinImage(CWnd* pParent = NULL);   // standard constructor
	virtual ~PsWinImage();
  static PsWinImage& Instance();
	static void Delete();

// Dialog Data
	enum { IDD = IDD_IMAGE };

protected:
  static PsWinImage* instance; // Singleton
	typedef std::map<PsImage*, CImage*> ImageCache;
	ImageCache imageList;
	CImage *imgC;

protected:

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();
};

