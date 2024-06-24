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
#include "PsMacView.h"
#include "PsMacTools.h"
#include "PsWinOverview.h"
//#include "PsDlgDocument.h"
#include "PsProject.h"
#include "PsController.h"

PsWinOverview*	PsWinOverview::instance = 0;

static OSStatus OnViewEvent(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsWinOverview *View = (PsWinOverview*)userData;
	OSStatus status = eventNotHandledErr;
	
	switch ( GetEventKind( event ) )
	{
		
		case kEventControlTrackingAreaEntered:
			View->bTracking = true;
			break;
			
		case kEventControlTrackingAreaExited:
		{
			//View->mouseCursor = CURSOR_DEFAULT;
			//SetThemeCursor(kThemeArrowCursor);
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
			
			View->FastUpdate();
			DrawCGImage(myContext, bounds.size.height, View->window_buffer.buffer, 0, 0);	
			
			//View->CleanBackground();
			//DrawCGImage(myContext, bounds.size.height, View->window_buffer2.buffer, 0, 0);	
			
			break;
		}
	}
	
CantGetGraphicsContext:
CantGetBoundingRectangle:
		return status;
}

static OSStatus OnMouseClick(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsWinOverview *View = (PsWinOverview*)userData;
	Point wheresMyMouse;
	
	GetEventParameter(event, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(Point), NULL, &wheresMyMouse);
	
	if (View->bTracking)
	{
		
		PsPoint p;
		p.x = wheresMyMouse.h - View->bounds.left;
		p.y = wheresMyMouse.v - View->bounds.top;
		
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
				if (View->bDragging || View->zooming)
					View->OnLeftMouseButtonDown(p);
				break;
		}
		
		HIViewSetNeedsDisplay(View->myHIView, true);
	}
	
	return eventNotHandledErr;
}

static OSStatus OnBoundsChange(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsWinOverview *View = (PsWinOverview*)userData;
	GetEventParameter(event, kEventParamCurrentBounds, typeQDRectangle, NULL, sizeof(Rect), NULL, &View->bounds);
	return eventNotHandledErr;
}

pascal void WinOverviewIdleTimer(EventLoopTimerRef inTimer, void* userData)
{
	PsWinOverview *View = (PsWinOverview*)userData;
	if (View 
		 && !View->bUpdated 
		 && !PsController::Instance().bMouseButtonIsDown 
		 && !View->zooming 
		 && !View->bDragging) {
		if (hardwareRenderer.ReserveHardwareMutex())
		{
			View->UpdateNow();
			View->bUpdated = true;
			hardwareRenderer.ReleaseHardwareMutex();
		}
	}
}

PsWinOverview::PsWinOverview() : PsMac(), DialogOverviewCx()
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
	
	
	static const HIViewID  myHIViewID = { 'view', 0 };
	
	bTracking = false;

	//-- création de la fenetre
	err = CreateWindowFromNib(sNibRef, CFSTR("PsWinOverview"), &window);
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

	//-- buffers
	HIRect outRect;
	HIViewGetBounds(myHIView, &outRect);
	m_RenduImage.Create(outRect.size.width, outRect.size.height, 24);
	window_buffer2.Create(outRect.size.width, outRect.size.height, 24);
	window_buffer.Create(outRect.size.width, outRect.size.height, 24);
	iHeight = outRect.size.height;
	
	CGContextSetLineCap(window_buffer.context, kCGLineCapRound);
		
	// timer
	InstallEventLoopTimer(GetCurrentEventLoop(), 0, 0.01, NewEventLoopTimerUPP(WinOverviewIdleTimer), this, &Timer);
	
	// FIXME : appel forcé à OnBoundsChange
	Rect bound; GetWindowBounds(window, kWindowStructureRgn, &bound);
	bound.left++; SetWindowBounds(window, kWindowStructureRgn, &bound);
	//--
	
CantCreateWindow:
		;
}

PsWinOverview::~PsWinOverview()
{
}

PsWinOverview&	PsWinOverview::Instance()
{
	if(!instance)
		instance = new PsWinOverview();
	return *instance;
}

void	PsWinOverview::Delete()
{
	if(instance)
	{
		delete instance;
		instance = 0;
	}
}

void PsWinOverview::Show()
{
	ShowWindow(window);
	Update();
}

void PsWinOverview::UpdateNow()
{
	//printf("PsWinOverview::UpdateNow() {\n");
	if (PsController::Instance().project)
	{
		active->SetPixelBufferContext();
		UpdateMiniImage();		
		active->RestorePreviousContext();
	}
	HIViewSetNeedsDisplay(myHIView, true);
	//printf("PsWinOverview::UpdateNow() }\n\n");
}

void PsWinOverview::DrawRedSelection()
{
	if (!PsController::Instance().project)
		return;

	PsRect z = 
    GetSelectionRectangle(window_buffer2.GetWidth(), 
           window_buffer2.GetHeight());

	SetPenColor(255, 0, 0);
	CGContextSetLineWidth (dc, 2.f);
	MovePenTo(z.left, z.top);
	DrawLineTo(z.right, z.top);
	DrawLineTo(z.right, z.bottom);
	DrawLineTo(z.left, z.bottom);
	DrawLineTo(z.left, z.top);
	CGContextSetLineWidth (dc, 1.f);
}

void PsWinOverview::GetLocation(PsPoint &p)
{
	Rect bound;
	GetWindowBounds(window, kWindowStructureRgn, &bound);
	p.x = bound.left;
	p.y = bound.top;
}

void PsWinOverview::SetLocation(int x, int y)
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
