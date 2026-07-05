//GraphPieLabel.cpp - Version 3.0 (Brian Convery, May, 2001)

// GraphPieLabel.cpp : implementation file
//

#include "stdafx.h"
#include "GraphPieLabel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
// CGraphPieLabel

CGraphPieLabel::CGraphPieLabel()
{
	lineXStart = 0;
	lineYStart = 0;
	lineXEnd = 0;
	lineYEnd = 0;
	topLeftX = 0;
	topLeftY = 0;
}

CGraphPieLabel::~CGraphPieLabel()
{
}


BEGIN_MESSAGE_MAP(CGraphPieLabel, CStatic)
	//{{AFX_MSG_MAP(CGraphPieLabel)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphPieLabel message handlers
