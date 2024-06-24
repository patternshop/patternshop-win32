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

#ifndef PS_MAC_TOOLS_H__

#define PS_MAC_TOOLS_H__

#include "PsHardware.h"
#include "FreeImage.h"

#include <assert.h>

typedef enum {
	PS_PROJECT_FILE,
	PS_MOTIF_FILE,
	PS_PATRON_FILE,
	PS_IMAGE_FILE
} PsFileKind;

bool PsOpenFileDialog(char *, PsFileKind);
bool PsSaveFileDialog(char *, PsFileKind);
void PsMessageBox(char *);
bool PsQuestionBox(char *);

CGImageRef LoadCGImageFromPng(const char *);
void DrawCGImage(CGContextRef, int, CGImageRef, int, int);
bool IsMouseInCGImage(CGImageRef, int, int, int, int);
const void *MyProviderGetBytePointer(void *);
void LoadSoftwareBufferFromPng(SoftwareBuffer&, const char *);

#endif /* PS_MAC_TOOLS_H__ */