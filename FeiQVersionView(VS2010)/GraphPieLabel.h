//GraphPieLabel.h - Version 3.0 (Brian Convery, May, 2001)

#if !defined(AFX_GRAPHPIELABEL_H__0A2648C1_5062_11D5_AD42_006008198224__INCLUDED_)
#define AFX_GRAPHPIELABEL_H__0A2648C1_5062_11D5_AD42_006008198224__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GraphPieLabel.h : header file
//

/******************************/
/*The CGraphPieLabel class    */
/*  defines the rectangular   */
/*  region for which the label*/
/*  will be drawn in.  This   */
/*  way, we can avoid having  */
/*  2 boxes overlaying each   */
/*  other.  This check is done*/
/*  in CGraph::DrawPieSeries()*/
/*  before setting topLeft    */
/*  values.                   */
/******************************/

/////////////////////////////////////////////////////////////////////////////
// CGraphPieLabel window

class CGraphPieLabel : public CStatic
{
// Construction
public:
	CGraphPieLabel();

// Attributes
public:
	int lineXStart;
	int lineYStart;
	int lineXEnd;
	int lineYEnd;
	int topLeftX;
	int topLeftY;
	int labelQuadrant;
	CString pieLabel;	//i.e.  30% - actual percentile

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphPieLabel)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGraphPieLabel();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGraphPieLabel)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPHPIELABEL_H__0A2648C1_5062_11D5_AD42_006008198224__INCLUDED_)
