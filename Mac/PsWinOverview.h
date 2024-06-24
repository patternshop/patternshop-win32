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

#ifndef PS_WIN_OVERVIEW_H__

#define PS_WIN_OVERVIEW_H__

#include "PsMac.h"
#include "PsHardware.h"
#include "DialogOverviewCx.h"

class PsWinOverview : public PsMac, public DialogOverviewCx
{
private:
	PsWinOverview();
	
public:
	virtual ~PsWinOverview();
	static PsWinOverview& Instance();
	static void Delete();

public:
	void Show();
	void Close();

public:
	void FastUpdate();
	void DrawRedSelection();
	void UpdateMiniImage();
	void CleanBackground();

public:
	void GetLocation(PsPoint&);
	void SetLocation(int, int);

public:
	void UpdateNow();
	
private:
	static PsWinOverview* instance; // Singleton

public:	
	WindowRef window;
	HIViewRef myHIView;
	HIViewTrackingAreaRef trakingArea;
	Rect bounds;
	bool bTracking;
	EventLoopTimerRef Timer;
	EventLoopTimerUPP sTimerUPP;
};

#endif /* PS_WIN_OVERVIEW_H__ */


