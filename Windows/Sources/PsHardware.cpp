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

#include "PsWin.h"
#include "PsTypes.h"
#include "PsHardware.h"

PsHardware hardwareRenderer;
uint8 pixels[3 * 5000 * 5000];

// these are used to construct an equilibrated 256 color palette
static uint8 _threeto8[8] = 
{
	0, 0111>>1, 0222>>1, 0333>>1, 0444>>1, 0555>>1, 0666>>1, 0377
};

static uint8 _twoto8[4] = 
{
	0, 0x55, 0xaa, 0xff
};

static uint8 _oneto8[2] = 
{
	0, 255
};

static int defaultOverride[13] = 
{
	0, 3, 24, 27, 64, 67, 88, 173, 181, 236, 247, 164, 91
};

// Windows Default Palette
static PALETTEENTRY defaultPalEntry[20] = 
{
	{ 0,   0,   0,    0 },
	{ 0x80,0,   0,    0 },
	{ 0,   0x80,0,    0 },
	{ 0x80,0x80,0,    0 },
	{ 0,   0,   0x80, 0 },
	{ 0x80,0,   0x80, 0 },
	{ 0,   0x80,0x80, 0 },
	{ 0xC0,0xC0,0xC0, 0 },

	{ 192, 220, 192,  0 },
	{ 166, 202, 240,  0 },
	{ 255, 251, 240,  0 },
	{ 160, 160, 164,  0 },

	{ 0x80,0x80,0x80, 0 },
	{ 0xFF,0,   0,    0 },
	{ 0,   0xFF,0,    0 },
	{ 0xFF,0xFF,0,    0 },
	{ 0,   0,   0xFF, 0 },
	{ 0xFF,0,   0xFF, 0 },
	{ 0,   0xFF,0xFF, 0 },
	{ 0xFF,0xFF,0xFF, 0 }
};

enum ColorsNumber{INDEXED,THOUSANDS,MILLIONS,MILLIONS_WITH_TRANSPARENCY};
enum ZAccuracy{NORMAL,ACCURATE};

PsHardware::PsHardware()
{
  m_pCDC = NULL;
  m_bIsLoad = false;
}

PsHardware::~PsHardware()
{
  if (m_pCDC)
    delete m_pCDC;
}

void PsHardware::SafeCreatePixelBuffer()
{
  if (!m_bIsLoad)
  {  
    if (!CreatePixelBuffer())
    {
      AfxMessageBox("CreatePixelBuffer failed. Upgrade your video card.");
      _exit(1);
    }
  }
}
/*
HardwareBuffer *PsHardware::GetHardBuffer(int iWidth, int iHeight)
{
  if (!m_bIsLoad)
    SafeCreatePixelBuffer();
  return NULL;
}

SoftwareBuffer *PsHardware::GetSoftBuffer(int iWidth, int iHeight)
{
   if (!m_bIsLoad)
    SafeCreatePixelBuffer();
  return NULL;
}
*/

void PsHardware::CreatePalette()
{
	PIXELFORMATDESCRIPTOR pfd;
	LOGPALETTE *pPal;
	int n, i;

	// get the initially choosen video mode
	n = ::GetPixelFormat(m_pCDC->GetSafeHdc());
	::DescribePixelFormat(m_pCDC->GetSafeHdc(), n, sizeof(pfd), &pfd);

	// if is an indexed one...
	if (pfd.dwFlags & PFD_NEED_PALETTE)
	{
		// ... construct an equilibrated palette (3 red bits, 3 green bits, 2 blue bits)
		// NOTE: this code has been taken from MFC example Cube
		n = 1 << pfd.cColorBits;
		pPal = (PLOGPALETTE) new char[sizeof(LOGPALETTE) + n * sizeof(PALETTEENTRY)];

		ASSERT(pPal != NULL);

		pPal->palVersion = 0x300;
		pPal->palNumEntries = unsigned short(n);
		for (i=0; i<n; i++)
		{
			pPal->palPalEntry[i].peRed=ComponentFromIndex(i, pfd.cRedBits, pfd.cRedShift);
			pPal->palPalEntry[i].peGreen=ComponentFromIndex(i, pfd.cGreenBits, pfd.cGreenShift);
			pPal->palPalEntry[i].peBlue=ComponentFromIndex(i, pfd.cBlueBits, pfd.cBlueShift);
			pPal->palPalEntry[i].peFlags=0;
		}

		// fix up the palette to include the default Windows palette
		if ((pfd.cColorBits == 8)                           &&
			(pfd.cRedBits   == 3) && (pfd.cRedShift   == 0) &&
			(pfd.cGreenBits == 3) && (pfd.cGreenShift == 3) &&
			(pfd.cBlueBits  == 2) && (pfd.cBlueShift  == 6)
			)
		{
			for (i = 1 ; i <= 12 ; i++)
				pPal->palPalEntry[defaultOverride[i]] = defaultPalEntry[i];
		}

		m_CurrentPalette.CreatePalette(pPal);
		delete pPal;

		// set the palette
		m_pOldPalette=m_pCDC->SelectPalette(&m_CurrentPalette, FALSE);
		m_pCDC->RealizePalette();
	}
}

bool PsHardware::SetupPixelFormat()
{
	// define a default desired video mode (pixel format)
	static PIXELFORMATDESCRIPTOR pfd = 
	{
		sizeof(PIXELFORMATDESCRIPTOR),	// size of this pfd
		1,								// version number
		PFD_DRAW_TO_WINDOW |			// support window
		PFD_SUPPORT_OPENGL,				// double buffered
		PFD_TYPE_RGBA,                  // RGBA type
		32,                             // 32-bit color depth
		0, 0, 0, 0, 0, 0,               // color bits ignored
		0,                              // no alpha buffer
		0,                              // shift bit ignored
		0,                              // no accumulation buffer
		0, 0, 0, 0,                     // accum bits ignored
		16,                             // 16-bit z-buffer
		0,                              // no stencil buffer
		0,                              // no auxiliary buffer
		PFD_MAIN_PLANE,                 // main layer
		0,                              // reserved
		0, 0, 0                         // layer masks ignored
	};

	ASSERT(m_pCDC != NULL);
	int pixelformat;
	if ( (pixelformat = ChoosePixelFormat(m_pCDC->GetSafeHdc(), &pfd)) == 0 )
	{
		AfxMessageBox("ChoosePixelFormat failed");
		return FALSE;
	}
	// try to set this video mode    
	if (SetPixelFormat(m_pCDC->GetSafeHdc(), pixelformat, &pfd) == FALSE)
	{
		// the requested video mode is not available so get a default one (the first)
		pixelformat = 1;	
		if (DescribePixelFormat(m_pCDC->GetSafeHdc(), pixelformat, sizeof(PIXELFORMATDESCRIPTOR), &pfd)==0)
		{
			// neither the requested nor the default are available: fail
			AfxMessageBox("SetPixelFormat failed (no OpenGL compatible video mode)");
			return FALSE;
		}
	}
	return TRUE;
}


bool PsHardware::CreateContext()
{
	// OpenGL rendering context creation
	PIXELFORMATDESCRIPTOR pfd;
	int n;

  // initialize the private member
  CWinApp* pApp = AfxGetApp();
  m_pCDC = new CClientDC(pApp->GetMainWnd());
	
	// choose the requested video mode
	if (!SetupPixelFormat()) 
  {
    AfxMessageBox("SetPixelFormat failed (no OpenGL compatible video mode)");
    return false;
  }
	// ask the system if the video mode is supported
	n = ::GetPixelFormat(m_pCDC->GetSafeHdc());
	::DescribePixelFormat(m_pCDC->GetSafeHdc(), n, sizeof(pfd), &pfd);

	// create a palette if the requested video mode has 256 colors (indexed mode)
	CreatePalette();

	// link the Win Device Context with the OGL Rendering Context
  m_hRC = wglCreateContext(m_pCDC->GetSafeHdc());
	if (!m_hRC)
  {
    AfxMessageBox("wglCreateContext failed (no OpenGL compatible video mode)");
    return false;
  }

  wglMakeCurrent (m_pCDC->GetSafeHdc(), m_hRC);

  return true;
}

bool PsHardware::CreatePixelBuffer()
{

  if (!CreateContext())
  {
    AfxMessageBox("CreateContext failed (no OpenGL compatible video mode)");
    return false;
  }

  HDC hDC = m_pCDC->GetSafeHdc();

  m_iBufferWidth = GetSystemMetrics(SM_CXSCREEN);
	m_iBufferHeight = GetSystemMetrics(SM_CYSCREEN);

	wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
	char *ext = NULL;
	if( wglGetExtensionsStringARB )
		ext = (char*)wglGetExtensionsStringARB( wglGetCurrentDC() );
	else 
    return false;

	if( strstr( ext, "WGL_ARB_pbuffer" ) == NULL ) 
    return false;
	else
	{
		wglCreatePbufferARB    = (PFNWGLCREATEPBUFFERARBPROC)wglGetProcAddress("wglCreatePbufferARB");
		wglGetPbufferDCARB     = (PFNWGLGETPBUFFERDCARBPROC)wglGetProcAddress("wglGetPbufferDCARB");
		wglReleasePbufferDCARB = (PFNWGLRELEASEPBUFFERDCARBPROC)wglGetProcAddress("wglReleasePbufferDCARB");
		wglDestroyPbufferARB   = (PFNWGLDESTROYPBUFFERARBPROC)wglGetProcAddress("wglDestroyPbufferARB");
		wglQueryPbufferARB     = (PFNWGLQUERYPBUFFERARBPROC)wglGetProcAddress("wglQueryPbufferARB");

		if( !wglCreatePbufferARB || !wglGetPbufferDCARB || !wglReleasePbufferDCARB ||
			!wglDestroyPbufferARB || !wglQueryPbufferARB ) 
      return false;
	}

	if( strstr( ext, "WGL_ARB_pixel_format" ) == NULL ) 
    return false;
	else
	{
		wglGetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)wglGetProcAddress("wglGetPixelFormatAttribivARB");
		wglGetPixelFormatAttribfvARB = (PFNWGLGETPIXELFORMATATTRIBFVARBPROC)wglGetProcAddress("wglGetPixelFormatAttribfvARB");
		wglChoosePixelFormatARB      = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");

		if( !wglGetExtensionsStringARB || !wglCreatePbufferARB || !wglGetPbufferDCARB ) 
      return false;
	}

	//-------------------------------------------------------------------------
	// Create a p-buffer for off-screen rendering.
	//-------------------------------------------------------------------------

	m_pbuffer.hPBuffer = NULL;
	m_pbuffer.nWidth   = m_iBufferWidth;
	m_pbuffer.nHeight  = m_iBufferHeight;
	int pf_attr[] =
	{
		WGL_SUPPORT_OPENGL_ARB, TRUE,       // P-buffer will be used with OpenGL
		WGL_DRAW_TO_PBUFFER_ARB, TRUE,      // Enable render to p-buffer
		WGL_RED_BITS_ARB, 8,                // At least 8 bits for RED channel
		WGL_GREEN_BITS_ARB, 8,              // At least 8 bits for GREEN channel
		WGL_BLUE_BITS_ARB, 8,               // At least 8 bits for BLUE channel
		WGL_ALPHA_BITS_ARB, 8,              // At least 8 bits for ALPHA channel
		WGL_DEPTH_BITS_ARB, 16,             // At least 16 bits for depth buffer
		WGL_DOUBLE_BUFFER_ARB, FALSE,       // We don't require double buffering
		0                                   // Zero terminates the list
	};
	uint32 count = 0;
	int pixelFormat;
	wglChoosePixelFormatARB( hDC,(const int*)pf_attr, NULL, 1, &pixelFormat, &count);

	if( count == 0 ) 
    return false;

	m_pbuffer.hPBuffer = wglCreatePbufferARB( hDC, pixelFormat, m_pbuffer.nWidth, m_pbuffer.nHeight, NULL );
	m_pbuffer.hDC      = wglGetPbufferDCARB( m_pbuffer.hPBuffer );
	m_pbuffer.hRC      = wglCreateContext( m_pbuffer.hDC );

	if( !m_pbuffer.hPBuffer ) 
    return false;

	if( !wglMakeCurrent( m_pbuffer.hDC, m_pbuffer.hRC ) )  
    return false;

  glEnable(GL_DEPTH_TEST);
	glClearColor(0.f,0.f,0.f,1.0f );
	glClearDepth(1.0f);
	glShadeModel(GL_FLAT);

	m_bIsLoad = true;

	return true;
}

uint8 PsHardware::ComponentFromIndex(int i, UINT nbits, UINT shift)
{
	uint8 val;

	val = (uint8) (i >> shift);
	switch (nbits) 
	{

	case 1:
		val &= 0x1;
		return _oneto8[val];
	case 2:
		val &= 0x3;
		return _twoto8[val];
	case 3:
		val &= 0x7;
		return _threeto8[val];

	default:
		return 0;
	}
}

void PsHardware::CopyToSoftBuffer(SoftwareBuffer &softwareBuffer)
{
  CImage &buffer = softwareBuffer.buffer; 
	SwapBuffers(NULL);
	int NbBytes = 3 * buffer.GetWidth() * buffer.GetHeight(); 
	glReadPixels(0, 0, buffer.GetWidth(), buffer.GetHeight(), GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
	BITMAPINFO bmi;
	memset(&bmi, 0, sizeof(bmi));
	BITMAPINFOHEADER header; 
	header.biWidth = buffer.GetWidth(); 
	header.biHeight = buffer.GetHeight(); 
	header.biSizeImage = NbBytes; 
	header.biSize = 40; 
	header.biPlanes = 1; 
	header.biBitCount = 3 * 8; // RGB 
	header.biCompression = 0; 
	header.biXPelsPerMeter = 0; 
	header.biYPelsPerMeter = 0; 
	header.biClrUsed = 0; 
	header.biClrImportant = 0; 
	bmi.bmiHeader = header;
	SetDIBitsToDevice(buffer.GetDC(), 0, 0, buffer.GetWidth(), buffer.GetHeight(), 0, 0, 0, buffer.GetHeight(), pixels, &bmi, DIB_RGB_COLORS);
	buffer.ReleaseDC();
}

void PsHardware::CopyToHardBuffer(GLuint &id, int iWidth, int iHeight, bool bCreate)
{
	glEnable(GL_TEXTURE_2D); 
  if (bCreate)
  {
    glGenTextures (1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, iWidth, iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  }
  else
  {
    glBindTexture(GL_TEXTURE_2D, id);
  }
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, iWidth, iHeight);
	glDisable(GL_TEXTURE_2D);
} 

bool PsHardware::IsLoad() 
{ 
  return m_bIsLoad; 
}

void SoftwareBuffer::Create(int iWidth, int iHeight, int iBpp)
{
  buffer.Create(iWidth, iHeight, iBpp);
}

void SoftwareBuffer::Destroy()
{
  buffer.Destroy();
}

void SoftwareBuffer::BitBlt(PsWin &Dest, int x, int y)
{
  Dest.DrawSoftwareBuffer(*this, x, y);
 // FIXME!:  buffer.BitBlt(*(Dest.dc), x, y);
}

int SoftwareBuffer::GetWidth()
{
  return buffer.GetWidth();
}

int SoftwareBuffer::GetHeight()
{
  return buffer.GetHeight();
}
