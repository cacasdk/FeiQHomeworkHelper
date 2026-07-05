//GraphDataSet.h - Version 3.0 (Brian Convery, May, 2001)

#if !defined(AFX_GRAPHDATASET_H__3CBDC4C1_494F_11D5_AD40_006008198224__INCLUDED_)
#define AFX_GRAPHDATASET_H__3CBDC4C1_494F_11D5_AD40_006008198224__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GraphDataSet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGraphDataSet window

class CGraphDataSet : public CStatic
{
// Construction
public:
	CGraphDataSet();

// Attributes
public:

private :
	int xPosition;
	int yValue;

// Operations
public:
	void SetXPosition(int x);
	void SetYValue(int y);
	int GetXData();
	int GetYData();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphDataSet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGraphDataSet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGraphDataSet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPHDATASET_H__3CBDC4C1_494F_11D5_AD40_006008198224__INCLUDED_)
