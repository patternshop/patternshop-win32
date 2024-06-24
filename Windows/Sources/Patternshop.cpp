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
#include "MainFrm.h"
#include "ChildFrm.h"
#include "PatternshopDoc.h"
#include "PatternshopView.h"
#include "PsHardware.h"
#include "PsWinProject.h"
#include "PsController.h"
#include "PsWinImage.h"
#include "PsWinOverview.h"
#include "PsWinSplash.h"
#include "PsWinTools.h"
#include "French/resource.h"
#include "MultiDocTemplate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPatternshopApp  theApp;
bool CPatternshopAppIsLoad = false;

BEGIN_MESSAGE_MAP(CPatternshopApp, CWinApp)
   ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
   ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
   ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
   ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

CPatternshopApp::CPatternshopApp()
{
   englishDLL = LoadLibrary(_T("English.dll"));
   frenchDLL = NULL;
}

/**
* Change the language
*/
void CPatternshopApp::SetLanguage(PsLanguage nLanguage)
{
   CString    strBuffer  = _T("");
   HINSTANCE  hInst      = NULL;
   LCID      lcid      = NULL;

   // Getting default system language
   //--------------------------------
   if ( nLanguage == Default )
   {
      lcid = GetThreadLocale();
      lcid &= 0xFF;
      switch(lcid)
      {
         case LANG_FRENCH:
            nLanguage = French;
            break;
         case LANG_ENGLISH:
         default:
            nLanguage = English;
            break;
      }
   }

   // Load the relevant resource DLL
   //-------------------------------
   switch ( nLanguage )
   {
      case English:
         hInst = englishDLL;
         setLanguage( English );
         break;
      case French:
         if ( frenchDLL == NULL )
         {
            frenchDLL = LoadLibrary(_T("French.dll"));
         }
         hInst = frenchDLL;
         setLanguage( French );
         break;
   }

   // Rebuild the resources
   //----------------------
   if ( hInst != NULL )
   {
      AfxSetResourceHandle( hInst );
   }
   else if ( englishDLL != NULL )
   {
      AfxSetResourceHandle( englishDLL );
   }
   else
   {
      MessageBox(0, "Localization DLL is missing.", "", MB_ICONINFORMATION);
   }

   // Check the application startup
   //------------------------------
   if ( m_pMainWnd != NULL )
   {
      CMainFrame* mainFrame = (CMainFrame*)m_pMainWnd;

      // Rebuild the application menu
      //-----------------------------
      mainFrame->RebuildMenuResource( this->docTemplate );

      // Rebuild the sub-windows
      //------------------------
      mainFrame->DisplaySubWindow();

      // Reload the string table
      //------------------------
      this->docTemplate->rebuildResources();
   }
}

BOOL CPatternshopApp::InitInstance()
{
   CMainFrame*         pMainFrame;
   CCommandLineInfo     cmdInfo;
   CWnd*              pWndPrev;
   WNDCLASS            wndcls;

   if ((pWndPrev = CWnd::FindWindow (_T (CLASS_NAME), NULL)))
   {
      if (this->m_lpCmdLine && strcmp(this->m_lpCmdLine, ""))
         pWndPrev->SendMessage (WM_UNIQUE, GlobalAddAtom (_T (this->m_lpCmdLine)), 0);
      pWndPrev->SetForegroundWindow();
      return FALSE;
   }

   memset (&wndcls, 0, sizeof (WNDCLASS));

   wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
   wndcls.lpfnWndProc = ::DefWindowProc;
   wndcls.hInstance = AfxGetInstanceHandle();
   wndcls.hIcon = LoadIcon (IDR_MAINFRAME);
   wndcls.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
   wndcls.lpszMenuName = NULL;
   wndcls.lpszClassName = _T (CLASS_NAME);

   if (!AfxRegisterClass (&wndcls))
   {
      AfxMessageBox (_T ("Class Registration Failed\n"));
      return FALSE;
   }

   if (!AfxOleInit())
   {
      AfxMessageBox (IDP_OLE_INIT_FAILED);
      return FALSE;
   }

   AfxEnableControlContainer();

   InitCommonControls();

   CWinApp::InitInstance();

   SetRegistryKey (_T("Patternshop"));
   LoadStdProfileSettings (4);  // Charge les options de fichier INI standard (y compris les derniers fichiers utilisés)

   this->SetLanguage( (PsLanguage)this->GetProfileInt("General", "Language", getLanguage() ) );

   // Inscrire les modèles de document de l'application. Ces modèles lient les documents, fenêtres frame et vues entre eux
   this->docTemplate = new PsMultiDocTemplate( IDR_PatternshopTYPE,
                                               RUNTIME_CLASS( CPatternshopDoc ),
                                               RUNTIME_CLASS( CChildFrame ),
                                               RUNTIME_CLASS( CPatternshopView ) );
   if ( this->docTemplate == NULL )
   {
      return FALSE;
   }

   AddDocTemplate( this->docTemplate );

   EnableShellOpen();
   RegisterShellFileTypes();

   pMainFrame = new CMainFrame;
   if (!pMainFrame || !pMainFrame->LoadFrame (IDR_MAINFRAME))
      return FALSE;
   m_pMainWnd = pMainFrame;

   hardwareRenderer.SafeCreatePixelBuffer();

   //m_pMainWnd->DragAcceptFiles(true); -> DECONSEILLE

   ParseCommandLine (cmdInfo);
   if (!ProcessShellCommand (cmdInfo))
      return FALSE;

   CPatternshopAppIsLoad = true;


   pMainFrame->Invalidate(TRUE);
   pMainFrame->m_wndReBar.Invalidate(TRUE);
   pMainFrame->m_wndDlgBar.Invalidate(TRUE);

   pMainFrame->ShowWindow (SW_SHOWMAXIMIZED);
   pMainFrame->UpdateWindow();



   return TRUE;
}

int    CPatternshopApp::ExitInstance()
{
   ::UnregisterClass (_T (CLASS_NAME), AfxGetInstanceHandle());

   PsController::Delete();
   PsWinProject::Delete();
   PsWinImage::Delete();
   PsWinTools::Delete();
   PsWinSplash::Delete();
   PsWinOverview::Delete();

   delete m_pMainWnd;

   return CWinApp::ExitInstance();
}

// boîte de dialogue CAboutDlg utilisée pour la boîte de dialogue 'À propos de' pour votre application

class CAboutDlg : public CDialog
{
public:
   CAboutDlg();

   // Données de boîte de dialogue
   enum { IDD = IDD_ABOUTBOX };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge DDX/DDV

   // Implémentation
protected:
   DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog (CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange (CDataExchange* pDX)
{
   CDialog::DoDataExchange (pDX);
}

BEGIN_MESSAGE_MAP (CAboutDlg, CDialog)
END_MESSAGE_MAP()

void CPatternshopApp::OnAppAbout()
{
   CAboutDlg aboutDlg;
   aboutDlg.DoModal();
}
