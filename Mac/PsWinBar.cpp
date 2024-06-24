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
#include "PsWinBar.h"
#include "PsController.h"

PsWinBar*	PsWinBar::instance = 0;

static OSStatus OnBoundsChange(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsWinBar *View = (PsWinBar*)userData;
	GetEventParameter(event, kEventParamCurrentBounds, typeQDRectangle, NULL, sizeof(Rect), NULL, &View->bounds);
	return eventNotHandledErr;
}

static OSStatus PsWinBarEvents(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsWinBar *doc = (PsWinBar*)userData;
	return doc->EventsHandler(myHandler, event);
}

PsWinBar::PsWinBar()
{
	OSStatus err = noErr;
	static const EventTypeSpec  myBoundsSpec[] = { kEventClassWindow, kEventWindowBoundsChanged };
	const EventTypeSpec kWindowEvents[] = { kEventClassCommand, kEventCommandProcess };

	//-- création de la fenetre
	err = CreateWindowFromNib(sNibRef, CFSTR("PsWinBar"), &window);
   require_noerr( err, CantCreateWindow );

	//-- handle des checkboxes
	InstallWindowEventHandler(window, NewEventHandlerUPP(PsWinBarEvents),
									  GetEventTypeCount( kWindowEvents), kWindowEvents, (void*)this, NULL );
	
	//-- handle sur le déplacement de la fenetre
	InstallWindowEventHandler(window, NewEventHandlerUPP (OnBoundsChange),
										GetEventTypeCount (myBoundsSpec), myBoundsSpec, (void *) this, NULL);

	// FIXME : appel forcé à OnBoundsChange
	Rect bound; GetWindowBounds(window, kWindowStructureRgn, &bound);
	bound.left++; SetWindowBounds(window, kWindowStructureRgn, &bound);
	//--

CantCreateWindow:
	;
}

PsWinBar&	PsWinBar::Instance()
{
	if(!instance)
		instance = new PsWinBar();
	return *instance;
}

PsWinBar::~PsWinBar()
{
}

void	PsWinBar::Delete()
{
	if(instance)
	{
		delete instance;
		instance = 0;
	}
}

void PsWinBar::Show()
{
	ShowWindow(window);
}

void PsWinBar::GetLocation(PsPoint &p)
{
	Rect bound;
	GetWindowBounds(window, kWindowStructureRgn, &bound);
	p.x = bound.left;
	p.y = bound.top;
}

void PsWinBar::SetLocation(int x, int y)
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

void PsWinBar::UpdateCheckboxStatuts()
{
	ControlRef controlBox;
	ControlID myControlBoxID;
	myControlBoxID.id = 0;
	
	myControlBoxID.signature = 'auto';		
	GetControlByID(window, &myControlBoxID, &controlBox);
	SetControlValue(controlBox, PsController::Instance().GetOption(PsController::OPTION_AUTOMATIC, true));

	myControlBoxID.signature = 'show';		
	GetControlByID(window, &myControlBoxID, &controlBox);
	SetControlValue(controlBox, PsController::Instance().GetOption(PsController::OPTION_BOX_SHOW, true));
}
	
OSStatus PsWinBar::EventsHandler(EventHandlerCallRef myHandler, EventRef event)
{
	OSStatus result = eventNotHandledErr;
	
	switch ( GetEventClass( event ) )
	{
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
						case 'auto': 
						{
							bool	value = !PsController::Instance().GetOption (PsController::OPTION_AUTOMATIC, true);
							PsController::Instance().SetOption(PsController::OPTION_AUTOMATIC, value);
							PsController::Instance().UpdateWindow();
							break;
						}
							
						case 'show': 
						{
							bool	value = !PsController::Instance().GetOption(PsController::OPTION_BOX_SHOW, true);
							PsController::Instance().SetOption (PsController::OPTION_BOX_SHOW, value);
							PsController::Instance().UpdateWindow();
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