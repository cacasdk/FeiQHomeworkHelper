#if !defined(AFX_FEIQVERDLG_H__F83DAFBF_A48F_45BF_9864_1230D288A851__INCLUDED_)
#define AFX_FEIQVERDLG_H__F83DAFBF_A48F_45BF_9864_1230D288A851__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FeiQVerDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFeiQVerDlg dialog
#include "Graph.h"
#include "resource.h"
class CFeiQVerDlg : public CDialog
{
// Construction
public:
	CFeiQVerDlg(CWnd* pParent = NULL);   // standard constructor

	CString GetVersionStr(CString strVer, int& nSort);
	IFQRootPtr m_ptrRoot;
	CGraph* m_Graph;
// Dialog Data
	//{{AFX_DATA(CFeiQVerDlg)
	enum { IDD = IDD_DIALOG_FEIQVER };
	CStatic	m_ctrlGraphFrame;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFeiQVerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFeiQVerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FEIQVERDLG_H__F83DAFBF_A48F_45BF_9864_1230D288A851__INCLUDED_)
