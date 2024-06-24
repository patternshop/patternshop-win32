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
#include "PsDlgExport.h"

static OSStatus PsDlgExportEvents(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsDlgExport *doc = (PsDlgExport*)userData;
	return doc->EventsHandler(myHandler, event);
}

static OSStatus OnDrawExport(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsDlgExport *View = (PsDlgExport*)userData;
	OSStatus status = noErr;
	CGContextRef myContext;	
	status = GetEventParameter (event, kEventParamCGContextRef, typeCGContextRef, NULL, 
										 sizeof (CGContextRef), NULL,  &myContext);
	require_noerr(status, CantGetGraphicsContext);
	View->DrawExportEvent(myContext);
CantGetGraphicsContext:
	return status;
}

static OSStatus OnDrawPreview(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsDlgExport *View = (PsDlgExport*)userData;
	OSStatus status = noErr;
	CGContextRef myContext;
	status = GetEventParameter (event, kEventParamCGContextRef, typeCGContextRef, NULL, 
										 sizeof (CGContextRef), NULL,  &myContext);
	require_noerr(status, CantGetGraphicsContext);
	View->DrawPreviewEvent(myContext);
CantGetGraphicsContext:
		return status;
}

PsDlgExport::PsDlgExport()
{
	OSStatus err = noErr; 
	
	// events de la fenetre
	const EventTypeSpec kWindowEvents[] = { kEventClassCommand, kEventCommandProcess };
	const EventTypeSpec kControlEvents[] = { kEventClassTextField, kEventTextDidChange };
	
	// creation de la fenetre
	err = CreateWindowFromNib(sNibRef, CFSTR("PsDlgExport"), &window);
	require_noerr( err, CantCreateWindow );
	InstallWindowEventHandler(window, NewEventHandlerUPP(PsDlgExportEvents),
									  GetEventTypeCount(kWindowEvents), kWindowEvents, (void*)this, NULL );
	
	//-- handle sur les vues
	static const EventTypeSpec myHIViewSpec[] = { kEventClassControl, kEventControlDraw };
	static const HIViewID exportImageViewID = { 'vie1', 0 };
	static const HIViewID previewImageViewID = { 'vie2', 0 };
	HIRect bounds;
	
	//-- handle sur l'affichage de l'image cible
	HIViewFindByID (HIViewGetRoot(window), exportImageViewID, &exportImageView);
	err = InstallEventHandler (GetControlEventTarget(exportImageView), NewEventHandlerUPP (OnDrawExport),
										GetEventTypeCount(myHIViewSpec), myHIViewSpec, (void *) this, NULL);

	HIViewGetBounds(exportImageView, &bounds);
	exportZone.x = bounds.origin.x;
	exportZone.y = bounds.origin.y;
	exportZone.width = bounds.size.width;
	exportZone.height = bounds.size.height;

	//-- handle sur l'affichage de prévisualisation en grille
	HIViewFindByID(HIViewGetRoot(window), previewImageViewID, &previewImageView);
	InstallEventHandler(GetControlEventTarget(previewImageView), NewEventHandlerUPP(OnDrawPreview),
								GetEventTypeCount(myHIViewSpec), myHIViewSpec, (void *) this, NULL);

	HIViewGetBounds(previewImageView, &bounds);
	previewZone.x = bounds.origin.x;
	previewZone.y = bounds.origin.y;
	previewZone.width = bounds.size.width;
	previewZone.height = bounds.size.height;
	
	// préparation des images
	assert(Initialize());

	// valeurs par défaut
	bNoProtect = false;
	bShowGrid = true;
	wmode = 2;
	hmode = 2;
	Update();

	// evenements des textboxes
	ControlRef control;
	ControlID myControlID;
	myControlID.id = 0;
	myControlID.signature = 'ltex';
	GetControlByID (window, &myControlID, &control);
	InstallControlEventHandler(control, NewEventHandlerUPP(PsDlgExportEvents),
										GetEventTypeCount(kControlEvents), kControlEvents, (void*)this, NULL);	
	myControlID.signature = 'htex';
	GetControlByID (window, &myControlID, &control);
	InstallControlEventHandler(control, NewEventHandlerUPP(PsDlgExportEvents),
										GetEventTypeCount(kControlEvents), kControlEvents, (void*)this, NULL);	
	myControlID.signature = 'dtex';
	GetControlByID (window, &myControlID, &control);
	InstallControlEventHandler(control, NewEventHandlerUPP(PsDlgExportEvents),
										GetEventTypeCount(kControlEvents), kControlEvents, (void*)this, NULL);
	myControlID.signature = 'ztex';
	GetControlByID (window, &myControlID, &control);
	InstallControlEventHandler(control, NewEventHandlerUPP(PsDlgExportEvents),
										GetEventTypeCount(kControlEvents), kControlEvents, (void*)this, NULL);

CantCreateWindow:
	;
}

/*
** dessin du rendu de la matrice
*/
void PsDlgExport::DrawExportEvent(CGContextRef myContext)
{
	// image
	HIRect bounds;
	HIViewGetBounds(exportImageView, &bounds);
	int x = (exportZone.width - exportImage.GetWidth()) / 2;
	int y = (exportZone.height - exportImage.GetHeight()) / 2;
	DrawCGImage(myContext, bounds.size.height, exportImage.buffer, x, y);	
	
	// cadre
	HIRect border;
	border.origin.x = x;
	border.origin.y = y;
	border.size.width = exportImage.GetWidth();
	border.size.height = exportImage.GetHeight();
	CGContextSetShouldAntialias(myContext, false);
	CGContextStrokeRect(myContext, border);
}

/*
** dessin de prévisualisation de la matrice en grille
*/
void PsDlgExport::DrawPreviewEvent(CGContextRef myContext)
{
	// image
	HIRect bounds;
	HIViewGetBounds(previewImageView, &bounds);
	int x = (previewZone.width - previewImage.GetWidth()) / 2;
	int y = (previewZone.height - previewImage.GetHeight()) / 2;
	DrawCGImage(myContext, bounds.size.height, previewImage.buffer, x, y);
	
	// cadre
	HIRect border;
	border.origin.x = x;
	border.origin.y = y;
	border.size.width = previewImage.GetWidth();
	border.size.height = previewImage.GetHeight();
	CGContextSetShouldAntialias(myContext, false);
	CGContextStrokeRect(myContext, border);
	
	// cadriage
	if (bShowGrid)
	{
		int delta_h = previewImage.GetHeight() / 3;
		int delta_w = previewImage.GetWidth() / 3;
		
		border.origin.x = x;
		border.origin.y = y + delta_h;
		border.size.width = previewImage.GetWidth();
		border.size.height = delta_h;
		CGContextStrokeRect(myContext, border);
		
		border.origin.x = x + delta_w;
		border.origin.y = y;
		border.size.width = delta_w;
		border.size.height = previewImage.GetHeight();
		CGContextStrokeRect(myContext, border);
	} 

}

OSStatus PsDlgExport::EventsHandler(EventHandlerCallRef myHandler, EventRef event)
{
	OSStatus result = eventNotHandledErr;
	
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
					GetKeyboardFocus(window, &control);
					if (control)
					{
						Size size;
						char buffer[1024]; 
						GetControlID(control, &myControlID);
						GetControlData(control, kControlNoPart, kControlEditTextTextTag, 1024, buffer, &size);
						buffer[size] = 0;
						
						if (myControlID.signature == 'ltex') 
						{
							double w_ = GetDoubleValue(wmode, buffer);
							if (w_ > 0)
								SetZ(w_ * 100.f / w);
						}
						
						if (myControlID.signature == 'htex') 
						{
							double h_ = GetDoubleValue(hmode, buffer);
							if (h_ > 0)
								SetZ(h_ * 100.f / h);
						}
						
						if (myControlID.signature == 'dtex') 
						{
							double v = atof(buffer);
							if (v >= 10) 
							{
								dpi = v;
								Update();
							}
						}
						
						if (myControlID.signature == 'ztex')
						{
							double z_ = atol(buffer);
							if (z_ >= 10)
								SetZ(z_);
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
						case 'ok  ': OnOkButton(); break;
						case 'not!': OnCancelButton(); break;
						case 'wmd2': wmode = 0; break;					
						case 'wmd3': wmode = 2; break;						
						case 'wmd4': wmode = 1; break;				
						case 'wmd5': wmode = 3; break;		
						case 'wmd6': wmode = 4; break;
						case 'wmd1': wmode = 5; break;
						case 'hmd2': hmode = 0; break;					
						case 'hmd3': hmode = 2; break;						
						case 'hmd4': hmode = 1; break;				
						case 'hmd5': hmode = 3; break;		
						case 'hmd6': hmode = 4; break;
						case 'hmd1': hmode = 5; break;							
						case 'grid': 
							bShowGrid = !bShowGrid;
							HIViewSetNeedsDisplay(previewImageView, true);
							break;
					}
					bNoProtect = true;
					Update();
					bNoProtect = false;
					break;
				}
			}
		}			
	}
	return result;
}

bool PsDlgExport::DoModal()
{
	bResult = false;
	ShowWindow(window);
	RunAppModalLoopForWindow(window);
	return bResult;
}

void PsDlgExport::OnOkButton()
{
	char buffer[1024];
	if (PsSaveFileDialog(buffer, PS_IMAGE_FILE))
		OnValidation(buffer);
	Close();
}

void PsDlgExport::OnCancelButton()
{
	Close();
}

void PsDlgExport::Close()
{
	HideWindow(window);
	DisposeWindow(window);
	QuitAppModalLoopForWindow(window);
}

void PsDlgExport::UpdateValue(OSType signature, char *buffer)
{
	ControlRef control;
	ControlID myControlID;
	
	// protection
	if (!bNoProtect)
	{
		GetKeyboardFocus(window, &control);
		GetControlID(control, &myControlID);
		if (myControlID.signature == signature) 
			return;
	}

	// mise à jour
	myControlID.signature = signature;
	myControlID.id = 0;
	GetControlByID(window, &myControlID, &control);
	SetControlData(control, kControlNoPart, kControlEditTextTextTag, strlen(buffer), buffer);	
}

void PsDlgExport::Update()
{
	char buffer[1024];
	GetTextValue(w * z / 100.f, wmode, buffer);
	UpdateValue('ltex', buffer);
	GetTextValue(h * z / 100.f, hmode, buffer);
	UpdateValue('htex', buffer);
	sprintf(buffer, "%.0f%%", z);
	UpdateValue('ztex', buffer);
	sprintf(buffer, "%.0f", dpi);
	UpdateValue('dtex', buffer);
}


