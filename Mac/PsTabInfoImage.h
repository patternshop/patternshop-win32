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

#ifndef PS_TAB_INFO_IMAGE_H__

#define PS_TAB_INFO_IMAGE_H__

#include "PsProject.h"

class PsWinProperties;

class PsTabInfoImage
{
public:
	PsTabInfoImage();
	~PsTabInfoImage();

public:
	void Update(PsImage*);
	void Disable();
	void Enable();

protected:
	void UpdateValue(OSType, char *);
	void UpdateLinkButton();
	
public:
	OSStatus EventsHandler(EventHandlerCallRef, EventRef);
	void InstallTextChangeEvent(OSType);
	void InstallEventHandlers();
	void OnCheckBoxClicked();

public:
	PsWinProperties *parent;
	bool bNoProtect;
};

#endif /* PS_TAB_INFO_IMAGE_H__ */

