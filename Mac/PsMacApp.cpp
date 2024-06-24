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

#include "PsHardware.h"
#include "PsMacView.h"

#include "PsWinSplash.h"
#include "PsWinTools.h"
#include "PsWinBar.h"
#include "PsWinProject.h"
#include "PsWinProperties.h"
#include "PsWinOverview.h"

#include "PsDlgDocument.h"
#include "PsDlgConfig.h"
#include "PsDlgExport.h"
#include "PsDlgPrint.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define DIRECTORY_CONFIG "/Library/Application Support/Patternshop/"
#define FILE_CONFIG "/Library/Application Support/Patternshop/general.conf"

IBNibRef	sNibRef;
WindowRef	toolbar;

//-----------------------------------------------------------------------------------

void UpdateMenuItem_OPTION_DOCUMENT_SHOW()
{
	bool value = PsController::Instance().GetOption(PsController::OPTION_DOCUMENT_SHOW, true);
	MenuRef subMenu;
	MenuRef menuBar = GetMenuRef(4);
	GetMenuItemHierarchicalMenu(menuBar, 1, &subMenu);
	CheckMenuItem(subMenu, 1, value);
}

//-----------------------------------------------------------------------------------

void UpdateMenuItem_OPTION_DOCUMENT_BLEND()
{
	bool value = PsController::Instance().GetOption(PsController::OPTION_DOCUMENT_BLEND, true);
	MenuRef subMenu;
	MenuRef menuBar = GetMenuRef(4);
	GetMenuItemHierarchicalMenu(menuBar, 1, &subMenu);
	CheckMenuItem(subMenu, 2, value);
}

//-----------------------------------------------------------------------------------

void UpdateMenuItem_OPTION_HIGHLIGHT_SHOW()
{
	bool value = PsController::Instance().GetOption(PsController::OPTION_HIGHLIGHT_SHOW, true);
	MenuRef subMenu;
	MenuRef menuBar = GetMenuRef(4);
	GetMenuItemHierarchicalMenu(menuBar, 2, &subMenu);
	CheckMenuItem(subMenu, 1, value);
}

//-----------------------------------------------------------------------------------

void OnInitApplication()
{	
	FILE *file = fopen(FILE_CONFIG, "r");
	if (file)
	{
		PsPoint p;
		PsController& c = PsController::Instance();
		
		fread(&p, sizeof(PsPoint), 1, file);
		PsWinTools::Instance().SetLocation(p.x, p.y);
		
		fread(&p, sizeof(PsPoint), 1, file);
		PsWinBar::Instance().SetLocation(p.x, p.y);
		
		fread(&p, sizeof(PsPoint), 1, file);
		PsWinProject::Instance().SetLocation(p.x, p.y);
	
		fread(&p, sizeof(PsPoint), 1, file);
		PsWinProperties::Instance().SetLocation(p.x, p.y);
		
		fread(&p, sizeof(PsPoint), 1, file);
		PsWinOverview::Instance().SetLocation(p.x, p.y);
		
		fread(&resolution_max, sizeof(int), 1, file);
		fread(&c.option[PsController::OPTION_BOX_SHOW], sizeof(int), 1, file);
		fread(&c.option[PsController::OPTION_DOCUMENT_BLEND], sizeof(int), 1, file);
		fread(&c.option[PsController::OPTION_DOCUMENT_SHOW], sizeof(int), 1, file);
		fread(&c.option[PsController::OPTION_HIGHLIGHT_SHOW], sizeof(int), 1, file);

		PsWinBar::Instance().UpdateCheckboxStatuts();

		UpdateMenuItem_OPTION_DOCUMENT_SHOW();
		UpdateMenuItem_OPTION_DOCUMENT_BLEND();
		UpdateMenuItem_OPTION_HIGHLIGHT_SHOW();
		
		fread(&PsMatrix::default_w, sizeof(PsMatrix::default_w), 1, file);
		fread(&PsMatrix::default_h, sizeof(PsMatrix::default_h), 1, file);
		fread(&PsMatrix::minimum_dim, sizeof(PsMatrix::minimum_dim), 1, file);
		
		fclose(file);
	}
}

//-----------------------------------------------------------------------------------

void OnExitApplication()
{
	if (!do_not_save_options)
	{
		mkdir(DIRECTORY_CONFIG, S_IRWXU);
		FILE *file = fopen(FILE_CONFIG, "w");
		if (file)
		{
			PsPoint p;
			PsController& c = PsController::Instance();
			
			PsWinTools::Instance().GetLocation(p);
			fwrite(&p, sizeof(PsPoint), 1, file);
			
			PsWinBar::Instance().GetLocation(p);
			fwrite(&p, sizeof(PsPoint), 1, file);
			
			PsWinProject::Instance().GetLocation(p);
			fwrite(&p, sizeof(PsPoint), 1, file);
			
			PsWinProperties::Instance().GetLocation(p);
			fwrite(&p, sizeof(PsPoint), 1, file);
			
			PsWinOverview::Instance().GetLocation(p);
			fwrite(&p, sizeof(PsPoint), 1, file);
			
			fwrite(&resolution_max, sizeof(int), 1, file);
			fwrite(&c.option[PsController::OPTION_BOX_SHOW], sizeof(int), 1, file);
			fwrite(&c.option[PsController::OPTION_DOCUMENT_BLEND], sizeof(int), 1, file);
			fwrite(&c.option[PsController::OPTION_DOCUMENT_SHOW], sizeof(int), 1, file);
			fwrite(&c.option[PsController::OPTION_HIGHLIGHT_SHOW], sizeof(int), 1, file);
			fwrite(&PsMatrix::default_w, sizeof(PsMatrix::default_w), 1, file);
			fwrite(&PsMatrix::default_h, sizeof(PsMatrix::default_h), 1, file);
			fwrite(&PsMatrix::minimum_dim, sizeof(PsMatrix::minimum_dim), 1, file);
			
			fclose(file);
		}
	}
}

//-----------------------------------------------------------------------------------

void DeleteConfig()
{
	unlink(FILE_CONFIG);
	rmdir(DIRECTORY_CONFIG);
}

//-----------------------------------------------------------------------------------

OSStatus
AppEventHandler( EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon )
{
	OSStatus result = eventNotHandledErr;
	char buffer[1024];
	
	PsProject *project = NULL;
	
	if (active)
		project = active->project;
	
	switch ( GetEventClass( inEvent ) )
	{
		case kEventClassCommand:
		{
			HICommandExtended cmd;
			verify_noerr( GetEventParameter( inEvent, kEventParamDirectObject, typeHICommand, NULL, sizeof( cmd ), NULL, &cmd ) );
			
			switch ( GetEventKind( inEvent ) )
			{
				case kEventCommandProcess:
				{
				
					switch ( cmd.commandID )
					{
						case kHICommandNew:
						{    
							PsDlgDocument MyDlgDocument;
							if (MyDlgDocument.DoModal())
							{
								CreateNewViewWindow();
								project = active->project;
								project->renderer.SetDocSize (MyDlgDocument.w, MyDlgDocument.h);
								project->renderer.dpi = MyDlgDocument.dpi;
								ShowWindow(active->window);
								active->UpdateNow();								
							}
							break;
						}
							
						case kHICommandQuit:
						{
							OnExitApplication();
							break;
						}
							
						case kHICommandOpen:
						{
							if (PsOpenFileDialog(buffer, PS_PROJECT_FILE))
							{
								aglSetCurrentContext(hardwareRenderer.aglContext);
								if (OpenFile(buffer))
									active->UpdateNow();
							}
							break;
						}
							
						case 'svas':
						{
							if (project)
							{
								if (PsSaveFileDialog(buffer, PS_PROJECT_FILE))
								{
									project->FileSave(buffer);
									active->UpdateTitle(buffer);
									if (active->Path)
										delete active->Path;
									active->Path = strdup(buffer);
									EnableSave();
								}
							}
							break;
						}
							
						case 'save':
						{
							assert(active->Path);
							project->FileSave(active->Path);
							break;
						}
							
						case 'impi':
						{
							if (project)
							{
								if (PsOpenFileDialog(buffer, PS_MOTIF_FILE))
									project->NewMotif(buffer);
								active->Update();
							}
							break;
						}
							
						case 'impp':
						{
							if (project)
							{
								if (PsOpenFileDialog(buffer, PS_PATRON_FILE))
									project->NewPattern(buffer);
								active->Update();
								if (dlgPropreties)
									dlgPropreties->UpdateInformation(project);
							}
							break;
						}
							
						case 'imim':
						{
							if (project)
							{
								if (PsOpenFileDialog(buffer, PS_MOTIF_FILE))
									project->NewImage(buffer);
								active->Update();
							}
							break;
						}
							
						case 'pref':
						{
							PsDlgConfig dlg;
							dlg.DoModal();
							break;
						}
						
						case 'expo':
						{
							//-- vérification qu'une matrice est bien sélectionnée
							if (!project || !project->matrix)
							{
								GetError(ERROR_MATRIX_SELECT);
								break;
							}
							//--
							
							//-- vérification qu'il n'y a pas de torsion
							if (project->matrix->i != 0 || project->matrix->j != 0)
							{
								GetError(ERROR_MATRIX_NO_EXPORTABLE);
								break;
							}
							//--
							
							PsDlgExport dlg;
							dlg.DoModal();
							
							break;
						}
						
						
						case 'expi':
						{
							if (project)
							{
								if (PsSaveFileDialog(buffer, PS_IMAGE_FILE))
								{
									int x, y;
									project->renderer.GetDocSize(x, y);
									project->RenderToFile(buffer, x, y);
								}
							}							
							break;
						}
						
						case 'prnt':
						{
							PsDlgPrint dlgPrint;
							dlgPrint.DoModal();
							break;
						}
							
						case 'newm':
						{
							if (project)
							{
								GetError(project->NewMatrix());
								active->Update();
							}
							break;
						}
							
						case 'resm':
						{
							if (project)
							{
								project->MatrixReset();
								active->Update();
							}
							break;
						}
							
						case 'shli':
						{
							bool value = !PsController::Instance().GetOption(PsController::OPTION_DOCUMENT_SHOW, true);
							PsController::Instance().SetOption(PsController::OPTION_DOCUMENT_SHOW, value);
							UpdateMenuItem_OPTION_DOCUMENT_SHOW();
							active->Update();
							break;
						}
							
						case 'mktr':
						{
							bool value = !PsController::Instance().GetOption (PsController::OPTION_DOCUMENT_BLEND, true);
							PsController::Instance().SetOption (PsController::OPTION_DOCUMENT_BLEND, value);
							UpdateMenuItem_OPTION_DOCUMENT_BLEND();
							active->Update();
							break;
						}
							
						case 'clcm':
						{
							bool value = !PsController::Instance().GetOption (PsController::OPTION_HIGHLIGHT_SHOW, true);
							PsController::Instance().SetOption (PsController::OPTION_HIGHLIGHT_SHOW, value);
							UpdateMenuItem_OPTION_HIGHLIGHT_SHOW();
							active->Update();
							break;
						}
						
						case 'redo':
						{
							if (project)
							{
								active->MenuLogRedo();
								active->OnUpdateEditUndo();
								active->OnUpdateEditRedo();
							}
							break;
						}
	
						case 'undo':
						{
							if (project)
							{
								active->MenuLogUndo();
								active->OnUpdateEditUndo();
								active->OnUpdateEditRedo();
							}
							break;
						}
						
						case 'szzt':
						{
							if (project)
							{
								PsDlgDocument dlgDimensions;
								dlgDimensions.SetNoPreset();
								dlgDimensions.w = project->GetWidth();
								dlgDimensions.h = project->GetHeight();
								dlgDimensions.dpi = project->GetDpi();
								if (dlgDimensions.DoModal())
								{
									project->renderer.SetDocSize(dlgDimensions.w, dlgDimensions.h);
									project->renderer.dpi = dlgDimensions.dpi;
									active->Update();
								}
							}
							break;
						}
						
						case 'dupi':
						{
							if (project)
							{
								active->MenuEditClone();
							}
							break;
						}
						
						case 'deli':
						{
							if (project)
							{
								active->MenuItemDel();
							}
							break;
						}
						
						case 'colm':
						{
							if (project)
							{
								GetError(project->MatrixColor());
								active->Update();
							}
							break;
						}
						
						case 'bug ':
						{
							// FIXME
							break;
						}
							
						default:
							break;
					}
					
					// DIRTY FIX : pour ne pas que la touche pomme ou shift ne reste appuyée quand on fait un raccourci clavier
					PsController::Instance().SetOption(PsController::OPTION_CONSTRAIN, false);
					PsController::Instance().SetOption(PsController::OPTION_REFLECT, false);
					
					//printf("%d\n", cmd.commandID);
					break;
				} 
			}
			break;
		}
			
		default:
			break;
	}
	
	PsController::Instance().UpdateDialogProject(); 
	
	return result;
}


//-----------------------------------------------------------------------------------

static pascal OSErr Handle_OpenDocuments(const AppleEvent *theAppleEvent, AppleEvent *outAppleEvent, long inHandlerRefcon)
{
    AEDescList  docList;
    FSRef       theFSRef;
    long        index;
    long        count = 0;
	
    OSErr err = AEGetParamDesc(theAppleEvent, keyDirectObject, typeAEList, &docList);
	require_noerr(err, CantGetDocList);
	
	err = AECountItems(&docList, &count);
	require_noerr(err, CantGetCount);
			
	for(index = 1; index <= count; index++)
	{
		err = AEGetNthPtr(&docList, index, typeFSRef, NULL, NULL, &theFSRef, sizeof(FSRef), NULL);// 5
		require_noerr(err, CantGetDocDescPtr);
		
		char path[1024];
		FSRefMakePath (&theFSRef, (unsigned char *) path, 1023);
		OpenFile(path);
	}
	AEDisposeDesc(&docList);
				
	PsController::Instance().UpdateDialogProject(); 
	
CantGetDocList:
CantGetCount:
CantGetDocDescPtr:
	return(err);
}

//-----------------------------------------------------------------------------------

static pascal OSErr Handle_PrintDocuments(const AppleEvent *inAppleEvent, AppleEvent *outAppleEvent, long inHandlerRefcon)
{
	return errAEEventNotHandled;
}   // Handle_PrintDocuments

//-----------------------------------------------------------------------------------

void DisableEverything();

//-----------------------------------------------------------------------------------

OSStatus RegisterMyHelpBook()
{
	CFBundleRef myApplicationBundle;
	CFURLRef myBundleURL;
	FSRef myBundleRef;
	OSStatus err = noErr;
	
	myApplicationBundle = NULL;
	myBundleURL = NULL;
	
	myApplicationBundle = CFBundleGetMainBundle();// 1
	if (myApplicationBundle == NULL) {err = fnfErr; goto bail;}
		
	myBundleURL = CFBundleCopyBundleURL(myApplicationBundle);// 2
	if (myBundleURL == NULL) {err = fnfErr; goto bail;}
			
	if (!CFURLGetFSRef(myBundleURL, &myBundleRef)) err = fnfErr;// 3
				
	if (err == noErr) err = AHRegisterHelpBook(&myBundleRef);// 4

bail:
	return err;
					
}

//-----------------------------------------------------------------------------------

bool InitApplication()
{
	OSStatus err;
	
	static const EventTypeSpec    kAppEvents[] =
	{
		{ kEventClassCommand, kEventCommandProcess }
	};
	
	// raccourci de l'aide
	RegisterMyHelpBook();
	
	// Initialisation du pixel buffer opengl
	hardwareRenderer.SafeCreatePixelBuffer();
	
	// Create a Nib reference, passing the name of the nib file (without the .nib extension).
	// CreateNibReference only searches into the application bundle.
	err = CreateNibReference( CFSTR("main"), &sNibRef );
	require_noerr( err, CantGetNibRef );
	
	// Getting user laguage
	//---------------------
	CFArrayRef languages = CFBundleCopyPreferredLocalizationsFromArray(CFBundleCopyBundleLocalizations(CFBundleGetMainBundle()));
	CFStringRef cflang = (CFStringRef) CFArrayGetValueAtIndex(languages, 0);
    const char *lang = CFStringGetCStringPtr(cflang, CFStringGetSystemEncoding ());

	// Updating selected langauge
	//---------------------------
	if ( strcmp( lang, "French" ) == 0 )
	{
		setLanguage( French );
	}
	else
	{
		setLanguage( English );
	}
	
	// splasher
	PsWinSplash::Instance().Show();
	
	// Once the nib reference is created, set the menu bar. "MainMenu" is the name of the menu bar
	// object. This name is set in InterfaceBuilder when the nib is created.
	err = SetMenuBarFromNib( sNibRef, CFSTR("MenuBar") );
	require_noerr( err, CantSetMenuBar );
	
	DisableEverything();

	// Install our handler for common commands on the application target
	InstallApplicationEventHandler( NewEventHandlerUPP( AppEventHandler ),
									GetEventTypeCount( kAppEvents ), kAppEvents,
									0, NULL );
	
	// Pour l'ouverture de fichiers via le finder
	err = AEInstallEventHandler(kCoreEventClass, kAEOpenDocuments, Handle_OpenDocuments, 0, false);
	require_noerr(err, CantInstallAppleEventHandlerOpenDocs);

	// Pour l'impression de fichiers via le finder (FIXME)
	err = AEInstallEventHandler(kCoreEventClass, kAEPrintDocuments, Handle_PrintDocuments, 0, false);
	require_noerr(err, CantInstallAppleEventHandlerOpenDocs);

	PsWinTools::Instance().Show();
	PsWinBar::Instance().Show();
	PsWinProject::Instance().Show();
	PsWinProperties::Instance().Show();
	PsWinOverview::Instance().Show();
	
	dlgPropreties = &PsWinProperties::Instance();
	
	// chargement des paramètres
	OnInitApplication();
	
	// splasher
	PsWinSplash::Instance().bNeedToBeClosed = true;
	
CantInstallAppleEventHandlerOpenDocs:
CantCreateWindow:
CantSetMenuBar:
CantGetNibRef:
		return err;
}

//-----------------------------------------------------------------------------------

void EnableEverything()
{
	MenuRef menuBar = GetMenuRef(1);
	EnableAllMenuItems(menuBar);
	EnableAllMenuItems(GetMenuRef(2));
	EnableAllMenuItems(GetMenuRef(3));
	EnableAllMenuItems(GetMenuRef(4));
	active->OnUpdateEditUndo();
	active->OnUpdateEditRedo();
}

//-----------------------------------------------------------------------------------

void DisableEverything()
{
	MenuRef menuBar = GetMenuRef(1);
	DisableAllMenuItems(menuBar);
	DisableAllMenuItems(GetMenuRef(2));
	DisableAllMenuItems(GetMenuRef(3));
	DisableAllMenuItems(GetMenuRef(4));
	EnableMenuItem(menuBar, 1);
	EnableMenuItem(menuBar, 2);
}

//-----------------------------------------------------------------------------------

void EnableSave()
{
	if (!active) DisableEverything();
	else EnableEverything();
	MenuRef menuBar = GetMenuRef(0);
	EnableMenuCommand(menuBar, 'save');
	EnableMenuCommand(menuBar, 'svas');
}

//-----------------------------------------------------------------------------------

void DisableSave()
{
	if (!active) DisableEverything();
	else EnableEverything();
	MenuRef menuBar = GetMenuRef(0);
	DisableMenuCommand(menuBar, 'save');
}

//-----------------------------------------------------------------------------------

bool OpenFile(char *path)
{
	PsMacView *View = CreateNewViewWindow();
	ErrID r = View->project->FileLoad(path);
	if (r == ERROR_NONE)
	{
		View->UpdateTitle(path);
		if (View->Path)
			delete View->Path;
		active->Path = strdup(path);
		active->UpdateNow();		
		ShowWindow(View->window);
		EnableSave();
		return true;
	}
	else
	{
		RemoveEventLoopTimer(active->gTimer);
		PsController::Instance().SetActive(NULL);
		DisposeWindow(View->window);
		delete View;
		active = NULL;
		GetError(r);
		return false;
	}
}

//-----------------------------------------------------------------------------------


