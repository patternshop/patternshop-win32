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
#include "PsProject.h"
#include "PsWinImage.h"

static OSStatus OnDraw(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsWinImage *View = (PsWinImage*)userData;
	OSStatus status = noErr;
	CGContextRef myContext;
	
	status = GetEventParameter (event, kEventParamCGContextRef, typeCGContextRef, NULL,
										 sizeof (CGContextRef), NULL,  &myContext);
	require_noerr(status, CantGetGraphicsContext);
		
	View->OnDraw(myContext);
	
CantGetGraphicsContext:
		return status;
}

void PsWinImage::OnDraw(CGContextRef myContext)
{
	HIRect bounds;
	OSStatus status = noErr;
	
	HIViewGetBounds(myHIView, &bounds);
	require_noerr(status, CantGetBoundingRectangle);
	
	if(image)
		DrawCGImage(myContext, bounds.size.height, image, 0, 0);

CantGetBoundingRectangle:
	;
}

static OSStatus OnClose(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	((PsWinImage*)userData)->OnClose();
	delete ((PsWinImage*)userData);
	return eventNotHandledErr;
}

PsWinImage::PsWinImage()
{
	OSStatus err = noErr;
	static const EventTypeSpec  myHIViewSpec[] = { kEventClassControl, kEventControlDraw };
	static const EventTypeSpec  myWindowSpec[] = { kEventClassWindow, kEventWindowClose };
	static const HIViewID  myHIViewID = { 'view', 0 };
	
	//-- création de la fenetre
	err = CreateWindowFromNib(sNibRef, CFSTR("PsWinImage"), &window);
   require_noerr( err, CantCreateWindow );
	
	//-- handle sur la fermeture
	InstallWindowEventHandler(window, NewEventHandlerUPP(::OnClose),
									  GetEventTypeCount(myWindowSpec), myWindowSpec, (void *) this, NULL);

	//-- handle sur l'affichage
	HIViewFindByID (HIViewGetRoot(window), myHIViewID, &myHIView);
	err = InstallEventHandler (GetControlEventTarget (myHIView), NewEventHandlerUPP (::OnDraw),
										GetEventTypeCount (myHIViewSpec), myHIViewSpec, (void *) this, NULL);

	//-- chargement des images
	image = NULL;
	
CantCreateWindow:
		;
}

PsWinImage::~PsWinImage()
{
	if (image)
		CGImageRelease(image);
}

void PsWinImage::Show()
{
	PsProject *project = PsController::Instance().project;
	assert(project);
	PsImage *pimage = project->image;
	assert(pimage);
	
	if (image)
		CGImageRelease(image);
	
	// transormation de l'image dans le format demandé
	int iWidth, iHeight, iBpp;
	pimage->GetTexture().GetSize(iWidth, iHeight);	
	uint8 *bits = pimage->GetTexture().GetBufferUncompressed(iBpp);
	
	int NbBytes = iWidth * iHeight * iBpp;
	int NbBytesWidth = iWidth * iBpp;
	uint8 *fixedbits = new uint8[NbBytes * sizeof(uint8)];
	for (int i = 0; i < NbBytes; ++i)
		fixedbits[i] = bits[NbBytes - i - 1];
	uint8 *dst = 0, *cur = bits;
	uint8 *src = fixedbits;
	for (int i = 0; i < iHeight; ++i)
	{
		dst = cur + NbBytesWidth - iBpp;
		for (int j = 0; j < iWidth; ++j)
		{
			if (src[0] > 127)
			{
				dst[0] = src[0];
				dst[1] = src[3];
				dst[2] = src[2];
				dst[3] = src[1];
			}
			else
			{
				memset(dst, 0xff, 4);
			}
			dst -= iBpp;
			src += iBpp;
		}
		cur += NbBytesWidth;
	}
	delete fixedbits;
	
	CGDataProviderDirectAccessCallbacks gProviderCallbacks = { MyProviderGetBytePointer, NULL, NULL, NULL };
	CGDataProviderRef provider = CGDataProviderCreateDirectAccess(bits, iWidth * iHeight * iBpp, &gProviderCallbacks);
	image = CGImageCreate(iWidth, iHeight, 8, iBpp * 8, iWidth * iBpp, CGColorSpaceCreateDeviceRGB(),
								  kCGImageAlphaNoneSkipFirst, provider, NULL, 0, kCGRenderingIntentDefault);
	assert(image);
	//--
	
	Rect bound;
	GetWindowBounds(window, kWindowStructureRgn, &bound);
	bound.right = bound.left + iWidth;
	bound.bottom = bound.top + iHeight;
	SetWindowBounds(window, kWindowContentRgn, &bound);
	
	ShowWindow(window);
}

void PsWinImage::OnClose()
{
	DisposeWindow(window);
}



