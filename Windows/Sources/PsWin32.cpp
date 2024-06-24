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
#include "Patternshop.h"
#include "PsWin32.h"

bool PsWin32::staticLoaded = false;
CFont PsWin32::nFont;
CFont PsWin32::bFont;
CPen PsWin32::penBlackDash;

IMPLEMENT_DYNAMIC(PsWin32, CDialog)

PsWin32::PsWin32(UINT nIDTemplate, CWnd* pParent /*=NULL*/)
: CDialog(nIDTemplate, pParent)
{
   dc = NULL;
}

PsWin32::~PsWin32()
{
   for (std::map<int,CPen*>::iterator i = m_PenMap.begin(); i != m_PenMap.end(); i++)
   {
      delete i->second;
      i->second = NULL;
   }
   m_PenMap.empty();

   for (std::map<int,CBrush*>::iterator i = m_BrushMap.begin(); i != m_BrushMap.end(); i++)
   {
      delete i->second;
      i->second = NULL;
   }
   m_BrushMap.empty();
}

void PsWin32::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(PsWin32, CDialog)
   ON_WM_LBUTTONDOWN()
   ON_WM_MOUSEMOVE()
   ON_WM_TIMER()
   ON_WM_CREATE()
   ON_WM_PAINT()
END_MESSAGE_MAP()

void PsWin32::OnLButtonDown(UINT nFlags, CPoint point)
{
   // FIXME
   CDialog::OnLButtonDown(nFlags, point);
}

void PsWin32::OnTimer(UINT_PTR nIDEvent)
{
   // FIXME
}

int PsWin32::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (CDialog::OnCreate(lpCreateStruct) == -1)
      return -1;

   UpdateSize();

   if (!dc)
   {
      windowDc = GetDC();
      dc = windowDc;
   }

   if (staticLoaded == false)
   {
      LOGBRUSH LogBrush;
      LogBrush.lbColor = RGB(0, 0, 0);
      LogBrush.lbStyle = PS_SOLID;
      penBlackDash.CreatePen( PS_COSMETIC | PS_ALTERNATE , 1, &LogBrush, 0, NULL );

      VERIFY(nFont.CreateFont(
         12,                        // nHeight
         0,                         // nWidth
         0,                         // nEscapement
         0,                         // nOrientation
         FW_NORMAL,                 // nWeight
         FALSE,                     // bItalic
         FALSE,                     // bUnderline
         0,                         // cStrikeOut
         ANSI_CHARSET,              // nCharSet
         OUT_DEFAULT_PRECIS,        // nOutPrecision
         CLIP_DEFAULT_PRECIS,       // nClipPrecision
         DEFAULT_QUALITY,           // nQuality
         DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
         "Tahoma"));                 // lpszFacename

      VERIFY(bFont.CreateFont(
         12,                        // nHeight
         0,                         // nWidth
         0,                         // nEscapement
         0,                         // nOrientation
         FW_BOLD,                 // nWeight
         FALSE,                     // bItalic
         FALSE,                     // bUnderline
         0,                         // cStrikeOut
         ANSI_CHARSET,              // nCharSet
         OUT_DEFAULT_PRECIS,        // nOutPrecision
         CLIP_DEFAULT_PRECIS,       // nClipPrecision
         DEFAULT_QUALITY,           // nQuality
         DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
         "Tahoma"));                 // lpszFacename

      staticLoaded = true;
   }

   SetTimer(0, 2000, NULL);

   return 0;
}

void PsWin32::UpdateSize()
{
   PsRect r;
   GetClientRect(&r);
   iWidth = r.right;
   iHeight = r.bottom;
}

void PsWin32::OnPaint()
{
   // FIXME
   CDialog::OnPaint();
}

void PsWin32::OnMouseMove(UINT nFlags, CPoint point) 
{
   //FIXME
   CDialog::OnMouseMove(nFlags, point);
}

void PsWin32::SetTarget(SoftwareBuffer *sfb)
{
   if (dc && windowDc != dc)
   {
      dc->DeleteDC();
      delete dc;
      dc = NULL;
   }

   if (sfb)
   {
      CDC *bdc = new CDC;
      bdc->CreateCompatibleDC(NULL);
      bdc->SelectObject(sfb->buffer);
      dc = bdc;
   }
}

void PsWin32::SetPenColor(int r, int g, int b)
{
   int key = r + g * 255.f + b * 255.f * 255.f;
   if (m_PenMap[key] == NULL)
      m_PenMap[key] = new CPen(PS_SOLID, 1, RGB(r, g, b));
   ASSERT(dc);
   dc->SelectObject(m_PenMap[key]);
}

void PsWin32::SetBrushColor(int r, int g, int b)
{
   int key = r + g * 255.f + b * 255.f * 255.f;
   if (m_BrushMap[key] == NULL)
      m_BrushMap[key] = new CBrush(RGB(r, g, b));
   ASSERT(dc);
   dc->SelectObject(m_BrushMap[key]);
}

void PsWin32::SetTextColor(int r, int g, int b)
{
   ASSERT(dc);
   dc->SetTextColor(RGB(r, g, b));
}

void PsWin32::DrawRectangle(int x1, int y1, int x2, int y2)
{
   ASSERT(dc);
   dc->Rectangle(x1, y1, x2, y2);
}

void PsWin32::MovePenTo(int x, int y)
{
   ASSERT(dc);
   dc->MoveTo(x, y);
}

void PsWin32::DrawLineTo(int x, int y)
{
   ASSERT(dc);
   dc->LineTo(x, y);
}

void PsWin32::DrawSoftwareBuffer(SoftwareBuffer &SoftBuffer, int x, int y)
{
   SoftBuffer.buffer.BitBlt(*dc, x, y);	
}

void PsWin32::DrawText(const char *Buffer, PsRect &Location, PsFont Font)
{
   ASSERT(dc);
   dc->SetBkMode(TRANSPARENT);
   switch (Font)
   {
   case PSFONT_NORMAL: dc->SelectObject(&nFont); break;
   case PSFONT_BOLD: dc->SelectObject(&bFont); break;
   default: ASSERT(0 && "UNDEFINED PsFont"); break;
   }
   dc->DrawText(Buffer, &Location, DT_LEFT | DT_VCENTER | DT_SINGLELINE); 
}

void PsWin32::SetDashBlackPen()
{
   ASSERT(dc);
   dc->SelectObject(&penBlackDash);
}

void PsWin32::DrawPolygon(PsPoint* points, int c)
{
   ASSERT(dc);
   dc->Polygon(points, c);
}

void PsWin32::DrawImage(SoftwareBuffer &image, int x, int y)
{
   image.buffer.BitBlt(dc->GetSafeHdc(), x, y);
}

void CopyImageWithTransparency(CPaintDC &dcPaint, CImage &image, int x, int y)
{
   CDC dc;
   dc.CreateCompatibleDC(NULL);
   dc.SelectObject(image);
   dcPaint.TransparentBlt(x, y, image.GetWidth(), image.GetHeight(), &dc, 0, 0, image.GetHeight(), image.GetHeight(), RGB(0, 255, 0));
}
