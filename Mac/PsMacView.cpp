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
#include "PsMacCursor.h"
#include "PsController.h"
#include "PsProject.h"
#include "PsWinTools.h"
#include "PsWinOverview.h"
#include "PsWinProperties.h"
#include "PsHardware.h"
#include "PsDlgConfig.h"

extern bool OPTION_REFLECT_ALWAYS;

PsMacViewbufferingType MyBufferingType = PSMACVIEW_PBUFFER_TO_QUARTZIMAGE;

//--------------------------------------------------------------------------------------------

PsMacView* active;
std::vector<PsMacView*> aWindowList;
PsWinPropertiesCx *dlgPropreties;

//--------------------------------------------------------------------------------------------

Boolean						SetUp (void);
void						CleanUp (void);
OSStatus					AppEventHandler( EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon );
OSStatus					HandleNew();
static OSStatus				WindowEventHandler( EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon );

//--------------------------------------------------------------------------------------------

PsMacView::PsMacView()
{
	project = 0;
	window = 0;
	gaglContext = 0;
	Path = 0;
	gTimer = NULL;
	bUpdated = false;
	bTracking = false;
	cgImage = NULL;
}

//--------------------------------------------------------------------------------------------

PsMacView *CreateNewViewWindow()
{
	HandleNew();
	DisableSave();
	return active;
}

//--------------------------------------------------------------------------------------------

DEFINE_ONE_SHOT_HANDLER_GETTER( WindowEventHandler )

//--------------------------------------------------------------------------------------------

static OSStatus OnViewEvent(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsMacView *View = (PsMacView*)userData;
	OSStatus status = eventNotHandledErr;
	
	switch ( GetEventClass( event ) )
	{
		case kEventClassControl:
		{
			switch ( GetEventKind( event ) )
			{
				case kEventControlTrackingAreaEntered:
				{
					View->bTracking = true;
					SetMacCursor(View->iCurrentCursor);
					break;
				}
					
				case kEventControlTrackingAreaExited:
				{
					View->bTracking = false;
					SetThemeCursor(kThemeArrowCursor);
					break;
				}
					
				case kEventControlDraw:
				{
					CGContextRef myContext;
					HIRect bounds;
					
					status = GetEventParameter (event,
														 kEventParamCGContextRef,
														 typeCGContextRef,
														 NULL,
														 sizeof (CGContextRef),
														 NULL,
														 &myContext);
					require_noerr(status, CantGetGraphicsContext);
					
					HIViewGetBounds (View->myHIView, &bounds);
					require_noerr(status, CantGetBoundingRectangle);
					
					if (MyBufferingType == PSMACVIEW_PBUFFER_TO_QUARTZIMAGE)
					{
						if (View->cgImage)
						{
							CGContextClearRect(myContext, CGRectMake(0, 0, View->width, View->height));
							CGContextDrawImage(myContext, CGRectMake(0, 0, View->width, View->height), View->cgImage);
							aglSetDrawable(View->gaglContext, NULL);
							//printf("aglSetDrawable NULL\n\n");				
						}
					}
				
					break;
				}
			}
			break;
		}
	}
CantGetGraphicsContext:
CantGetBoundingRectangle:
	return status;
}

//--------------------------------------------------------------------------------------------

pascal void MacViewIdleTimer(EventLoopTimerRef inTimer, void* userData)
{
	PsMacView *View = (PsMacView*)userData;
	if (View && !View->bUpdated)
	{
		if (hardwareRenderer.ReserveHardwareMutex())
		{
			View->UpdateNow();
			View->bUpdated = true;
			hardwareRenderer.ReleaseHardwareMutex();
			PsWinOverview::Instance().Update();
		}
	}
}

// --------------------------------------------------------------------------

GLenum aglDebugStr (void)
{
	GLenum err = aglGetError();
	if (AGL_NO_ERROR != err)
		PsMessageBox ((char *)aglErrorString(err));
	return err;
}

OSStatus MySetWindowAsDrawableObject()
{
	OSStatus err = noErr;
	GLint attributes[] =  { AGL_RGBA,
		AGL_DOUBLEBUFFER,
		AGL_DEPTH_SIZE, 24,
		AGL_NONE };
	AGLPixelFormat myAGLPixelFormat;
	
	if (!active->gaglContext)
	{
		myAGLPixelFormat = aglChoosePixelFormat (NULL, 0, attributes);
		err = aglDebugStr ();
		if (myAGLPixelFormat) 
		{
			active->gaglContext = aglCreateContext (myAGLPixelFormat, hardwareRenderer.aglContext);
			err = aglDebugStr ();
		}
	}
		
	if (!aglSetCurrentContext (active->gaglContext))
		err = aglDebugStr ();
		
	//printf("gaglContext\n");
				
	return err;
}

// --------------------------------------------------------------------------

OSStatus 
HandleNew()
{
	OSStatus err;
	static int iWinNumber = 1;
	
	const EventTypeSpec    kWindowEvents[] =
	{
		{ kEventClassCommand, kEventCommandProcess },
		{ kEventClassMouse, kEventMouseUp },
		{ kEventClassMouse, kEventMouseDown },
		{ kEventClassMouse, kEventMouseMoved },
		{ kEventClassMouse, kEventMouseDragged },
		{ kEventClassWindow, kEventWindowBoundsChanged },
		{ kEventClassWindow, kEventWindowClose },
		{ kEventClassWindow, kEventWindowFocusAcquired },
		{ kEventClassKeyboard, kEventRawKeyDown },
		{ kEventClassKeyboard, kEventRawKeyUp },
		{ kEventClassKeyboard, kEventRawKeyModifiersChanged }
	};
	
	PsMacView *View = new PsMacView;
	
	// Create a window
	err = CreateWindowFromNib( sNibRef, CFSTR("MainWindow"), &(View->window) );
	require_noerr( err, CantCreateWindow );
	
	static const HIViewID  myHIViewID = { 'view', 42 };
	HIViewFindByID(HIViewGetRoot(View->window), myHIViewID, &(View->myHIView));
	
	active = View;
	
	char buffer[1024];
	sprintf(buffer, "Sans titre - %d", iWinNumber);
	CFStringRef tmp = CFStringCreateWithCString(NULL, buffer, kCFStringEncodingASCII);
	SetWindowTitleWithCFString(View->window, tmp);
	CFRelease(tmp);
	
	if (!View->project) 
	{
		View->project = new PsProject();
		View->project->renderer.SetDocSize(2480, 3508);
		PsController::Instance().SetActive(View->project);
	}

	//-- traking area
	HIViewNewTrackingArea (View->myHIView, NULL, NULL, &(View->trakingArea));
	
	//-- handle
	InstallWindowEventHandler( View->window, GetWindowEventHandlerUPP(),
										GetEventTypeCount( kWindowEvents ), kWindowEvents,
										NULL, NULL );
	
	// Handle sur l'affichage
	static const EventTypeSpec  myHIViewSpec[] = 
	{
		{ kEventClassControl, kEventControlTrackingAreaEntered },	
		{ kEventClassControl, kEventControlTrackingAreaExited },
		{ kEventClassControl, kEventControlDraw }
	};
	
	err = InstallEventHandler (GetControlEventTarget (View->myHIView), // 6
										  NewEventHandlerUPP (OnViewEvent),
										  GetEventTypeCount (myHIViewSpec),
										  myHIViewSpec,
										  (void *) View, NULL);
	 	
	InstallEventLoopTimer(GetCurrentEventLoop(), 0, 0.01, NewEventLoopTimerUPP(MacViewIdleTimer), View, &View->gTimer);
	
	// Position new windows in a staggered arrangement on the main screen
	RepositionWindow( View->window, NULL, kWindowCascadeOnMainScreen );
	
	aWindowList.push_back(View);
	
	// The window was created hidden, so show it
	//ShowWindow( View->window );
			
	iWinNumber++;
	
CantCreateWindow:
		return err;
}

//--------------------------------------------------------------------------------------------

void PsMacView::UpdateTitle(char *buffer)
{
	char *p = buffer + strlen(buffer) - 1;
	while (*p != '/' && p != buffer) p--;
	p++; 
	CFStringRef tmp = CFStringCreateWithCString(NULL, p, kCFStringEncodingUTF8);
	SetWindowTitleWithCFString(window, tmp);
	CFRelease(tmp);
}

//--------------------------------------------------------------------------------------------

PsMacView *GetMacViewFromHandle(WindowRef window)
{
	for (int i = 0; i < aWindowList.size(); ++i)
		if (aWindowList[i]->window == window)
			return aWindowList[i];
	return NULL;
}

//--------------------------------------------------------------------------------------------

static OSStatus
WindowEventHandler( EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon )
{
	OSStatus    err = eventNotHandledErr;
	
	switch ( GetEventClass( inEvent ) )
	{	
		/*
		case kEventClassCommand:
		{
			HICommandExtended cmd;
			verify_noerr( GetEventParameter( inEvent, kEventParamDirectObject, typeHICommand, NULL, sizeof( cmd ), NULL, &cmd ) );
			
			switch ( GetEventKind( inEvent ) )
			{
				case kEventCommandProcess:
					switch ( cmd.commandID )
					{
						default:
							break;
					}
					break;
			}
			break;
		}
		*/
			
		case kEventClassWindow:
			switch ( GetEventKind( inEvent ) )
			{
				case kEventWindowBoundsChanged:
				{
					if (active)
					{
						UInt32 attributes;
						::GetEventParameter(inEvent, kEventParamAttributes, typeUInt32, NULL,
												  sizeof(UInt32), NULL, &attributes);
						::GetEventParameter(inEvent, kEventParamCurrentBounds,
												  typeQDRectangle, NULL, sizeof(Rect), NULL, &active->bounds);		      					
						
						Rect bound;
						GetWindowBounds(active->window, kWindowContentRgn, &bound);
						
						if (resolution_max != 0)
						{
							int rwidth = bound.right - bound.left;
							int rheight = bound.bottom - bound.top;
							if (rwidth > resolution_max || rheight > resolution_max) 
							{	
								if (rwidth > resolution_max) bound.right = bound.left + resolution_max;
								if (rheight > resolution_max) bound.bottom = bound.top + resolution_max;
								SetWindowBounds(active->window, kWindowContentRgn, &bound);
								active->bounds = bound;
							}
						}
						
						if (active->project) 
						{
							int width = active->bounds.right - active->bounds.left;
							int height = active->bounds.bottom - active->bounds.top;								
							if (width != active->width || height != active->height)
							{
								active->project->renderer.SetSize(width, height);
								active->UpdateNow();
							}
						}
					}
					break;
				}
				
				case kEventWindowClose:
				{
					PsProject *project = PsController::Instance().project;
					if (project && project->bNeedSave)
					{
						if (GetQuestion(QUESTION_SAVE_CLOSING))
						{
							char buffer[1024];
							if (!active->Path && PsSaveFileDialog(buffer, PS_PROJECT_FILE))
							{
								active->UpdateTitle(buffer);
								active->Path = strdup(buffer);
							}
							project->FileSave(active->Path);
						}
					}
													 
					PsController::Instance().SetActive(NULL);
					if (dlgPropreties)
						dlgPropreties->UpdateInformation(NULL);
					HIViewDisposeTrackingArea(active->trakingArea);
					RemoveEventLoopTimer(active->gTimer);
					active->gTimer = NULL;
					if (active->gaglContext)
					{
						aglSetDrawable(active->gaglContext, NULL);
						aglSetCurrentContext (NULL);
						aglDestroyContext(active->gaglContext);
					}
					active->gaglContext = 0;
					for (std::vector<PsMacView*>::iterator i = aWindowList.begin(); i != aWindowList.end(); ++i)
					{
						if (*i == active)
						{
							aWindowList.erase(i);
							break;
						}
					}
					delete active;
					active = NULL;
					if (aWindowList.size() == 0)
						DisableSave();
					
					PsWinOverview::Instance().Update();
					
					break;
				}
					
				case kEventWindowFocusAcquired:
				{	
					WindowRef window;
					::GetEventParameter(inEvent, kEventParamDirectObject, typeWindowRef, NULL,
											  sizeof(WindowRef), NULL, &window);
					
					PsMacView *View = GetMacViewFromHandle(window);
					assert(View);
					
					if (active != View)
					{
						// mise à jour de la sélection du projet
						active = View;
						PsController::Instance().SetActive(View->project);
						
						// mise à jour de l'état du bouton enregistrer
						if (active->Path) EnableSave();
						else DisableSave();
					}

					break;
				}
					
			}
			break;
			
		case kEventClassMouse:
		{
			if (active && active->bTracking)
			{
				Point wheresMyMouse;
				GetEventParameter (inEvent, kEventParamMouseLocation, typeQDPoint, 
										 NULL, sizeof(Point), NULL, &wheresMyMouse);
				switch ( GetEventKind( inEvent ) )
				{
					case kEventMouseUp:
					{
						PsController::Instance ().MouseRelease (0, wheresMyMouse.h - active->bounds.left,  
																			 wheresMyMouse.v - active->bounds.top);
															
						//force redraw
						//printf("MyBufferingType = PSMACVIEW_PBUFFER_TO_QUARTZIMAGE\n\n");
						MyBufferingType = PSMACVIEW_PBUFFER_TO_QUARTZIMAGE;
						active->Update();
						break;
					}
						
					case kEventMouseDown:
					{

						PsController::Instance ().MouseClick (0, wheresMyMouse.h - active->bounds.left, 
																		  wheresMyMouse.v - active->bounds.top);
						//printf("MyBufferingType = PSMACVIEW_DIRECT_RENDERING\n\n");
						MyBufferingType = PSMACVIEW_DIRECT_RENDERING;
						active->Update();
						break;
					}
						
					case kEventMouseMoved:
					case kEventMouseDragged:
					{
						PsController::Instance().MouseMove(wheresMyMouse.h - active->bounds.left, 
																	wheresMyMouse.v - active->bounds.top);
						break;
					}
				}
			}
			break;
		}
			
		case kEventClassKeyboard:
		{
			switch ( GetEventKind( inEvent ) )
			{
				case kEventRawKeyDown:
				{
					UInt32 key;
					GetEventParameter (inEvent, kEventParamKeyCode, typeUInt32,
											 NULL, sizeof(UInt32), NULL, &key);
					active->OnKeyDown(key);
					break;
				}
				
				case kEventRawKeyUp:
				{
					UInt32 key;
					GetEventParameter (inEvent, kEventParamKeyCode, typeUInt32,
											 NULL, sizeof(UInt32), NULL, &key);
					active->OnKeyUp(key);
					break;
				}
					
				case kEventRawKeyModifiersChanged:
				{
					UInt32 mod;
					GetEventParameter (inEvent, kEventParamKeyModifiers, typeUInt32,
											 NULL, sizeof(UInt32), NULL, &mod);
					if (mod & 512) PsController::Instance ().SetOption(PsController::OPTION_CONSTRAIN, true);
					else PsController::Instance ().SetOption(PsController::OPTION_CONSTRAIN, false);
					if (mod & 256) PsController::Instance ().SetOption(PsController::OPTION_REFLECT, true);
					else if(!OPTION_REFLECT_ALWAYS) PsController::Instance ().SetOption(PsController::OPTION_REFLECT, false);
					break;
				}	
			}
			break;
		}
			
		default:
			break;
	}
	
	return err;
}

//--------------------------------------------------------------------------------------------

void PsMacView::UpdateNow()
{
	if (!project)
		return;

	//printf("PsMacView::UpdateNow() {\n");

	width  = (bounds.right - bounds.left);
	height  = (bounds.bottom - bounds.top);
	
	if (MyBufferingType == PSMACVIEW_DIRECT_RENDERING)
	{
		MySetWindowAsDrawableObject();
	}

	if (MyBufferingType == PSMACVIEW_PBUFFER_TO_QUARTZIMAGE)
	{
		SetPixelBufferContext();
	}
		
	project->RenderToScreen();

	if (MyBufferingType == PSMACVIEW_PBUFFER_TO_QUARTZIMAGE)
	{
		
		#ifndef __BIG_ENDIAN__
		glReadPixels(0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, tmppixelbuffer);
		for (int j = 0; j < height; ++j)
		{
			for (int i = 0; i < width; ++i)
			{
				int index = (j * width + i) * 4;
				uint8 swap = tmppixelbuffer[index];
				tmppixelbuffer[index] = tmppixelbuffer[index + 3];
				tmppixelbuffer[index + 3] = swap;
				swap = tmppixelbuffer[index + 1];
				tmppixelbuffer[index + 1] = tmppixelbuffer[index + 2];
				tmppixelbuffer[index + 2] = swap;
			}
		}
		#else
		glReadPixels(0, 0, width, height, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, tmppixelbuffer);
		#endif
		CGDataProviderDirectAccessCallbacks gProviderCallbacks = { MyProviderGetBytePointer, NULL, NULL, NULL };
		CGDataProviderRef provider = CGDataProviderCreateDirectAccess(tmppixelbuffer, width * height * 4, &gProviderCallbacks);
		CGColorSpaceRef cs = CGColorSpaceCreateDeviceRGB();
		if (cgImage) CGImageRelease(cgImage);
		cgImage = CGImageCreate(width, height, 8, 32, width * 4, cs, kCGImageAlphaNoneSkipFirst, provider, NULL, 0, kCGRenderingIntentDefault);
		HIViewSetNeedsDisplay(myHIView, true);
		CGDataProviderRelease(provider);
	}
	
	if (MyBufferingType == PSMACVIEW_DIRECT_RENDERING)
	{
		aglSwapBuffers(gaglContext);	
		aglSetDrawable(gaglContext, GetWindowPort(window));
	}
	else
	{
		HIViewSetNeedsDisplay(myHIView, true);
	}
	
	// A déplacer éventuellement
	static int lsize = -1;
	static PsProject *p = 0;
	if (project->LogUndoCount() != lsize || project != p)
	{
		OnUpdateEditUndo();
		lsize = project->LogUndoCount();
		p = project;
	}
	//--
	
	//printf("PsMacView::UpdateNow() }\n\n");

}

//--------------------------------------------------------------------------------------------

void	PsMacView::MenuLogRedo()
{
	if (project)
	{
		project->LogRedo();
		Update();
		if (dlgPropreties)
			dlgPropreties->UpdateInformation(project);
	}
}

void PsMacView::OnUpdateEditRedo()
{
	char	buffer[256];
	
	MenuRef menuBar = GetMenuRef(2);
	if (project->LogCanRedo())
	{
		EnableMenuCommand(menuBar, 'redo');
		sprintf(buffer, "Refaire %s", project->LogRedoLastName());
	}
	else
	{
		DisableMenuCommand(menuBar, 'redo');
		strcpy(buffer, "Refaire");
	}
	
	CFStringRef tmp = CFStringCreateWithCString(NULL, buffer, kCFStringEncodingUTF8);
	SetMenuItemTextWithCFString(menuBar, 2, tmp);
}

//--------------------------------------------------------------------------------------------

void	PsMacView::MenuLogUndo()
{
	if (project)
	{
		project->LogUndo();
		Update();
		if (dlgPropreties)
			dlgPropreties->UpdateInformation(project);
	}
}

void PsMacView::OnUpdateEditUndo()
{
	char	buffer[256];
	
	MenuRef menuBar = GetMenuRef(2);
	if (project->LogCanUndo())
	{
		EnableMenuCommand(menuBar, 'undo');
		sprintf (buffer, "Annuler %s", project->LogUndoLastName());
	}
	else
	{
		DisableMenuCommand(menuBar, 'undo');
		strcpy (buffer, "Annuler");
	}
	
	CFStringRef tmp = CFStringCreateWithCString(NULL, buffer, kCFStringEncodingUTF8);
	SetMenuItemTextWithCFString(menuBar, 1, tmp);
}

//--------------------------------------------------------------------------------------------

void PsMacView::MenuMatrixClone()
{
	if (project)
	{
		
		GetError(project->CloneMatrix());
		
		PsController::Instance().UpdateDialogProject();
		Update();
	}
}

//--------------------------------------------------------------------------------------------

void PsMacView::MenuEditClone()
{
	if (project)
	{
		if (project->image)
		{
			GetError(project->CloneImage());
			PsController::Instance().UpdateDialogProject();
			Update();
		}
		else if (project->matrix)
		{
			MenuMatrixClone();
		}
	}
}

//--------------------------------------------------------------------------------------------

void	PsMacView::MenuItemDel()
{
	if (project)
	{
		if (project->image)
		{
			if (GetQuestion(QUESTION_DELETE_IMAGE))
			{
				GetError(project->DelImage());
				Update();
			}
		}
		else if (project->matrix)
		{
			MenuMatrixDel();
		}
	}
}

//--------------------------------------------------------------------------------------------

void	PsMacView::MenuMatrixDel()
{
	if (project && project->matrix)
	{
		if (GetQuestion(QUESTION_DELETE_MATRIX))
		{
			GetError(project->DelMatrix());
			Update();
		}
	}
}

//--------------------------------------------------------------------------------------------

void	PsMacView::OnKeyDown(UInt32 nChar)
{
	PsProject *project = PsController::Instance().project;
	switch (nChar)
	{
		case 9: //'v': 
			if (PsController::Instance().tool == PsController::TOOL_MAGNIFY
				 || PsController::Instance().tool == PsController::TOOL_SCROLL)
			{
				PsWinTools::Instance().currentTool = 0;
				PsController::Instance().SetTool(PsController::TOOL_MODIFY);
				PsWinTools::Instance().Show();
			}
			break;
			
		case 49: // Espace
			if (PsController::Instance().tool == PsController::TOOL_MODIFY
				 || PsController::Instance().tool == PsController::TOOL_SCROLL)
			{
				PsWinTools::Instance().currentTool = 1;
				PsController::Instance().SetTool(PsController::TOOL_SCROLL);
				PsWinTools::Instance().Show();
			}
			break;
			
		case 13: //'z':
			if (PsController::Instance().tool == PsController::TOOL_MAGNIFY
				 || PsController::Instance().tool == PsController::TOOL_MODIFY)
			{
				PsWinTools::Instance().currentTool = 2;
				PsController::Instance().SetTool(PsController::TOOL_MAGNIFY);
				PsWinTools::Instance().Show();
			}
			break;
			
		case 123: // LEFT
			if (project)
			{
				if (project->image)
				{
					float x, y;
					project->image->GetPosition(x, y);
					project->image->SetPosition(x - 1, y);
				}
				else if (project->matrix)
					project->matrix->SetPosition(project->matrix->x - 1, project->matrix->y);
				Update();
				dlgPropreties->UpdateInformation(project);
			}
			break;
			
		case 124: // RIGHT
			if (project)
			{
				if (project->image)
				{
					float x, y;
					project->image->GetPosition(x, y);
					project->image->SetPosition(x + 1, y);
				}
				else if (project->matrix)
					project->matrix->SetPosition(project->matrix->x + 1, project->matrix->y);
				Update();
				dlgPropreties->UpdateInformation(project);
			}
			break;
			
		case 126: // UP
			if (project)
			{
				if (project->image)
				{
					float x, y;
					project->image->GetPosition(x, y);
					project->image->SetPosition(x, y - 1);
				}
				else if (project->matrix)
					project->matrix->SetPosition(project->matrix->x, project->matrix->y - 1);
				Update();
				dlgPropreties->UpdateInformation(project);
			}
			break;
			
		case 125: // DOWN
			if (project)
			{
				if (project->image)
				{
					float x, y;
					project->image->GetPosition(x, y);
					project->image->SetPosition(x, y + 1);
				}
				else if (project->matrix)
					project->matrix->SetPosition(project->matrix->x, project->matrix->y + 1);
				Update();
				dlgPropreties->UpdateInformation(project);
			}
			break;
	}
}

//--------------------------------------------------------------------------------------------

void	PsMacView::OnKeyUp(UInt32 nChar)
{
	switch (nChar)
	{
		case 49: //' ':
		case 13: //'z':
			switch (PsController::Instance().prev_tool)
			{
				case PsController::TOOL_MODIFY: 
				case PsController::TOOL_MODIFY_MOVE: 
				case PsController::TOOL_MODIFY_SIZE:
				case PsController::TOOL_MODIFY_TORSIO:
				{
					PsWinTools::Instance().currentTool = 0;
					PsController::Instance().SetTool(PsController::TOOL_MODIFY);
					PsWinTools::Instance().Show();
					break;
				}
				case PsController::TOOL_SCROLL : 
				case PsController::TOOL_SCROLL_DRAG : 
				{
					PsWinTools::Instance().currentTool = 1;
					PsController::Instance().SetTool(PsController::TOOL_SCROLL);
					PsWinTools::Instance().Show();
					break;
				}
				case PsController::TOOL_MAGNIFY : 
				case PsController::TOOL_MAGNIFY_ZOOM :  
				{
					PsWinTools::Instance().currentTool = 2;
					PsController::Instance().SetTool(PsController::TOOL_MAGNIFY);
					PsWinTools::Instance().Show();
				}
			}
			break;
	}
}

//--------------------------------------------------------------------------------------------

void PsMacView::SetPixelBufferContext()
{
	oldContext = aglGetCurrentContext();
	if (oldContext != hardwareRenderer.aglContext)
	{
		//printf("hardwareRenderer\n");
		aglSetCurrentContext(hardwareRenderer.aglContext);
	}
}
	
void PsMacView::RestorePreviousContext()
{
	if (oldContext && oldContext != hardwareRenderer.aglContext)
	{
		//printf("restore context\n");
		aglSetCurrentContext(oldContext);
	}
}
