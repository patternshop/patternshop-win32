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
#include "PatternshopView.h"
#include "MainFrm.h"
#include "PsWinProjectWin32.h"
#include "PsWinImage.h"
#include "PsWinOverview.h"
#include "PsWinTools.h"
#include "PsWinSplash.h"
#include "PsDlgConfig.h"

char* my_StartingDirectory = 0;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
	ON_WM_MENUSELECT()
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_FICHIER_PR32850, &CMainFrame::OnFichierPr32850)
	ON_MESSAGE(WM_UNIQUE, OnReceiveUnique)
	ON_COMMAND(ID_HELP, &CMainFrame::OnHelp)
	ON_BN_CLICKED(IDC_CHECK1, &CMainFrame::MenuOptionAutomatic)
	ON_UPDATE_COMMAND_UI(IDC_CHECK1, &CMainFrame::OnUpdateOptionAutomatic)
	ON_COMMAND(IDC_CHECK4, &CMainFrame::MenuOptionBoxShow)
	ON_UPDATE_COMMAND_UI(IDC_CHECK4, &CMainFrame::OnUpdateOptionBoxShow)
	ON_COMMAND(ID_AIDE_SIGNALERUNBOGUE, &CMainFrame::OnAideSignalerunbogue)
END_MESSAGE_MAP()

// construction ou destruction de CMainFrame

static UINT  indicators[] =
{
   ID_SEPARATOR,
   ID_INDICATOR_CAPS,
   ID_INDICATOR_NUM,
   ID_INDICATOR_SCRL,
};

CMainFrame::CMainFrame()
{
	winProject = NULL;
}

CMainFrame::~CMainFrame()
{
	if (my_StartingDirectory) delete[] my_StartingDirectory;
}

void CMainFrame::OnMenuSelect(UINT a, UINT b, HMENU m)
{
	PsWinSplash::Instance().DestroyWindow();
	CMDIFrameWnd::OnMenuSelect(a, b, m);
}

int  CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CWinApp* pApp = AfxGetApp();

	SetTitle("Patternshop");

	this->Invalidate(TRUE);
	ShowWindow(SW_SHOWMAXIMIZED);

	PsWinSplash::Instance().Create(IDD_SPLASH, NULL);
	PsWinSplash::Instance().ShowWindow(SW_SHOW);
	PsWinSplash::Instance().RedrawWindow();

	PsController& c = PsController::Instance();
	resolution_max = pApp->GetProfileInt("General", "Resolution", resolution_max);
	c.option[PsController::OPTION_BOX_SHOW] = !!pApp->GetProfileInt("General", "OPTION_BOX_SHOW", c.option[PsController::OPTION_BOX_SHOW]);
	c.option[PsController::OPTION_DOCUMENT_BLEND] = !!pApp->GetProfileInt("General", "OPTION_DOCUMENT_BLEND", c.option[PsController::OPTION_DOCUMENT_BLEND]);
	c.option[PsController::OPTION_DOCUMENT_SHOW] = !!pApp->GetProfileInt("General", "OPTION_DOCUMENT_SHOW", c.option[PsController::OPTION_DOCUMENT_SHOW]);
	c.option[PsController::OPTION_HIGHLIGHT_SHOW] = !!pApp->GetProfileInt("General", "OPTION_HIGHLIGHT_SHOW", c.option[PsController::OPTION_HIGHLIGHT_SHOW]);
	PsMatrix::default_w = pApp->GetProfileInt("PsMatrix", "w", PsMatrix::default_w);
	PsMatrix::default_h = pApp->GetProfileInt("PsMatrix", "h", PsMatrix::default_h);
	PsMatrix::minimum_dim = pApp->GetProfileInt("PsMatrix", "m", PsMatrix::minimum_dim);

	EnableDocking(CBRS_ALIGN_ANY);

	m_wndStatusBar.Create(this);
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));

	my_StartingDirectory = new char[1024];
	memset(my_StartingDirectory, 0, 1024);
	GetCurrentDirectory(1024, my_StartingDirectory);

	this->DisplaySubWindow();

	return 0;
}

void CMainFrame::DisplaySubWindow()
{
	CWinApp* pApp = AfxGetApp();

	PsRect r, k;
	GetClientRect(&r);
	GetWindowRect(&k);

	int w = 200;
	int xpos = r.right - w - 5;
	int ypos = k.top + ((k.bottom - k.top) - r.bottom);

	PsWinTools::Delete();
	PsWinTools::Instance().Create(IDD_TOOLS, this);
	PsWinTools::Instance().ShowWindow(SW_SHOW);

	PsWinImage::Delete();
	PsWinImage::Instance().Create(IDD_IMAGE, this);

	PsWinProperties::Delete();
	int x = pApp->GetProfileInt("Transformation", "x", xpos - w - 40);
	int y = pApp->GetProfileInt("Transformation", "y", ypos);
	PsWinProperties::Instance().Create(this, 0, WS_EX_TOOLWINDOW);
	PsWinProperties::Instance().MoveWindow(x, y, w + 40, 390);
	PsRect n, m;
	PsWinProperties::Instance().GetWindowRect(&n);
	PsWinProperties::Instance().ShowWindow(SW_SHOW);
	PsWinProperties::Instance().UpdateContentSize();

	PsWinOverview::Delete();
	x = pApp->GetProfileInt("Overview", "x", xpos);
	y = pApp->GetProfileInt("Overview", "y", ypos);
	PsWinOverview::Instance().Create(IDD_OVERVIEW, this);
	PsWinOverview::Instance().GetWindowRect(&m);
	PsWinOverview::Instance().MoveWindow(x, y, w, m.bottom - m.top);
	PsWinOverview::Instance().ShowWindow(SW_SHOW);
	ypos += m.bottom - m.top;

	PsWinProject::Delete();
	x = pApp->GetProfileInt("PsProject", "x", xpos);
	y = pApp->GetProfileInt("PsProject", "y", ypos);
	winProject = new PsWinProjectWin32();
	PsWinProject::setInstance(winProject);
	winProject->Create(IDD_PROJECT, this);
	winProject->GetWindowRect(&m);
	winProject->MoveWindow(x, y, w, m.bottom - m.top);
	PsController::Instance().UpdateDialogProject();
	winProject->ShowWindow(SW_SHOW);
	ypos += m.bottom - m.top;

	m_wndDlgBar.DestroyWindow();
	m_wndDlgBar.Create(this, IDD_QUICKBAR, CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR);
	m_wndReBar.DestroyWindow();
	m_wndReBar.Create(this);
	m_wndReBar.AddBar(&m_wndDlgBar);
}

void CMainFrame::RebuildMenuResource(CDocTemplate* pTemplate)
{
	// Update application menu
	//------------------------
	CMenu* pMenuNew = new CMenu;
	pMenuNew->LoadMenu(IDR_MAIN);
	m_hMenuDefault = pMenuNew->GetSafeHmenu();

	// Update current document menu
	//-----------------------------
	CMenu* pMenuFrame = new CMenu;
	pMenuFrame->LoadMenu(IDR_FRAME);
	((CMultiDocTemplate*)pTemplate)->m_hMenuShared = pMenuFrame->GetSafeHmenu();

	// Update visible menu
	//--------------------
	if (this->MDIGetActive() != NULL)
	{
		this->SetMenu(pMenuFrame);
	}
	else
	{
		this->SetMenu(pMenuNew);
	}
}

BOOL  CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CMDIFrameWnd::PreCreateWindow(cs))
		return FALSE;

	cs.lpszClass = _T(CLASS_NAME);

	return TRUE;
}

// diagnostics pour CMainFrame

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

void CMainFrame::OnFichierPr32850()
{
	PsDlgConfig dlgConfig;
	dlgConfig.DoModal();
}

void CMainFrame::OnClose()
{
	if (!do_not_save_options)
	{
		PsRect n;
		CWinApp* pApp = AfxGetApp();

		PsWinProperties::Instance().GetWindowRect(&n);
		pApp->WriteProfileInt("Transformation", "x", n.left);
		pApp->WriteProfileInt("Transformation", "y", n.top);
		PsWinOverview::Instance().GetWindowRect(&n);
		pApp->WriteProfileInt("Overview", "x", n.left);
		pApp->WriteProfileInt("Overview", "y", n.top);
		winProject->GetWindowRect(&n);
		pApp->WriteProfileInt("PsProject", "x", n.left);
		pApp->WriteProfileInt("PsProject", "y", n.top);

		pApp->WriteProfileInt("PsMatrix", "w", PsMatrix::default_w);
		pApp->WriteProfileInt("PsMatrix", "h", PsMatrix::default_h);
		pApp->WriteProfileInt("PsMatrix", "m", PsMatrix::minimum_dim);

		PsController& c = PsController::Instance();
		pApp->WriteProfileInt("General", "Resolution", resolution_max);
		pApp->WriteProfileInt("General", "Language", (int)getLanguage());
		//pApp->WriteProfileInt("General", "OPTION_BOX_MOVE",  c.option[PsController::OPTION_BOX_MOVE]);
		pApp->WriteProfileInt("General", "OPTION_BOX_SHOW", c.option[PsController::OPTION_BOX_SHOW]);
		pApp->WriteProfileInt("General", "OPTION_DOCUMENT_BLEND", c.option[PsController::OPTION_DOCUMENT_BLEND]);
		pApp->WriteProfileInt("General", "OPTION_DOCUMENT_SHOW", c.option[PsController::OPTION_DOCUMENT_SHOW]);
		pApp->WriteProfileInt("General", "OPTION_HIGHLIGHT_SHOW", c.option[PsController::OPTION_HIGHLIGHT_SHOW]);
	}
	CMDIFrameWnd::OnClose();
}

LRESULT    CMainFrame::OnReceiveUnique(WPARAM wParam, LPARAM lParam)
{
	char  buffer[1024];

	GlobalGetAtomName((ATOM)wParam, buffer, 1023);
	AfxGetApp()->OpenDocumentFile(buffer);
	GlobalDeleteAtom((ATOM)wParam);
	return 0;
}

void CMainFrame::OnHelp()
{
	char tmpbuffer[1024];

	strcpy(tmpbuffer,
		my_StartingDirectory);

	switch (getLanguage())
	{
	case French:
		strcat(tmpbuffer,
			"\\help_FR.chm::/help.htm");
		break;
	default:
		strcat(tmpbuffer,
			"\\help_EN.chm::/help.htm");
		break;
	}

	::HtmlHelp(GetSafeHwnd(), tmpbuffer, HH_DISPLAY_TOPIC, NULL);
}

void CMainFrame::MenuOptionAutomatic()
{
	bool  value = !PsController::Instance().GetOption(PsController::OPTION_AUTOMATIC, true);
	PsController::Instance().SetOption(PsController::OPTION_AUTOMATIC, value);
	if (PsController::Instance().active)
		PsController::Instance().active->Update();
}

void CMainFrame::OnUpdateOptionAutomatic(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(PsController::Instance().GetOption(PsController::OPTION_AUTOMATIC, true));
}

void CMainFrame::MenuOptionBoxShow()
{
	bool  value = !PsController::Instance().GetOption(PsController::OPTION_BOX_SHOW, true);
	PsController::Instance().SetOption(PsController::OPTION_BOX_SHOW, value);
	if (PsController::Instance().active)
		PsController::Instance().active->Update();
}

void CMainFrame::OnUpdateOptionBoxShow(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(PsController::Instance().GetOption(PsController::OPTION_BOX_SHOW, true));
}

/*
** ProgressBar : -1 crée, -2 détruit, 0~100 affiche la barre
*/
void    CMainFrame::ProgressBar(int pos)
{
	PsRect  rc;

	switch (pos)
	{
	case -1:
		m_wndStatusBar.GetItemRect(0, &rc);
		m_wndProgress.Create(WS_CHILD | WS_VISIBLE, rc, &m_wndStatusBar, 1);

		m_wndProgress.SetRange(0, 100);
		m_wndProgress.SetStep(1);
		m_wndProgress.SetPos(0);
		break;

	case -2:
		m_wndProgress.DestroyWindow();
		break;

	default:
		m_wndProgress.SetPos(pos);
		break;
	}
}

void PopupURL(char* url)
{
	if ((int)ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL) < 32)
	{
		AfxMessageBox(url);
	}
}

void CMainFrame::OnAideSignalerunbogue()
{
	// FIXME
}

afx_msg BOOL CMainFrame::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}
