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

#ifndef PS_DLG_CONFIG_H__

#define PS_DLG_CONFIG_H__

class PsDlgConfig
{
public:
	PsDlgConfig();

public:
	bool DoModal();
	void Update();
	void Close();
	
private:
	void OnOkButton();
	void OnCancelButton();
	void OnDefaultButton();

private:
	bool bResult;
	
public:
	OSStatus EventsHandler(EventHandlerCallRef, EventRef);
	
public:
	WindowRef window;
};

extern bool do_not_save_options;
extern int resolution_max;

#endif /* PS_DLG_CONFIG_H__ */
