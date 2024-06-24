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
#include "PsDlgConfig.h"
#include "PsMatrix.h"
#include "PsMessage.h"
#include <unistd.h>

bool do_not_save_options = false;
int resolution_max = 0;

static OSStatus PsDlgConfigEvents(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsDlgConfig *doc = (PsDlgConfig*)userData;
	return doc->EventsHandler(myHandler, event);
}

PsDlgConfig::PsDlgConfig()
{
	OSStatus err = noErr;
	
	// events de la fenetre
	const EventTypeSpec kWindowEvents[] = 
	{
		{ kEventClassCommand, kEventCommandProcess }
	};

	// creation de la fenetre
	err = CreateWindowFromNib(sNibRef, CFSTR("PsDlgConfig"), &window);
	require_noerr( err, CantCreateWindow );
	InstallWindowEventHandler(window, NewEventHandlerUPP(PsDlgConfigEvents),
									  GetEventTypeCount(kWindowEvents), kWindowEvents, (void*)this, NULL );
	
	// evenements des textboxes
	ControlRef control;
	ControlID myControlID;
	char buffer[1024];
	
	myControlID.signature = 'ltex';
	myControlID.id = 0;
	GetControlByID (window, &myControlID, &control);
	sprintf(buffer, "%d", PsMatrix::default_w);
	SetControlData(control, kControlNoPart, kControlEditTextTextTag, strlen(buffer), buffer);

	myControlID.signature = 'htex';
	myControlID.id = 0;
	GetControlByID (window, &myControlID, &control);
	sprintf(buffer, "%d", PsMatrix::default_h);
	SetControlData(control, kControlNoPart, kControlEditTextTextTag, strlen(buffer), buffer);

	myControlID.signature = 'mtex';
	myControlID.id = 0;
	GetControlByID (window, &myControlID, &control);
	sprintf(buffer, "%d", PsMatrix::minimum_dim);
	SetControlData(control, kControlNoPart, kControlEditTextTextTag, strlen(buffer), buffer);
	
CantCreateWindow:
	;
}

OSStatus PsDlgConfig::EventsHandler(EventHandlerCallRef myHandler, EventRef event)
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
						case 'ok  ': OnOkButton(); break;
						case 'not!': OnCancelButton(); break;
						case 'defa': OnDefaultButton(); break; 
					}
					break;
				}
			}
		}			
	}
	return result;
}

bool PsDlgConfig::DoModal()
{
	bResult = false;
	ShowWindow(window);
	RunAppModalLoopForWindow(window);
	return bResult;
}

void PsDlgConfig::OnOkButton()
{
	ControlRef control;
	ControlID myControlID;
	char buffer[1024]; 
	Size size;

	bResult = true;
	
	myControlID.signature = 'ltex';
	myControlID.id = 0;
	GetControlByID (window, &myControlID, &control);
	GetControlData(control, kControlNoPart, kControlEditTextTextTag, 1024, buffer, &size);
	buffer[size] = 0;
	int w = atoi(buffer);
	
	myControlID.signature = 'htex';
	myControlID.id = 0;
	GetControlByID (window, &myControlID, &control);
	GetControlData(control, kControlNoPart, kControlEditTextTextTag, 1024, buffer, &size);
	buffer[size] = 0;
	int h = atoi(buffer);
	
	myControlID.signature = 'mtex';
	myControlID.id = 0;
	GetControlByID (window, &myControlID, &control);
	GetControlData(control, kControlNoPart, kControlEditTextTextTag, 1024, buffer, &size);
	buffer[size] = 0;
	int min = atoi(buffer);
	if (min > 1) PsMatrix::minimum_dim = min;
	
	if (w >= PsMatrix::minimum_dim) PsMatrix::default_w = w;
	if (h >= PsMatrix::minimum_dim) PsMatrix::default_h = h;
	
	myControlID.signature = 'slid';
	myControlID.id = 0;
	GetControlByID (window, &myControlID, &control);
	switch (GetControl32BitValue(control))
	{
		case 2: resolution_max = 1024; break;
		case 1: resolution_max = 800; break;
		case 0: resolution_max = 640; break;
		default: resolution_max = 0; break;
	}
	
	Close();
}

void PsDlgConfig::OnCancelButton()
{
	Close();
}

void PsDlgConfig::OnDefaultButton()
{
	if (GetQuestion(QUESTION_RESET_CONFIG_FILE))
	{
		DeleteConfig();
		do_not_save_options = true;
		// FIXME : restart ?
	}
	Close();
}

void PsDlgConfig::Close()
{
	HideWindow(window);
	DisposeWindow(window);
	QuitAppModalLoopForWindow(window);
}
