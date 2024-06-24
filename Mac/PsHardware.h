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
#ifndef GL_OFFSCREEN_RENDERER_H__

#define GL_OFFSCREEN_RENDERER_H__

#include "PsTypes.h"

#include <AGL/agl.h>
#include <AGL/aglRenderers.h>
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <OpenGL/glu.h>
#include <Carbon/Carbon.h>

class PsWin;

struct SoftwareBuffer
{
	SoftwareBuffer();
	virtual ~SoftwareBuffer();
	
	void Create(int,int,int);
	void Destroy();
	
	void BitBlt(PsWin &w, int x, int y);
	
	inline int GetWidth() { return CGImageGetWidth(buffer); }
	inline int GetHeight() { return CGImageGetHeight(buffer); }
	inline bool IsNull() { return buffer == NULL; }
	
	CGImageRef buffer;
	CGContextRef context;
	uint8 *bits;
};

struct HardwareBuffer
{
	GLuint texture;
};

class PsHardware
{
public:	
	PsHardware();
	virtual ~PsHardware();
	
public:
	void SafeCreatePixelBuffer();
	void CopyToSoftBuffer(SoftwareBuffer&, bool bAlpha = false);
	void CopyToHardBuffer(GLuint&, int, int, bool bCreate = true);
	
	inline bool IsLoad() { return m_bIsLoad; }
	inline void ReleaseHardwareMutex() { m_bHardwareMutex = true; }
	
	inline bool ReserveHardwareMutex()
	{
		if (m_bHardwareMutex)
		{
			m_bHardwareMutex = false;
			return true;
		}
		return false;
	}

private:
	bool CreatePixelBuffer();
	
private:
	bool m_bIsLoad;
	bool m_bHardwareMutex; // FIXME: mettre un vrai mutex
	
public:
	AGLPixelFormat aglPixFmt; 
	AGLContext aglContext;
	AGLPbuffer aglPbuffer;
	AGLContext gShareContext;
};

extern PsHardware hardwareRenderer;

#endif /* GL_OFFSCREEN_RENDERER_H__ */