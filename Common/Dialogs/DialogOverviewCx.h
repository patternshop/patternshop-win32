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
#ifndef DIALOG_OVERVIEW_CORE_H__

#define DIALOG_OVERVIEW_CORE_H__

#include "PsWin.h"
#include "PsHardware.h"
#include "PsRender.h"

class DialogOverviewCx
{
public:
	DialogOverviewCx();

public:
  inline void Update() { bUpdated = false; }

protected:
  PsRect GetSelectionRectangle(int, int);
 
public:
	void OnLeftMouseButtonDown(PsPoint);
	void OnLeftMouseButtonUp(PsPoint);

public:
  SoftwareBuffer m_RenduImage;
  SoftwareBuffer window_buffer, window_buffer2;
  float r_size_x, r_size_y, r_zoom;
  bool bDragging, zooming;
  int border_size; 
  bool bUpdated;
};

#endif /* DIALOG_OVERVIEW_CORE_H__ */
