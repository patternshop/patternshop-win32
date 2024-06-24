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
#ifndef PS_DLG_INFO_LIGHT_H__

#define PS_DLG_INFO_LIGHT_H__

class PsDlgInfoLight
{
public:
	PsDlgInfoLight();
	~PsDlgInfoLight();

public:
	bool ShowModal() { return false; }

public:
	static float p, e, r;
};

#endif /* PS_DLG_INFO_LIGHT_H__ */