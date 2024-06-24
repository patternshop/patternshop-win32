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
#include "stdafx.h"
#include <atlimage.h>
#include "Patternshop.h"
#include "PatternshopView.h"
#include "PsWinProperties.h"
#include "PsWinImage.h"
#include "PsWinProject.h"

PsWinProperties* PsWinProperties::instance = 0;

PsWinPropertiesCx* dlgPropreties = 0;

PsWinProperties::PsWinProperties() : CAllControlsSheet()
{
   AddControlPages();
}

PsWinProperties&	PsWinProperties::Instance()
{
   if ( !instance )
   {
      instance = new PsWinProperties();
      dlgPropreties = instance;
   }
   return *instance;
}

void	PsWinProperties::Delete()
{
   if(instance)
   {
      delete instance;
      instance = 0;
      dlgPropreties = 0;
   }
}

void PsWinProperties::UpdateContentSize()
{
   CRect m_rectPage;
   GetTabControl()->GetWindowRect(m_rectPage);
   GetTabControl()->SetWindowPos(NULL, 0, 0,
      220,
      m_rectPage.Height(),
      SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
   SetActivePage(&m_matrixProperties);
}

void PsWinProperties::AddControlPages()
{
   m_psh.pszCaption = "Transformations";
   AddPage(&m_matrixProperties);
   AddPage(&m_imageProperties);
   AddPage(&m_PatternProperties);
} 

void PsWinProperties::UpdateInformation(PsProject* proj)
{
   if (proj)
   {
      UpdateMatrixInformation(proj->matrix);
      UpdateImageInformation(proj->image);
   }
   else
   {
      UpdateMatrixInformation(NULL);
      UpdateImageInformation(NULL);
   }
}

void PsWinProperties::FocusImageInformation()
{
   if (GetActivePage() != &m_imageProperties)
   {
      SetActivePage(&m_imageProperties);		
      PsWinImage::Instance().Invalidate(true);
   }
}

void PsWinProperties::FocusMatrixInformation()
{
   if (GetActivePage() != &m_matrixProperties)
   {
      SetActivePage(&m_matrixProperties);
   }
}

void PsWinProperties::UpdateMatrixInformation(PsMatrix* matrix)
{
   m_matrixProperties.Update(matrix);
   UpdatePatternInformation(); // FIXME
}

void PsWinProperties::UpdateImageInformation(PsImage* image)
{
   m_imageProperties.Update(image);
   UpdatePatternInformation(); // FIXME
}

void PsWinProperties::UpdatePatternInformation()
{
   m_PatternProperties.Update();
}
