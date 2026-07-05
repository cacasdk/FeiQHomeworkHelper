//GraphDataColor.cpp - Version 3.0 (Brian Convery, May, 2001)

// GraphDataColor.cpp : implementation file
//

#include "stdafx.h"
#include "Graph.h"
#include "GraphDataColor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGraphDataColor

CGraphDataColor::CGraphDataColor(int group, COLORREF color)
{
	dataGroup = group;
	groupColor = color;
}

CGraphDataColor::~CGraphDataColor()
{
}


BEGIN_MESSAGE_MAP(CGraphDataColor, CStatic)
	//{{AFX_MSG_MAP(CGraphDataColor)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphDataColor message handlers

COLORREF CGraphDataColor::GetColor()
{
	return groupColor;
}
