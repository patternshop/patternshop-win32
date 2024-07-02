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

#include "stdafx.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "resource.h"
#include <atlimage.h>
#include "wglext.h"  

#include "PsTypes.h"

class PsWin;

#ifndef PBUFFER_STRUCT_DEFINED
# define PBUFFER_STRUCT_DEFINED
typedef struct {

	HPBUFFERARB hPBuffer;
	HDC         hDC;
	HGLRC       hRC;
	int         nWidth;
	int         nHeight;

} PBUFFER;
#endif /* PBUFFER_STRUCT_DEFINED */

struct HardwareBuffer
{
	GLuint texture;
};

struct SoftwareBuffer
{
	void Create(int, int, int);
	void Destroy();
	void BitBlt(PsWin&, int, int);
	int GetWidth();
	int GetHeight();
	CImage buffer;
};

class PsHardware
{
public:
	PsHardware();
	virtual ~PsHardware();

public:
	void SafeCreatePixelBuffer();
	void CopyToSoftBuffer(SoftwareBuffer&);
	void CopyToHardBuffer(GLuint&, int, int, bool bCreate = true);
	bool IsLoad();

private:
	bool SetupPixelFormat();
	bool CreatePixelBuffer();

private:
	PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;
	PFNWGLCREATEPBUFFERARBPROC    wglCreatePbufferARB;
	PFNWGLGETPBUFFERDCARBPROC     wglGetPbufferDCARB;
	PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDCARB;
	PFNWGLDESTROYPBUFFERARBPROC   wglDestroyPbufferARB;
	PFNWGLQUERYPBUFFERARBPROC     wglQueryPbufferARB;
	PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB;
	PFNWGLGETPIXELFORMATATTRIBFVARBPROC wglGetPixelFormatAttribfvARB;
	PFNWGLCHOOSEPIXELFORMATARBPROC      wglChoosePixelFormatARB;
	bool m_bIsLoad;
	PBUFFER m_pbuffer;
	int m_iBufferWidth;
	int m_iBufferHeight;

private:
	bool CreateContext();
	void CreatePalette();
	CDC* GetCDC() { return m_pCDC; }
	uint8 ComponentFromIndex(int, UINT, UINT);

private:
	CDC* m_pCDC;
	HGLRC m_hRC;
	HCURSOR m_hMouseCursor;
	CPalette m_CurrentPalette;
	CPalette* m_pOldPalette;

};

extern PsHardware hardwareRenderer;

#endif /* GL_OFFSCREEN_RENDERER_H__ */