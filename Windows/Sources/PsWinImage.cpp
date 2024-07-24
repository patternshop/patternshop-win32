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

#include "PsProject.h"
#include "Patternshop.h"
#include "PsWinImage.h"

PsWinImage* PsWinImage::instance = 0;

IMPLEMENT_DYNAMIC(PsWinImage, CDialog)
PsWinImage::PsWinImage(CWnd* pParent /*=NULL*/) :
	PsWin32(PsWinImage::IDD, pParent)
{
}

PsWinImage::~PsWinImage()
{
	ImageCache::iterator i = imageList.begin();
	for (; i != imageList.end(); i++)
	{
		i->second->Destroy();
		delete i->second;
		i->second = NULL;
	}
}

PsWinImage& PsWinImage::Instance()
{
	if (!instance)
		instance = new PsWinImage();
	return *instance;
}

void	PsWinImage::Delete()
{
	if (instance)
	{
		delete instance;
		instance = 0;
	}
}

BEGIN_MESSAGE_MAP(PsWinImage, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void PsWinImage::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	PsProjectController* project = PsController::Instance().project;
	CImage* img = NULL;
	if (project && project->image)
	{
		img = imageList[project->image];
		if (!img)
		{
			int bpp;
			uint8* data =
				project->image->GetTexture().GetBufferUncompressed(bpp);
			img = new CImage;
			img->Create(project->image->GetTexture().width,
				project->image->GetTexture().height, bpp * 8);
			for (int j = 0; j < project->image->GetTexture().height; j++)
			{
				for (int i = 0; i < project->image->GetTexture().width; i++)
				{
					if (bpp == 4 && data[(i + j *
						project->image->GetTexture().width) * bpp + 3] < 200)
					{
						img->SetPixelRGB(i, project->image->GetTexture().height - j - 1,
							255, 255, 255);
					}
					else
					{
						img->SetPixelRGB(i, project->image->GetTexture().height - j - 1,
							data[(i + j * project->image->GetTexture().width) * bpp + 2],
							data[(i + j * project->image->GetTexture().width) * bpp + 1],
							data[(i + j * project->image->GetTexture().width) * bpp]);
					}
				}
			}
			delete[] data;
			imageList[project->image] = img;
		}
		if (imgC != img)
		{
			PsRect r, k;
			GetWindowRect(&r);
			GetClientRect(&k);
			int hdiff = r.bottom - r.top - k.bottom;
			SetWindowPos(NULL, r.left, r.top, img->GetWidth(), img->GetHeight() + hdiff, SWP_NOMOVE);
			Invalidate(true);
			imgC = img;
		}
		if (img)
			img->BitBlt(dc, 0, 0);
	}
}
