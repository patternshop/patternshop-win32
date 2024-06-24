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
#include "PsWinTools.h"
#include "PsController.h"

PsWinTools*	PsWinTools::instance = 0;

static OSStatus OnDraw(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsWinTools *View = (PsWinTools*)userData;
	OSStatus status = noErr;
	CGContextRef myContext;
	HIRect bounds;
	
	status = GetEventParameter (event, kEventParamCGContextRef, typeCGContextRef, NULL,
			sizeof (CGContextRef), NULL,  &myContext);
	require_noerr(status, CantGetGraphicsContext);
	
	HIViewGetBounds (View->myHIView, &bounds);
	require_noerr(status, CantGetBoundingRectangle);

	CGContextSetRGBFillColor( myContext, 212.f / 255.f, 208.f / 255.f, 200.f / 255.f, 1 );
	CGContextSetRGBStrokeColor( myContext, 212.f / 255.f, 208.f / 255.f, 200.f / 255.f, 1 );
	CGContextFillRect( myContext, bounds );

	DrawCGImage(myContext, bounds.size.height, View->ImageModifyUp, 10, 10);
	DrawCGImage(myContext, bounds.size.height, View->ImageScrollUp, 40, 10);
	DrawCGImage(myContext, bounds.size.height, View->ImageMagnifyUp, 10, 35);

	switch (View->currentTool)
	{
		case 0: DrawCGImage(myContext, bounds.size.height, View->ImageModifyDown, 10, 9); break;
		case 1: DrawCGImage(myContext, bounds.size.height, View->ImageScrollDown, 39, 9); break;
		case 2: DrawCGImage(myContext, bounds.size.height, View->ImageMagnifyDown, 9, 34); break;
	}
	
CantGetGraphicsContext:
CantGetBoundingRectangle:
	return status;
}

static OSStatus OnMouseClick(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsWinTools *View = (PsWinTools*)userData;
	Point wheresMyMouse;
	
	GetEventParameter(event, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(Point), NULL, &wheresMyMouse);
	
	int x = wheresMyMouse.h - View->bounds.left;
	int y = wheresMyMouse.v - View->bounds.top;
	
	if (IsMouseInCGImage(View->ImageModifyUp, 10, 10, x, y)) 
	{
		View->currentTool = 0;
		PsController::Instance().SetTool(PsController::TOOL_MODIFY);
	}
	else if (IsMouseInCGImage(View->ImageScrollDown, 40, 10, x, y))
	{
		View->currentTool = 1;
		PsController::Instance().SetTool(PsController::TOOL_SCROLL);
	}
	else if (IsMouseInCGImage(View->ImageMagnifyDown, 10, 35, x, y)) 
	{
		View->currentTool = 2;
		PsController::Instance().SetTool(PsController::TOOL_MAGNIFY);
	}
	
	HIViewSetNeedsDisplay(View->myHIView, true);
	
	return eventNotHandledErr;
}

static OSStatus OnBoundsChange(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsWinTools *View = (PsWinTools*)userData;
	GetEventParameter(event, kEventParamCurrentBounds, typeQDRectangle, NULL, sizeof(Rect), NULL, &View->bounds);
	return eventNotHandledErr;
}

PsWinTools::PsWinTools()
{
	OSStatus err = noErr;
	static const EventTypeSpec  myHIViewSpec[] = { kEventClassControl, kEventControlDraw };
	static const EventTypeSpec  myMouseSpec[] = { kEventClassMouse, kEventMouseDown };
	static const EventTypeSpec  myBoundsSpec[] = { kEventClassWindow, kEventWindowBoundsChanged };
	
	static const HIViewID  myHIViewID = { 'view', 0 };

	//-- création de la fenetre
	err = CreateWindowFromNib(sNibRef, CFSTR("PsWinTools"), &window);
   require_noerr( err, CantCreateWindow );

	//-- handle sur l'affichage
	HIViewFindByID (HIViewGetRoot(window), myHIViewID, &myHIView);
	err = InstallEventHandler (GetControlEventTarget (myHIView), NewEventHandlerUPP (OnDraw),
			GetEventTypeCount (myHIViewSpec), myHIViewSpec, (void *) this, NULL);
	
	//-- handle sur la souris
	InstallWindowEventHandler(window, NewEventHandlerUPP (OnMouseClick),
									  GetEventTypeCount (myMouseSpec), myMouseSpec, (void *) this, NULL);
	
	//-- handle sur le déplacement de la fenetre
	InstallWindowEventHandler(window, NewEventHandlerUPP (OnBoundsChange),
										GetEventTypeCount (myBoundsSpec), myBoundsSpec, (void *) this, NULL);

	//-- chargement des images
	ImageModifyUp = LoadCGImageFromPng("tool_modify1");
	ImageModifyDown = LoadCGImageFromPng("tool_modify3");
	ImageMagnifyUp = LoadCGImageFromPng("tool_magnify1");
	ImageMagnifyDown = LoadCGImageFromPng("tool_magnify3");
	ImageScrollUp = LoadCGImageFromPng("tool_scroll1");
	ImageScrollDown = LoadCGImageFromPng("tool_scroll3");
	
	// FIXME : appel forcé à OnBoundsChange
	Rect bound; GetWindowBounds(window, kWindowStructureRgn, &bound);
	bound.left++; SetWindowBounds(window, kWindowStructureRgn, &bound);
	//--
	
	currentTool = 0;
	
CantCreateWindow:
	;
}

PsWinTools&	PsWinTools::Instance()
{
	if(!instance)
		instance = new PsWinTools();
	return *instance;
}

PsWinTools::~PsWinTools()
{
	CGImageRelease(ImageModifyUp);
	CGImageRelease(ImageModifyDown);
	CGImageRelease(ImageMagnifyUp);
	CGImageRelease(ImageMagnifyDown);
	CGImageRelease(ImageScrollUp);
	CGImageRelease(ImageScrollDown);
}

void	PsWinTools::Delete()
{
	if(instance)
	{
		delete instance;
		instance = 0;
	}
}

void PsWinTools::Show()
{
	ShowWindow(window);
	HIViewSetNeedsDisplay(myHIView, true);
}

void PsWinTools::GetLocation(PsPoint &p)
{
	Rect bound;
	GetWindowBounds(window, kWindowStructureRgn, &bound);
	p.x = bound.left;
	p.y = bound.top;
}

void PsWinTools::SetLocation(int x, int y)
{
	Rect bound;
	GetWindowBounds(window, kWindowStructureRgn, &bound);
	int w = bound.right - bound.left;
	int h = bound.bottom - bound.top;
	bound.left = x;
	bound.right = x + w;
	bound.top = y;
	bound.bottom = y + h;
	SetWindowBounds(window, kWindowStructureRgn, &bound);
}
