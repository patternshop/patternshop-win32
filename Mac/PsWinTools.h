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

#ifndef PS_WIN_TOOLS_H__

#define PS_WIN_TOOLS_H__

class PsWinTools
{
private:
	PsWinTools();
	~PsWinTools();

public:
	static PsWinTools& Instance();
	static void Delete();

public:
	void Show();

private:
	static PsWinTools* instance; // Singleton
	
public:
	void GetLocation(PsPoint&);
	void SetLocation(int, int);
	
public:
	WindowRef window;
	HIViewRef myHIView;
	Rect bounds;
	
public:
	CGImageRef ImageModifyUp;
	CGImageRef ImageModifyDown;
	CGImageRef ImageMagnifyUp;
	CGImageRef ImageMagnifyDown;
	CGImageRef ImageScrollUp;
	CGImageRef ImageScrollDown;
	int currentTool;
};

#endif /* PS_WIN_TOOLS_H__ */
