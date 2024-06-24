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
#include "PsMacApp.h"
#include "PsMacTools.h"
#include "PsWinSplash.h"

PsWinSplash*	PsWinSplash::instance = 0;

static OSStatus OnDraw(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsWinSplash *View = (PsWinSplash*)userData;
	OSStatus status = noErr;
	CGContextRef myContext;
	
	status = GetEventParameter (event, kEventParamCGContextRef, typeCGContextRef, NULL,
										 sizeof (CGContextRef), NULL,  &myContext);
	require_noerr(status, CantGetGraphicsContext);
		
	View->OnDraw(myContext);
	
CantGetGraphicsContext:
		return status;
}

void PsWinSplash::OnDraw(CGContextRef myContext)
{
	HIRect bounds;
	OSStatus status = noErr;
	
	HIViewGetBounds (myHIView, &bounds);
	require_noerr(status, CantGetBoundingRectangle);
	
	DrawCGImage(myContext, bounds.size.height, Background, 0, 0);

CantGetBoundingRectangle:
	;
}

static OSStatus OnMouseClick(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	// FIXME
	return eventNotHandledErr;
}

pascal void WinSplashIdleTimer(EventLoopTimerRef inTimer, void* userData)
{
	PsWinSplash *View = (PsWinSplash*)userData;
	if (View && View->bNeedToBeClosed)
	{
		View->Close();
	}
}

PsWinSplash::PsWinSplash()
{
	OSStatus err = noErr;
	static const EventTypeSpec  myHIViewSpec[] = { kEventClassControl, kEventControlDraw };
	static const EventTypeSpec  myMouseSpec[] = { kEventClassMouse, kEventMouseDown };
	static const HIViewID  myHIViewID = { 'view', 0 };
	
	bNeedToBeClosed = false;
	
	//-- création de la fenetre
	err = CreateWindowFromNib(sNibRef, CFSTR("PsWinSplash"), &window);
   require_noerr( err, CantCreateWindow );
	
	//-- handle sur l'affichage
	HIViewFindByID (HIViewGetRoot(window), myHIViewID, &myHIView);
	err = InstallEventHandler (GetControlEventTarget (myHIView), NewEventHandlerUPP (::OnDraw),
										GetEventTypeCount (myHIViewSpec), myHIViewSpec, (void *) this, NULL);
	
	//-- handle sur la souris
	InstallWindowEventHandler(window, NewEventHandlerUPP (OnMouseClick),
									  GetEventTypeCount (myMouseSpec), myMouseSpec, (void *) this, NULL);
		
	//-- chargement des images
	Background = LoadCGImageFromPng("splash1");
	
	//-- timer
	InstallEventLoopTimer(GetCurrentEventLoop(), 2, 0.25, NewEventLoopTimerUPP(WinSplashIdleTimer), this, &gTimer);
	
CantCreateWindow:
		;
}

PsWinSplash& PsWinSplash::Instance()
{
	if(!instance)
		instance = new PsWinSplash();
	return *instance;
}

PsWinSplash::~PsWinSplash()
{
	CGImageRelease(Background);
}

void	PsWinSplash::Delete()
{
	if(instance)
	{
		delete instance;
		instance = 0;
	}
}

void PsWinSplash::Show()
{
	ShowWindow(window);
}

void PsWinSplash::Close()
{
	DisposeWindow(window);
	Delete();
}



