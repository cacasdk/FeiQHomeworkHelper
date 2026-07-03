#if !defined(AFX_SENDEDIT_H__63B74869_C09B_4FE4_B5FF_A6E7915B704D__INCLUDED_)
#define AFX_SENDEDIT_H__63B74869_C09B_4FE4_B5FF_A6E7915B704D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSendEdit window

class CSendEdit : public CEdit
{
// Construction
public:
	CSendEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSendEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSendEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDEDIT_H__63B74869_C09B_4FE4_B5FF_A6E7915B704D__INCLUDED_)
