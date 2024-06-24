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
#include "PsTabInfoPattern.h"
#include "PatternshopView.h"
#include "PsTabInfoPattern.h"
#include "PsDlgDocument.h"
#include "PsProject.h"
#include "PsController.h"

IMPLEMENT_DYNAMIC(PsTabInfoPattern, CPropertyPage)

PsTabInfoPattern::PsTabInfoPattern()
: CPropertyPage(PsTabInfoPattern::IDD)
{
}

int PsTabInfoPattern::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (CPropertyPage::OnCreate(lpCreateStruct) == -1)
      return -1;
   PsRect r;
   GetClientRect(&r);
   window_buffer2.Create(r.right, r.bottom, 24);
   window_buffer.Create(r.right, r.bottom, 24);
   CDC dc;
   dc.CreateCompatibleDC(NULL);
   dc.SelectObject(window_buffer2);
   CBrush gray(GetSysColor(COLOR_BTNFACE));
   CPen penGray(PS_SOLID, 1, GetSysColor(COLOR_BTNFACE));
   dc.SelectObject(&gray);
   dc.SelectObject(&penGray);
   dc.Rectangle(0, 0, window_buffer2.GetWidth(), window_buffer2.GetHeight());
   SetTimer(0, 100, NULL);
   return 0;
}

PsTabInfoPattern::~PsTabInfoPattern()
{
}

void PsTabInfoPattern::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_SLIDER1, m_SliderQuality);
   DDX_Control(pDX, IDC_TX, tX);
   DDX_Control(pDX, IDC_TY, tY);
   DDX_Control(pDX, IDC_TZ, tZ);
   DDX_Control(pDX, IDC_RX, rX);
   DDX_Control(pDX, IDC_RY, rY);
   DDX_Control(pDX, IDC_RZ, rZ);
   DDX_Control(pDX, IDC_Z, Z);
}

BEGIN_MESSAGE_MAP(PsTabInfoPattern, CPropertyPage)
   ON_WM_PAINT()
   ON_WM_TIMER()
   ON_WM_CREATE()
   ON_WM_ERASEBKGND()
   ON_WM_LBUTTONDOWN()
   ON_WM_SHOWWINDOW()
   ON_WM_SYSCOMMAND()
   ON_EN_CHANGE(IDC_TX, OnChangeTX)
   ON_EN_CHANGE(IDC_TY, OnChangeTY)
   ON_EN_CHANGE(IDC_TZ, OnChangeTZ)
   ON_EN_CHANGE(IDC_RX, OnChangeRX)
   ON_EN_CHANGE(IDC_RY, OnChangeRY)
   ON_EN_CHANGE(IDC_RZ, OnChangeRZ)
   ON_EN_CHANGE(IDC_Z, OnChangeZ)
   ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, &PsTabInfoPattern::OnNMReleasedcaptureSlider1)
END_MESSAGE_MAP()

void PsTabInfoPattern::CleanBackground()
{
   CDC dc;
   dc.CreateCompatibleDC(NULL);
   dc.SelectObject(window_buffer2);

   CBrush gray(GetSysColor(COLOR_BTNFACE));
   CPen penGray(PS_SOLID, 1, GetSysColor(COLOR_BTNFACE));
   dc.SelectObject(&gray);
   dc.SelectObject(&penGray);
   dc.Rectangle(0, 0, window_buffer2.GetWidth(), window_buffer2.GetHeight());
}

void PsTabInfoPattern::UpdateMiniImage()
{
   CleanBackground();
   dpCore.UpdateMiniImage(window_buffer.GetWidth(), window_buffer.GetHeight() / 2);

   //-- copie dans le buffer principal
   CDC dc;
   dc.CreateCompatibleDC(NULL);
   dc.SelectObject(window_buffer2);
   CDC dc2;
   dc2.CreateCompatibleDC(NULL);
   dc2.SelectObject(dpCore.m_RenduImage.buffer); 
   dc.BitBlt(dpCore.dst_x1, dpCore.dst_y1, dpCore.dst_width, dpCore.dst_height, &dc2, 0, 0, SRCCOPY);
   dc2.DeleteDC();
   //--

   //-- dessin du cadre
   /*PsRect r;
   r.left = dst_x1 - 1; 
   r.top = dst_y1 - 1; 
   r.right = dst_x2 + 1;
   r.bottom = dst_y2 + 1; 
   dc.DrawEdge(&r, EDGE_SUNKEN, BF_RECT);*/
   //--

}

void PsTabInfoPattern::UpdateNow()
{
   if (PsController::Instance().active &&
      PsController::Instance().project)
   {
      UpdateMiniImage();
   }
   UpdateText();
   Invalidate(true);
}

void PsTabInfoPattern::FastUpdate()
{
   CDC dc;
   dc.CreateCompatibleDC(NULL);
   dc.SelectObject(window_buffer);

   if (!PsController::Instance().project)
      CleanBackground();

   window_buffer2.BitBlt(dc.GetSafeHdc(), 0, 0);
} 

void PsTabInfoPattern::OnPaint()
{
   FastUpdate();
   CPaintDC dc(this);
   window_buffer.BitBlt(dc.GetSafeHdc(), 0, 0);
}

BOOL PsTabInfoPattern::OnEraseBkgnd(CDC* pDC)
{
   return TRUE;
}

void PsTabInfoPattern::OnLButtonDown(UINT nFlags, CPoint point)
{
   dpCore.OnButtonDown(point.x, point.y);
   CPropertyPage::OnLButtonDown(nFlags, point);
}

void PsTabInfoPattern::Update()
{
   dpCore.Update();
}

void PsTabInfoPattern::OnSysCommand(UINT nID, LPARAM lParam)
{
   if(nID == SC_CLOSE) return;
   CDialog::OnSysCommand(nID, lParam);
}

void PsTabInfoPattern::OnTimer(UINT_PTR nIDEvent)
{
   if (!dpCore.bUpdated && !PsController::Instance().bMouseButtonIsDown)
   {
      UpdateNow();
      dpCore.bUpdated = true;   
   }
}

void PsTabInfoPattern::OnShowWindow(BOOL bShow, UINT nStatus)
{
   static bool bInit = false;
   CPropertyPage::OnShowWindow(bShow, nStatus);
   if (!bInit)
   {
      m_SliderQuality.SetRange(0, 2);
      m_SliderQuality.SetPos(1);
      bInit = true;
   }
   if (!bShow) dpCore.OnShowWindow(false); 
   else dpCore.OnShowWindow(true); 
}

void PsTabInfoPattern::OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
   dpCore.SetQuality(m_SliderQuality.GetPos());
}

PsLayer *PsTabInfoPattern::GetCurrentLayer()
{
   if (!PsController::Instance().project) 
      return NULL;
   PsProject *project = PsController::Instance().project;
   if (!project->pattern) 
      return NULL;
   return project->pattern->aLayers[project->iLayerId];
}

void PsTabInfoPattern::OnChangeTX()
{
   CWnd *c = GetFocus();
   if (c != &tX) return;
   PsLayer *layer = GetCurrentLayer();
   char buffer[1024];
   tX.GetWindowTextA(buffer, 1024);
   layer->vTranslation.X = atof(buffer);
   PsController::Instance().UpdateWindow();
}

void PsTabInfoPattern::OnChangeTY()
{
   CWnd *c = GetFocus();
   if (c != &tY) return;
   PsLayer *layer = GetCurrentLayer();
   char buffer[1024];
   tY.GetWindowTextA(buffer, 1024);
   layer->vTranslation.Y = atof(buffer);
   PsController::Instance().UpdateWindow();
}

void PsTabInfoPattern::OnChangeTZ()
{
   CWnd *c = GetFocus();
   if (c != &tZ) return;
   PsLayer *layer = GetCurrentLayer();
   char buffer[1024];
   tZ.GetWindowTextA(buffer, 1024);
   layer->vTranslation.Z = atof(buffer);
   PsController::Instance().UpdateWindow();
}

void PsTabInfoPattern::OnChangeRX()
{
   CWnd *c = GetFocus();
   if (c != &rX) return;
   PsLayer *layer = GetCurrentLayer();
   char buffer[1024];
   rX.GetWindowTextA(buffer, 1024);
   layer->rRotation.Roll = atof(buffer);
   PsController::Instance().UpdateWindow();
}

void PsTabInfoPattern::OnChangeRY()
{
   CWnd *c = GetFocus();
   if (c != &rY) return;
   PsLayer *layer = GetCurrentLayer();
   char buffer[1024];
   rY.GetWindowTextA(buffer, 1024);
   layer->rRotation.Pitch = atof(buffer);
   PsController::Instance().UpdateWindow();
}

void PsTabInfoPattern::OnChangeRZ()
{
   CWnd *c = GetFocus();
   if (c != &rZ) return;
   PsLayer *layer = GetCurrentLayer();
   char buffer[1024];
   rZ.GetWindowTextA(buffer, 1024);
   layer->rRotation.Yaw = atof(buffer);
   PsController::Instance().UpdateWindow();
}

void PsTabInfoPattern::OnChangeZ()
{
   CWnd *c = GetFocus();
   if (c != &Z) return;
   PsLayer *layer = GetCurrentLayer();
   char buffer[1024];
   Z.GetWindowTextA(buffer, 1024);
   layer->fScale = atof(buffer) / 100.f;
   PsController::Instance().UpdateWindow();
}

void PsTabInfoPattern::UpdateText()
{
   CWnd *c = GetFocus();

   PsLayer *layer = GetCurrentLayer(); 
   if (layer)
   {
      char buffer[1024];

      Enable();

      sprintf(buffer, "%.2f", layer->vTranslation.X);
      tX.SetWindowText(buffer);
      sprintf(buffer, "%.2f", layer->vTranslation.Y);
      tY.SetWindowText(buffer);
      sprintf(buffer, "%.2f", layer->vTranslation.Z);
      tZ.SetWindowText(buffer);

      sprintf(buffer, "%.2f", layer->rRotation.Roll);
      rX.SetWindowText(buffer);
      sprintf(buffer, "%.2f", layer->rRotation.Pitch);
      rY.SetWindowText(buffer);
      sprintf(buffer, "%.2f", layer->rRotation.Yaw);
      rZ.SetWindowText(buffer);

      sprintf(buffer, "%.2f", layer->fScale * 100.f);
      Z.SetWindowText(buffer);
   }
   else
   {
      Disable();
   }
}

void PsTabInfoPattern::Enable()
{
   tX.EnableWindow(TRUE);
   tY.EnableWindow(TRUE);
   tZ.EnableWindow(TRUE);
   rX.EnableWindow(TRUE);
   rY.EnableWindow(TRUE);
   rZ.EnableWindow(TRUE);
   Z.EnableWindow(TRUE);
}

void PsTabInfoPattern::Disable()
{
   tX.EnableWindow(FALSE);
   tY.EnableWindow(FALSE);
   tZ.EnableWindow(FALSE);
   rX.EnableWindow(FALSE);
   rY.EnableWindow(FALSE);
   rZ.EnableWindow(FALSE);
   Z.EnableWindow(FALSE);

   tX.SetWindowText("");
   tY.SetWindowText("");
   tZ.SetWindowText("");
   rX.SetWindowText("");
   rY.SetWindowText("");
   rZ.SetWindowText("");
   Z.SetWindowText("");
}