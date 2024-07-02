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
#pragma once

#include "propsht.h"
#include "PsWinPropertiesCx.h"

#include "PsTabInfoMatrix.h"
#include "PsTabInfoImage.h"
#include "PsTabInfoPattern.h"

class PsWinProperties : public CAllControlsSheet, public PsWinPropertiesCx
{
public:
	static PsWinProperties& Instance();
	static void Delete();
	~PsWinProperties() {}

private:
	PsWinProperties();

private:
	static PsWinProperties* instance; // Singleton

public:
	void FocusImageInformation();
	void FocusMatrixInformation();
	void UpdateContentSize();

protected:
	virtual void UpdateInformation(PsProject*);
	virtual void UpdateMatrixInformation(PsMatrix*);
	virtual void UpdateImageInformation(PsImage*);
	virtual void UpdatePatternInformation();

protected:
	void AddControlPages(void);

protected:
	PsTabInfoMatrix m_matrixProperties;
	PsTabInfoImage m_imageProperties;
	PsTabInfoPattern m_PatternProperties;
};

extern bool OPTION_REFLECT_ALWAYS;
