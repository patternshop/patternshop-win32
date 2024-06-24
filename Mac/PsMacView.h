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
#ifndef PS_MAC_VIEW_H__

#define PS_MAC_VIEW_H__

#include "PsProject.h"

#include <Carbon/Carbon.h>

#include <AGL/agl.h>
#include <AGL/aglRenderers.h>
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <OpenGL/glu.h>

class PsMacView
{
public:
	PsMacView();

public:
	inline void Update() { bUpdated = false; }
	
	void UpdateTitle(char *);
	void UpdateNow();
	void OnKeyDown(UInt32);
	void OnKeyUp(UInt32);

public:
	void MenuLogRedo();
	void OnUpdateEditRedo();
	void MenuLogUndo();
	void OnUpdateEditUndo();
	
public:
	void MenuEditClone();
	void MenuMatrixClone();
	
public:
	void MenuMatrixDel();
	void MenuItemDel();

public:
	void SetPixelBufferContext();
	void RestorePreviousContext();

public:
	PsProject*	project;
	WindowRef	window;
	HIViewRef myHIView;
	RgnHandle rgn;
	HIViewTrackingAreaRef trakingArea;
	AGLContext gaglContext;
	CGImageRef cgImage;
	Rect bounds;
	char *Path;
	bool bUpdated;
	uint8 tmppixelbuffer[5000 * 5000 * 4];
	EventLoopTimerRef gTimer;
	EventLoopTimerUPP sTimerUPP;
	long width;
	long height;
	bool bTracking;
	PsCursor iCurrentCursor;
	AGLContext oldContext;
};

PsMacView *CreateNewViewWindow();

extern PsMacView* active;

enum PsMacViewbufferingType
{
	PSMACVIEW_PBUFFER_TO_QUARTZIMAGE, // fast
	PSMACVIEW_DIRECT_RENDERING // slow
};

extern PsMacViewbufferingType MyBufferingType;

#endif /* PS_MAC_VIEW_H__ */
