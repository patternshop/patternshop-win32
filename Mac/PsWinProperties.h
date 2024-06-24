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
#ifndef PS_WIN_PROPERTIES_H__

#define PS_WIN_PROPERTIES_H__

#include "PsWinPropertiesCx.h"
#include "PsTabInfoImage.h"
#include "PsTabInfoMatrix.h"
#include "PsTabInfoPattern.h"

class PsWinProperties : public PsWinPropertiesCx
{
private:
	PsWinProperties();
	~PsWinProperties();

public:
	static PsWinProperties& Instance();
	static void Delete();

public:
	void Show();
	void UpdateInformation(PsProject*);
	void FocusImageInformation();
	void FocusMatrixInformation();
	void GetLocation(PsPoint&);
	void SetLocation(int, int);
	
private:
	static PsWinProperties* instance; // Singleton

protected:
	PsTabInfoMatrix m_matrixProperties;
	PsTabInfoImage m_imageProperties;
	PsTabInfoPattern m_PatternProperties;
	
protected:
	bool bNoProtect;
	
public:
	OSStatus EventsHandler(EventHandlerCallRef, EventRef);
	void InstallTextChangeEvent(OSType);
	void UpdateValue(OSType, char *);
	void DisableControl(OSType);
	void EnableControl(OSType);
	void OnTabChange(UInt16);

public:
	WindowRef window;
	Rect bounds;
};

OSStatus PsWinPropertiesEvents(EventHandlerCallRef myHandler, EventRef event, void *userData);

#endif /* PS_WIN_PROPERTIES_H__ */
