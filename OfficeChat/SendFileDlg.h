#if !defined(AFX_SENDFILEDLG_H__354259BD_1B07_44A3_8A42_A1B0380C02C6__INCLUDED_)
#define AFX_SENDFILEDLG_H__354259BD_1B07_44A3_8A42_A1B0380C02C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendFileDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSendFileDlg dialog

class CSendFileDlg : public CDialog
{
// Construction
public:
	BOOL GetFileInfomation(const char *path, WIN32_FIND_DATA *fdata);
	void RefreshList();
	CSendFileDlg(CWnd* pParent = NULL);   // standard constructor

	CStringArray m_strSendFilePathArr;
	CStringArray m_strSendFileNameArr;
// Dialog Data
	//{{AFX_DATA(CSendFileDlg)
	enum { IDD = IDD_DIALOG_FILES };
	CListCtrl	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendFileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendFileDlg)
	afx_msg void OnButton1();
	afx_msg void OnButton3();
	afx_msg void OnButton4();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDFILEDLG_H__354259BD_1B07_44A3_8A42_A1B0380C02C6__INCLUDED_)
