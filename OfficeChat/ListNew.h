#if !defined(AFX_LISTNEW_H__72E264B5_43C8_11D9_95EE_00E04CB27B6F__INCLUDED_)
#define AFX_LISTNEW_H__72E264B5_43C8_11D9_95EE_00E04CB27B6F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListNew.h : header file

/////////////////////////////////////////////////////////////////////////////
// CListNew window

#include "afxtempl.h"

class CListNew : public CListCtrl
{
// Construction
public:
	CListNew();

// Attributes
public:
	int m_nItem;           //Current Row
	int m_nSubItem;        //Current Col

protected:

// Operations
public:
	void SetListColumnWidth(CArray<int, int> &colWidthArr);
	void SetCurrentRow(int nItem);
	void SetCurrentCol(int nCol);
	int GetCurrentRow();
	int GetCurrentCol();
	void SetDefaultTitle(CString strListColStyle = "123456");

	void SetItemColumnValueA(int nItem,int nSubItem,char *cText[]);
	void SetItemValue(int nItem);
	void SetItemColumnValue(int nItem,int nSubItem,LPCTSTR szText);
	void SetItemColumnValueB(int nItem,int nSubItem,LPTSTR cText[]);
	void SetItemColumnValueC(int nItem,int nSubItem,CStringArray &textArr);
	//void SetAutoSize(int nStartNum,int nCount);
    void SetAutoSize();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListNew)
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	int GetCurColItemPos(int nOldPos);
	CStringArray strColTitleArr;
	BOOL SetListStyle();
	virtual ~CListNew();

	// Generated message map functions
protected:
	//{{AFX_MSG(CListNew)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTNEW_H__72E264B5_43C8_11D9_95EE_00E04CB27B6F__INCLUDED_)
