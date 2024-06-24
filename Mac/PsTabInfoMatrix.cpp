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
#include "PsWinProperties.h"
#include "PsTabInfoMatrix.h"
#include "PsAction.h"

bool OPTION_REFLECT_ALWAYS = false;

PsTabInfoMatrix::PsTabInfoMatrix()
{
	bNoProtect = true; // FIXME ?

	parent = NULL;
	imageAngle = LoadCGImageFromPng("angle");
	imageTorsio = LoadCGImageFromPng("torsio");
	imageLink = LoadCGImageFromPng("link");
}

PsTabInfoMatrix::~PsTabInfoMatrix()
{
}

void PsTabInfoMatrix::UpdateValue(OSType signature, char *buffer)
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
	
	// mise à jour
	myControlID.signature = signature;
	myControlID.id = 0;
	GetControlByID(parent->window, &myControlID, &control);
	SetControlData(control, kControlNoPart, kControlEditTextTextTag, strlen(buffer), buffer);	
}

void PsTabInfoMatrix::Update(PsMatrix* matrix)
{	
	if (!matrix)
	{
		Disable();
		return;
	}
	
	Enable();

	float x, y, a, d, f, i, j;
	matrix->GetPosition(x, y);
	char buffer[1024];
	
	sprintf(buffer, "%.2f", x);
	UpdateValue('mx  ', buffer);
	
	sprintf(buffer, "%.2f", y);
	UpdateValue('my  ', buffer);
	
	a = matrix->GetAngle() * 180.0f / 3.14159265f;
	while (a < 0.0f)
		a += 360.0f;
	while (a > 360.0f)
		a -= 360.0f;
	sprintf(buffer, "%.2f", a);
	UpdateValue('mr  ', buffer);
	
	matrix->GetTorsion(i, j);
	d = i / 3.14159265f;
	if (d < 0) d = -1 * d;
	else d = 1 * d;
	f = j;
	if (f < 0) f = -1 * f;
	else f = 1 * f;
	
	sprintf(buffer, "%.2f", d);
	UpdateValue('mth ', buffer);
	
	sprintf(buffer, "%.2f", f);
	UpdateValue('mtv ', buffer);
	
	sprintf(buffer, "%.2f", matrix->w);
	UpdateValue('ml  ', buffer);
	
	sprintf(buffer, "%.2f", matrix->h);
	UpdateValue('mh  ', buffer);
	
	sprintf(buffer, "%.2f", matrix->w / PsMatrix::default_w * 100.f);
	UpdateValue('mlp ', buffer);
	
	sprintf(buffer, "%.2f", matrix->h / PsMatrix::default_h * 100.f);
	UpdateValue('mhp ', buffer);
	
	sprintf(buffer, "%d", matrix->div_x);
	UpdateValue('mdl ', buffer);
	
	sprintf(buffer, "%d", matrix->div_y);
	UpdateValue('mdh ', buffer);
	
	UpdateLinkButton();
	
	ControlRef controlBox;
	ControlID myControlBoxID;
	myControlBoxID.id = 0;
	
	myControlBoxID.signature = 'md  ';		
	GetControlByID(parent->window, &myControlBoxID, &controlBox);
	if (matrix->div_is_active) 
	{
		SetControlValue(controlBox, true);
		OnCheckBoxClicked();
	}
	else
	{
		SetControlValue(controlBox, false);
		OnCheckBoxClicked();
	}
		 
	myControlBoxID.signature = 'mf  ';		
	GetControlByID(parent->window, &myControlBoxID, &controlBox);
	if (OPTION_REFLECT_ALWAYS) SetControlValue(controlBox, true);
	else SetControlValue(controlBox, false);

}


void PsTabInfoMatrix::UpdateLinkButton()
{
	PsProject *project = PsController::Instance().project;
	assert(project);
	PsMatrix* matrix = project->matrix;	
	assert(matrix);
	ControlRef controlBox;
	ControlID myControlBoxID;
	myControlBoxID.id = 0;
	myControlBoxID.signature = 'bul1';		
	GetControlByID(parent->window, &myControlBoxID, &controlBox);
	if (matrix->constraint) SetControlValue(controlBox, true);
	else SetControlValue(controlBox, false);
}

OSStatus PsTabInfoMatrix::EventsHandler(EventHandlerCallRef myHandler, EventRef event)
{
	OSStatus result = eventNotHandledErr;
	
	if (!PsController::Instance().project
		 || !PsController::Instance().project->matrix)
		return eventNotHandledErr;

	PsProject *project = PsController::Instance().project;
	PsMatrix* matrix = project->matrix;	

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

						if (myControlID.signature == 'mx  ')
						{
							project->LogAdd(new LogMove(*project, matrix, matrix->x, matrix->y));
							matrix->x = atof(buffer);
							PsController::Instance().UpdateWindow();
						}
						else if (myControlID.signature == 'my  ')
						{
							project->LogAdd(new LogMove(*project, matrix, matrix->x, matrix->y));
							matrix->y = atof(buffer);
							PsController::Instance().UpdateWindow();
						}
						else if (myControlID.signature == 'mr  ')
						{
							project->LogAdd(new LogRotate(*project, matrix, matrix->r));
							matrix->r = (atof(buffer) * 3.14159265f) / 180.f;
							PsController::Instance().UpdateWindow();
						}
						else if (myControlID.signature == 'mtv ')
						{
							PsController::Instance().project->matrix->i = atof(buffer);
							PsController::Instance().UpdateWindow();
						}
						else if (myControlID.signature == 'mth ')
						{
							PsController::Instance().project->matrix->j = atof(buffer);
							PsController::Instance().UpdateWindow();
						}
						else if (myControlID.signature == 'ml  ')
						{
							if (atof(buffer) >= PsMatrix::minimum_dim)
							{
								project->LogAdd (new LogResize (*project, matrix, matrix->x, matrix->y, matrix->w, matrix->h));
								if (matrix->constraint)
									matrix->h *= atof(buffer) / matrix->w;
								matrix->w = atof(buffer);
								PsController::Instance().UpdateWindow();
								bNoProtect = false;
								Update(matrix); 
								bNoProtect = true;
							}
						}
						else if (myControlID.signature == 'mh  ')
						{
							if (atof(buffer) >= PsMatrix::minimum_dim)
							{
								project->LogAdd (new LogResize (*project, matrix, matrix->x, matrix->y, matrix->w, matrix->h));
								if (matrix->constraint)
									matrix->w *= atof(buffer) / matrix->h;
								matrix->h = atof(buffer);
								PsController::Instance().UpdateWindow();
								bNoProtect = false;
								Update(matrix); 
								bNoProtect = true;
							}
						}
						else if (myControlID.signature == 'mlp ')
						{
							float w = atof(buffer) * PsMatrix::default_w / 100.f; 
							if (w >= PsMatrix::minimum_dim)
							{
								project->LogAdd (new LogResize (*project, matrix, matrix->x, matrix->y, matrix->w, matrix->h));
								if (matrix->constraint)
									matrix->h *= w / matrix->w;
								matrix->w = w;
								PsController::Instance().UpdateWindow();
								bNoProtect = false;
								Update(matrix); 
								bNoProtect = true;
							}
						}
						else if (myControlID.signature == 'mhp ')
						{
							float h = atof(buffer) * PsMatrix::default_h / 100.f; 
							if (h >= PsMatrix::minimum_dim)
							{
								project->LogAdd (new LogResize (*project, matrix, matrix->x, matrix->y, matrix->w, matrix->h));
								if (matrix->constraint)
									matrix->w *= h / matrix->h;
								matrix->h = h;
								PsController::Instance().UpdateWindow();
								bNoProtect = false;
								Update(matrix); 
								bNoProtect = true;
							}
						}
						else if (myControlID.signature == 'mdl ') 
						{
							matrix->div_x = atof(buffer);
							PsController::Instance().UpdateWindow();
						}
						else if (myControlID.signature == 'mdh ') 
						{
							matrix->div_y = atof(buffer);
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
						case 'md  ': 
						{
							if (matrix) 
							{
								matrix->div_is_active = !matrix->div_is_active;
								OnCheckBoxClicked();
							}
							break;
						}
						case 'mf  ': 
						{
							OPTION_REFLECT_ALWAYS = !OPTION_REFLECT_ALWAYS;
							PsController::Instance().SetOption(PsController::OPTION_REFLECT, OPTION_REFLECT_ALWAYS);
							break;							
						}
						case 'bul1':
							if (matrix) matrix->constraint = !matrix->constraint;
							break;
					}
					break;
				}
			}
		}			
	}
	return result;
}

void PsTabInfoMatrix::OnCheckBoxClicked()
{
	ControlRef dW, dH;
	ControlID myControlBoxID;
			
	if (PsController::Instance().project 
		 && PsController::Instance().project->matrix)
	{
		PsProject *project = PsController::Instance().project;
		PsMatrix* matrix = project->matrix;		
		PsController::Instance().UpdateWindow();
		
		myControlBoxID.id = 0;
		myControlBoxID.signature = 'mdl ';
		GetControlByID(parent->window, &myControlBoxID, &dW);
		myControlBoxID.signature = 'mdh ';
		GetControlByID(parent->window, &myControlBoxID, &dH);
		
		if (!matrix->div_is_active)
		{
			DeactivateControl(dW);
			DeactivateControl(dH);
		}
		else
		{
			ActivateControl(dW);
			ActivateControl(dH);
		}

	}
}

void PsTabInfoMatrix::InstallTextChangeEvent(OSType signature)
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

static OSStatus PsTabInfoMatrixDrawAngle(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsTabInfoMatrix *View = (PsTabInfoMatrix*)userData;
	OSStatus status = noErr;
	CGContextRef myContext;
	HIRect bounds;
	
	status = GetEventParameter (event, kEventParamCGContextRef, typeCGContextRef, NULL,
										 sizeof (CGContextRef), NULL,  &myContext);
	require_noerr(status, CantGetGraphicsContext);	
	HIViewRef myHIView;
	static const HIViewID  myHIViewID = { 'va1 ', 0 };
	HIViewFindByID(HIViewGetRoot(View->parent->window), myHIViewID, &myHIView);
	HIViewGetBounds (myHIView, &bounds);
	require_noerr(status, CantGetBoundingRectangle);
	DrawCGImage(myContext, bounds.size.height, View->imageAngle, 0, 0);
	
CantGetBoundingRectangle:
CantGetGraphicsContext:
		return status;
}

static OSStatus PsTabInfoMatrixDrawAngle2(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsTabInfoMatrix *View = (PsTabInfoMatrix*)userData;
	OSStatus status = noErr;
	CGContextRef myContext;
	HIRect bounds;
	
	status = GetEventParameter (event, kEventParamCGContextRef, typeCGContextRef, NULL,
										 sizeof (CGContextRef), NULL,  &myContext);
	require_noerr(status, CantGetGraphicsContext);	
	HIViewRef myHIView;
	static const HIViewID  myHIViewID = { 'va2 ', 0 };
	HIViewFindByID(HIViewGetRoot(View->parent->window), myHIViewID, &myHIView);
	HIViewGetBounds (myHIView, &bounds);
	require_noerr(status, CantGetBoundingRectangle);
	DrawCGImage(myContext, bounds.size.height, View->imageAngle, 0, 0);
	
CantGetBoundingRectangle:
CantGetGraphicsContext:
		return status;
}

static OSStatus PsTabInfoMatrixDrawTorsio(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsTabInfoMatrix *View = (PsTabInfoMatrix*)userData;
	OSStatus status = noErr;
	CGContextRef myContext;
	HIRect bounds;
	
	status = GetEventParameter (event, kEventParamCGContextRef, typeCGContextRef, NULL,
										 sizeof (CGContextRef), NULL,  &myContext);
	require_noerr(status, CantGetGraphicsContext);	
	HIViewRef myHIView;
	static const HIViewID  myHIViewID = { 'vt1 ', 0 };
	HIViewFindByID(HIViewGetRoot(View->parent->window), myHIViewID, &myHIView);
	HIViewGetBounds (myHIView, &bounds);
	require_noerr(status, CantGetBoundingRectangle);
	DrawCGImage(myContext, bounds.size.height, View->imageTorsio, 0, 0);
	
CantGetBoundingRectangle:
CantGetGraphicsContext:
		return status;
}

void PsTabInfoMatrix::InstallEventHandlers()
{
	InstallTextChangeEvent('mx  ');
	InstallTextChangeEvent('my  ');
	InstallTextChangeEvent('mr  ');
	InstallTextChangeEvent('mth ');
	InstallTextChangeEvent('mtv ');
	InstallTextChangeEvent('ml  ');
	InstallTextChangeEvent('mh  ');
	InstallTextChangeEvent('mlp ');
	InstallTextChangeEvent('mhp ');
	InstallTextChangeEvent('mdl ');
	InstallTextChangeEvent('mdh ');
	
	//-- handle sur l'affichage des images
	HIViewRef myHIView;
	static const HIViewID  myHIViewID = { 'va1 ', 0 };
	static const HIViewID  myHIViewID3 = { 'va2 ', 0 };
	static const HIViewID  myHIViewID2 = { 'vt1 ', 0 };
	static const EventTypeSpec  myHIViewSpec[] = { kEventClassControl, kEventControlDraw };
	HIViewFindByID(HIViewGetRoot(parent->window), myHIViewID, &myHIView);
	InstallEventHandler (GetControlEventTarget (myHIView), NewEventHandlerUPP(PsTabInfoMatrixDrawAngle),
										GetEventTypeCount (myHIViewSpec), myHIViewSpec, (void *) this, NULL);
	HIViewFindByID(HIViewGetRoot(parent->window), myHIViewID2, &myHIView);
	InstallEventHandler (GetControlEventTarget (myHIView), NewEventHandlerUPP(PsTabInfoMatrixDrawTorsio),
								GetEventTypeCount (myHIViewSpec), myHIViewSpec, (void *) this, NULL);
	HIViewFindByID(HIViewGetRoot(parent->window), myHIViewID3, &myHIView);
	InstallEventHandler (GetControlEventTarget (myHIView), NewEventHandlerUPP(PsTabInfoMatrixDrawAngle2),
								GetEventTypeCount (myHIViewSpec), myHIViewSpec, (void *) this, NULL);
	
	ControlRef control;
	ControlID myControlID;
	myControlID.id = 0;
	ControlButtonContentInfo info;
	info.contentType = kControlContentCGImageRef;
	info.u.imageRef = imageLink;
	myControlID.signature = 'bul1';
	GetControlByID(parent->window, &myControlID, &control);
	SetBevelButtonContentInfo(control, &info);
	myControlID.signature = 'bul2';
	GetControlByID(parent->window, &myControlID, &control);
	SetBevelButtonContentInfo(control, &info);
}

void PsTabInfoMatrix::Disable()
{
	ControlRef control;
	ControlID controlId;
	controlId.id = 129;
	controlId.signature = 'tabs';
	GetControlByID(parent->window, &controlId, &control);
	DeactivateControl(control);

	UpdateValue('mx  ', "");
	UpdateValue('my  ', "");
	UpdateValue('mr  ', "");
	UpdateValue('mth ', "");
	UpdateValue('mtl ', "");
	UpdateValue('ml  ', "");
	UpdateValue('mh  ', "");
	UpdateValue('mlp ', "");
	UpdateValue('mhp ', "");
	UpdateValue('mdl ', "");
	UpdateValue('mdh ', "");
}

void PsTabInfoMatrix::Enable()
{
	ControlRef control;
	ControlID controlId;
	controlId.id = 129;
	controlId.signature = 'tabs';
	GetControlByID(parent->window, &controlId, &control);
	ActivateControl(control);
}
