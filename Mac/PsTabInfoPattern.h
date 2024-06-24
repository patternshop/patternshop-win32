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
#ifndef PS_TAB_INFO_PATTERN_H__

#define PS_TAB_INFO_PATTERN_H__

#include "PsProject.h"

#include "DialogPatternCx.h"
#include "PsHardware.h"
#include "PsPAttern.h"

class PsWinProperties;

class PsTabInfoPattern
{
public:
	PsTabInfoPattern();
	~PsTabInfoPattern();

public:
	void Update();
	void Disable();
	void Enable();

protected:
	void UpdateValue(OSType, char *);
	PsLayer *GetCurrentLayer();
	void UpdateNow();
	void UpdateText();
	void UpdateMiniImage();
  
public:
	OSStatus EventsHandler(EventHandlerCallRef, EventRef);
	void InstallTextChangeEvent(OSType);
	void InstallEventHandlers();

public: // events
	void OnPaint(CGContextRef);
	void OnTimer();
	void OnShowWindow(bool);
	void OnChangeTX(const char*);
	void OnChangeTY(const char*);
	void OnChangeRX(const char*);
	void OnChangeRY(const char*);
	void OnChangeRZ(const char*);
	void OnChangeZ(const char*);

public:
	DialogPatternCx dpCore;
	PsWinProperties *parent;
	bool bNoProtect;
	
public: // zone d'affichage
	HIViewRef myHIView;
	HIViewTrackingAreaRef trakingArea;
	CGImageRef mybuffer;
	Rect bounds;

	bool bTracking;
	EventLoopTimerRef Timer;
	EventLoopTimerUPP sTimerUPP;
	bool bVisible;
	
};

#endif /* PS_TAB_INFO_PATTERN_H__ */
