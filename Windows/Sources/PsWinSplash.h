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

class PsWinSplash : public CDialog
{
	DECLARE_DYNAMIC(PsWinSplash)

public:
  static PsWinSplash& Instance();
	static void Delete();
	virtual ~PsWinSplash();

private:
	PsWinSplash(CWnd* pParent = NULL);

	enum { IDD = IDD_SPLASH };

private:
  static PsWinSplash* instance; // Singleton
  DWORD m_CreationTime;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnTimer(UINT_PTR nIDEvent);
	afx_msg int   OnCreate(LPCREATESTRUCT lpCreateStruct);
};
