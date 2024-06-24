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

#include "PsWinProperties.h"
#include "PsTabInfoPattern.h"
#include "PsWinImage.h"
#include "PsWinProject.h"
#include "PsDlgDocument.h"

#include "PsAction.h"
#include "PsProject.h"
#include "PsController.h"

pascal void InfoPatternIdleTimer(EventLoopTimerRef inTimer, void* userData)
{
	PsTabInfoPattern *View = (PsTabInfoPattern*)userData;
	if (View) View->OnTimer();
}

PsTabInfoPattern::PsTabInfoPattern()
{
	bNoProtect = true; // FIXME
	parent = NULL;
	bTracking = false;
	mybuffer = NULL;
	bVisible = false;
	
	// mise ˆ jour pŽriode de la miniature
	InstallEventLoopTimer(GetCurrentEventLoop(), 0, 0.01, NewEventLoopTimerUPP(InfoPatternIdleTimer), this, &Timer);
}

PsTabInfoPattern::~PsTabInfoPattern()
{
	if (mybuffer)
	{
		CGImageRelease(mybuffer);
		mybuffer = NULL;
	}
}

void PsTabInfoPattern::OnShowWindow(bool bShow)
{
	dpCore.OnShowWindow(bShow);
	
	if (bShow)
	{
		Update();
	}
	
	bVisible = bShow;
}

void PsTabInfoPattern::UpdateText()
{
	PsLayer *layer = GetCurrentLayer();
	 
	if (layer)
	{
		Enable();
		char buffer[1024];
		sprintf(buffer, "%.2f", layer->vTranslation.X);
		UpdateValue('tx  ', buffer);
		sprintf(buffer, "%.2f", layer->vTranslation.Y);
		UpdateValue('ty  ', buffer);
		sprintf(buffer, "%.2f", layer->rRotation.Roll);
		UpdateValue('rx  ', buffer);
		sprintf(buffer, "%.2f", layer->rRotation.Pitch);
		UpdateValue('ry  ', buffer);
		sprintf(buffer, "%.2f", layer->rRotation.Yaw);
		UpdateValue('rz  ', buffer);
		sprintf(buffer, "%.2f", layer->fScale * 100.f);
		UpdateValue('z   ', buffer);
	}
	else
	{
		Disable();
	}
}

void PsTabInfoPattern::Update()
{
	if (!PsController::Instance().project)
	{
		if (mybuffer) 
		{
			CGImageRelease(mybuffer);
			mybuffer = NULL;
		}
	}
	dpCore.Update();
	UpdateText();
	dpCore.bUpdated = false;
}

PsLayer *PsTabInfoPattern::GetCurrentLayer()
{
	if (!PsController::Instance().project) 
		return NULL;
	PsProject *project = PsController::Instance().project;
	if (!project->pattern) 
		return NULL;
	return project->pattern->aLayers[project->iLayerId];
}

void PsTabInfoPattern::UpdateValue(OSType signature, char *buffer)
{
	ControlRef control;
	ControlID myControlID;
	
	// protection
	if (!bNoProtect)
	{
		GetKeyboardFocus(parent->window, &control);
		GetControlID(control, &myControlID);
		if (myControlID.signature == signature) 
			return;
	}
	
	// mise ˆ jour
	myControlID.signature = signature;
	myControlID.id = 0;
	GetControlByID(parent->window, &myControlID, &control);
	SetControlData(control, kControlNoPart, kControlEditTextTextTag, strlen(buffer), buffer);	
}

void PsTabInfoPattern::InstallTextChangeEvent(OSType signature)
{
	static const EventTypeSpec kControlEvents[] = { kEventClassTextField, kEventTextDidChange };
	ControlRef control;
	ControlID myControlID;
	myControlID.id = 0;
	myControlID.signature = signature;
	GetControlByID(parent->window, &myControlID, &control);
	InstallControlEventHandler(control, NewEventHandlerUPP(PsWinPropertiesEvents),
										GetEventTypeCount(kControlEvents), kControlEvents, (void*)parent, NULL);	
}

static OSStatus OnViewEvent(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsTabInfoPattern *View = (PsTabInfoPattern*)userData;
	OSStatus status = eventNotHandledErr;
	
	switch ( GetEventKind( event ) )
	{
		case kEventControlTrackingAreaEntered:
			View->bTracking = true;
			break;
			
		case kEventControlTrackingAreaExited:
			View->bTracking = false;
			break;
			
		case kEventControlDraw:
		{	
			CGContextRef myContext;
			status = GetEventParameter (event, kEventParamCGContextRef, typeCGContextRef, NULL,
												 sizeof (CGContextRef), NULL,  &myContext);
			require_noerr(status, CantGetGraphicsContext);
			CGContextSetShouldAntialias(myContext, false);
			View->OnPaint(myContext);
			break;
		}
	}
	
CantGetGraphicsContext:
CantGetBoundingRectangle:
		return status;
}

static OSStatus OnMouseClick(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsTabInfoPattern *View = (PsTabInfoPattern*)userData;
	Point wheresMyMouse;
	
	GetEventParameter(event, kEventParamWindowMouseLocation, typeQDPoint, NULL, sizeof(Point), NULL, &wheresMyMouse);
	
	if (View->bTracking)
	{
		PsPoint p;
		p.x = wheresMyMouse.h - 27; // static hiview location
		p.y = wheresMyMouse.v - 61;
		switch ( GetEventKind( event ) )
		{
			case kEventMouseDown:
				View->dpCore.OnButtonDown(p.x, p.y);
				break;
		}
		HIViewSetNeedsDisplay(View->myHIView, true);
	}
	
	return eventNotHandledErr;
}


void PsTabInfoPattern::InstallEventHandlers()
{
	
	static const EventTypeSpec  myHIViewSpec[] =
	{
		{ kEventClassControl, kEventControlTrackingAreaEntered },	
		{ kEventClassControl, kEventControlTrackingAreaExited },
		{ kEventClassControl, kEventControlDraw }
	};

	static const EventTypeSpec  myMouseSpec[] = 
	{ 
		{ kEventClassMouse, kEventMouseDown }
	};
	
	static const HIViewID  myHIViewID = { 'mini', 0 };
	
	//-- handle sur la vue
	HIViewFindByID (HIViewGetRoot(parent->window), myHIViewID, &myHIView);
	HIViewNewTrackingArea (myHIView, NULL, NULL, &trakingArea);
	InstallEventHandler (GetControlEventTarget (myHIView), NewEventHandlerUPP (OnViewEvent),
						GetEventTypeCount (myHIViewSpec), myHIViewSpec, (void *) this, NULL);
	
	//-- handle sur la souris
	InstallWindowEventHandler(parent->window, NewEventHandlerUPP (OnMouseClick),
									  GetEventTypeCount (myMouseSpec), myMouseSpec, (void *) this, NULL);
		
	InstallTextChangeEvent('tx  ');
	InstallTextChangeEvent('ty  ');
	InstallTextChangeEvent('z   ');
	InstallTextChangeEvent('rx  ');
	InstallTextChangeEvent('ry  ');
	InstallTextChangeEvent('rz  ');

}

void PsTabInfoPattern::Disable()
{
	ControlRef control;
	ControlID controlId;
	controlId.id = 131;
	controlId.signature = 'tabs';
	GetControlByID(parent->window, &controlId, &control);
	DeactivateControl(control);
	UpdateValue('tx  ', "");
	UpdateValue('ty  ', "");
	UpdateValue('z   ', "");	
	UpdateValue('rx  ', "");
	UpdateValue('ry  ', "");
	UpdateValue('rz  ', "");
}

void PsTabInfoPattern::Enable()
{
	ControlRef control;
	ControlID controlId;
	controlId.id = 131;
	controlId.signature = 'tabs';
	GetControlByID(parent->window, &controlId, &control);
	ActivateControl(control);
}

void PsTabInfoPattern::OnTimer()
{
	if (bVisible && !dpCore.bUpdated && !PsController::Instance().bMouseButtonIsDown)
	{
		if (hardwareRenderer.ReserveHardwareMutex())
		{
			UpdateNow();
			dpCore.bUpdated = true;
			hardwareRenderer.ReleaseHardwareMutex();
		}
	}
}

void PsTabInfoPattern::OnChangeTX(const char *buffer)
{
	PsLayer *layer = GetCurrentLayer();
	layer->vTranslation.X = atof(buffer);
}

void PsTabInfoPattern::OnChangeTY(const char *buffer)
{
	PsLayer *layer = GetCurrentLayer();
	layer->vTranslation.Y = atof(buffer);
}

void PsTabInfoPattern::OnChangeRX(const char *buffer)
{
	PsLayer *layer = GetCurrentLayer();
	layer->rRotation.Roll = atof(buffer);
}

void PsTabInfoPattern::OnChangeRY(const char *buffer)
{
	PsLayer *layer = GetCurrentLayer();
	layer->rRotation.Pitch = atof(buffer);
}

void PsTabInfoPattern::OnChangeRZ(const char *buffer)
{
	PsLayer *layer = GetCurrentLayer();
	layer->rRotation.Yaw = atof(buffer);
}

void PsTabInfoPattern::OnChangeZ(const char *buffer)
{
	PsLayer *layer = GetCurrentLayer();
	layer->fScale = atof(buffer) / 100.f;
}

OSStatus PsTabInfoPattern::EventsHandler(EventHandlerCallRef myHandler, EventRef event)
{
	OSStatus result = eventNotHandledErr;
	
	if (!PsController::Instance().project)
		return eventNotHandledErr;
	
	switch ( GetEventClass( event ) )
	{
		case kEventClassTextField:
		{
			switch ( GetEventKind( event ) )
			{
				case kEventTextDidChange:
				{
					ControlRef control;
					ControlID myControlID;
					GetKeyboardFocus(parent->window, &control);
					if (control)
					{
						Size size;
						char buffer[1024]; 
						GetControlID(control, &myControlID);
						GetControlData(control, kControlNoPart, kControlEditTextTextTag, 1024, buffer, &size);
						buffer[size] = 0;
						
						if (size > 0)
						{
							switch (myControlID.signature)
							{
								case 'tx  ': OnChangeTX(buffer); break;
								case 'ty  ': OnChangeTY(buffer); break;
								case 'rx  ': OnChangeRX(buffer); break;
								case 'ry  ': OnChangeRY(buffer); break;
								case 'rz  ': OnChangeRZ(buffer); break;
								case 'z   ': OnChangeZ(buffer); break;
								default: break;
							}
							
							PsController::Instance().UpdateWindow();
						}
					}
					break;
				}
			}
			break;
		}
			
		case kEventClassCommand:
		{
			HICommandExtended cmd;
			verify_noerr( GetEventParameter( event, kEventParamDirectObject, typeHICommand, NULL, sizeof( cmd ), NULL, &cmd ) );
			
			switch ( GetEventKind( event ) )
			{
				case kEventCommandProcess:
				{
					switch ( cmd.commandID )
					{
						case 'qa  ':
						{
							ControlID myControlID;
							myControlID.signature = 'qa  ';
							myControlID.id = 0;
							ControlRef sliderItem;
							GetControlByID(parent->window, &myControlID, &sliderItem);
							SInt32 v = GetControl32BitValue(sliderItem);
							dpCore.SetQuality(v);
							break;
						}
					}
					break;
				}
			}
		}			
	}
	return result;
}

void PsTabInfoPattern::UpdateMiniImage()
{
	//printf("PsTabInfoPattern::UpdateMiniImage() {\n");
	active->SetPixelBufferContext();
	HIRect outRect;
	HIViewGetBounds(myHIView, &outRect);
	dpCore.UpdateMiniImage((int)outRect.size.width, (int)outRect.size.height);
	CGRect subRect = CGRectMake(0, 0, dpCore.dst_width, dpCore.dst_height);
	CGImageRef subImage = CGImageCreateWithImageInRect(dpCore.m_RenduImage.buffer, subRect);
	if (mybuffer) CGImageRelease(mybuffer);
	mybuffer = subImage;
	active->RestorePreviousContext();
	//printf("PsTabInfoPattern::UpdateMiniImage() }\n\n");
}

void PsTabInfoPattern::UpdateNow()
{
	if (PsController::Instance().project)
	{
		UpdateMiniImage();
	}
	HIViewSetNeedsDisplay(myHIView, true);
}

void PsTabInfoPattern::OnPaint(CGContextRef myContext)
{				
	HIRect lbounds;
	HIViewGetBounds (myHIView, &lbounds);
	dpCore.dst_x1 = (lbounds.size.width - CGImageGetWidth(mybuffer)) / 2; 
	dpCore.dst_y1 = (lbounds.size.height - CGImageGetHeight(mybuffer)) / 2;
	dpCore.dst_x2 = dpCore.dst_x1 + dpCore.dst_width;
	dpCore.dst_y2 = dpCore.dst_y1 + dpCore.dst_height;		
	DrawCGImage(myContext, lbounds.size.height, mybuffer, dpCore.dst_x1, dpCore.dst_y1);
}


