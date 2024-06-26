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
#include "PsController.h"

#define CLASS_NAME	"PatternShop_413641"
#define WM_UNIQUE	(WM_USER + 0)

class PsMultiDocTemplate;

class CPatternshopApp : public CWinApp
{
friend class CMultiDocTemplate;

public:
	CPatternshopApp();

private:
#ifdef LANG_DLL
   HINSTANCE            englishDLL;
   HINSTANCE            frenchDLL;
#endif
   PsMultiDocTemplate*  docTemplate;

public:
   void SetLanguage(PsLanguage nLanguage);

public:
// Substitutions
	virtual BOOL	InitInstance();
	virtual int		ExitInstance();

// Implémentation
	afx_msg void OnAppAbout();

	DECLARE_MESSAGE_MAP()
};

extern CPatternshopApp theApp;
