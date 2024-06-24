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
#include "PsMac.h"
#include "PsMacTools.h"

PsMac::PsMac() 
{
}

PsMac::~PsMac() 
{
}

void PsMac::SetDashBlackPen() 
{
	const float lengths[] = { 4, 1 };
	CGContextSetRGBStrokeColor(dc, 0.f, 0.f, 0.f, 1);
	CGContextSetLineDash(dc, 0, lengths, 2);
}

void PsMac::SetPenColor(int r, int g, int b) 
{
	CGContextSetRGBStrokeColor( dc, r / 255.f, g / 255.f, b / 255.f, 1 );
	CGContextSetLineDash ( dc, 0, 0, 0 );
}

void PsMac::SetBrushColor(int r, int g, int b) 
{
	CGContextSetRGBFillColor( dc, r / 255.f, g / 255.f, b / 255.f, 1 );
}

void PsMac::DrawRectangle(int x1, int y1, int x2, int y2)
{
	HIRect lbounds;
	lbounds.origin.x = x1;
	lbounds.origin.y = iHeight - y2;
	lbounds.size.width = x2 - x1;
	lbounds.size.height = y2 - y1;
	CGContextFillRect(dc, lbounds);
	CGContextAddRect (dc, lbounds);
	CGContextStrokePath(dc);
}

void PsMac::MovePenTo(int x, int y) 
{
	if (iHeight < y) y = iHeight;	
	CGContextMoveToPoint(dc, x, iHeight - y);
}

void PsMac::DrawLineTo(int x, int y) 
{
	if (iHeight < y) y = iHeight;
	CGContextAddLineToPoint(dc, x, iHeight - y);
	CGContextStrokePath(dc);
	CGContextMoveToPoint(dc, x, iHeight - y);
}

void PsMac::DrawSoftwareBuffer(SoftwareBuffer &buffer, int x, int y) 
{
	HIRect lbounds;
	lbounds.origin.x = x;
	lbounds.origin.y = iHeight - y - buffer.GetHeight();
	lbounds.size.width = buffer.GetWidth();
	lbounds.size.height = buffer.GetHeight();
	assert(lbounds.size.width > 0 && lbounds.size.height > 0);
	CGContextDrawImage(dc, lbounds, buffer.buffer);
	//DrawCGImage(dc, iHeight, buffer.buffer, x, y);	
}

void PsMac::SetTextColor(int r, int g, int b) 
{
	CGContextSetRGBStrokeColor( dc, r / 255.f, g / 255.f, b / 255.f, 1 );
	CGContextSetRGBFillColor( dc, r / 255.f, g / 255.f, b / 255.f, 1 );
}

void PsMac::DrawText(const char *buffer, PsRect &rect, PsFont font) 
{
	CGContextSelectFont(dc, "Monaco", 10, kCGEncodingMacRoman);
	CGContextSetTextDrawingMode(dc, kCGTextFill);
	CGContextSetTextPosition(dc, rect.left, iHeight - (rect.bottom + rect.top) / 2 - 5);
	CGContextShowText(dc, buffer, strlen(buffer));	
}

void PsMac::DrawPolygon(PsPoint *points, int c)
{
	MovePenTo(points[c - 1].x, points[c - 1].y);
	for (int i = 0; i < c; ++i)
		DrawLineTo(points[i].x, points[i].y);
}

void PsMac::SetTarget(SoftwareBuffer *sfb)
{
	if (sfb)
	{
		dc = sfb->context;
		assert(dc);
	}
}
