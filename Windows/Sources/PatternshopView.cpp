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
#include <htmlhelp.h>
#include "Patternshop.h"
#include "PatternshopDoc.h"
#include "PatternshopView.h"
#include "PsWinOverview.h"
#include "PsDlgPrint.h"
#include "PsProjectController.h"
#include "PsWinPropertiesCx.h"
#include "PsWinTools.h"
#include "PsController.h"
#include "PsMessage.h"
#include "PsDlgOpen.h"
#include "PsWinSplash.h"
#include "PsWinProject.h"
#include "PsWinProperties.h"
#include "PsWinImage.h"
#include "PsDlgExport.h"
#include "PsDlgDocument.h"
#include "PsFileTypes.h"
#include "PsProjectLoad.h"

#define IMAGE_DIRECTORY "\\Image\\"
#define TEMPLATE_DIRECTORY "\\Template\\"

extern char* my_StartingDirectory;

PsDlgPrint dlg;

void Get_READ_FORMATS(char* buffer3)
{
	char buffer2[1024];

	strcpy(buffer3, "");
	sprintf(buffer2, "%s|%s|", READ_FORMATS[0][0], READ_FORMATS[0][1]);
	strcat(buffer3, buffer2);
	for (int i = 1; i < 10; i++)
	{
		sprintf(buffer2, "%s (%s)|%s|", READ_FORMATS[i][0], READ_FORMATS[i][1], READ_FORMATS[i][1]);
		strcat(buffer3, buffer2);
	}
	//sprintf(buffer2, "%s|%s||", READ_FORMATS[9][0], READ_FORMATS[9][1]);
	strcat(buffer3, "|");
}

void Get_WRITE_FORMATS(char* buffer3)
{
	char buffer2[1024];

	strcpy(buffer3, "");
	for (int i = 0; i < 5; i++)
	{
		sprintf(buffer2, "%s (%s)|%s|", WRITE_FORMATS[i][0], WRITE_FORMATS[i][1], WRITE_FORMATS[i][1]);
		strcat(buffer3, buffer2);
	}
	sprintf(buffer2, "|");
	strcat(buffer3, buffer2);
}

// CPatternshopView

IMPLEMENT_DYNCREATE(CPatternshopView, CView)

BEGIN_MESSAGE_MAP(CPatternshopView, CView)
	ON_COMMAND(ID_FILE_IMAGE, MenuFileImage)
	ON_COMMAND(ID_FILE_PATTERN, MenuFilePattern)
	ON_COMMAND(ID_FICHIER_IMPORTERUNEIMAGELIBRE, OnFichierImporteruneimagelibre)
	ON_COMMAND(ID_FILE_EXPORT, MenuFileExport)
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_EXPORT_RACCORD, MenuFileExportRaccord)

	ON_COMMAND(ID_EDIT_UNDO, MenuLogUndo)
	ON_COMMAND(ID_EDIT_REDO, MenuLogRedo)
	ON_COMMAND(ID_EDIT_CLONE, MenuEditClone)
	ON_COMMAND(ID_EDIT_REPLACE, MenuEditReplace)
	ON_COMMAND(ID_EDIT_DELIMAGE, MenuImageDel)

	ON_COMMAND(ID_MATRIX_CLONE, MenuMatrixClone)
	ON_COMMAND(ID_MATRIX_COLOR, MenuMatrixColor)
	ON_COMMAND(ID_MATRIX_DEL, MenuMatrixDel)
	ON_COMMAND(ID_MATRIX_NEW, MenuMatrixNew)
	ON_COMMAND(ID_MATRIX_RESET, MenuMatrixReset)

	ON_COMMAND(ID_OPTION_DOCUMENT_BLEND, MenuOptionDocumentBlend)
	ON_COMMAND(ID_OPTION_DOCUMENT_SHOW, MenuOptionDocumentShow)
	ON_COMMAND(ID_OPTION_HIGHLIGHT_SHOW, MenuOptionHighlightShow)

	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_RBUTTONUP()
	ON_WM_SETFOCUS()
	ON_WM_ERASEBKGND()
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_DROPFILES()
	ON_WM_TIMER()

	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CPatternshopView::OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &CPatternshopView::OnUpdateEditRedo)
	ON_UPDATE_COMMAND_UI(ID_OPTION_HIGHLIGHT_SHOW, &CPatternshopView::OnUpdateOptionHighlightShow)
	ON_UPDATE_COMMAND_UI(ID_OPTION_DOCUMENT_SHOW, &CPatternshopView::OnUpdateOptionDocumentShow)
	ON_UPDATE_COMMAND_UI(ID_OPTION_DOCUMENT_BLEND, &CPatternshopView::OnUpdateOptionDocumentBlend)

	ON_COMMAND(ID_PROJET_TAILLEDELAZONEDETRAVAIL, &CPatternshopView::OnProjetTailledelazonedetravail)
END_MESSAGE_MAP()

CPatternshopView::CPatternshopView() :
	project_controller(0)
{
	m_hDib = NULL;
	m_hOldDC = NULL;
	m_hMemDC = NULL;
	m_hOldRC = NULL;
	m_hMemRC = NULL;
	m_hOldDib = NULL;
	m_pBitmapBits = NULL;
	PsController::Instance().SetActive(this);
}

CPatternshopView::~CPatternshopView()
{
	if (project_controller)
		delete project_controller;
	project_controller = 0;
	PsWinOverview::Instance().Invalidate();
}

int CPatternshopView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	DragAcceptFiles(true);
	SetTimer(0, 25, NULL);
	PsController::Instance().SetActive(this);
	project_controller = new PsProjectController();
	return 0;
}

// --- MENUS ---

void  CPatternshopView::MenuFileImage()
{
	char buffer3[1024];
	Get_READ_FORMATS(buffer3);
	PsDlgOpen  dialog(true, 0, 0, 0, _T(buffer3));

	char tmpbuffer[1024];
	strcpy(tmpbuffer, my_StartingDirectory);
	strcat(tmpbuffer, IMAGE_DIRECTORY);
	//OutputDebugString(tmpbuffer);
	dialog.m_ofn.lpstrInitialDir = tmpbuffer;

	if (dialog.DoModal() == IDOK)
	{
		GetError(project_controller->NewMotif((LPCTSTR)dialog.GetPathName()));
		Update();
	}
}

void  CPatternshopView::MenuFilePattern()
{
	PsDlgOpen  dialog(true, 0, 0, 0, _T("TIFF (*.TIF;*.TIFF)|*.tif;*.tiff||"));

	char tmpbuffer[1024];
	strcpy(tmpbuffer, my_StartingDirectory);
	strcat(tmpbuffer, TEMPLATE_DIRECTORY);
	//OutputDebugString(tmpbuffer);
	dialog.m_ofn.lpstrInitialDir = tmpbuffer;

	if (dialog.DoModal() == IDOK)
	{
		GetError(project_controller->NewPattern((LPCTSTR)dialog.GetPathName()));
		//Update();
		//dlgPropreties->FocusMatrixInformation();
		PsController::Instance().UpdateDialogProject();
	}
}

void CPatternshopView::OnFichierImporteruneimagelibre()
{
	char buffer3[1024];
	Get_READ_FORMATS(buffer3);
	PsDlgOpen  dialog(true, 0, 0, 0, _T(buffer3));

	char tmpbuffer[1024];
	strcpy(tmpbuffer, my_StartingDirectory);
	strcat(tmpbuffer, IMAGE_DIRECTORY);
	//OutputDebugString(tmpbuffer);
	dialog.m_ofn.lpstrInitialDir = tmpbuffer;

	if (dialog.DoModal() == IDOK)
	{
		GetError(project_controller->NewImage((LPCTSTR)dialog.GetPathName()));
		Update();
	}
}

void      CPatternshopView::OnDropFiles(HDROP hDropInfo)
{
	int nb = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	for (int i = 0; i < nb; i++)
	{
		int taille = DragQueryFile(hDropInfo, i, NULL, 0) + 1;
		char* fic = new char[taille + 1];
		DragQueryFile(hDropInfo, i, fic, taille);
		//--

		GetError(project_controller->NewMotif((LPCTSTR)fic));

		//--
		delete fic;
	}
	Update();
	DragFinish(hDropInfo); //vidage de la mem...
}

void CPatternshopView::SetAutoPosition()
{
	PsRect r;
	GetParentFrame()->GetWindowRect(&r);

	// ajustement de la taille de la fenetre
	float max = 800;
	float ratio = (float)project_controller->GetWidth() / (float)project_controller->GetWidth();
	r.right = (LONG)(r.bottom * ratio);
	if (r.right > max)
	{
		ratio = max / (float)r.right;
		r.right *= ratio;
		r.bottom *= ratio;
	}
	if (r.bottom > max)
	{
		ratio = max / (float)r.bottom;
		r.right *= ratio;
		r.bottom *= ratio;
	}
	GetParentFrame()->SetWindowPos(NULL, r.left + 200, r.top, r.right, r.bottom, 0);
	project_controller->renderer.CenterView();
}

void CPatternshopView::SetProjectSize(int w, int h, int dpi)
{
	project_controller->renderer.SetDocSize(w, h);
	project_controller->renderer.dpi = dpi;
	Update();
}

BOOL  CPatternshopView::MenuFileLoad(const char* name)
{
	ErrID r;

	PsProjectLoad& loader = PsProjectLoad(*project_controller);
	r = loader.loadProject(name);

	if (r != ERROR_NONE)
	{
		GetError(r);
		return FALSE;
	}

	SetProjectSize(project_controller->GetWidth(), project_controller->renderer.doc_y, project_controller->renderer.dpi);
	SetAutoPosition();

	PsController::Instance().SetActive(this);
	project_controller->matrix = NULL;
	project_controller->image = NULL;
	//-- sélection automatique de la première matrice
	if (project_controller->matrices.size() != 0) project_controller->SelectMatrix(*(project_controller->matrices.begin()));
	//--
	Update();
	dlgPropreties->FocusMatrixInformation();

	return TRUE;
}

void  CPatternshopView::MenuFileExport()
{
	int x;
	int y;
	char buffer3[1024];
	Get_WRITE_FORMATS(buffer3);
	PsDlgOpen  dialog(false, ".tif", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T(buffer3));
	if (dialog.DoModal() == IDOK)
	{
		project_controller->renderer.GetDocSize(x, y);
		project_controller->RenderToFile(dialog.GetPathName(), x, y);
	}
}

void  CPatternshopView::MenuFileExportRaccord()
{
	PsDlgExport dialogExport;

	//-- vérification qu'une matrice est bien sélectionnée
	if (!project_controller || !project_controller->matrix)
	{
		GetError(ERROR_MATRIX_SELECT);
		return;
	}
	//--

	//-- vérification qu'il n'y a pas de torsion
	if (project_controller->matrix->HasTorsion())
	{
		GetError(ERROR_MATRIX_NO_EXPORTABLE);
		return;
	}
	//--

	dialogExport.DoModal();
}

void    CPatternshopView::MenuEditClone()
{
	if (project_controller)
	{
		if (project_controller->image)
		{

			GetError(project_controller->CloneImage());

			PsController::Instance().UpdateDialogProject();
			Update();
		}
		else if (project_controller->matrix)
		{
			MenuMatrixClone();
		}
	}
}

void  CPatternshopView::MenuEditReplace()
{
	char buffer3[1024];
	Get_READ_FORMATS(buffer3);
	PsDlgOpen  dialog(true, 0, 0, 0, _T(buffer3));

	char tmpbuffer[1024];
	strcpy(tmpbuffer, my_StartingDirectory);
	strcat(tmpbuffer, IMAGE_DIRECTORY);
	//OutputDebugString(tmpbuffer);
	dialog.m_ofn.lpstrInitialDir = tmpbuffer;


	if (project_controller && project_controller->image)
	{
		if (dialog.DoModal() == IDOK)
		{

			PsWinProject::Instance().relaseThumb(&(project_controller->image->GetTexture()));
			GetError(project_controller->ReplaceImage(dialog.GetPathName()));

			Update();
			PsController::Instance().UpdateDialogProject();
		}
	}
}

void  CPatternshopView::MenuImageDel()
{
	if (project_controller)
	{
		if (project_controller->image)
		{
			if (GetQuestion(QUESTION_DELETE_IMAGE))
			{
				GetError(project_controller->DelImage());
				Update();
			}
		}
		else if (project_controller->matrix)
		{
			MenuMatrixDel();
		}
	}
}

void  CPatternshopView::MenuLogRedo()
{
	if (project_controller)
	{
		project_controller->LogRedo();
		Update();
		dlgPropreties->UpdateInformation(project_controller);
	}
}

void    CPatternshopView::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
	char  buffer[256];

	if (project_controller->LogCanRedo())
	{
		pCmdUI->Enable(true);
		sprintf(buffer, "&Refaire %s\tCtrl+Y", project_controller->LogRedoLastName());
	}
	else
	{
		pCmdUI->Enable(false);
		strcpy(buffer, "&Refaire\tCtrl+Y");
	}

	pCmdUI->SetText(buffer);
}

void  CPatternshopView::MenuLogUndo()
{
	if (project_controller)
	{
		project_controller->LogUndo();
		Update();
		dlgPropreties->UpdateInformation(project_controller);
	}
}

void    CPatternshopView::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	char  buffer[256];

	if (project_controller->LogCanUndo())
	{
		pCmdUI->Enable(true);
		sprintf(buffer, "&Annuler %s\tCtrl+Z", project_controller->LogUndoLastName());
	}
	else
	{
		pCmdUI->Enable(false);
		strcpy(buffer, "&Annuler\tCtrl+Z");
	}

	pCmdUI->SetText(buffer);
}

void  CPatternshopView::MenuMatrixClone()
{
	if (project_controller)
	{

		GetError(project_controller->CloneMatrix());

		PsController::Instance().UpdateDialogProject();
		Update();
	}
}

void  CPatternshopView::MenuMatrixColor()
{
	if (project_controller)
	{
		GetError(project_controller->MatrixColor());
		Update();
	}
}

void  CPatternshopView::MenuMatrixDel()
{
	if (project_controller && project_controller->matrix)
	{
		if (GetQuestion(QUESTION_DELETE_MATRIX))
		{
			GetError(project_controller->DelMatrix());
			Update();
		}
	}
}

void  CPatternshopView::MenuMatrixNew()
{
	if (project_controller)
	{
		GetError(project_controller->NewMatrix());
		Update();
	}
}

void  CPatternshopView::MenuMatrixReset()
{
	GetError(project_controller->MatrixReset());
	Update();
}

void  CPatternshopView::MenuOptionDocumentBlend()
{
	bool  value = !PsController::Instance().GetOption(PsController::OPTION_DOCUMENT_BLEND, true);
	PsController::Instance().SetOption(PsController::OPTION_DOCUMENT_BLEND, value);
	Update();
}

void CPatternshopView::OnUpdateOptionDocumentBlend(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(PsController::Instance().GetOption(PsController::OPTION_DOCUMENT_BLEND, true));
}

void  CPatternshopView::MenuOptionDocumentShow()
{
	bool  value = !PsController::Instance().GetOption(PsController::OPTION_DOCUMENT_SHOW, true);
	PsController::Instance().SetOption(PsController::OPTION_DOCUMENT_SHOW, value);
	Update();
}

void CPatternshopView::OnUpdateOptionDocumentShow(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(PsController::Instance().GetOption(PsController::OPTION_DOCUMENT_SHOW, true));
}

void  CPatternshopView::MenuOptionHighlightShow()
{
	bool  value = !PsController::Instance().GetOption(PsController::OPTION_HIGHLIGHT_SHOW, true);
	PsController::Instance().SetOption(PsController::OPTION_HIGHLIGHT_SHOW, value);
	Update();
}

void CPatternshopView::OnUpdateOptionHighlightShow(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(PsController::Instance().GetOption(PsController::OPTION_HIGHLIGHT_SHOW, true));
}

// --- EVENTS ---

BOOL CPatternshopView::OnEraseBkgnd(CDC* pDC)
{
	// OGL has his own iColor erasing so tell Windows to skip (avoids flicker)
	return TRUE;
}


void  CPatternshopView::OnDraw(CDC* pDC)
{
	m_buffer.buffer.BitBlt(pDC->GetSafeHdc(), 0, 0);
}

BOOL  CPatternshopView::OnPreparePrinting(CPrintInfo* pInfo)
{
	if (dlg.DoModal() != IDOK)
		return FALSE;
	return DoPreparePrinting(pInfo);
}

void CPatternshopView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	int      x = 0,
		y = 0;
	CImage   printBackbuffer;

	project_controller->renderer.GetDocSize(x, y);
	project_controller->RenderToFile("printer.png", x, y);
	printBackbuffer.Load("printer.png");

	// Get the real print area size
	//-----------------------------
	int horzres = pDC->GetDeviceCaps(HORZRES); // gives the height of the physical display in pixels
	int vertres = pDC->GetDeviceCaps(VERTRES); // gives the width of the physical display in pixels

	// Get the Dpi adjustment ratio
	//-----------------------------
	float ratio = (float)horzres
		/ (float)dlg.format_w;

	printBackbuffer.StretchBlt(pDC->GetSafeHdc(),
		dlg.x * ratio,
		dlg.y * ratio,
		x * ratio * (dlg.z / 100.f),
		y * ratio * (dlg.z / 100.f));
}

void  CPatternshopView::SetRenderSize(int cx, int cy)
{
	if (project_controller)
	{
		project_controller->renderer.SetSize(cx, cy);
	}
}

void CPatternshopView::OnSize(UINT nType, int cx, int cy)
{
	if (0 < cx && 0 < cy)
	{
		CView::OnSize(nType, cx, cy);
		SetRenderSize(cx, cy);
		m_buffer.buffer.Destroy();
		m_buffer.buffer.Create(cx, cy, 24);
		UpdateNow();
	}
}

void  CPatternshopView::OnSetFocus(CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);
	PsController::Instance().SetActive(this);
	Update();
}

void  CPatternshopView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CView::OnLButtonDown(nFlags, point);
	PsController::Instance().SetActive(this);
	PsController::Instance().MouseClick(0, point.x, point.y);
}

void  CPatternshopView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CView::OnLButtonUp(nFlags, point);
	PsController::Instance().MouseRelease(0, point.x, point.y);
}

void  CPatternshopView::OnRButtonDown(UINT nFlags, CPoint point)
{
	CView::OnRButtonDown(nFlags, point);
	PsController::Instance().MouseClick(1, point.x, point.y);
	Update();
}

void  CPatternshopView::OnRButtonUp(UINT nFlags, CPoint point)
{
	CView::OnRButtonDown(nFlags, point);
	PsController::Instance().MouseRelease(1, point.x, point.y);
	Update();
}


void  CPatternshopView::OnMouseMove(UINT nFlags, CPoint point)
{
	CView::OnMouseMove(nFlags, point);
	PsController::Instance().MouseMove(point.x, point.y);
}

void  CPatternshopView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
	PsProjectController* project_controller = PsController::Instance().project_controller;
	switch (nChar)
	{
	case 'V':
		if (PsController::Instance().tool == PsController::TOOL_MAGNIFY
			|| PsController::Instance().tool == PsController::TOOL_SCROLL)
			PsWinTools::Instance().OnBnClickedTool1();
		break;

	case 'Z':
		if (PsController::Instance().tool == PsController::TOOL_MODIFY
			|| PsController::Instance().tool == PsController::TOOL_SCROLL)
			PsWinTools::Instance().OnBnClickedTool3();
		break;

	case VK_SPACE:
		if (PsController::Instance().tool == PsController::TOOL_MAGNIFY
			|| PsController::Instance().tool == PsController::TOOL_MODIFY)
			PsWinTools::Instance().OnBnClickedTool2();
		break;

	case VK_LEFT:
		if (project_controller)
		{
			if (project_controller->image)
			{
				float x, y;
				project_controller->image->GetPosition(x, y);
				project_controller->image->SetPosition(x - 1, y);
			}
			else if (project_controller->matrix)
				project_controller->matrix->SetPosition(project_controller->matrix->x - 1, project_controller->matrix->y);
			Update();
			dlgPropreties->UpdateInformation(project_controller);
		}
		break;

	case VK_RIGHT:
		if (project_controller)
		{
			if (project_controller->image)
			{
				float x, y;
				project_controller->image->GetPosition(x, y);
				project_controller->image->SetPosition(x + 1, y);
			}
			else if (project_controller->matrix)
				project_controller->matrix->SetPosition(project_controller->matrix->x + 1, project_controller->matrix->y);
			Update();
			dlgPropreties->UpdateInformation(project_controller);
		}
		break;

	case VK_UP:
		if (project_controller)
		{
			if (project_controller->image)
			{
				float x, y;
				project_controller->image->GetPosition(x, y);
				project_controller->image->SetPosition(x, y - 1);
			}
			else if (project_controller->matrix)
				project_controller->matrix->SetPosition(project_controller->matrix->x, project_controller->matrix->y - 1);
			Update();
			dlgPropreties->UpdateInformation(project_controller);
		}
		break;

	case VK_DOWN:
		if (project_controller)
		{
			if (project_controller->image)
			{
				float x, y;
				project_controller->image->GetPosition(x, y);
				project_controller->image->SetPosition(x, y + 1);
			}
			else if (project_controller->matrix)
				project_controller->matrix->SetPosition(project_controller->matrix->x, project_controller->matrix->y + 1);
			Update();
			dlgPropreties->UpdateInformation(project_controller);
		}
		break;
	}
}

void  CPatternshopView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CView::OnKeyUp(nChar, nRepCnt, nFlags);

	switch (nChar)
	{
	case VK_SPACE:
	case 'Z':
		switch (PsController::Instance().prev_tool)
		{
		case PsController::TOOL_MODIFY:
		case PsController::TOOL_MODIFY_MOVE:
		case PsController::TOOL_MODIFY_SIZE:
		case PsController::TOOL_MODIFY_TORSIO:
			PsWinTools::Instance().OnBnClickedTool1(); break;
		case PsController::TOOL_SCROLL:
		case PsController::TOOL_SCROLL_DRAG:
			PsWinTools::Instance().OnBnClickedTool2(); break;
		case PsController::TOOL_MAGNIFY:
		case PsController::TOOL_MAGNIFY_ZOOM:
			PsWinTools::Instance().OnBnClickedTool3(); break;
		}
		break;
	}
}

#ifdef _DEBUG
void  CPatternshopView::AssertValid() const
{
	CView::AssertValid();
}

void  CPatternshopView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDocument* CPatternshopView::GetDocument() const
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDocument)));
	return (CDocument*)m_pDocument;
}
#endif //_DEBUG

void CPatternshopView::SetMouseCursor(HCURSOR mcursor)
{
	if (mcursor != NULL)
		m_hMouseCursor = mcursor;
	if (GetFocus() == this)
		OnSetCursor(NULL, 0, 0);
}

BOOL CPatternshopView::OnSetCursor(CWnd* /*pWnd*/, UINT /*nHitTest*/, UINT /*message*/)
{
	ASSERT(m_hMouseCursor != NULL);
	::SetCursor(m_hMouseCursor);
	return TRUE;
}

void CPatternshopView::UpdateNow()
{
	static BOOL bBusy = FALSE;
	if (bBusy || !project_controller || !hardwareRenderer.IsLoad())
		return;

#ifdef _DEBUG
	DWORD dTime = GetTickCount();
#endif /* _DEBUG */

	bBusy = TRUE;
	project_controller->RenderToScreen();
	hardwareRenderer.CopyToSoftBuffer(m_buffer);
	bBusy = FALSE;
	Invalidate(true);

#ifdef _DEBUG
	char message[1024];
	sprintf(message, "Render time: %d\n", GetTickCount() - dTime);
	OutputDebugString(message);
#endif /* _DEBUG */

}

void CPatternshopView::OnTimer(UINT_PTR nIDEvent)
{
	if (!updated)
	{
		PsWinOverview::Instance().Update();
		UpdateNow();
		updated = true;
	}

	if (!OPTION_REFLECT_ALWAYS)
		PsController::Instance().SetOption(PsController::OPTION_REFLECT, ::GetKeyState(VK_CONTROL) < 0);

	PsController::Instance().SetOption(PsController::OPTION_CONSTRAIN, ::GetKeyState(VK_SHIFT) < 0);

	// FIXME: forward save warning
	PsProjectController* project_controller = PsController::Instance().project_controller;
	if (project_controller)
	{
		CPatternshopDoc* pDoc = (CPatternshopDoc*)GetDocument();
		pDoc->SetModifiedFlag(project_controller->bNeedSave);
	}
}

void CPatternshopView::OnDestroy()
{
	PsController::Instance().SetActive(NULL);
	dlgPropreties->UpdateInformation(NULL);
	PsController::Instance().UpdateDialogProject();
}

void CPatternshopView::OnProjetTailledelazonedetravail()
{
	if (project_controller)
	{
		PsDlgDocument dlgDimensions;
		dlgDimensions.bNoDefault = true;
		dlgDimensions.w = project_controller->GetWidth();
		dlgDimensions.h = project_controller->GetHeight();
		dlgDimensions.dpi = project_controller->GetDpi();
		if (dlgDimensions.DoModal() == IDOK)
			SetProjectSize(dlgDimensions.w, dlgDimensions.h, dlgDimensions.dpi);
	}
}
