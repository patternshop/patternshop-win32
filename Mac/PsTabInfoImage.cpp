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
#include "PsTabInfoImage.h"
#include "PsWinImage.h"
#include "PsWinProject.h"
#include "PsAction.h"

PsTabInfoImage::PsTabInfoImage()
{
	bNoProtect = true; // FIXME
	parent = NULL;
}

PsTabInfoImage::~PsTabInfoImage()
{
}

void PsTabInfoImage::Update(PsImage* image)
{
	char buffer[1024];
	PsProject *project = PsController::Instance().project;
	
	if (!project || !image)
	{
		Disable();
		return;
	}

	Enable();

	float	angle;
	angle = image->GetAngle() * 180.0f / 3.14159265f;
	while (angle < 0.0f)
		angle += 360.0f;
	while (angle > 360.0f)
		angle -= 360.0f;
	if (image->parent)
	{
		sprintf(buffer, "%.2f", image->x / (SHAPE_SIZE * 2) 
				  * project->matrix->w 
				  + project->matrix->w / 2);
		UpdateValue('ix  ', buffer);
		sprintf(buffer, "%.2f", image->y / (SHAPE_SIZE * 2)
				  * project->matrix->h 
				  + project->matrix->h / 2);
		UpdateValue('iy  ', buffer);
	}
	else
	{
		sprintf(buffer, "%.2f", image->x);
		UpdateValue('ix  ', buffer);
		sprintf(buffer, "%.2f", image->y);
		UpdateValue('iy  ', buffer);
	}
	
	sprintf(buffer, "%.2f", image->w);
	UpdateValue('il  ', buffer);
	sprintf(buffer, "%.2f", image->h);
	UpdateValue('ih  ', buffer);
	sprintf(buffer, "%.2f", angle);
	UpdateValue('ir  ', buffer);
	sprintf(buffer, "%.2f", image->w / image->GetTexture().width * 100.0f);
	UpdateValue('ilp ', buffer);
	sprintf(buffer, "%.2f", image->h / image->GetTexture().height * 100.0f);
	UpdateValue('ihp ', buffer);
	
	UpdateLinkButton();
}

void PsTabInfoImage::UpdateLinkButton()
{
	PsProject *project = PsController::Instance().project;
	assert(project);
	PsImage *image = PsController::Instance().project->image;
	assert(image);
	ControlRef controlBox;
	ControlID myControlBoxID;
	myControlBoxID.id = 0;
	myControlBoxID.signature = 'bul2';		
	GetControlByID(parent->window, &myControlBoxID, &controlBox);
	if (image->constraint) SetControlValue(controlBox, true);
	else SetControlValue(controlBox, false);
}

OSStatus PsTabInfoImage::EventsHandler(EventHandlerCallRef myHandler, EventRef event)
{
	OSStatus result = eventNotHandledErr;
	
	if (!PsController::Instance().project
		 || !PsController::Instance().project->image)
		return eventNotHandledErr;
	
	PsProject *project = PsController::Instance().project;
	PsImage *image = PsController::Instance().project->image;
	
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
						
						if (myControlID.signature == 'ix  ')
						{
							float value = atof(buffer);
							if (project->image->parent)
								if (value < 0.f) value = 0.f;
								else if (value > project->matrix->w) value = project->matrix->w;
							float x, y;
							project->image->GetPosition(x, y);
							project->LogAdd (new LogMove (*project, project->image, x, y));
							if (project->image->parent) project->image->x = (value - project->matrix->w / 2) / project->matrix->w * (SHAPE_SIZE * 2);
							else project->image->x = value;
							PsController::Instance().UpdateWindow();
						}
						else if (myControlID.signature == 'iy  ')
						{
							float value = atof(buffer);
							if (project->image->parent)
								if (value < 0.f) value = 0.f;
								else if (value > project->matrix->h) value = project->matrix->h;
							float x, y;
							project->image->GetPosition(x, y);
							project->LogAdd (new LogMove (*project, project->image, x, y));
							if (project->image->parent) project->image->y = (value - project->matrix->h / 2) / project->matrix->h * (SHAPE_SIZE * 2);
							else project->image->y = value;
							PsController::Instance().UpdateWindow();
						}
						else if (myControlID.signature == 'ir  ')
						{
							project->LogAdd (new LogRotate (*project, project->image, project->image->r));
							project->image->r = (atof(buffer) * 3.14159265f) / 180.f;
							PsController::Instance().UpdateWindow();
						}
						else if (myControlID.signature == 'il  ')
						{
							PsImage *image = project->image;
							float x, y;
							image->GetPosition(x, y);
							project->LogAdd (new LogResize (*project, image, x, y, image->w, image->h));
							image->w = atof(buffer);
							sprintf(buffer, "%.2f", image->w * 100 / image->GetTexture().width);
							UpdateValue('ilp ', buffer);
							if (image->constraint)
							{
								UpdateValue('ihp ', buffer);
								image->h = atof(buffer) * image->GetTexture().height / 100;
								sprintf(buffer, "%.2f", image->h);
								UpdateValue('ih  ', buffer);
							}
							PsController::Instance().UpdateWindow();
						}
						else if (myControlID.signature == 'ih  ')
						{
							PsImage *image = project->image;
							float x, y;
							image->GetPosition(x, y);
							project->LogAdd (new LogResize (*project, image, x, y, image->w, image->h));
							image->h = atof(buffer);
							sprintf(buffer, "%.2f", image->h * 100 / image->GetTexture().height);
							UpdateValue('ihp ', buffer);
							if (image->constraint)
							{
								UpdateValue('ilp ', buffer);
								image->w = atof(buffer) * image->GetTexture().width / 100;
								sprintf(buffer, "%.2f", image->w);
								UpdateValue('il  ', buffer);
							}
							PsController::Instance().UpdateWindow();
						}
						else if (myControlID.signature == 'ilp ')
						{
							PsImage *image = PsController::Instance().project->image;
							image->w = atof(buffer) * image->GetTexture().width / 100;
							sprintf(buffer, "%.2f", image->w);
							UpdateValue('il  ', buffer);
							if (image->constraint)
							{
								ControlRef control2;
								ControlID myControlID2;
								myControlID2.id = 0;
								myControlID2.signature = 'ilp ';
								GetControlByID(parent->window, &myControlID2, &control2);
								GetControlData(control2, kControlNoPart, kControlEditTextTextTag, 1024, buffer, &size);
								buffer[size] = 0;
								UpdateValue('ihp ', buffer);
								image->h = atof(buffer) * image->GetTexture().height / 100;
								sprintf(buffer, "%.2f", image->h);
								UpdateValue('ih  ', buffer);
							}
							PsController::Instance().UpdateWindow();
						}
						else if (myControlID.signature == 'ihp ')
						{
							PsImage *image = PsController::Instance().project->image;
							image->h = atof(buffer) * image->GetTexture().height / 100;
							sprintf(buffer, "%.2f", image->h);
							UpdateValue('ih  ', buffer);
							if (image->constraint)
							{
								ControlRef control2;
								ControlID myControlID2;
								myControlID2.id = 0;
								myControlID2.signature = 'ihp ';
								GetControlByID(parent->window, &myControlID2, &control2);
								GetControlData(control2, kControlNoPart, kControlEditTextTextTag, 1024, buffer, &size);
								buffer[size] = 0;
								UpdateValue('ilp ', buffer);
								image->w = atof(buffer) * image->GetTexture().width / 100;
								sprintf(buffer, "%.2f", image->w);
								UpdateValue('il  ', buffer);
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
						case 'isho':
						{
							PsWinImage* dlgImage = new PsWinImage;
							dlgImage->Show();
							break;
						}
						case 'iupd':
						{
							char buffer[1024]; 
							if (PsOpenFileDialog(buffer, PS_MOTIF_FILE))
							{
								PsWinProject::Instance().relaseThumb (&(project->image->GetTexture()));
								GetError(project->ReplaceImage(buffer));
									
								PsController::Instance().UpdateWindow();
								PsController::Instance().UpdateDialogProject(); 
							}
							break;
						}
						case 'bul2':
						{
							if (image)
							{
								if (!image->constraint)
								{
									image->constraint = true;
									Size size;
									char w[1024], h[1024];
									ControlRef control2;
									ControlID myControlID2;
									myControlID2.id = 0;
									myControlID2.signature = 'ilp ';
									GetControlByID(parent->window, &myControlID2, &control2);
									GetControlData(control2, kControlNoPart, kControlEditTextTextTag, 1024, w, &size);
									w[size] = 0;
									myControlID2.signature = 'ihp ';
									GetControlByID(parent->window, &myControlID2, &control2);
									GetControlData(control2, kControlNoPart, kControlEditTextTextTag, 1024, h, &size);
									h[size] = 0;
									if (atof(w) != atof(h))
									{
										if (atof(w) > atof(h))
										{
											UpdateValue('ihp ', w);
											image->h = atof(w) * image->GetTexture().height / 100;
											sprintf(w, "%.2f", image->h);
											UpdateValue('ih  ', w);
										}
										else 
										{
											UpdateValue('ilp ', h);
											image->w = atof(h) * image->GetTexture().width / 100;
											sprintf(h, "%.2f", image->w);
											UpdateValue('il  ', h);
										}
									}
									PsController::Instance().UpdateWindow();
								}
								else
								{
									image->constraint = false;
								}
							}
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

void PsTabInfoImage::UpdateValue(OSType signature, char *buffer)
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

void PsTabInfoImage::InstallTextChangeEvent(OSType signature)
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

void PsTabInfoImage::InstallEventHandlers()
{
	InstallTextChangeEvent('ix  ');
	InstallTextChangeEvent('iy  ');
	InstallTextChangeEvent('ir  ');
	InstallTextChangeEvent('il  ');
	InstallTextChangeEvent('ih  ');
	InstallTextChangeEvent('ilp ');
	InstallTextChangeEvent('ihp ');
}

void PsTabInfoImage::Disable()
{
	ControlRef control;
	ControlID controlId;
	controlId.id = 130;
	controlId.signature = 'tabs';
	GetControlByID(parent->window, &controlId, &control);
	DeactivateControl(control);
	UpdateValue('ix  ', "");
	UpdateValue('iy  ', "");
	UpdateValue('ir  ', "");	
	UpdateValue('il  ', "");
	UpdateValue('ih  ', "");
	UpdateValue('ilp ', "");
	UpdateValue('ihp ', "");
}

void PsTabInfoImage::Enable()
{
	ControlRef control;
	ControlID controlId;
	controlId.id = 130;
	controlId.signature = 'tabs';
	GetControlByID(parent->window, &controlId, &control);
	ActivateControl(control);
}
