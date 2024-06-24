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
#include "PsHardware.h"
#include "PsMessage.h"
#include "PsMacTools.h"
#include "PsWin.h"

PsHardware hardwareRenderer;

PsHardware::PsHardware()
{
  m_bIsLoad = false;
  m_bHardwareMutex = true;
}

PsHardware::~PsHardware()
{
}

void PsHardware::SafeCreatePixelBuffer()
{
  if (!m_bIsLoad)
  {  
    if (!CreatePixelBuffer())
    {
		 GetError(ERROR_VIDEOCARD);
		 exit(1);
    }
  }
}

bool aglReportError(void)
{
	GLenum err = aglGetError();
	if (AGL_NO_ERROR != err) {
		return false;
	}
	return true;
}

bool PsHardware::CreatePixelBuffer()
{
	GLint attrib[] = { AGL_RGBA, AGL_DEPTH_SIZE, 16, AGL_NONE };
	//	GLint attrib[] = { AGL_RGBA, AGL_RED_SIZE, 8, AGL_ALPHA_SIZE, 8, AGL_DEPTH_SIZE, 24, AGL_CLOSEST_POLICY, AGL_NONE };
	//	GLint attrib[] = { AGL_RGBA, AGL_RED_SIZE, 5, AGL_CLOSEST_POLICY, AGL_DEPTH_SIZE, 16, AGL_NONE };
	
	// build context
	aglPixFmt = aglChoosePixelFormat (NULL, 0, attrib);
	if (!aglReportError ())
		return false;

	if (aglPixFmt) 
	{
		aglContext = aglCreateContext (aglPixFmt, NULL);
		if (!aglReportError ())
			return false;
	}

	assert(aglContext);
	
	if (aglContext)
	{
		size_t iWidth = CGDisplayPixelsWide(CGMainDisplayID()),
		       iHeight = CGDisplayPixelsHigh(CGMainDisplayID());
		//printf("%d %d\n", iWidth, iHeight); 
		if (!aglCreatePBuffer(iWidth, iHeight, GL_TEXTURE_RECTANGLE_EXT, GL_RGBA, 0, &aglPbuffer))
		{
			if (!aglReportError ())
				return false;
		}
	}
	
	if (!aglSetCurrentContext (aglContext))
		return false;
	
	if (!aglSetPBuffer (aglContext, aglPbuffer, 0, 0, 0))
		return false;
	
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.f,0.f,0.f,1.0f );
	glClearDepth(1.0f);
	glShadeModel(GL_FLAT);
	
	return true;
}
							
void PsHardware::CopyToSoftBuffer(SoftwareBuffer &buffer, bool bAlpha)
{
	int iBpp = 32;
	int iWidth = buffer.GetWidth();
	int iHeight = buffer.GetHeight();
	assert(iWidth > 0 && iHeight > 0);
	int NbBytes = 4 * iWidth * iHeight; 
	int NbBytesWidth = iWidth * iBpp / 8;
	uint8 *bits = new uint8[NbBytes * sizeof(uint8)];
	glReadPixels(0, 0, iWidth, iHeight, GL_BGRA_EXT, GL_UNSIGNED_BYTE, bits);
	
	if (buffer.buffer)
		CGImageRelease(buffer.buffer);
	
	uint8 *fixedbits = new uint8[NbBytes * sizeof(uint8)];
	for (int i = 0; i < NbBytes; ++i)
		fixedbits[i] = bits[NbBytes - i - 1];
	uint8 *dst = 0, *cur = bits;
	uint8 *src = fixedbits;
	for (int i = 0; i < iHeight; ++i)
	{
		dst = cur + NbBytesWidth - 4;
		for (int j = 0; j < iWidth; ++j)
		{
			memcpy(dst, src, 4 * sizeof(uint8));
			dst -= 4;
			src += 4;
		}
		cur += NbBytesWidth;
	}
	delete fixedbits;
	
	CGDataProviderDirectAccessCallbacks gProviderCallbacks = { MyProviderGetBytePointer, NULL, NULL, NULL };
	CGDataProviderRef provider = CGDataProviderCreateDirectAccess(bits, NbBytes, &gProviderCallbacks);
	buffer.buffer = CGImageCreate(iWidth, iHeight, 8, iBpp, NbBytesWidth, CGColorSpaceCreateDeviceRGB(),
								  bAlpha ? kCGImageAlphaFirst : kCGImageAlphaNoneSkipFirst, provider, NULL, 0, kCGRenderingIntentDefault);
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
	
SoftwareBuffer::SoftwareBuffer()
{
	buffer = NULL;
	context = NULL;
	bits = NULL;
}

SoftwareBuffer::~SoftwareBuffer()
{
	Destroy();
}

void SoftwareBuffer::Create(int iWidth, int iHeight, int iBpp)
{
	iBpp = 32; // FIXME
	
	assert(iWidth > 0 && iHeight > 0);
	
	//-- data
	bits = new uint8[iWidth * iHeight * iBpp / 8];
	memset(bits, 255, iWidth * iHeight * iBpp / 8);
	
	//-- CGImage
	CGDataProviderDirectAccessCallbacks gProviderCallbacks = { MyProviderGetBytePointer, NULL, NULL, NULL };
	CGDataProviderRef provider = CGDataProviderCreateDirectAccess(bits, iWidth * iHeight * iBpp / 8, &gProviderCallbacks);
	buffer = CGImageCreate(iWidth, iHeight, 8, iBpp, iWidth * iBpp / 8, CGColorSpaceCreateDeviceRGB(),
									kCGImageAlphaNoneSkipFirst, provider, NULL, 0, kCGRenderingIntentDefault);
	assert(buffer);
	//--
	
	//-- CGContext
	context = CGBitmapContextCreate(bits, iWidth, iHeight, 8, iWidth * iBpp / 8, 
											  CGColorSpaceCreateDeviceRGB(), kCGImageAlphaNoneSkipFirst);
	assert(context);
	//--
	
	CGContextSetShouldAntialias(context, false);
	
}

void SoftwareBuffer::Destroy()
{
	if (bits)
		delete[] bits;
	if (buffer)
		CGImageRelease(buffer);
	if (context)
		CGContextRelease(context);
}

void SoftwareBuffer::BitBlt(PsWin &w, int x, int y)
{ 
	w.DrawSoftwareBuffer(*this, x, y); 
}




