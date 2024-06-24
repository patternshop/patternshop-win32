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
#include "PsMacCursor.h"
#include "PsWinProject.h"

#define BACKBUFFER_STEPSIZE 32

static OSStatus OnViewEvent(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsWinProjectMac *View = (PsWinProjectMac*)userData;
	OSStatus status = eventNotHandledErr;
	
	switch ( GetEventKind( event ) )
	{

		case kEventControlTrackingAreaEntered:
			View->bTracking = true;
			break;
			
		case kEventControlTrackingAreaExited:
		{
			View->mouseCursor = CURSOR_DEFAULT;
			SetThemeCursor(kThemeArrowCursor);
			View->bTracking = false;
			break;
		}
			
		case kEventControlDraw:
		{	
			CGContextRef myContext;
			HIRect bounds;
			status = GetEventParameter (event, kEventParamCGContextRef, typeCGContextRef, NULL,
												 sizeof (CGContextRef), NULL,  &myContext);
			require_noerr(status, CantGetGraphicsContext);
	
			CGContextSetShouldAntialias(myContext, false);
			
			HIViewGetBounds (View->myHIView, &bounds);
			require_noerr(status, CantGetBoundingRectangle);
			int iWinHeigth = bounds.size.height;

			if (View->totalHSize - iWinHeigth >= 0)
				View->scrollbar->SetSize(View->totalHSize - iWinHeigth);
			else
			{
				View->scrollbar->SetSize(0);
				SetControl32BitValue(((PsScrollBarMac*)View->scrollbar)->inControl, 0);
				//View->scrollbar->SetPos(0);
			}
			
			//-- copy du backbuffer
			int pos = GetControl32BitValue(((PsScrollBarMac*)View->scrollbar)->inControl);
			
			CGImageRef myImage = View->buffer.buffer;
			int x = 0, y = -pos;
			
			CGContextTranslateCTM( myContext, 0, iWinHeigth);
			CGContextScaleCTM(myContext, 1.0, -1.0 );
			int iY = -y - View->iHeight + iWinHeigth;
			//printf("y:%d h:%d\n", iY, CGImageGetHeight(myImage));
			CGContextDrawImage(myContext, CGRectMake(x, iY, 
						CGImageGetWidth(myImage), CGImageGetHeight(myImage)), myImage);
			CGContextScaleCTM(myContext, 1.0, -1.0 );
			CGContextTranslateCTM( myContext, 0, -iWinHeigth);
				
			//DrawCGImage(myContext, bounds.size.height, View->buffer.buffer, 0, 0);	
			break;
		}
	}

CantGetGraphicsContext:
CantGetBoundingRectangle:
		return status;
}

static OSStatus OnMouseClick(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsWinProjectMac *View = (PsWinProjectMac*)userData;
	Point wheresMyMouse;
	
	GetEventParameter(event, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(Point), NULL, &wheresMyMouse);
	
	if (View->bTracking)
	{

		PsPoint p;
		p.x = wheresMyMouse.h - View->bounds.left;
		p.y = wheresMyMouse.v - View->bounds.top;
		
		int pos = GetControl32BitValue(((PsScrollBarMac*)View->scrollbar)->inControl);
		p.y += pos;
		
		switch ( GetEventKind( event ) )
		{
			case kEventMouseDown:
				View->OnLeftMouseButtonDown(p);
				break;
		
			case kEventMouseUp:
				View->OnLeftMouseButtonUp(p);
				break;
		
			case kEventMouseDragged:
			case kEventMouseMoved:
				View->OnMyMouseMove(p);
				break;
		}
	}
	
	return eventNotHandledErr;
}

static OSStatus OnBoundsChange(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsWinProjectMac *View = (PsWinProjectMac*)userData;
	GetEventParameter(event, kEventParamCurrentBounds, typeQDRectangle, NULL, sizeof(Rect), NULL, &View->bounds);
	return eventNotHandledErr;
}

static OSStatus OnScrollEvent(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsWinProjectMac *View = (PsWinProjectMac*)userData;
	View->Update();
	return eventNotHandledErr;
}

PsWinProjectMac::PsWinProjectMac()
{
	OSStatus err = noErr;
	
	static const EventTypeSpec  myHIViewSpec[] =
	{
		{ kEventClassControl, kEventControlTrackingAreaEntered },	
		{ kEventClassControl, kEventControlTrackingAreaExited },
		{ kEventClassControl, kEventControlDraw }
	};
	
	static const EventTypeSpec  myMouseSpec[] = 
	{ 
		{ kEventClassMouse, kEventMouseDown },
		{ kEventClassMouse, kEventMouseUp },
		{ kEventClassMouse, kEventMouseDragged },
		{ kEventClassMouse, kEventMouseMoved }
	};
	
	static const EventTypeSpec  myBoundsSpec[] = 
	{ 
		kEventClassWindow, kEventWindowBoundsChanged 
	};
	
	static const EventTypeSpec  myScrollSpec[] =
	{
		kEventClassControl, kEventControlValueFieldChanged 
	};
		
	static const HIViewID  myHIViewID = { 'view', 0 };
	
	//-- création de la fenetre
	err = CreateWindowFromNib(sNibRef, CFSTR("PsWinProject"), &window);
   require_noerr( err, CantCreateWindow );
	
	//-- handle sur la vue
	HIViewFindByID (HIViewGetRoot(window), myHIViewID, &myHIView);
	HIViewNewTrackingArea (myHIView, NULL, NULL, &trakingArea);
	err = InstallEventHandler (GetControlEventTarget (myHIView), NewEventHandlerUPP (OnViewEvent),
										GetEventTypeCount (myHIViewSpec), myHIViewSpec, (void *) this, NULL);
	
	//-- handle sur la souris
	InstallWindowEventHandler(window, NewEventHandlerUPP (OnMouseClick),
									  GetEventTypeCount (myMouseSpec), myMouseSpec, (void *) this, NULL);
	
	//-- handle sur le déplacement de la fenetre
	InstallWindowEventHandler(window, NewEventHandlerUPP (OnBoundsChange),
									  GetEventTypeCount (myBoundsSpec), myBoundsSpec, (void *) this, NULL);
	
	HIRect outRect;
	HIViewGetBounds(myHIView, &outRect);
	
	//-- création du backbuffer
	iHeight = 0;
	UpdateSize();
	
	//-- mise en gris du fond
	CGContextSetRGBFillColor( dc, 212.f / 255.f, 208.f / 255.f, 200.f / 255.f, 1 );
	CGContextSetRGBStrokeColor( dc, 212.f / 255.f, 208.f / 255.f, 200.f / 255.f, 1 );
	CGContextFillRect( dc, outRect );
	
	//-- chargement des images
	LoadSoftwareBufferFromPng(viewImage, "view");
	LoadSoftwareBufferFromPng(boxImage, "box");
	LoadSoftwareBufferFromPng(directoryImage, "directory");
	LoadSoftwareBufferFromPng(openImage, "open");
	LoadSoftwareBufferFromPng(closeImage, "close");
	LoadSoftwareBufferFromPng(directoryImageB, "directoryb");
	LoadSoftwareBufferFromPng(openImageB, "openb");
	LoadSoftwareBufferFromPng(closeImageB, "closeb");
	LoadSoftwareBufferFromPng(spot, "spot");
	
	// création de la scrollbar
	ControlRef control;
	ControlID myControlID;	
	myControlID.signature = 'scro';
	myControlID.id = 0;
	GetControlByID (window, &myControlID, &control);
	scrollbar = new PsScrollBarMac(control);
	
	//-- handle sur la scrollbar
	InstallEventHandler (GetControlEventTarget(control), NewEventHandlerUPP(OnScrollEvent),
								GetEventTypeCount (myScrollSpec), myScrollSpec, (void *) this, NULL);
	
	// FIXME : appel forcé à OnBoundsChange
	Rect bound; GetWindowBounds(window, kWindowStructureRgn, &bound);
	bound.left++; SetWindowBounds(window, kWindowStructureRgn, &bound);
	//--

CantCreateWindow:
	;	
}

void PsWinProjectMac::Show()
{
	ShowWindow(window);
	totalHSize = 0;
	Update();
}

void PsWinProjectMac::Update()
{ 
	GenericUpdate(); 
	if (totalHSize && iHeight < totalHSize)
	{
		iHeight = ((totalHSize / BACKBUFFER_STEPSIZE) + 1) * BACKBUFFER_STEPSIZE;
		//printf("%d %d\n", totalHSize, iHeight);
		if (!buffer.IsNull()) 
			buffer.Destroy();
		buffer.Create(iWidth, iHeight, 24);
		CGContextSetShouldAntialias(buffer.context, false);
		dc = buffer.context;
		GenericUpdate(); 
	}
	HIViewSetNeedsDisplay(myHIView, true);
}

void PsWinProjectMac::UpdateSize()
{
	HIRect lbounds;
	HIViewGetBounds(myHIView, &lbounds);
	iWidth = lbounds.size.width;
	if (iHeight < lbounds.size.height)
	{
		iHeight = lbounds.size.height;
		//printf("%d\n", iHeight);
		if (!buffer.IsNull()) 
			buffer.Destroy();
		buffer.Create(iWidth, iHeight, 24);
		CGContextSetShouldAntialias(buffer.context, false);
		dc = buffer.context;
	}
}

void PsWinProjectMac::UpdateMouseCursor() 
{
	SetMacCursor(mouseCursor);
}

void PsWinProjectMac::GetLocation(PsPoint &p)
{
	Rect bound;
	GetWindowBounds(window, kWindowStructureRgn, &bound);
	p.x = bound.left;
	p.y = bound.top;
}

void PsWinProjectMac::SetLocation(int x, int y)
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

PsScrollBarMac::PsScrollBarMac(ControlRef inControl) 
{
	this->inControl = inControl;
}

int PsScrollBarMac::GetWidth() 
{ 
	return 15; 
}

void PsScrollBarMac::SetSize(int iSize) 
{
	 SetControl32BitMaximum(inControl, iSize);
}

int PsScrollBarMac::GetPos() 
{ 	
	return 0;//GetControl32BitValue(inControl); 
}

void PsScrollBarMac::Enable() 
{
	EnableControl(inControl);
}

void PsScrollBarMac::Disable() 
{
	/*
	DisableControl(inControl);
	SetControl32BitValue(inControl, 0); 
	 */
}


