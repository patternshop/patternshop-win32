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
#ifndef PS_WIN_PROJECT_MAC_H__

#define PS_WIN_PROJECT_MAC_H__

#include "PsMac.h"
#include "PsProject.h"

#include <Carbon/Carbon.h>

class PsScrollBarMac : public PsScrollBar
{
public:
	PsScrollBarMac(ControlRef);
	int GetWidth();
	void SetSize(int);
	int GetPos();
	void Enable();
	void Disable();
	ControlRef inControl;
};

class PsWinProjectMac : public PsMac, public PsWinProjectModel
{
public:
	PsWinProjectMac();

public:
	void Show();
	void Update();
	
public:
	SoftwareBuffer buffer;	
	WindowRef window;
	HIViewRef myHIView;
	HIViewTrackingAreaRef trakingArea;
	Rect bounds;
	bool bTracking;

public:
	void UpdateSize();
	void UpdateMouseCursor();
	void DrawInsertionCaret();

public:
	void GetLocation(PsPoint&);
	void SetLocation(int, int);
};


#endif /* PS_WIN_PROJECT_MAC_H__ */
