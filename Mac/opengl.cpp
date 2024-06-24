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
#include "PsMacView.h"
#include "PsMacTools.h"

#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

EventLoopTimerRef gTimer = NULL;

short gBitsPerPixel = 32;
short gContextWidth = 1200;
short gContextHeight = 1400;

Rect gRectPort = {0, 0, 0, 0};

// --------------------------------------------------------------------------

void ReportError (char * strError)
{
	PsMessageBox(strError);
}

//-----------------------------------------------------------------------------------------------------------------------

// if error dump agl errors to debugger string, return error

GLenum aglDebugStr (void)
{
	GLenum err = aglGetError();
	if (AGL_NO_ERROR != err)
		ReportError ((char *)aglErrorString(err));
	return err;
}

//-----------------------------------------------------------------------------------------------------------------------

// OpenGL Drawing
void DrawGL (Rect * pRectPort)
{
	GLint width = pRectPort->right - pRectPort->left;
	GLint height = pRectPort->bottom - pRectPort->top;
	
	if (!active)
		return;
	
	if (active->gaglContext == 0)
		return;
	
	aglSetCurrentContext (active->gaglContext); // ensure our context is current prior to drawing
	aglUpdateContext (active->gaglContext); // test not normally needed
	
	if (!active->project)
		return;
	
	active->project->RenderToScreen();
	aglSwapBuffers(active->gaglContext); // send swap command
}

// --------------------------------------------------------------------------

pascal void IdleTimer (EventLoopTimerRef inTimer, void* userData)
{
	#pragma unused (inTimer, userData)
	DrawGL (&gRectPort);
}

// --------------------------------------------------------------------------

EventLoopTimerUPP GetTimerUPP (void)
{
	static EventLoopTimerUPP	sTimerUPP = NULL;
	
	if (sTimerUPP == NULL)
		sTimerUPP = NewEventLoopTimerUPP (IdleTimer);
	
	return sTimerUPP;
}

// --------------------------------------------------------------------------

OSStatus MySetWindowAsDrawableObject()
{
	OSStatus err = noErr;
	GLint attributes[] =  { AGL_RGBA,
		AGL_DOUBLEBUFFER,
		AGL_DEPTH_SIZE, 24,
		AGL_NONE };
	AGLPixelFormat myAGLPixelFormat;
	
	myAGLPixelFormat = aglChoosePixelFormat (NULL, 0, attributes);
	err = aglDebugStr ();
	if (myAGLPixelFormat) {
		active->gaglContext = aglCreateContext (myAGLPixelFormat, NULL);
		err = aglDebugStr ();
	}
	if (! aglSetDrawable (active->gaglContext, GetWindowPort (active->window)))
		err = aglDebugStr ();
	if (!aglSetCurrentContext (active->gaglContext))
		err = aglDebugStr ();
	return err;
}


// --------------------------------------------------------------------------

Boolean SetUp (void)
{
	active->gaglContext = 0;
	MySetWindowAsDrawableObject();
	if (active->gaglContext)
	{
		InstallEventLoopTimer (GetCurrentEventLoop(), 0, 0.0005, GetTimerUPP (), 0, &gTimer);
	}	
	if (active->gaglContext) 
		return true;
	return false;
}

// --------------------------------------------------------------------------

void CleanupAGL(AGLContext ctx)
{
	aglSetDrawable (ctx, NULL);
	aglSetCurrentContext (NULL);
	aglDestroyContext (ctx);
}

void CleanUp (void)
{
	RemoveEventLoopTimer(gTimer);
	gTimer = NULL;
	if (active->gaglContext)
		CleanupAGL (active->gaglContext); // Cleanup the OpenGL context
	active->gaglContext = 0;
	RemoveEventLoopTimer(gTimer);
}
