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

#ifndef PS_MAC_H__

#define PS_MAC_H__

#include "PsWin.h"
#include "PsHardware.h"

class PsMac : public PsWin
{	
public:
	PsMac();
   virtual ~PsMac();

public:
	CGContextRef dc;

public:
	void SetDashBlackPen();
	void SetPenColor(int, int, int);
	void SetBrushColor(int, int, int);
	void SetTextColor(int, int, int);
	void DrawRectangle(int, int, int, int);
	void MovePenTo(int, int);
	void DrawLineTo(int, int);
	void DrawSoftwareBuffer(SoftwareBuffer&, int, int);
	void DrawText(const char*, PsRect&, PsFont);
	void DrawPolygon(PsPoint*, int);
	void SetTarget(SoftwareBuffer*);
};

#endif /* PS_MAC_H__ */

