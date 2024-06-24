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

#include "PsDlgColor.h"

PsDlgColor::PsDlgColor() 
{
	// not much here...
	info.theColor.profile = 0L;
	info.dstProfile = 0L;
	info.flags = 0L;
	info.placeWhere = kCenterOnMainScreen;
	info.pickerType = 0L;
	info.eventProc = NULL;
	info.colorProc = NULL;
	info.colorProcData = 0L;

	SetColor(255, 255, 255);
}

PsDlgColor::~PsDlgColor() 
{
}

bool PsDlgColor::ShowModal() 
{ 
	if (PickColor(&info) != noErr)
		return false;
	return true; 
}

void PsDlgColor::SetColor(int r, int g, int b) 
{
	// pass in existing color
	info.theColor.color.rgb.red = r * 65535 / 255;
	info.theColor.color.rgb.green = g * 65535 / 255;
	info.theColor.color.rgb.blue = b * 65535 / 255;
}

int PsDlgColor::GetColorRValue() { return info.theColor.color.rgb.red * 255 / 65535; }
int PsDlgColor::GetColorGValue() { return info.theColor.color.rgb.green * 255 / 65535; }
int PsDlgColor::GetColorBValue() { return info.theColor.color.rgb.blue * 255 / 65535; }
