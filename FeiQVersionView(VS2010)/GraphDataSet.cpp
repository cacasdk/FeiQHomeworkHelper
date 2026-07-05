//GraphDataSet.h - Version 3.0 (Brian Convery, May, 2001)

// GraphDataSet.cpp : implementation file
//

#include "stdafx.h"
#include "GraphDataSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGraphDataSet

CGraphDataSet::CGraphDataSet()
{
	xPosition = 0;
	yValue = 0;
}

CGraphDataSet::~CGraphDataSet()
{
}


BEGIN_MESSAGE_MAP(CGraphDataSet, CStatic)
	//{{AFX_MSG_MAP(CGraphDataSet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphDataSet message handlers

void CGraphDataSet::SetXPosition(int x)
{
	xPosition = x;
}

void CGraphDataSet::SetYValue(int y)
{
	yValue = y;
}

int CGraphDataSet::GetXData()
{
	return xPosition;
}

int CGraphDataSet::GetYData()
{
	return yValue;
}
