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

#ifndef PSMULTIDOCTEMPLATE_H
#define PSMULTIDOCTEMPLATE_H

#include "Patternshop.h"
#include "MainFrm.h"
#include "ChildFrm.h"

class PsMultiDocTemplate : public CMultiDocTemplate
{
	// Constructors
public:
	PsMultiDocTemplate(UINT nIDResource,
		CRuntimeClass* pDocClass,
		CRuntimeClass* pFrameClass,
		CRuntimeClass* pViewClass);

public:
	void rebuildResources();
};

#endif /* PSMULTIDOCTEMPLATE_H */
