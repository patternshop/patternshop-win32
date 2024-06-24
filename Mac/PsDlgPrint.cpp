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
#include "PsDlgPrint.h"
#include "PsProject.h"

static PsDlgPrint *instPrint = NULL;

static OSStatus PsDlgPrintEvents(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsDlgPrint *doc = (PsDlgPrint*)userData;
	return doc->EventsHandler(myHandler, event);
}

static OSStatus OnDrawView(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsDlgPrint *View = (PsDlgPrint*)userData;
	OSStatus status = noErr;
	CGContextRef myContext;
	status = GetEventParameter (event, kEventParamCGContextRef, typeCGContextRef, NULL, 
										 sizeof (CGContextRef), NULL,  &myContext);
	require_noerr(status, CantGetGraphicsContext);
	View->OnDrawView(myContext);
CantGetGraphicsContext:
		return status;
}

static OSStatus PsDlgPrintDrawLink(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsDlgPrint *View = (PsDlgPrint*)userData;
	OSStatus status = noErr;
	CGContextRef myContext;
	HIRect bounds;
	
	status = GetEventParameter (event, kEventParamCGContextRef, typeCGContextRef, NULL,
										 sizeof (CGContextRef), NULL,  &myContext);
	require_noerr(status, CantGetGraphicsContext);	
	HIViewRef myHIView;
	static const HIViewID  myHIViewID = { 'link', 0 };
	HIViewFindByID(HIViewGetRoot(View->window), myHIViewID, &myHIView);
	HIViewGetBounds (myHIView, &bounds);
	require_noerr(status, CantGetBoundingRectangle);
	DrawCGImage(myContext, bounds.size.height, View->imageLink, 0, 0);
	
CantGetBoundingRectangle:
CantGetGraphicsContext:
		return status;
}

PsDlgPrint::PsDlgPrint()
{
	OSStatus err = noErr; 
	
	// events de la fenetre
	const EventTypeSpec kWindowEvents[] = { kEventClassCommand, kEventCommandProcess };
	
	// creation de la fenetre
	err = CreateWindowFromNib(sNibRef, CFSTR("PsDlgPrint"), &window);
	require_noerr( err, CantCreateWindow );
	InstallWindowEventHandler(window, NewEventHandlerUPP(PsDlgPrintEvents),
									  GetEventTypeCount(kWindowEvents), kWindowEvents, (void*)this, NULL );
	
	static const EventTypeSpec myHIViewSpec[] = { kEventClassControl, kEventControlDraw };
	
	//-- handle sur la vue
	static const HIViewID myHViewID = { 'view', 0 };
	HIViewFindByID(HIViewGetRoot(window), myHViewID, &myHView);
	err = InstallEventHandler(GetControlEventTarget(myHView), NewEventHandlerUPP(::OnDrawView),
						GetEventTypeCount(myHIViewSpec), myHIViewSpec, (void *) this, NULL);

	//-- handle sur l'affichage de l'icone de lien
	HIViewRef myHView2;
	static const HIViewID myHViewID2 = { 'link', 0 };
	imageLink = LoadCGImageFromPng("link");
	HIViewFindByID(HIViewGetRoot(window), myHViewID2, &myHView2);
	err = InstallEventHandler(GetControlEventTarget(myHView2), NewEventHandlerUPP(PsDlgPrintDrawLink),
									  GetEventTypeCount(myHIViewSpec), myHIViewSpec, (void *) this, NULL);	
	
	
	PsProject *project = PsController::Instance().project;
	
	bCadre = true;
	bNoProtect = false;
	xmode = 1;
	ymode = 1;
	lmode = 1;
	hmode = 1;
	
	x = 0;
	y = 0;	
	w = project->GetWidth();
	h = project->GetHeight();
	z = 100;
	
	// Format A4 vertical (FIXME: possibilité de choisir)
	format_w = 8.2666667 * project->GetDpi();
	format_h = 11.693333 * project->GetDpi();

	// evenements des textboxes
	InstallTextChangeEvent('xtex');
	InstallTextChangeEvent('ytex');
	InstallTextChangeEvent('ltex');
	InstallTextChangeEvent('htex');
	InstallTextChangeEvent('ztex');

	instPrint = this;

CantCreateWindow:
	;
}

void PsDlgPrint::Initialize()
{
	HIRect bounds;
	HIViewGetBounds(myHView, &bounds);
	
	//-- buffers
	float zx =(float)format_w /(float)bounds.size.width;
	float zy =(float)format_h /(float)bounds.size.height;
	float zoom = zx < zy ? zy : zx;
	imageBuffer.Create(format_w / zoom, format_h / zoom, 24);
	iHeight = imageBuffer.GetHeight() - 1;
	r_zoom = zoom;
	
	PrepareMiniImage();
	
	//-- valeurs par défaut
	bCenter = true;
	bAuto = true;
	UpdateFillSize();
	
	Update();
}

void PsDlgPrint::InstallTextChangeEvent(OSType signature)
{
	static const EventTypeSpec kControlEvents[] = { kEventClassTextField, kEventTextDidChange };
	ControlRef control;
	ControlID myControlID;
	myControlID.id = 0;
	myControlID.signature = signature;
	GetControlByID(window, &myControlID, &control);
	InstallControlEventHandler(control, NewEventHandlerUPP(PsDlgPrintEvents),
			GetEventTypeCount(kControlEvents), kControlEvents, (void*)this, NULL);	
}

/*
** dessin du rendu
*/
void PsDlgPrint::OnDrawView(CGContextRef myContext)
{
	HIRect bounds;
	HIViewGetBounds (myHView, &bounds);
	DrawCGImage(myContext, bounds.size.height, imageBuffer.buffer, 
					(bounds.size.width - imageBuffer.GetWidth()) / 2.f,
					(bounds.size.height - imageBuffer.GetHeight()) / 2.f);
}

void PsDlgPrint::UpdateAutoCenter()
{
	ControlRef control;
	ControlID controlId;
	controlId.id = 0;
	if (bCenter)
	{
		controlId.signature = 'xtex';
		GetControlByID(window, &controlId, &control);
		DeactivateControl(control);
		controlId.signature = 'ytex';
		GetControlByID(window, &controlId, &control);
		DeactivateControl(control);
		
		x = (format_w - w * z / 100.f) / 2;
		y = (format_h - h * z / 100.f) / 2;
		Update();
	}
	else
	{
		controlId.signature = 'xtex';
		GetControlByID(window, &controlId, &control);
		ActivateControl(control);
		controlId.signature = 'ytex';
		GetControlByID(window, &controlId, &control);
		ActivateControl(control);
	}
}

void PsDlgPrint::UpdateFillSize()
{
	ControlRef control;
	ControlID controlId;
	controlId.id = 0;
	if (bAuto)
	{
		controlId.signature = 'ztex';
		GetControlByID(window, &controlId, &control);
		DeactivateControl(control);
		controlId.signature = 'ltex';
		GetControlByID(window, &controlId, &control);
		DeactivateControl(control);
		controlId.signature = 'htex';
		GetControlByID(window, &controlId, &control);
		DeactivateControl(control);

		z = (float)format_w / (float)w * 100.f;
		if ((float)format_w / (float)w > (float)format_h / (float)h)
			z = (float)format_h / (float)h * 100.f;

		UpdateAutoCenter();
	}
	else
	{
		controlId.signature = 'ztex';
		GetControlByID(window, &controlId, &control);
		ActivateControl(control);
		controlId.signature = 'ltex';
		GetControlByID(window, &controlId, &control);
		ActivateControl(control);
		controlId.signature = 'htex';
		GetControlByID(window, &controlId, &control);
		ActivateControl(control);
	}
}

OSStatus PsDlgPrint::EventsHandler(EventHandlerCallRef myHandler, EventRef event)
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
						
						if (myControlID.signature == 'xtex') 
						{
							x = GetIntegerValue(buffer, xmode);
						}
						if (myControlID.signature == 'ytex') 
						{
							y = GetIntegerValue(buffer, ymode);
						}
						if (myControlID.signature == 'ltex') 
						{
							float w_ = GetIntegerValue(buffer, lmode);
							if (w_ > 0)
								SetZ(w_ / w * 100.f);
						}
						if (myControlID.signature == 'htex') 
						{
							float h_ = GetIntegerValue(buffer, hmode);
							if (h_ > 0)
								SetZ(h_ / h * 100.f);
						}
						if (myControlID.signature == 'ztex') 
						{
							float z_ = atol(buffer);
							if (z_ >= 10)
								SetZ(z_);
						}
						UpdateMiniImage();
						HIViewSetNeedsDisplay(myHView, true);
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
							
						case 'xmd2': xmode = 0; break;					
						case 'xmd3': xmode = 1; break;						
						case 'xmd4': xmode = 2; break;				
						case 'xmd5': xmode = 3; break;		
						case 'xmd6': xmode = 4; break;

						case 'ymd2': ymode = 0; break;					
						case 'ymd3': ymode = 1; break;						
						case 'ymd4': ymode = 2; break;				
						case 'ymd5': ymode = 3; break;		
						case 'ymd6': ymode = 4; break;
							
						case 'lmd2': lmode = 0; break;					
						case 'lmd3': lmode = 1; break;						
						case 'lmd4': lmode = 2; break;				
						case 'lmd5': lmode = 3; break;		
						case 'lmd6': lmode = 4; break;
							
						case 'hmd2': hmode = 0; break;					
						case 'hmd3': hmode = 1; break;						
						case 'hmd4': hmode = 2; break;				
						case 'hmd5': hmode = 3; break;		
						case 'hmd6': hmode = 4; break;

						case 'cent': 
							bCenter = !bCenter;
							UpdateAutoCenter();
							break;

						case 'auto':
							bAuto = !bAuto;
							UpdateFillSize();
							break;

						case 'cadr':
							bCadre = !bCadre;
							break;
					}
					Update();
					break;
				}
			}
		}			
	}
	return result;
}

void PsDlgPrint::OnCancelButton()
{
	Close();
}

void PsDlgPrint::Close()
{
	HideWindow(window);
	DisposeWindow(window);
	QuitAppModalLoopForWindow(window);
}

void PsDlgPrint::UpdateValue(OSType signature, char *buffer)
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

void PsDlgPrint::Update()
{
	char buffer[1024];	
	GetTextValue(x, xmode, buffer);
	UpdateValue('xtex', buffer);
	GetTextValue(y, ymode, buffer);
	UpdateValue('ytex', buffer);
	GetTextValue((int)(w * z /100.f), lmode, buffer);
	UpdateValue('ltex', buffer);
	GetTextValue((int)(h * z /100.f), hmode, buffer);
	UpdateValue('htex', buffer);
	sprintf(buffer, "%.0f%%", z);
	UpdateValue('ztex', buffer);
	UpdateMiniImage();
	HIViewSetNeedsDisplay(myHView, true);
}

void PsDlgPrint::GetTextValue(double px, int indexType, char *buffer)
{
	double v = -1;
	float dpi = PsController::Instance().project->renderer.dpi;
	switch (indexType)
	{
		case 0:
			v = px / dpi;
			sprintf(buffer, "%.3f", v);
			break;
		case 1: 
			v = px / (dpi / 2.54f);
			sprintf(buffer, "%.2f", v);
			break;
		case 2: 
			v = px * 10.f / (dpi / 2.54f);
			sprintf(buffer, "%.3f", v);
			break; 
		case 3:
			v = px / dpi / 0.0138888f;
			sprintf(buffer, "%.1f", v); 
			break;
		case 4:
			v = px / dpi * 6.f;
			sprintf(buffer, "%.1f", v);
			break;
			break;
		default:  
			sprintf(buffer, "?");
			break;
	}
}

void PsDlgPrint::PrepareMiniImage()
{
	PsRender& renderer = PsController::Instance().project->renderer;	
	float size_x = renderer.size_x;
	float size_y = renderer.size_y;
	float scroll_x = renderer.scroll_x;
	float scroll_y = renderer.scroll_y;
	float zoom = renderer.zoom;
	bool showbox = PsController::Instance().GetOption(PsController::OPTION_BOX_SHOW);
	bool showmat = PsController::Instance().GetOption(PsController::OPTION_HIGHLIGHT_SHOW);
	bool showdoc = PsController::Instance().GetOption(PsController::OPTION_DOCUMENT_SHOW);
	int height = imageBuffer.GetHeight();
	
	PsController::Instance().SetOption(PsController::OPTION_BOX_SHOW, false);
	PsController::Instance().SetOption(PsController::OPTION_HIGHLIGHT_SHOW, false);
	PsController::Instance().SetOption(PsController::OPTION_DOCUMENT_SHOW, false);
	
	renderer.SetSize(imageBuffer.GetWidth(), height);
	renderer.CenterView();
	
	PsController::Instance().project->RenderToScreen();
	m_RenduImage.Create(500, 500, 24);
	hardwareRenderer.CopyToSoftBuffer(m_RenduImage);
	
	PsRect z; // calcul de la zone de l'image
	z.left = (0 - renderer.x1) / (renderer.x2 - renderer.x1) * imageBuffer.GetWidth();
	z.right = (renderer.doc_x - renderer.x1) / (renderer.x2 - renderer.x1) * imageBuffer.GetWidth();
	z.bottom = height - (renderer.doc_y - renderer.y1) / (renderer.y2 - renderer.y1) * height;
	z.top = height - (0 - renderer.y1) / (renderer.y2 - renderer.y1) * height;
	float r_size_x = z.right - z.left;
	float r_size_y = z.bottom - z.top;
	r_zoom2 = renderer.zoom; 
	
	CGRect subRect = CGRectMake(z.left, m_RenduImage.GetHeight() - height + z.top, r_size_x, r_size_y);
	CGImageRef subImage = CGImageCreateWithImageInRect(m_RenduImage.buffer, subRect);
	imageOriginal.buffer = subImage;
	
	PsController::Instance().SetOption(PsController::OPTION_HIGHLIGHT_SHOW, showmat);
	PsController::Instance().SetOption(PsController::OPTION_BOX_SHOW, showbox);
	PsController::Instance().SetOption(PsController::OPTION_DOCUMENT_SHOW, showdoc);
	renderer.SetSize(size_x, size_y);
	renderer.SetZoom(zoom);
	renderer.SetScroll(scroll_x, scroll_y);	
}

void PsDlgPrint::UpdateMiniImage()
{
	SetTarget(&imageBuffer);

	SetPenColor(0, 0, 0);
	SetBrushColor(255, 255, 255);
	DrawRectangle(0, 0, imageBuffer.GetWidth() - 1, imageBuffer.GetHeight() - 1);
	
	int l_x = x / r_zoom;
	int l_y = y / r_zoom;
	
	float coef = z / 100.f / r_zoom * r_zoom2;
	int o_width = imageOriginal.GetWidth();
	int o_height = imageOriginal.GetHeight();
	int d_width = o_width * coef;
	int d_height = o_height * coef;

	int s_width = o_width;
	if (d_width + l_x > imageBuffer.GetWidth())
	{
		s_width = o_width + (imageBuffer.GetWidth() - d_width - l_x - 1) / coef;
	}
	
	int s_height = o_height;
	if (d_height + l_y > imageBuffer.GetHeight())
	{
		s_height = o_height + (imageBuffer.GetHeight() - d_height - l_y - 1) / coef;
	}
	
	CGRect subRect = CGRectMake(0, 0, s_width, s_height);
	CGImageRef subImage = CGImageCreateWithImageInRect(imageOriginal.buffer, subRect);
	SoftwareBuffer cropedBuffer;
	cropedBuffer.buffer = subImage;

	HIRect lbounds;
	lbounds.origin.x = l_x;
	lbounds.size.width = s_width * coef;
	lbounds.size.height = s_height * coef;
	lbounds.origin.y = iHeight - l_y - lbounds.size.height;
	CGContextDrawImage(dc, lbounds, cropedBuffer.buffer);

	if (bCadre)
	{
		SetDashBlackPen();
		int x1 = l_x > 0 ? l_x : 0;
		int x2 = l_x + d_width < imageBuffer.GetWidth() ? l_x + d_width : imageBuffer.GetWidth();
		int y1 = l_y > 0 ? l_y : 0;
		int y2 = l_y + d_height < imageBuffer.GetHeight() ? l_y + d_height : imageBuffer.GetHeight();
		MovePenTo(x1, y1);
		DrawLineTo(x2, y1);
		DrawLineTo(x2, y2);
		DrawLineTo(x1, y2);
		DrawLineTo(x1, y1);
	}
	
	SetPenColor(0, 0, 0);
	MovePenTo(0, 0);
	DrawLineTo(imageBuffer.GetWidth() - 1, 0);
	DrawLineTo(imageBuffer.GetWidth() - 1, imageBuffer.GetHeight() - 1);
	DrawLineTo(0, imageBuffer.GetHeight() - 1);
	DrawLineTo(0, 0);
	
	SetTarget(NULL);
}

void PsDlgPrint::SetZ(float z_)
{
	z = z_;
	if (bCenter) UpdateAutoCenter();
	else Update();
}

int PsDlgPrint::GetIntegerValue(char *string, int iMode)
{
	char *buffer = strdup(string);
	for (int i = 0; i < strlen(buffer); ++i)
		if (buffer[i] == ',')
			buffer[i] = '.';
	float v = atof(buffer);
	delete buffer;
	float dpi = PsController::Instance().project->GetDpi();
	switch (iMode)
	{
		case 0: return v * dpi; break;
		case 1: return v * (dpi / 2.54f); break;
		case 2: return v / 10.f * (dpi / 2.54f); break;
		case 3: return v * dpi * 0.0138888f; break;
		case 4: return v * dpi / 6.f; break;
		default: break;
	}
	return -1;
}


/*-------------------------------------------------------------*/


#include <ApplicationServices/ApplicationServices.h>

/*------------------------------------------------------------------------------
	Globals
------------------------------------------------------------------------------*/
static	Handle	gflatPageFormat = NULL;		// used in FlattenAndSavePageFormat

/*------------------------------------------------------------------------------
	Prototypes
------------------------------------------------------------------------------*/

void	DoEventLoop(void);
void	DoHighLevel(EventRecord *AERecord);
static 	OSErr	AEPrintDocument(const AppleEvent *inputEvent, AppleEvent *outputEvent, SInt32 handlerRefCon);
static 	OSErr	AEOpenHandler(const AppleEvent *inputEvent, AppleEvent *outputEvent, SInt32 handlerRefCon);
static 	OSErr	AEOpenDocHandler(const AppleEvent *inputEvent, AppleEvent *outputEvent, SInt32 handlerRefCon);
static 	OSErr	AEQuitHandler(const AppleEvent *inputEvent, AppleEvent *outputEvent, SInt32 handlerRefCon);
  
OSStatus 	DoPageSetupDialog(PMPrintSession printSession, PMPageFormat* pageFormat);
OSStatus 	DoPrintDialog(PMPrintSession printSession, PMPageFormat pageFormat,
				PMPrintSettings* printSettings);
OSStatus	DoPrintLoop(PMPrintSession printSession, PMPageFormat pageFormat,
				PMPrintSettings printSettings);
void		DoPrintSample(void);

OSStatus 	FlattenAndSavePageFormat(PMPageFormat pageFormat);
OSStatus 	LoadAndUnflattenPageFormat(PMPageFormat* pageFormat);
OSStatus	DetermineNumberOfPagesInDoc(PMPageFormat pageFormat, UInt32* numPages);
OSStatus 	DrawPage(CGContextRef context, UInt32 pageNumber);
void 		PostPrintingErrors(OSStatus status);

/*------------------------------------------------------------------------------

    Function:	DoPrintSample
    
    Parameters:
        None
    Description:
        Sample print loop.

------------------------------------------------------------------------------*/

bool PsDlgPrint::DoModal()
{
	OSStatus		status = noErr;
	PMPageFormat	pageFormat = kPMNoPageFormat;
	PMPrintSettings	printSettings = kPMNoPrintSettings;
	PMPrintSession	printSession = NULL;
	
	//	Initialize the printing manager and create a printing session.
	status = PMCreateSession(&printSession);
	if (status != noErr) 
		return false;	// pointless to continue if PMCreateSession fails
		
	//	Display the Page Setup dialog.
	if (status == noErr)
		status = DoPageSetupDialog(printSession, &pageFormat);
	
	// on ajuste les dpi
	PMResolution r;
	float dpi = PsController::Instance().project->renderer.dpi;
	r.hRes = dpi;
	r.vRes = dpi;
	PMSetResolution(pageFormat, &r);
	
	Boolean bRes;
	PMSessionValidatePageFormat(printSession, pageFormat, &bRes);
	
	// recupère la taille du papier
	PMRect pageRect;
	PMGetAdjustedPageRect (pageFormat, &pageRect);
	format_w = pageRect.right;
	format_h = pageRect.bottom;
	
	// Display our Custom dialog
	Initialize();
	if (!DoModalPreview())
		status = kPMCancel;
		 
	//	Display the Print dialog.
	if (status == noErr)
		status = DoPrintDialog(printSession, pageFormat, &printSettings);
	
	//	Execute the print loop.
	if (status == noErr)
		status = DoPrintLoop(printSession, pageFormat, printSettings);
	
	if(status == kPMCancel)
		status = noErr;
	
	if (status != noErr)
		PostPrintingErrors(status);
	
	if (pageFormat != kPMNoPageFormat)
		(void)PMRelease(pageFormat);
	if (printSettings != kPMNoPrintSettings)
		(void)PMRelease(printSettings);
	
	//	Terminate the current printing session. 
	(void) PMRelease(printSession);
	
	return bResult;
}

bool PsDlgPrint::DoModalPreview()
{
	bResult = false;
	ShowWindow(window);
	RunAppModalLoopForWindow(window);
	return bResult;
}

void PsDlgPrint::OnOkButton()
{
	bResult = true;
	Close();
}

void DoHighLevel(EventRecord *AERecord)
{
    
    AEProcessAppleEvent(AERecord);
    
}
/*------------------------------------------------------------------------------

    Function:	DoPageSetupDialog
    
    Parameters:
        printSession	-	current printing session
        pageFormat	-	a PageFormat object addr
    
    Description:
        If the caller passes in an empty PageFormat object, DoPageSetupDialog
        creates a new one, otherwise it validates the one provided by the caller.
        It then invokes the Page Setup dialog and checks for Cancel. Finally it
        flattens the PageFormat object so it can be saved with the document.
        Note that the PageFormat object is modified by this function.
	
------------------------------------------------------------------------------*/
OSStatus 	DoPageSetupDialog(PMPrintSession printSession, PMPageFormat* pageFormat)
{
	OSStatus	status = noErr;
	Boolean		accepted;
	
	//	Set up a valid PageFormat object.
	if (*pageFormat == kPMNoPageFormat)
            {
            status = PMCreatePageFormat(pageFormat);
		
            //	Note that PMPageFormat is not session-specific, but calling
            //	PMSessionDefaultPageFormat assigns values specific to the printer
            //	associated with the current printing session.
            if ((status == noErr) && (*pageFormat != kPMNoPageFormat))
                status = PMSessionDefaultPageFormat(printSession, *pageFormat);
            }
	else
            status = PMSessionValidatePageFormat(printSession, *pageFormat, kPMDontWantBoolean);

	//	Display the Page Setup dialog.	
	if (status == noErr)
            {
            status = PMSessionPageSetupDialog(printSession, *pageFormat, &accepted);
            if (status == noErr && !accepted)
                status = kPMCancel;		// user clicked Cancel button
            }	
				
	//	If the user did not cancel, flatten and save the PageFormat object
	//	with our document.
	if (status == noErr)
            status = FlattenAndSavePageFormat(*pageFormat);

	return status;
	
}	//	DoPageSetupDialog



/*------------------------------------------------------------------------------
	Function:	DoPrintDialog
		
	Parameters:
		printSession	-	current printing session
		pageFormat	-	a PageFormat object addr
		printSettings	-	a PrintSettings object addr
			
	Description:
		If the caller passes an empty PrintSettings object, DoPrintDialog creates
		a new one, otherwise it validates the one provided by the caller.
		It then invokes the Print dialog and checks for Cancel.
		Note that the PrintSettings object is modified by this function.
		
------------------------------------------------------------------------------*/
OSStatus 	DoPrintDialog(PMPrintSession printSession, PMPageFormat pageFormat,
				PMPrintSettings* printSettings)
{
	OSStatus	status = noErr;
	Boolean		accepted;
	UInt32		realNumberOfPagesinDoc;
	
	//	In this sample code the caller provides a valid PageFormat reference but in
	//	your application you may want to load and unflatten the PageFormat object
	//	that was saved at PageSetup time.  See LoadAndUnflattenPageFormat below.
	
	//	Set up a valid PrintSettings object.
	if (*printSettings == kPMNoPrintSettings)
            {
            status = PMCreatePrintSettings(printSettings);	

            //	Note that PMPrintSettings is not session-specific, but calling
            //	PMSessionDefaultPrintSettings assigns values specific to the printer
            //	associated with the current printing session.
            if ((status == noErr) && (*printSettings != kPMNoPrintSettings))
                status = PMSessionDefaultPrintSettings(printSession, *printSettings);
            }
	else
            status = PMSessionValidatePrintSettings(printSession, *printSettings, kPMDontWantBoolean);
	
	//	Before displaying the Print dialog, we calculate the number of pages in the
	//	document.  On Mac OS X this is useful because we can prime the Print dialog
	//	with the actual page range of the document and prevent the user from entering
	//	out-of-range numbers.  This is not possible on Mac OS 8 and 9 because the driver,
	//	not the printing manager, controls the page range fields in the Print dialog.

	//	Calculate the number of pages required to print the entire document.
	if (status == noErr)
            status = DetermineNumberOfPagesInDoc(pageFormat, &realNumberOfPagesinDoc);

	//	Set a valid page range before displaying the Print dialog
	if (status == noErr)
            status = PMSetPageRange(*printSettings, 1, realNumberOfPagesinDoc);

	//	Display the Print dialog.
	if (status == noErr)
            {
            status = PMSessionPrintDialog(printSession, *printSettings, pageFormat, &accepted);
            if (status == noErr && !accepted)
                status = kPMCancel;		// user clicked Cancel button
            }
		
	return status;
	
}	//	DoPrintDialog

static OSStatus MyPMSessionBeginCGDocument(PMPrintSession printSession, 
                PMPrintSettings printSettings, PMPageFormat pageFormat)
{
    OSStatus err = noErr;
    // Tell the printing system we want to print by drawing to a CGContext,
    // not a QD port. 

	
    // Use the simpler call if it is present.
    if(&PMSessionBeginCGDocument != nil){
	err = PMSessionBeginCGDocument(printSession, printSettings, pageFormat);
    }else{

        CFStringRef s[1] = { kPMGraphicsContextCoreGraphics };
        CFArrayRef  graphicsContextsArray = CFArrayCreate(NULL, (const void**)s, 1, &kCFTypeArrayCallBacks);
        err = PMSessionSetDocumentFormatGeneration(printSession, kPMDocumentFormatPDF, graphicsContextsArray, NULL);
        CFRelease(graphicsContextsArray);
	if(!err)
	    err = PMSessionBeginDocument(printSession, printSettings, pageFormat);
 
    }

    return err;
}

static OSStatus MyPMSessionGetCGGraphicsContext(PMPrintSession printSession, CGContextRef *printingContextP)
{
    OSStatus err = noErr;
   
    // Use the simpler call if it is present.
    if(&PMSessionGetCGGraphicsContext != nil){
	err = PMSessionGetCGGraphicsContext(printSession, printingContextP);
    }else{
    err = PMSessionGetGraphicsContext(printSession, kPMGraphicsContextCoreGraphics, (void**)printingContextP);
    }
    return err;
}

/*------------------------------------------------------------------------------
	Function:
		DoPrintLoop
	
	Parameters:
		printSession	-	current printing session
		pageFormat	-	a PageFormat object addr
		printSettings	-	a PrintSettings object addr
	
	Description:
		DoPrintLoop calculates which pages to print and executes the print
		loop, calling DrawPage for each page.
				
------------------------------------------------------------------------------*/
OSStatus DoPrintLoop(PMPrintSession printSession, PMPageFormat pageFormat,
            PMPrintSettings printSettings)
{
    OSStatus	status = noErr, tempErr;
    UInt32	realNumberOfPagesinDoc,
                pageNumber,
                firstPage,
                lastPage;
    CFStringRef	jobName = CFSTR("Patternshop Printing");

    //	Since this sample code doesn't have a window, give the spool file a name.
    status = PMSetJobNameCFString(printSettings, jobName);

    //	Get the user's Print dialog selection for first and last pages to print.
    if (status == noErr)
        {
        status = PMGetFirstPage(printSettings, &firstPage);
        if (status == noErr)
            status = PMGetLastPage(printSettings, &lastPage);
        }

    //	Check that the selected page range does not exceed the actual number of
    //	pages in the document.
    if (status == noErr)
        {
        status = DetermineNumberOfPagesInDoc(pageFormat, &realNumberOfPagesinDoc);
        if (realNumberOfPagesinDoc < lastPage)
            lastPage = realNumberOfPagesinDoc;
        }

    //	Before executing the print loop, tell the Carbon Printing Manager which pages
    //	will be spooled so that the progress dialog can reflect an accurate page count.
    //	This is recommended on Mac OS X.  On Mac OS 8 and 9, we have no control over
    //	what the printer driver displays.
	
    if (status == noErr)
        status = PMSetFirstPage(printSettings, firstPage, false);
    if (status == noErr)
        status = PMSetLastPage(printSettings, lastPage, false);
    	
    //	Note, we don't have to worry about the number of copies.  The printing
    //	manager handles this.  So we just iterate through the document from the
    //	first page to be printed, to the last.
    if (status == noErr)
        {
            //	Begin a new print job.
            status = MyPMSessionBeginCGDocument(printSession, printSettings, pageFormat);
            if (status == noErr)
            {
                //	Print the selected range of pages in the document.		
                pageNumber = firstPage;
            
                /* Note that we check PMSessionError immediately before beginning a new
                    page. This handles user cancelling appropriately. Also, if we got
		    an error on any previous iteration of the print loop, we break
		    out of the loop.
                */
                while ( (pageNumber <= lastPage) && 
		    (status == noErr) && (PMSessionError(printSession) == noErr) )
                {
                //	Note, we don't have to deal with the classic Printing Manager's
                //	128-page boundary limit.
				
                //	Set up a page for printing.  Under the classic Printing Manager, applications
                //	could provide a page rect different from the one in the print record to achieve
                //	scaling. This is no longer recommended and on Mac OS X, the PageRect argument
                //	is ignored.
                status = PMSessionBeginPage(printSession, pageFormat, NULL);
                if (status == noErr){
		    CGContextRef printingContext;
                        
                    //	Get the printing graphics context, in this case a Quartz context,
                    //	for drawing the page. The origin is the lower left corner of the sheet with
		    //  the y axis going up the page.
                    status = MyPMSessionGetCGGraphicsContext(printSession, &printingContext);
                    if (status == noErr) {
                        //	Draw the page.
                        status = DrawPage(printingContext, pageNumber);
                     }
                                    
                    //	Close the page.
                    tempErr = PMSessionEndPage(printSession);
		    if(status == noErr)
			status = tempErr;
                }
                    
                //	And loop.
                    pageNumber++;
                } // end while loop
			
                // Close the print job.  This dismisses the progress dialog on Mac OS X.
            	tempErr = PMSessionEndDocument(printSession);
                if(status == noErr)
                    status = tempErr;
            }
        }
		
	//	Only report a printing error once we have completed the print loop. This
	//	ensures that every PMBeginXXX call that returns no error is followed by
        //	a matching PMEndXXX call, so the Printing Manager can release all temporary 
        //	memory and close properly.
	tempErr = PMSessionError(printSession);
        if(status == noErr)
            status = tempErr;
	if (status != noErr && status != kPMCancel)
            PostPrintingErrors(status);
		
    return status;
}	//	DoPrintLoop



/*------------------------------------------------------------------------------
	Function:
		FlattenAndSavePageFormat
	
	Parameters:
		pageFormat	-	a PageFormat object
	
	Description:
		Flattens a PageFormat object so it can be saved with the document.
		Assumes caller passes a validated PageFormat object.
		
------------------------------------------------------------------------------*/
OSStatus FlattenAndSavePageFormat(PMPageFormat pageFormat)
{
    OSStatus	status;
    Handle	flatFormatHandle = NULL;
	
    //	Flatten the PageFormat object to memory.
    status = PMFlattenPageFormat(pageFormat, &flatFormatHandle);

    if(status == noErr){
        //	Write the PageFormat data to file.
        //	In this sample code we simply copy it to a global.	
        gflatPageFormat = flatFormatHandle;
    }

    return status;
}	//	FlattenAndSavePageFormat



/*------------------------------------------------------------------------------
    Function:	LoadAndUnflattenPageFormat
	
    Parameters:
        pageFormat	- PageFormat object read from document file
	
    Description:
        Gets flattened PageFormat data from the document and returns a PageFormat
        object.
        The function is not called in this sample code but your application
        will need to retrieve PageFormat data saved with documents.
		
------------------------------------------------------------------------------*/
OSStatus	LoadAndUnflattenPageFormat(PMPageFormat* pageFormat)
{
    OSStatus	status = noErr;
    Handle	flatFormatHandle = NULL;

    //	Read the PageFormat flattened data from file.
    //	In this sample code we simply copy it from a global.
    flatFormatHandle = gflatPageFormat;
    if(flatFormatHandle){
        //	Convert the PageFormat flattened data into a PageFormat object.
        status = PMUnflattenPageFormat(flatFormatHandle, pageFormat);
    }else{
        *pageFormat = kPMNoPageFormat;
    }
	
    return status;
}	//	LoadAndUnflattenPageFormat



/*------------------------------------------------------------------------------
    Function:	DetermineNumberOfPagesInDoc
	
    Parameters:
    	pageFormat	- a PageFormat object addr
        numPages	- on return, the size of the document in pages
			
    Description:
    	Calculates the number of pages needed to print the entire document.
		
------------------------------------------------------------------------------*/
OSStatus	DetermineNumberOfPagesInDoc(PMPageFormat pageFormat, UInt32* numPages)
{
    OSStatus	status;
    PMRect	pageRect;

    //	PMGetAdjustedPageRect returns the page size taking into account rotation,
    //	resolution and scaling settings.
    status = PMGetAdjustedPageRect(pageFormat, &pageRect);

    //	In this sample code we simply return a hard coded number.  In your application,
    //	you will need to figure out how many page rects are needed to image the
    //	current document.
    *numPages = 1;

    return status;
    
}	//	DetermineNumberOfPagesinDoc

//    Handy utility function for retrieving an int from a CFDictionaryRef
static int GetIntFromDictionaryForKey( CFDictionaryRef desc, CFStringRef key )
{
	CFNumberRef value;
	int num = 0;
	if ( (value = (CFNumberRef)CFDictionaryGetValue(desc, key)) == NULL
		  || CFGetTypeID(value) != CFNumberGetTypeID())
		return 0;
	CFNumberGetValue(value, kCFNumberIntType, &num);
	return num;
}

/*------------------------------------------------------------------------------
    Function:	DrawPage
	
    Parameters:
        printSession	- current printing session
        pageNumber	- the logical page number in the document
	
    Description:
        Draws the contents of a single page.
		
------------------------------------------------------------------------------*/
OSStatus DrawPage(CGContextRef context, UInt32 pageNumber)
{
	int iWidth, iHeight;
	
	PsProject *project = PsController::Instance().project;
	project->renderer.GetDocSize(iWidth, iHeight);
	project->RenderToFile("/tmp/printer.png", iWidth, iHeight);

	// Get the URL to the bundle resource.
	CFStringRef name = CFStringCreateWithCString (kCFAllocatorDefault, "/tmp/printer.png", kCFStringEncodingUTF8);
	CFURLRef url = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, name, kCFURLPOSIXPathStyle, NULL);
	CFRelease(name);
	assert(url != NULL);
	
	// Create the data provider object
	CGDataProviderRef provider = CGDataProviderCreateWithURL(url);
	CFRelease (url);
	
	// Create the image object from that provider.
	CGImageRef image = CGImageCreateWithPNGDataProvider(provider, NULL, true,
														  kCGRenderingIntentDefault);
	CGDataProviderRelease (provider);
	
	assert(image);
	

	
	/*
	int horzres = pDC->GetDeviceCaps(HORZRES);
	int vertres = pDC->GetDeviceCaps(VERTRES);
	float ratio = (float)horzres / (float)x;
	*/
	float ratio = 1.f;
	
	CGContextDrawImage(context, CGRectMake(instPrint->x * ratio, 
														instPrint->y * ratio, 
														iWidth * instPrint->z / 100.f, 
														iHeight * instPrint->z / 100.f), image);
	
	return noErr;
}	//	DrawPage

/*------------------------------------------------------------------------------
    Function:	PostPrintingErrors
	
    Parameters:
        status	-	error code
	
    Description:
        This is where we could post an alert to report any problem reported
        by the Printing Manager.
		
------------------------------------------------------------------------------*/
void 	PostPrintingErrors(OSStatus status)
{
#pragma unused (status)	
}	//	PostPrintingErrors


