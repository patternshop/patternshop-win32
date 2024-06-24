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
#ifndef PS_DLG_PRINT_H__

#define PS_DLG_PRINT_H__

#include "PsMac.h"

class PsDlgPrint : public PsMac
{
public:
	PsDlgPrint();

public:
	bool DoModal();
	bool DoModalPreview();
	void Update();
	void Close();

private:
	void Initialize();
	void OnOkButton();
	void OnCancelButton();
	void UpdateMiniImage();
	void PrepareMiniImage();
	void UpdateAutoCenter();
	void UpdateFillSize();
	void SetZ(float z_);
	
private:
	bool bResult;
	bool bNoProtect;

public:
	int x, y, h, w;
	float z;
	float r_zoom, r_zoom2;
	int format_w, format_h;
	bool bCenter, bAuto, bCadre;

public:
	void InstallTextChangeEvent(OSType);
	OSStatus EventsHandler(EventHandlerCallRef, EventRef);
	void OnDrawView(CGContextRef);

protected:
	int GetIntegerValue(char *, int);
	void GetTextValue(double, int, char *);
	void UpdateValue(OSType, char *);

public:
	WindowRef window;
	HIViewRef myHView;
	CGImageRef imageLink;
	SoftwareBuffer imageBuffer, imageOriginal;
	SoftwareBuffer m_RenduImage;
	int lmode, hmode, xmode, ymode;
};

#endif /* PS_DLG_PRINT_H__ */
