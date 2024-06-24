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

#ifndef PS_DLG_EXPORT_H__

#define PS_DLG_EXPORT_H__

#include "PsDlgExportCx.h"

class PsDlgExport : public PsDlgExportCx
{
public:
	PsDlgExport();

public:
	bool DoModal();
	void Update();
	void Close();

private:
	void OnOkButton();
	void OnCancelButton();

private:
	bool bResult;
	bool bShowGrid;
	bool bNoProtect;

public:
	OSStatus EventsHandler(EventHandlerCallRef, EventRef);
	void DrawExportEvent(CGContextRef);
	void DrawPreviewEvent(CGContextRef);
	void UpdateValue(OSType, char *);
	
public:
	WindowRef window;
	HIViewRef exportImageView;
	HIViewRef previewImageView;
	int wmode, hmode;
};

#endif /* PS_DLG_EXPORT_H__ */
