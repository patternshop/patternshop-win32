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
#include "PsDlgDocument.h"

static OSStatus PsDlgDocumentEvents(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsDlgDocument *doc = (PsDlgDocument*)userData;
	return doc->EventsHandler(myHandler, event);
}

PsDlgDocument::PsDlgDocument()
{
	OSStatus err = noErr;
	
	// events de la fenetre
	const EventTypeSpec kWindowEvents[] = 
	{
		{ kEventClassCommand, kEventCommandProcess }
	};
	
	// events des textboxes
	const EventTypeSpec kControlEvents[] =
	{
		//{ kEventClassTextInput, kEventTextInputUnicodeForKeyEvent }
		{ kEventClassTextField, kEventTextDidChange }
	};
	
	// creation de la fenetre
	err = CreateWindowFromNib(sNibRef, CFSTR("PsDlgDocument"), &window);
	require_noerr( err, CantCreateWindow );
	InstallWindowEventHandler(window, NewEventHandlerUPP(PsDlgDocumentEvents),
									  GetEventTypeCount( kWindowEvents), kWindowEvents, (void*)this, NULL );
	
	// evenements des textboxes
	ControlRef control;
	ControlID myControlID;
	
	myControlID.signature = 'ltex';
	myControlID.id = 0;
	GetControlByID (window, &myControlID, &control);
	InstallControlEventHandler(control, NewEventHandlerUPP(PsDlgDocumentEvents),
										GetEventTypeCount( kControlEvents), kControlEvents, (void*)this, NULL );
	
	myControlID.signature = 'htex';
	myControlID.id = 0;
	GetControlByID (window, &myControlID, &control);
	InstallControlEventHandler(control, NewEventHandlerUPP(PsDlgDocumentEvents),
										GetEventTypeCount( kControlEvents), kControlEvents, (void*)this, NULL );	

	myControlID.signature = 'dtex';
	myControlID.id = 0;
	GetControlByID (window, &myControlID, &control);
	InstallControlEventHandler(control, NewEventHandlerUPP(PsDlgDocumentEvents),
										GetEventTypeCount( kControlEvents), kControlEvents, (void*)this, NULL );	
	
	// valeurs par défaut
	w = 2480;
	h = 3508;
	dpi = 300;
	
	wmode = 3;
	hmode = 3;
	dpimode = 1;
	
CantCreateWindow:
	;
}

bool PsDlgDocument::DoModal()
{
	bResult = false;
	ShowWindow(window);
	Update();
	RunAppModalLoopForWindow(window);
	return bResult;
}

void PsDlgDocument::Close()
{
	HideWindow(window);
	DisposeWindow(window);
	QuitAppModalLoopForWindow(window);
}

OSStatus PsDlgDocument::EventsHandler(EventHandlerCallRef myHandler, EventRef event)
{
	OSStatus result = eventNotHandledErr;
	bool bDoUpdate = false;
	
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
					GetKeyboardFocus (window, &control);
					if (control)
					{
						GetControlID(control, &myControlID);
					
						char buffer[1024]; 
						Size size;
						GetControlData(control, kControlNoPart, kControlEditTextTextTag, 1024, buffer, &size);
						buffer[size] = 0;

						if (myControlID.signature == 'ltex')
							w = GetValue(buffer, wmode);
					
						if (myControlID.signature == 'htex')
							h = GetValue(buffer, hmode);
						
						if (myControlID.signature == 'dtex')
						{
							if (dpimode == 1) dpi = atof(buffer);
							else dpi = atof(buffer) * 2.54f;
						}
						
						SetNoPreset();
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
						case 'ok  ': OnOkButton(); break;
						case 'not!': OnCancelButton(); break;
							
						// types prédéfinis
						case 'set1': w = 1748; h = 2480; break;	
						case 'set2': w = 2480; h = 3508; break;	
						case 'set3': w = 3508; h = 4961; break;	
						case 'set4': w = 4961; h = 7016; break;	
						case 'set5': w = 7016; h = 9921; break;
						
						// unitée pour la largeur
						case 'wmd1': wmode = 1; break;
						case 'wmd2': wmode = 2; break;					
						case 'wmd3': wmode = 3; break;						
						case 'wmd4': wmode = 4; break;				
						case 'wmd5': wmode = 5; break;		
						case 'wmd6': wmode = 6; break;
						
						// unitée pour la hauteur
						case 'hmd1': hmode = 1; break;
						case 'hmd2': hmode = 2; break;					
						case 'hmd3': hmode = 3; break;						
						case 'hmd4': hmode = 4; break;				
						case 'hmd5': hmode = 5; break;		
						case 'hmd6': hmode = 6; break;
							
						// unité de résolution
						case 'dst1' : dpimode = 1; break;
						case 'dst2' : dpimode = 2; break;

					}
					bDoUpdate = true;
					break;
				}
			}
		}
	}
	if (bDoUpdate)
		Update();
	return result;
}

void PsDlgDocument::OnOkButton()
{
	bResult = true;
	Close();
}

void PsDlgDocument::OnCancelButton()
{
	Close();
}

void PsDlgDocument::Update()
{
	ControlRef control;
	ControlID myControlID;

	// largeur
	myControlID.signature = 'ltex';
	myControlID.id = 0;
	GetControlByID (window, &myControlID, &control);
	UpdateValue(w, control, wmode);

	// hauteur
	myControlID.signature = 'htex';
	myControlID.id = 0;
	GetControlByID (window, &myControlID, &control);
	UpdateValue(h, control, hmode);

	// résolution
	myControlID.signature = 'dtex';
	myControlID.id = 0;
	GetControlByID (window, &myControlID, &control);
	char buffer[1024];
	switch (dpimode)
	{
		case 1: sprintf(buffer, "%d", dpi); break;
		case 2: 
			float v = dpi / 2.54f;
			sprintf(buffer, "%.2f", v); 
			break;
	}
	SetControlData(control, kControlNoPart, kControlEditTextTextTag, strlen(buffer), buffer);
}

void PsDlgDocument::UpdateValue(int px, ControlRef control, int iMode)
{
	float v;
	iMode--;
	char buffer[1024];
	float dpi = this->dpi;
	switch (iMode)
	{
		case 0:
			sprintf(buffer, "%d", px);
			break;
		case 1:
			v = px / dpi;
			sprintf(buffer, "%.3f", v);
			break;
		case 2: 
			v = px / (dpi / 2.54f);
			sprintf(buffer, "%.1f", v);
			break;
		case 3: 
			v = px * 10.f / (dpi / 2.54f);
			sprintf(buffer, "%.3f", v);
			break;  
		case 4:
			v = px / dpi / 0.0138888f;
			sprintf(buffer, "%.1f", v); 
			break;
		case 5:
			v = px / dpi * 6.f;
			sprintf(buffer, "%.1f", v);
			break;
		default: break;
	}
	for (int i = 0; i < strlen(buffer); ++i)
		if (buffer[i] == '.')
			buffer[i] = ',';
	SetControlData(control, kControlNoPart, kControlEditTextTextTag, strlen(buffer), buffer);
}

int PsDlgDocument::GetValue(char *string, int iMode)
{
	iMode--;
	char *buffer = strdup(string);
	for (int i = 0; i < strlen(buffer); ++i)
		if (buffer[i] == ',')
			buffer[i] = '.';
	float v = atof(buffer);
	delete buffer;
	float dpi = this->dpi;
	switch (iMode)
	{
		case 0: return v; break;
		case 1: return v * dpi; break;
		case 2: return v * (dpi / 2.54f); break;
		case 3: return v / 10.f * (dpi / 2.54f); break;
		case 4: return v * dpi * 0.0138888f; break;
		case 5: return v * dpi / 6.f; break;
		default: break;
	}
	return -1;
}

void PsDlgDocument::SetNoPreset()
{
	ControlID myControlID;
	myControlID.signature = 'pres';
	myControlID.id = 0;
	ControlRef combobox;
	GetControlByID(window, &myControlID, &combobox);
	SetControl32BitValue(combobox, 1);
}


