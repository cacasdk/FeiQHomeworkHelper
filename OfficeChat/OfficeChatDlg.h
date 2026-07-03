#if !defined(AFX_OFFICECHATDLG_H__A67C7F7F_50F8_4B88_8676_531436160811__INCLUDED_)
#define AFX_OFFICECHATDLG_H__A67C7F7F_50F8_4B88_8676_531436160811__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OfficeChatDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COfficeChatDlg dialog
#include "resource.h"
#include "listnew.h"
#include "SendEdit.h"

class COfficeChatDlg : public CDialog
{
// Construction
public:
	BOOL GetFileInfomation(const char *path, WIN32_FIND_DATA *fdata);
	void UpdateSize();
	void DelBuddyFromList(CString strIP);
	void AddBuddyToList(CString strMemoName, CString strHostName, CString strLoginName, CString strGroupName, CString strMemoGroupName, CString strIP);
	void RecvMsg(CString strMsg, CString strIP, BOOL bInitSelUser = FALSE);
	COfficeChatDlg(CWnd* pParent = NULL);   // standard constructor

	IFQRootPtr m_ptrRoot;
	CString m_strOnlineIps;
	CStringArray m_strSendFilePathArr;
	CStringArray m_strSendFileNameArr;
	BOOL m_bListBusy;

	CFont m_sendFont, m_recvFont;
	CString strSelfName;

	CString strOldSelUserIP;
// Dialog Data
	//{{AFX_DATA(COfficeChatDlg)
	enum { IDD = IDD_DIALOG_OFFICECHAT };
	CSendEdit	m_sendEdit;
	CListNew	m_usersList;
	CString	m_strChatLog;
	CString	m_strSendUsers;
	CString	m_strSendStatus;
	CString	m_strOnlineMembers;
	CString	m_strContent;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COfficeChatDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COfficeChatDlg)
	virtual void OnOK();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnButton1();
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnButton2();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnButtonSendfile();
	afx_msg void OnSendFiles();
	afx_msg void OnSendDirs();
	afx_msg void OnButtonMore();
	afx_msg void OnSet();
	afx_msg void OnSendCtrlenter();
	afx_msg void OnUpdateSendCtrlenter(CCmdUI* pCmdUI);
	afx_msg void OnSendEnter();
	afx_msg void OnUpdateSendEnter(CCmdUI* pCmdUI);
	afx_msg void OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu);
	afx_msg void OnSendUserchangetip();
	afx_msg void OnUpdateSendUserchangetip(CCmdUI* pCmdUI);
	afx_msg void OnSendMode();
	afx_msg void OnUpdateSendMode(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OFFICECHATDLG_H__A67C7F7F_50F8_4B88_8676_531436160811__INCLUDED_)
