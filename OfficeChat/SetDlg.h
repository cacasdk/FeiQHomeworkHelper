#if !defined(AFX_SETDLG_H__E52CAC38_55E9_4480_A6B4_7D8A311FDCCA__INCLUDED_)
#define AFX_SETDLG_H__E52CAC38_55E9_4480_A6B4_7D8A311FDCCA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetDlg dialog

class CSetDlg : public CDialog
{
// Construction
public:
	CSetDlg(CWnd* pParent = NULL);   // standard constructor

	LOGFONT fontSend, fontRecv;
	COLORREF clrSend, clrRecv;
	LONG m_nShowNameMode;
// Dialog Data
	//{{AFX_DATA(CSetDlg)
	enum { IDD = IDD_DIALOG_SET };
	CCheckListBox	m_nameShowList;
	int		m_nMsgMode;
	BOOL	m_bOfficeChat;
	int		m_bSendMsgEnter;
	BOOL	m_bSaveWndSize;
	BOOL	m_nUserChangeTip;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetDlg)
	afx_msg void OnButton1();
	afx_msg void OnButton3();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETDLG_H__E52CAC38_55E9_4480_A6B4_7D8A311FDCCA__INCLUDED_)
