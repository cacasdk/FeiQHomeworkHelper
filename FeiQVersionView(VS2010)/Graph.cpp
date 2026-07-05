//Graph.cpp - Version 3.0 (Brian Convery, May, 2001)

#include "stdafx.h"
#include "afxtempl.h"
#include "GraphSeries.h"
#include "GraphLegend.h"
#include "math.h"
#include "Graph.h"
#include "GraphDataColor.h"
#include "GraphPieLabel.h"	//for pie labels

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGraph

CGraph::CGraph()
{
	graphSeries = new CObList();
	seriesSize = 0;
	minTick = 0;
	maxTick = 100;
	numTicks = 10;
	tickSpace = 10;

	graphHasLegend = FALSE;
	legendWidth = 0;
	legendMaxText= 0;
	graphAlignment = VERTICAL_ALIGN;  

	graphType = 0;

	xAxisAlign = 0;  //horizontal
	xAxisLabelLength = 0;

	xTickFontSize = 12;
	yTickFontSize = 12;
	legendFontSize = 12;

	depth = 0;
	depthRatio = 0.05;

	line3DXBase = 0;
	line3DYBase = 0;

	SetGridLines(FALSE);

	graphQuadType = 1;
	quadSetManually = FALSE;
}

CGraph::CGraph(int type)
{
	graphSeries = new CObList();
	colorList = new CObList();
	seriesSize = 0;
	minTick = 0;
	maxTick = 100;
	numTicks = 10;
	tickSpace = 10;

	graphHasLegend = FALSE;
	legendWidth = 0;
	legendMaxText= 0;
	graphAlignment = VERTICAL_ALIGN;  

	graphType = type;

	xAxisAlign = 0;  //in degrees == horizontal
	xAxisLabelLength = 0;

	xTickFontSize = 12;
	yTickFontSize = 12;
	legendFontSize = 12;

	depth = 0;
	depthRatio = 0.05;

	line3DXBase = 0;
	line3DYBase = 0;

	SetGridLines(FALSE);

	graphQuadType = 1;
	if(type != BAR_GRAPH)
		quadSetManually = TRUE;
	else
		quadSetManually = FALSE;
}

CGraph::~CGraph()
{
	POSITION pos;
	CGraphSeries* pSeries;

	for( pos = graphSeries->GetHeadPosition(); pos != NULL; )
	{
		pSeries = (CGraphSeries*) graphSeries->GetNext( pos );
		graphSeries->RemoveAt(pos);
		delete pSeries;
	}

	graphSeries->RemoveAll();
	delete graphSeries;

	CGraphDataColor* dataColor;
	for( pos = colorList->GetHeadPosition(); pos != NULL; )
	{
		dataColor = (CGraphDataColor*) colorList->GetNext( pos );
		colorList->RemoveAt(pos);
		delete dataColor;
	}

	colorList->RemoveAll();
	delete colorList;
	
}


BEGIN_MESSAGE_MAP(CGraph, CStatic)
	//{{AFX_MSG_MAP(CGraph)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraph message handlers

void CGraph::SetGraphType(int gType)
{
	graphType = gType;
}

void CGraph::SetXAxisAlignment(int alignValue)
{
	xAxisAlign = alignValue;
}

int CGraph::GetXAxisAlignment()
{
	return xAxisAlign;
}

void CGraph::SetColor(int dataGroup, COLORREF groupColor)
{
	CGraphDataColor *dataColor = new CGraphDataColor(dataGroup, groupColor);
	colorList->AddTail(dataColor);
}

COLORREF CGraph::GetColor(int dataGroup)
{
	POSITION pos;
	if((colorList->GetCount() <= dataGroup) || (colorList->GetCount() == 0))
		return BLACK;
	
	CGraphDataColor* dataColor;
	pos = colorList->GetHeadPosition();
	for(int i = 0; i < dataGroup; i++)
		colorList->GetNext(pos);

	dataColor = (CGraphDataColor*) colorList->GetAt( pos );
	return dataColor->GetColor();	
}

void CGraph::DrawGraph(CDC* pDC)
{
	CString tickLabel;
	CWnd* graphWnd = pDC->GetWindow();
	CRect graphRect;
	graphWnd->GetClientRect(&graphRect);
	TEXTMETRIC	tm;

	//reset graph to be clear background
	COLORREF backColor;
	backColor = RGB(255,255,255);  //replace with desired background color
	CBrush backBrush (backColor);
	CBrush* pOldBackBrush;
	pOldBackBrush = pDC->SelectObject(&backBrush);
	pDC->Rectangle(0, 0, graphRect.Width(), graphRect.Height());
	pDC->SelectObject(pOldBackBrush);

	maxHeight = graphRect.Height() - 20;  //for frame window and status bar
	maxWidth = graphRect.Width() - 5;  //for frame window

	//draw graph title
	CFont titleFont;
	int tFontSize = 24;
	while((graphTitle.GetLength() * (tFontSize / 2)) > maxWidth)
	{
		tFontSize -= 2;
	}
	titleFont.CreateFont(tFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
	CFont* pOldFont = (CFont*) pDC->SelectObject(&titleFont);

	pDC->GetTextMetrics(&tm);

	int charWidth = tm.tmAveCharWidth;
	//next line is centered....trust me !!
	pDC->TextOut((graphRect.Width() / 2) - ((graphTitle.GetLength() / 2) * charWidth),
				10, graphTitle);
	pDC->SelectObject(pOldFont);

	if((graphType == PIE_GRAPH) || (graphType == PIE_GRAPH_3D))
	{

		//since pie does not have axis lines, set to full size minus 5 on each side
		//these are needed for legend to plot itself
		xAxisWidth = maxWidth - 10;
		yAxisHeight = maxHeight - 50;  //10 buffer, 20 for title, and 20 for series label
		xApexPoint = 5;
		yApexPoint = maxHeight - 5;
	}
	else
	{
		//compute label size for axis alignment
		CFont sideFont, axisFont;
		int tFontSize = 16;
		if(maxWidth > maxHeight)
		{
			while((axisYLabel.GetLength() * (tFontSize / 2)) > maxHeight)
			{
				tFontSize -= 2;
			}
		}
		else
		{
			while((axisXLabel.GetLength() * (tFontSize / 2)) > maxWidth)
			{
				tFontSize -= 2;
			}
		}
		axisFont.CreateFont(tFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
			ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
		sideFont.CreateFont(tFontSize, 0, 900, 0, 700, FALSE, FALSE, 0,
			ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));

		CFont* pOldFont = (CFont*) pDC->SelectObject(&sideFont);
		pDC->GetTextMetrics(&tm);
		int charWidth = tm.tmAveCharWidth;
		int labelHeight = tm.tmHeight + 10;
		if(graphAlignment)
		{
			if(axisYLabel.GetLength() == 0)
				labelHeight = 0;
		}
		else
		{
			if(axisXLabel.GetLength() == 0)
				labelHeight = 0;
		}
		pDC->SelectObject(pOldFont);

		tickLabel.Format(_T("%d"), maxTick);

		//determine axis specifications 
		if(graphAlignment)
		{
			if(graphType == SCATTER_GRAPH)
			{
				xApexPoint = 5 + (xAxisLabelLength * charWidth) + labelHeight; //labelHeight added for y-axis label height
				yApexPoint = (maxHeight - 15) - tm.tmHeight;
				yAxisHeight = yApexPoint - 40;
				xAxisWidth = (maxWidth - 5) - xApexPoint;
			}
			else
			{
				xApexPoint = 5 + (tickLabel.GetLength() * (yTickFontSize / 2)) + 45; 
				if(!xAxisAlign)  //horizontal
					yApexPoint = (maxHeight - 5) - 45;		//apex points are the cross section of axis lines
				else
					yApexPoint = (maxHeight - 5) - (xAxisLabelLength * (xTickFontSize / 2)) - 10;
				yAxisHeight = yApexPoint - 40;
				xAxisWidth = (maxWidth - 5) - xApexPoint;
			}
		}
		else
		{
			xApexPoint = 5 + (xAxisLabelLength * charWidth) + labelHeight; //labelHeight added for y-axis label height
			yApexPoint = (maxHeight - 15) - tm.tmHeight;
			yAxisHeight = yApexPoint - 40;
			xAxisWidth = (maxWidth - 5) - xApexPoint;
		}
	}

	//draw legend
	if(graphHasLegend)
	{
		int legendLeft = DrawLegend(pDC);
		xAxisWidth = legendLeft - 10 - xApexPoint;
	}

	if((!quadSetManually) && (graphType == BAR_GRAPH))
	{
		//computer number of quadrants needed based on data
		POSITION pos;
		pos = graphSeries->GetHeadPosition();
		CGraphSeries* tmpSeries;
		int minXValue = 0;
		int minYValue = 0;
		for(int x = 1; x <= graphSeries->GetCount(); x++)
		{
			tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);
			for(int s = 0; s < seriesSize; s++)
			{
				//to allow scalability (height may be less than tickRange)
				int curXValue = tmpSeries->GetXDataValue();
				int curYValue = tmpSeries->GetYDataValue();
				if(curXValue < minXValue)
					minXValue = curXValue;
				if(curYValue < minYValue)
					minYValue = curYValue;
			}
		}
		graphQuadType = 1;
		if((minXValue < 0) && (minYValue < 0))
			graphQuadType = 4;
		if((minXValue < 0) && (minYValue >= 0))
			graphQuadType = 2;
		if((minXValue >= 0) && (minYValue < 0))
			graphQuadType = 3;
	}

	if(graphType == BAR_GRAPH)
	{
		switch(graphQuadType)
		{
			case 2 :
				xApexPoint += xAxisWidth / 2;
				break;
			case 3 :
				yApexPoint -= yAxisHeight / 2;
				break;
			case 4 :
				xApexPoint += xAxisWidth / 2;
				yApexPoint -= yAxisHeight / 2;
				break;
		}
	}

	if((graphType != 2) && (graphType != 32))  //pie & 3d pie
	{
		inRedraw = FALSE;
		//draw axis lines
		DrawAxis(pDC);
	}

	//draw series data and labels
	DrawSeries(pDC);

	//lines below are commented for doing redraw.  If you
	//want the axis lines on top of the other graph elements
	//uncomment this functionality...I may add it back later
	//but there were some bugs doing it this way.

	//redraw axis lines in case graph elements overwrote the axis lines
//	if((graphType != 2) && (graphType != 32) && (graphQuadType > 1))  //pie & 3d pie
//	{
//		inRedraw = TRUE;
		//draw axis lines
//		DrawAxis(pDC);
//	}
}

void CGraph::DrawAxis(CDC* pDC)
{
	TEXTMETRIC	tm;
	pDC->SetTextColor(BLACK);
	CFont sideFont, axisFont, dummyFont;
	int charWidth, labelHeight;
	double tickScale;
	int tickXLocation, tickYLocation;
	int seriesSpace;
	CFont* pOldFont;
	int x, y;

	COLORREF axisLineColor;
	axisLineColor = BLACK;
	CPen axisPen (PS_SOLID, 2, axisLineColor);
	dummyFont.CreateFont(12, 0, 0, 0, 700, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
	pOldFont = (CFont*)pDC->SelectObject(&dummyFont);
	switch(graphType)
	{
		case BAR_GRAPH :
		case LINE_GRAPH :
		case SCATTER_GRAPH :
		case BOX_WHISKER_GRAPH :
//		case RADAR_GRAPH :
		case STACKED_BAR_GRAPH :
		case XY_LINE_GRAPH :
				CPen* pOldAxisPen;
				pOldAxisPen = pDC->SelectObject(&axisPen);
				switch(graphQuadType)
				{
					case 1 :
						//draw y axis
						pDC->MoveTo(xApexPoint, yApexPoint);  
						pDC->LineTo(xApexPoint, yApexPoint - yAxisHeight);

						//draw x axis
						pDC->MoveTo(xApexPoint, yApexPoint);  
						pDC->LineTo(xApexPoint + xAxisWidth, yApexPoint);
						break;
					case 2 :
						//draw y axis
						pDC->MoveTo(xApexPoint, yApexPoint);  
						pDC->LineTo(xApexPoint, yApexPoint - yAxisHeight);

						//draw x axis
						pDC->MoveTo(xApexPoint - (xAxisWidth / 2), yApexPoint);  
						pDC->LineTo(xApexPoint + (xAxisWidth / 2), yApexPoint);
						break;
					case 3 :
						//draw y axis
						pDC->MoveTo(xApexPoint, yApexPoint + (yAxisHeight / 2));  
						pDC->LineTo(xApexPoint, yApexPoint - (yAxisHeight / 2));

						//draw x axis
						pDC->MoveTo(xApexPoint, yApexPoint);  
						pDC->LineTo(xApexPoint + xAxisWidth, yApexPoint);
						break;
					case 4 :
						//draw y axis
						pDC->MoveTo(xApexPoint, yApexPoint + (yAxisHeight / 2));  
						pDC->LineTo(xApexPoint, yApexPoint - (yAxisHeight / 2));

						//draw x axis
						pDC->MoveTo(xApexPoint - (xAxisWidth / 2), yApexPoint);  
						pDC->LineTo(xApexPoint + (xAxisWidth / 2), yApexPoint);
						break;
				}
				pDC->SelectObject(pOldAxisPen);

				int tFontSize;
				//draw labels
				tFontSize = 16;
				if(maxWidth > maxHeight)
				{
					while((axisYLabel.GetLength() * (tFontSize / 2)) > maxHeight)
					{
						tFontSize -= 2;
					}
				}
				else
				{
					while((axisXLabel.GetLength() * (tFontSize / 2)) > maxWidth)
					{
						tFontSize -= 2;
					}
				}
				axisFont.CreateFont(tFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
					ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
				sideFont.CreateFont(tFontSize, 0, 900, 0, 700, FALSE, FALSE, 0,
					ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));

				pDC->SelectObject(&sideFont);
				pDC->GetTextMetrics(&tm);
				charWidth = tm.tmAveCharWidth;

				if(graphAlignment)
				{
					switch(graphQuadType)
					{
						case 1 :
						case 2 :
							pDC->TextOut(10, (yApexPoint - (yAxisHeight / 2)) + ((axisYLabel.GetLength() * charWidth) / 2), axisYLabel);
							break;
						case 3 :
						case 4 :
							pDC->TextOut(10, yApexPoint + ((axisYLabel.GetLength() * charWidth) / 2), axisYLabel);
							break;
					}


					pDC->SelectObject(&axisFont);
					pDC->GetTextMetrics(&tm);
					charWidth = tm.tmAveCharWidth;
					switch(graphQuadType)
					{
						case 1 :
							pDC->TextOut(xApexPoint + (xAxisWidth / 2) - ((axisXLabel.GetLength() / 2) * charWidth), maxHeight - 5 - 6, axisXLabel);
							break;
						case 2 :
							pDC->TextOut(xApexPoint - ((axisXLabel.GetLength() / 2) * charWidth), maxHeight - 5 - 6, axisXLabel);
							break;
						case 3 :
							pDC->TextOut(xApexPoint + (xAxisWidth / 2) - ((axisXLabel.GetLength() / 2) * charWidth), maxHeight - 5 - 6, axisXLabel);
							break;
						case 4 :
							pDC->TextOut(xApexPoint - ((axisXLabel.GetLength() / 2) * charWidth), maxHeight - 5 - 6, axisXLabel);
							break;
					}

					
					pDC->SelectObject(pOldFont);

					tickScale = 0.00;
					tickScale = (yAxisHeight - 10) / numTicks;

					//draw y axis ticks
					if(!inRedraw)
					{
						switch(graphQuadType)
						{
							case 1 :
								for(y = 1; y <= numTicks; y++) 
								{
									tickYLocation = (int)(yApexPoint - (y * tickScale));

									//draw tick mark
									pDC->MoveTo(xApexPoint - 3, tickYLocation);
									pDC->LineTo(xApexPoint + 3, tickYLocation);

									if(graphHasGridLines)
									{
										//draw grid lines
										COLORREF gridLineColor;
										gridLineColor = DARK_GREY;
										CPen gridPen (PS_SOLID, 1, gridLineColor);
										CPen* pOldPen;
										pOldPen = pDC->SelectObject(&gridPen);
										pDC->MoveTo(xApexPoint, tickYLocation);
										pDC->LineTo(xApexPoint + xAxisWidth, tickYLocation);
										pDC->SelectObject(pOldPen);
									}

									//draw tick label
									CString tickLabel;
									tickLabel.Format(_T("%d"), minTick + (y * tickSpace));

									CFont yFont;
									yFont.CreateFont(yTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
										ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
										DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
									pDC->SelectObject(&yFont);

									pDC->TextOut(xApexPoint - 10 - (tickLabel.GetLength() * (yTickFontSize / 2)), tickYLocation - 6, tickLabel);
									pDC->SelectObject(pOldFont);
									topYTick = tickYLocation;
								}
								break;
								pDC->SelectObject(pOldFont);
							case 2 :
								if(graphType == SCATTER_GRAPH)
								{
									double oldTickScale;
									int oldNumTicks;
									oldNumTicks = numTicks;
									oldTickScale = tickScale;

									numTicks /= 2;
									tickScale *= 2;
									for(y = 1; y <= numTicks; y++)  
									{
										tickYLocation = (int)(yApexPoint - (y * tickScale));

										//draw tick mark
										pDC->MoveTo(xApexPoint - 3, tickYLocation);
										pDC->LineTo(xApexPoint + 3, tickYLocation);

										if(graphHasGridLines)
										{
											//draw grid lines
											COLORREF gridLineColor;
											gridLineColor = LIGHT_GREY;
											CPen gridPen (PS_SOLID, 1, gridLineColor);
											CPen* pOldPen;
											pOldPen = pDC->SelectObject(&gridPen);
											pDC->MoveTo(xApexPoint - (xAxisWidth / 2), tickYLocation);
											pDC->LineTo(xApexPoint + (xAxisWidth / 2), tickYLocation);
											pDC->SelectObject(pOldPen);
										}

										//draw tick label
										CString tickLabel;
										tickLabel.Format(_T("%d"), minTick + (y * tickSpace));

										CFont yFont;
										yFont.CreateFont(yTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&yFont);

										pDC->TextOut(xApexPoint - (xAxisWidth / 2) - 10 - (tickLabel.GetLength() * (yTickFontSize / 2)), tickYLocation - 6, tickLabel);
										pDC->SelectObject(pOldFont);
										topYTick = tickYLocation;
									}
									//draw the 0
									pDC->TextOut(xApexPoint - (xAxisWidth / 2) - 10 - (yTickFontSize / 2), yApexPoint - 6, "0");

									//reset numTicks and tickScale back to normal
									tickScale = oldTickScale;
									numTicks = oldNumTicks;
								}
								else
								{
									for(y = 1; y <= numTicks; y++)  
									{
										tickYLocation = (int)(yApexPoint - (y * tickScale));

										//draw tick mark
										pDC->MoveTo(xApexPoint - 3, tickYLocation);
										pDC->LineTo(xApexPoint + 3, tickYLocation);

										if(graphHasGridLines)
										{
											//draw grid lines
											COLORREF gridLineColor;
											gridLineColor = DARK_GREY;
											CPen gridPen (PS_SOLID, 1, gridLineColor);
											CPen* pOldPen;
											pOldPen = pDC->SelectObject(&gridPen);
											pDC->MoveTo(xApexPoint - (xAxisWidth / 2), tickYLocation);
											pDC->LineTo(xApexPoint + (xAxisWidth / 2), tickYLocation);
											pDC->SelectObject(pOldPen);
										}

										//draw tick label
										CString tickLabel;
										tickLabel.Format(_T("%d"), minTick + (y * tickSpace));

										CFont yFont;
										yFont.CreateFont(yTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&yFont);

										pDC->TextOut(xApexPoint - (xAxisWidth / 2) - 10 - (tickLabel.GetLength() * (yTickFontSize / 2)), tickYLocation - 6, tickLabel);
										topYTick = tickYLocation;

									}
									//draw the 0
									pDC->TextOut(xApexPoint - (xAxisWidth / 2) - 10 - (yTickFontSize / 2), yApexPoint - 6, "0");
								}
								pDC->SelectObject(pOldFont);
								break;
							case 3 :
								for(y = 0; y <= numTicks / 2; y++) 
								{
									tickYLocation = (int)(yApexPoint + 5 - (yAxisHeight / 2) + (y * tickScale));

									//draw tick mark
									pDC->MoveTo(xApexPoint - 3, tickYLocation);
									pDC->LineTo(xApexPoint + 3, tickYLocation);

									if(graphHasGridLines)
									{
										//draw grid lines
										COLORREF gridLineColor;
										gridLineColor = DARK_GREY;
										CPen gridPen (PS_SOLID, 1, gridLineColor);
										CPen* pOldPen;
										pOldPen = pDC->SelectObject(&gridPen);
										pDC->MoveTo(xApexPoint, tickYLocation);
										pDC->LineTo(xApexPoint, tickYLocation);
										pDC->SelectObject(pOldPen);
									}

									//draw tick label
									CString tickLabel;
									tickLabel.Format(_T("%d"), maxTick - (y * tickSpace));

									CFont yFont;
									yFont.CreateFont(yTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
										ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
										DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
									pDC->SelectObject(&yFont);

									pDC->TextOut(xApexPoint - 10 - (tickLabel.GetLength() * (yTickFontSize / 2)), tickYLocation - 6, tickLabel);
									topYTick = tickYLocation;

								}
								//draw the 0
								pDC->TextOut(xApexPoint - (xAxisWidth / 2) - 10 - (yTickFontSize / 2), yApexPoint - 6, "0");
								
								for(; y <= numTicks; y++) 
								{
									tickYLocation = (int)(yApexPoint + 5 - (yAxisHeight / 2) + (y * tickScale));

									//draw tick mark
									pDC->MoveTo(xApexPoint - 3, tickYLocation);
									pDC->LineTo(xApexPoint + 3, tickYLocation);

									if(graphHasGridLines)
									{
										//draw grid lines
										COLORREF gridLineColor;
										gridLineColor = DARK_GREY;
										CPen gridPen (PS_SOLID, 1, gridLineColor);
										CPen* pOldPen;
										pOldPen = pDC->SelectObject(&gridPen);
										pDC->MoveTo(xApexPoint, tickYLocation);
										pDC->LineTo(xApexPoint, tickYLocation);
										pDC->SelectObject(pOldPen);
									}

									//draw tick label
									CString tickLabel;
									tickLabel.Format(_T("%d"), minTick + ((numTicks - y) * tickSpace));

									pDC->TextOut(xApexPoint - 10 - (tickLabel.GetLength() * (yTickFontSize / 2)), tickYLocation - 6, tickLabel);
									topYTick = tickYLocation;

								}
								pDC->SelectObject(pOldFont);
								break;
							pDC->SelectObject(pOldFont);
							case 4 :
								for(y = 1; y <= numTicks / 2; y++) 
								{
									tickYLocation = (int)(yApexPoint - (y * tickScale));

									//draw tick mark
									pDC->MoveTo(xApexPoint - 3, tickYLocation);
									pDC->LineTo(xApexPoint + 3, tickYLocation);

									if(graphHasGridLines)
									{
										//draw grid lines
										COLORREF gridLineColor;
										gridLineColor = DARK_GREY;
										CPen gridPen (PS_SOLID, 1, gridLineColor);
										CPen* pOldPen;
										pOldPen = pDC->SelectObject(&gridPen);
										pDC->MoveTo(xApexPoint - (xAxisWidth / 2), tickYLocation);
										pDC->LineTo(xApexPoint + (xAxisWidth / 2), tickYLocation);
										pDC->SelectObject(pOldPen);
									}

									//draw tick label
									CString tickLabel;
									tickLabel.Format(_T("%d"), minTick + (y * tickSpace));

									CFont yFont;
									yFont.CreateFont(yTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
										ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
										DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
									pDC->SelectObject(&yFont);

									pDC->TextOut(xApexPoint - (xAxisWidth / 2) - 10 - (tickLabel.GetLength() * (yTickFontSize / 2)), tickYLocation - 6, tickLabel);
									topYTick = tickYLocation;

								}
								//draw the 0
								pDC->TextOut(xApexPoint - (xAxisWidth / 2) - 10 - (yTickFontSize / 2), yApexPoint - 6, "0");
								
								for(; y <= numTicks; y++)  
								{
									tickYLocation = (int)(yApexPoint - (yAxisHeight / 2) + (y * tickScale));

									//draw tick mark
									pDC->MoveTo(xApexPoint - 3, tickYLocation);
									pDC->LineTo(xApexPoint + 3, tickYLocation);

									if(graphHasGridLines)
									{
										//draw grid lines
										COLORREF gridLineColor;
										gridLineColor = DARK_GREY;
										CPen gridPen (PS_SOLID, 1, gridLineColor);
										CPen* pOldPen;
										pOldPen = pDC->SelectObject(&gridPen);
										pDC->MoveTo(xApexPoint - (xAxisWidth / 2), tickYLocation);
										pDC->LineTo(xApexPoint + (xAxisWidth / 2), tickYLocation);
										pDC->SelectObject(pOldPen);
									}

									//draw tick label
									CString tickLabel;
									tickLabel.Format(_T("%d"), minTick + ((numTicks - y) * tickSpace));

									pDC->TextOut(xApexPoint - (xAxisWidth / 2) - 10 - (tickLabel.GetLength() * (yTickFontSize / 2)), tickYLocation - 6, tickLabel);
									topYTick = tickYLocation;

								}
								pDC->SelectObject(pOldFont);
								break;
						}
					}

					//draw X axis tick marks
					switch(graphQuadType)
					{
						case 1 :
							if(graphType != SCATTER_GRAPH)
							{
								POSITION pos;
								pos = graphSeries->GetHeadPosition();
								CGraphSeries* tmpSeries;
								for(int x = 1; x <= graphSeries->GetCount(); x++)
								{
									tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

									seriesSpace = xAxisWidth / graphSeries->GetCount();
									tickXLocation = xApexPoint + ((x * seriesSpace) - (seriesSpace / 2));

									pDC->MoveTo(tickXLocation,yApexPoint - 3);
									pDC->LineTo(tickXLocation,yApexPoint + 3);

									//draw tick label
									CString tickLabel;
									CFont xFont;
									xFont.CreateFont(xTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
										ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
										DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
									pDC->SelectObject(&xFont);
									tickLabel = tmpSeries->GetLabel();
									if(!xAxisAlign)  //horizontal
										pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * 8) / 2), yApexPoint + 8, tickLabel);
									else
									{
										CFont sideFont2;
										sideFont2.CreateFont(xTickFontSize, 0, (xAxisAlign * 10), 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&sideFont2);
										pDC->GetTextMetrics(&tm);
										charWidth = tm.tmAveCharWidth;
										if(xAxisAlign < 180)
											pDC->TextOut(tickXLocation - 8, yApexPoint + 8 + (xAxisLabelLength * charWidth), tickLabel);
										else
											pDC->TextOut(tickXLocation + 2, yApexPoint + 8, tickLabel);

										pDC->SelectObject(pOldFont);
									}
									rightXTick = tickXLocation;
								}
							}
							else
							{
								//scatter graphs will use the same tick marking on both axis lines
								seriesSpace = (xAxisWidth - 20) / numTicks;
								for(int x = 1; x <= numTicks; x++)  
								{
									tickXLocation = xApexPoint + (x * seriesSpace);

									//draw tick mark
									pDC->MoveTo(tickXLocation, yApexPoint - 3);
									pDC->LineTo(tickXLocation, yApexPoint + 3);

									//draw tick label
									CString tickLabel;
									CFont xFont;
									tickLabel.Format(_T("%d"), x * tickSpace);

									xFont.CreateFont(xTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
										ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
										DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
									pDC->SelectObject(&xFont);
									pDC->GetTextMetrics(&tm);
									charWidth = tm.tmAveCharWidth;

									pDC->TextOut(tickXLocation - (tickLabel.GetLength() * charWidth / 2), yApexPoint + 8, tickLabel);

									rightXTick = tickXLocation;
									pDC->SelectObject(pOldFont);
								}

							}
							break;
						case 2 :
							if(graphType != SCATTER_GRAPH)
							{
								POSITION pos;
								pos = graphSeries->GetHeadPosition();
								CGraphSeries* tmpSeries;
								seriesSpace = xAxisWidth / graphSeries->GetCount();
								for(x = 1; x <= graphSeries->GetCount() / 2; x++)
								{
									tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

									tickXLocation = xApexPoint - (xAxisWidth / 2) + ((x * seriesSpace) - (seriesSpace / 2));

									pDC->MoveTo(tickXLocation,yApexPoint - 3);
									pDC->LineTo(tickXLocation,yApexPoint + 3);

									if(!inRedraw)
									{
										//draw tick label
										CString tickLabel;
										tickLabel = tmpSeries->GetLabel();
										if(!xAxisAlign)  //horizontal
										{
											CFont xFont;
											xFont.CreateFont(xTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
												ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
												DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
											pDC->SelectObject(&xFont);
											pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * 8) / 2), yApexPoint + 8, tickLabel);
											pDC->SelectObject(pOldFont);
										}
										else
										{
											CFont sideFont2;
											sideFont2.CreateFont(xTickFontSize, 0, (xAxisAlign * 10), 0, 700, FALSE, FALSE, 0,
												ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
												DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
											pDC->SelectObject(&sideFont2);
											pDC->GetTextMetrics(&tm);
											charWidth = tm.tmAveCharWidth;
											if(xAxisAlign < 180)
												pDC->TextOut(tickXLocation - 8, yApexPoint + 8 + (xAxisLabelLength * charWidth), tickLabel);
											else
												pDC->TextOut(tickXLocation + 2, yApexPoint + 8, tickLabel);

											pDC->SelectObject(pOldFont);
										}
									}
									rightXTick = tickXLocation;
								}
								if((graphSeries->GetCount() % 2) > 0)
								{
									tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);
									
									tickXLocation = xApexPoint;

									pDC->MoveTo(tickXLocation,yApexPoint - 3);
									pDC->LineTo(tickXLocation,yApexPoint + 3);

									if(!inRedraw)
									{
										//draw tick label
										CString tickLabel;
										tickLabel = tmpSeries->GetLabel();
										if(!xAxisAlign)  //horizontal
										{
											CFont xFont;
											xFont.CreateFont(xTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
												ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
												DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
											pDC->SelectObject(&xFont);
											pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * 8) / 2), yApexPoint + 8, tickLabel);
											pDC->SelectObject(pOldFont);
										}
										else
										{
											CFont sideFont2;
											sideFont2.CreateFont(xTickFontSize, 0, (xAxisAlign * 10), 0, 700, FALSE, FALSE, 0,
												ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
												DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
											pDC->SelectObject(&sideFont2);
											pDC->GetTextMetrics(&tm);
											charWidth = tm.tmAveCharWidth;
											if(xAxisAlign < 180)
												pDC->TextOut(tickXLocation - 8, yApexPoint + 8 + (xAxisLabelLength * charWidth), tickLabel);
											else
												pDC->TextOut(tickXLocation + 2, yApexPoint + 8, tickLabel);

											pDC->SelectObject(pOldFont);
										}
									}
									rightXTick = tickXLocation;
									x++;
								}
								for(; x <= graphSeries->GetCount(); x++)
								{
									tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

									tickXLocation = xApexPoint - (xAxisWidth / 2) + ((x * seriesSpace) - (seriesSpace / 2));

									pDC->MoveTo(tickXLocation,yApexPoint - 3);
									pDC->LineTo(tickXLocation,yApexPoint + 3);

									if(!inRedraw)
									{
										//draw tick label
										CString tickLabel;
										tickLabel = tmpSeries->GetLabel();
										if(!xAxisAlign)  //horizontal
										{
											CFont xFont;
											xFont.CreateFont(xTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
												ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
												DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
											pDC->SelectObject(&xFont);
											pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * 8) / 2), yApexPoint + 8, tickLabel);
											pDC->SelectObject(pOldFont);
										}
										else
										{
											CFont sideFont2;
											sideFont2.CreateFont(xTickFontSize, 0, (xAxisAlign * 10), 0, 700, FALSE, FALSE, 0,
												ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
												DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
											pDC->SelectObject(&sideFont2);
											pDC->GetTextMetrics(&tm);
											charWidth = tm.tmAveCharWidth;
											if(xAxisAlign < 180)
												pDC->TextOut(tickXLocation - 8, yApexPoint + 8 + (xAxisLabelLength * charWidth), tickLabel);
											else
												pDC->TextOut(tickXLocation + 2, yApexPoint + 8, tickLabel);

											pDC->SelectObject(pOldFont);
										}
									}
									rightXTick = tickXLocation;
								}
							}
							else
							{
								//scatter graphs will use the same tick marking on both axis lines
								seriesSpace = xAxisWidth / numTicks;
								for(x = 0; x <= numTicks / 2; x++)  
								{
									tickXLocation = xApexPoint + 5 - (xAxisWidth / 2) + (x * seriesSpace);

									//draw tick mark
									pDC->MoveTo(tickXLocation, yApexPoint - 3);
									pDC->LineTo(tickXLocation, yApexPoint + 3);

									if(!inRedraw)
									{
										//draw tick label
										CString tickLabel;
										tickLabel.Format(_T("%d"), minTick + (x * tickSpace));

										CFont xFont;
										xFont.CreateFont(xTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&xFont);
										pDC->GetTextMetrics(&tm);
										charWidth = tm.tmAveCharWidth;

										pDC->TextOut(tickXLocation - (tickLabel.GetLength() * charWidth / 2), yApexPoint + 8, tickLabel);
									}
									rightXTick = tickXLocation;
									pDC->SelectObject(pOldFont);
								}
								//draw the 0
								tickXLocation = xApexPoint;
								if(!inRedraw)
								{
									//draw tick label

									CFont xFont;
									xFont.CreateFont(xTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
										ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
										DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
									pDC->SelectObject(&xFont);
									pDC->GetTextMetrics(&tm);
									charWidth = tm.tmAveCharWidth;

									pDC->TextOut(tickXLocation - (charWidth / 2), yApexPoint + 8, "0");
								}

								for(; x <= numTicks; x++)  
								{
									tickXLocation = xApexPoint - (xAxisWidth / 2) + (x * seriesSpace);

									//draw tick mark
									pDC->MoveTo(tickXLocation, yApexPoint - 3);
									pDC->LineTo(tickXLocation, yApexPoint + 3);

									if(!inRedraw)
									{
										//draw tick label
										CString tickLabel;
										tickLabel.Format(_T("%d"), (x / 2) * tickSpace);

										CFont xFont;
										xFont.CreateFont(xTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&xFont);
										pDC->GetTextMetrics(&tm);
										charWidth = tm.tmAveCharWidth;

										pDC->TextOut(tickXLocation - (tickLabel.GetLength() * charWidth / 2), yApexPoint + 8, tickLabel);
									}
									rightXTick = tickXLocation;
									pDC->SelectObject(pOldFont);
								}
							}
							break;
						case 3 :
							if(graphType != SCATTER_GRAPH)
							{
								POSITION pos;
								pos = graphSeries->GetHeadPosition();
								CGraphSeries* tmpSeries;
								for(int x = 1; x <= graphSeries->GetCount(); x++)
								{
									tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

									seriesSpace = xAxisWidth / graphSeries->GetCount();
									tickXLocation = xApexPoint + ((x * seriesSpace) - (seriesSpace / 2));

									pDC->MoveTo(tickXLocation,yApexPoint - 3);
									pDC->LineTo(tickXLocation,yApexPoint + 3);

									if(!inRedraw)
									{
										//draw tick label
										CString tickLabel;
										tickLabel = tmpSeries->GetLabel();
										if(!xAxisAlign)  //horizontal
										{
											CFont xFont;
											xFont.CreateFont(xTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
												ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
												DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
											pDC->SelectObject(&xFont);
											pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * 8) / 2), yApexPoint + (yAxisHeight / 2) + 8, tickLabel);
											pDC->SelectObject(pOldFont);
										}
										else
										{
											CFont sideFont2;
											sideFont2.CreateFont(xTickFontSize, 0, (xAxisAlign * 10), 0, 700, FALSE, FALSE, 0,
												ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
												DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
											pDC->SelectObject(&sideFont2);
											pDC->GetTextMetrics(&tm);
											charWidth = tm.tmAveCharWidth;
											if(xAxisAlign < 180)
												pDC->TextOut(tickXLocation - 8, yApexPoint + (yAxisHeight / 2) + 8 + (xAxisLabelLength * charWidth), tickLabel);
											else
												pDC->TextOut(tickXLocation + 2, yApexPoint + (yAxisHeight / 2) + 8, tickLabel);

											pDC->SelectObject(pOldFont);
										}
									}
									rightXTick = tickXLocation;
								}
							}
							else
							{
								//scatter graphs will use the same tick marking on both axis lines
								seriesSpace = (xAxisWidth - 20) / numTicks;
								for(int x = 1; x <= numTicks; x++)  
								{
									tickXLocation = xApexPoint + (x * seriesSpace);

									//draw tick mark
									pDC->MoveTo(tickXLocation, yApexPoint - 3);
									pDC->LineTo(tickXLocation, yApexPoint + 3);

									if(!inRedraw)
									{
										//draw tick label
										CString tickLabel;
										tickLabel.Format(_T("%d"), x * tickSpace);

										CFont xFont;
										xFont.CreateFont(xTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&xFont);
										pDC->GetTextMetrics(&tm);
										charWidth = tm.tmAveCharWidth;

										pDC->TextOut(tickXLocation - (tickLabel.GetLength() * charWidth / 2), yApexPoint + (yAxisHeight / 2) + 8, tickLabel);
									}
									rightXTick = tickXLocation;
									pDC->SelectObject(pOldFont);
								}

							}
							break;
						case 4 :
							if(graphType != SCATTER_GRAPH)
							{
								POSITION pos;
								pos = graphSeries->GetHeadPosition();
								CGraphSeries* tmpSeries;
								seriesSpace = xAxisWidth / graphSeries->GetCount();
								for(x = 1; x <= graphSeries->GetCount() / 2; x++)
								{
									tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

									tickXLocation = xApexPoint - (xAxisWidth / 2) + ((x * seriesSpace) - (seriesSpace / 2));

									pDC->MoveTo(tickXLocation,yApexPoint - 3);
									pDC->LineTo(tickXLocation,yApexPoint + 3);

									if(!inRedraw)
									{
										//draw tick label
										CString tickLabel;
										tickLabel = tmpSeries->GetLabel();
										if(!xAxisAlign)  //horizontal
										{
											CFont xFont;
											xFont.CreateFont(xTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
												ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
												DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
											pDC->SelectObject(&xFont);
											pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * 8) / 2), yApexPoint + (yAxisHeight / 2) + 8, tickLabel);
											pDC->SelectObject(pOldFont);
										}
										else
										{
											CFont sideFont2;
											sideFont2.CreateFont(xTickFontSize, 0, (xAxisAlign * 10), 0, 700, FALSE, FALSE, 0,
												ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
												DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
											pDC->SelectObject(&sideFont2);
											pDC->GetTextMetrics(&tm);
											charWidth = tm.tmAveCharWidth;
											if(xAxisAlign < 180)
												pDC->TextOut(tickXLocation - 8, yApexPoint + (yAxisHeight / 2) + 8 + (xAxisLabelLength * charWidth), tickLabel);
											else
												pDC->TextOut(tickXLocation + 2, yApexPoint + (yAxisHeight / 2) + 8, tickLabel);

											pDC->SelectObject(pOldFont);
										}
									}
									rightXTick = tickXLocation;
								}
								if((graphSeries->GetCount() % 2) > 0)
								{
									tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);
									
									tickXLocation = xApexPoint;

									pDC->MoveTo(tickXLocation,yApexPoint - 3);
									pDC->LineTo(tickXLocation,yApexPoint + 3);

									if(!inRedraw)
									{
										//draw tick label
										CString tickLabel;
										tickLabel = tmpSeries->GetLabel();
										if(!xAxisAlign)  //horizontal
										{
											CFont xFont;
											xFont.CreateFont(xTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
												ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
												DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
											pDC->SelectObject(&xFont);
											pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * 8) / 2), yApexPoint + (yAxisHeight / 2) + 8, tickLabel);
											pDC->SelectObject(pOldFont);
										}
										else
										{
											CFont sideFont2;
											sideFont2.CreateFont(xTickFontSize, 0, (xAxisAlign * 10), 0, 700, FALSE, FALSE, 0,
												ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
												DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
											pDC->SelectObject(&sideFont2);
											pDC->GetTextMetrics(&tm);
											charWidth = tm.tmAveCharWidth;
											if(xAxisAlign < 180)
												pDC->TextOut(tickXLocation - 8, yApexPoint + (yAxisHeight / 2) + 8 + (xAxisLabelLength * charWidth), tickLabel);
											else
												pDC->TextOut(tickXLocation + 2, yApexPoint + (yAxisHeight / 2) + 8, tickLabel);

											pDC->SelectObject(pOldFont);
										}
									}
									rightXTick = tickXLocation;
									x++;
								}
								for(; x <= graphSeries->GetCount(); x++)
								{
									tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

									tickXLocation = xApexPoint - (xAxisWidth / 2) + ((x * seriesSpace) - (seriesSpace / 2));

									pDC->MoveTo(tickXLocation,yApexPoint - 3);
									pDC->LineTo(tickXLocation,yApexPoint + 3);

									if(!inRedraw)
									{
										//draw tick label
										CString tickLabel;
										tickLabel = tmpSeries->GetLabel();
										if(!xAxisAlign)  //horizontal
										{
											CFont xFont;
											xFont.CreateFont(xTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
												ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
												DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
											pDC->SelectObject(&xFont);
											pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * 8) / 2), yApexPoint + (yAxisHeight / 2) + 8, tickLabel);
											pDC->SelectObject(pOldFont);
										}
										else
										{
											CFont sideFont2;
											sideFont2.CreateFont(xTickFontSize, 0, (xAxisAlign * 10), 0, 700, FALSE, FALSE, 0,
												ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
												DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
											pDC->SelectObject(&sideFont2);
											pDC->GetTextMetrics(&tm);
											charWidth = tm.tmAveCharWidth;
											if(xAxisAlign < 180)
												pDC->TextOut(tickXLocation - 8, yApexPoint + (yAxisHeight / 2) + 8 + (xAxisLabelLength * charWidth), tickLabel);
											else
												pDC->TextOut(tickXLocation + 2, yApexPoint + (yAxisHeight / 2) + 8, tickLabel);

											pDC->SelectObject(pOldFont);
										}
									}
									rightXTick = tickXLocation;
								}
							}
							else
							{
								//scatter graphs will use the same tick marking on both axis lines
								seriesSpace = (xAxisWidth - 20) / numTicks;
								for(x = 1; x <= numTicks / 2; x++)  
								{
									tickXLocation = xApexPoint - (xAxisWidth / 2) + (x * seriesSpace);

									//draw tick mark
									pDC->MoveTo(tickXLocation, yApexPoint + (yAxisHeight / 2) - 3);
									pDC->LineTo(tickXLocation, yApexPoint + (yAxisHeight / 2) + 3);

									if(!inRedraw)
									{
										//draw tick label
										CString tickLabel;
										tickLabel.Format(_T("%d"), x * tickSpace);

										CFont xFont;
										xFont.CreateFont(xTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&xFont);
										pDC->GetTextMetrics(&tm);
										charWidth = tm.tmAveCharWidth;

										pDC->TextOut(tickXLocation - (tickLabel.GetLength() * charWidth / 2), yApexPoint + 8, tickLabel);
									}
									rightXTick = tickXLocation;
									pDC->SelectObject(pOldFont);
								}
								//draw the 0

								for(; x <= numTicks; x++)  
								{
									tickXLocation = xApexPoint - (xAxisWidth / 2) + (x * seriesSpace);

									//draw tick mark
									pDC->MoveTo(tickXLocation, yApexPoint + (yAxisHeight / 2) - 3);
									pDC->LineTo(tickXLocation, yApexPoint + (yAxisHeight / 2) + 3);

									if(!inRedraw)
									{
										//draw tick label
										CString tickLabel;
										tickLabel.Format(_T("%d"), x * tickSpace);

										CFont xFont;
										xFont.CreateFont(xTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&xFont);
										pDC->GetTextMetrics(&tm);
										charWidth = tm.tmAveCharWidth;

										pDC->TextOut(tickXLocation - (tickLabel.GetLength() * charWidth / 2), yApexPoint + 8, tickLabel);
									}
									rightXTick = tickXLocation;
									pDC->SelectObject(pOldFont);
								}
							}
							break;
					}
				}
				else
				{
					//reverse above stuff and treat as horizontal graph
					switch(graphQuadType)
					{
						case 1 :
							pDC->TextOut(10, (yApexPoint - (yAxisHeight / 2)) + (axisYLabel.GetLength() / 2), axisXLabel);

							pDC->SelectObject(&axisFont);
							pDC->GetTextMetrics(&tm);
							charWidth = tm.tmAveCharWidth;
							labelHeight = tm.tmHeight + 10;
							if(axisYLabel.GetLength() == 0)
								labelHeight = 0;
							pDC->TextOut(xApexPoint + (xAxisWidth / 2) - ((axisXLabel.GetLength() / 2) * charWidth), maxHeight - 5 - 6, axisYLabel);
							
							pDC->SelectObject(pOldFont);

							//to allow scalability (height may be less than tickRange)
							tickScale = 0.00;

							//draw y axis ticks
							if(graphType != SCATTER_GRAPH)
							{
								tickScale = (yAxisHeight - 10) / seriesSize;
								POSITION pos;
								pos = graphSeries->GetHeadPosition();
								CGraphSeries* tmpSeries;
								for(y = 1; y <= graphSeries->GetCount(); y++)  
								{
									tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

									seriesSpace = yAxisHeight / graphSeries->GetCount();
									tickYLocation = yApexPoint - ((y * seriesSpace) - (seriesSpace / 2));

									//draw tick mark
									pDC->MoveTo(xApexPoint - 3, tickYLocation);
									pDC->LineTo(xApexPoint + 3, tickYLocation);

									if(!inRedraw)
									{
										//draw tick label
										CString tickLabel;
										tickLabel = tmpSeries->GetLabel();

										CFont yFont;
										yFont.CreateFont(yTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, FIXED_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&yFont);
										charWidth = tm.tmAveCharWidth;

										pDC->TextOut(xApexPoint - (xAxisLabelLength * charWidth) - 5, tickYLocation - (tm.tmHeight / 2), tickLabel);
									}
									topYTick = tickYLocation;
									pDC->SelectObject(pOldFont);
								}
							}
							else
							{
								//scatter graphs will use the same tick marking on both axis lines
								for(y = 1; y <= numTicks; y++)  
								{
									tickScale = (yAxisHeight - 10) / numTicks;
									tickYLocation = yApexPoint - (int)(y * tickScale);

									//draw tick mark
									pDC->MoveTo(xApexPoint - 3, tickYLocation);
									pDC->LineTo(xApexPoint + 3, tickYLocation);

									if(!inRedraw)
									{
										//draw tick label
										CString tickLabel;
										tickLabel.Format(_T("%d"), y * tickSpace);

										CFont yFont;
										yFont.CreateFont(yTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&yFont);

										pDC->TextOut(xApexPoint - 10 - (tickLabel.GetLength() * (yTickFontSize / 2)), tickYLocation - 6, tickLabel);
									}
									topYTick = tickYLocation;
									pDC->SelectObject(pOldFont);
								}
							}

							//draw X axis tick marks
							tickScale = (xAxisWidth - 10) / numTicks;
							for(x = 1; x <= numTicks; x++)
							{
								tickXLocation = xApexPoint + (int)(x * tickScale);

								pDC->MoveTo(tickXLocation,yApexPoint - 3);
								pDC->LineTo(tickXLocation,yApexPoint + 3);

								if(!inRedraw)
								{
									if(graphHasGridLines)
									{
										//draw grid lines
										COLORREF gridLineColor;
										gridLineColor = DARK_GREY;
										CPen gridPen (PS_SOLID, 1, gridLineColor);
										CPen* pOldPen;
										pOldPen = pDC->SelectObject(&gridPen);
										pDC->MoveTo(tickXLocation, yApexPoint);
										pDC->LineTo(tickXLocation, yApexPoint - yAxisHeight);
										pDC->SelectObject(pOldPen);
									}

									//draw tick label
									CString tickLabel;
									tickLabel.Format(_T("%d"), minTick + (x * tickSpace));
									if(!xAxisAlign)  //horizontal
									{
										CFont xFont;
										xFont.CreateFont(xTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&xFont);
										charWidth = tm.tmAveCharWidth;
										pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), yApexPoint + 8, tickLabel);
										pDC->SelectObject(pOldFont);
									}
									else
									{
										CFont sideFont2;
										sideFont2.CreateFont(xTickFontSize, 0, (xAxisAlign * 10), 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&sideFont2);
										pDC->GetTextMetrics(&tm);
										charWidth = tm.tmAveCharWidth;
										if(xAxisAlign < 180)
											pDC->TextOut(tickXLocation - 8, yApexPoint + 8 + (xAxisLabelLength * charWidth), tickLabel);
										else
											pDC->TextOut(tickXLocation + 2, yApexPoint + 8, tickLabel);

										pDC->SelectObject(pOldFont);
									}
								}
								rightXTick = tickXLocation;
							}
							break;
						case 2 :
							pDC->TextOut(10, (yApexPoint - (yAxisHeight / 2)) + (axisYLabel.GetLength() / 2), axisXLabel);

							pDC->SelectObject(&axisFont);
							pDC->GetTextMetrics(&tm);
							charWidth = tm.tmAveCharWidth;
							labelHeight = tm.tmHeight + 10;
							if(axisYLabel.GetLength() == 0)
								labelHeight = 0;
							pDC->TextOut(xApexPoint - ((axisXLabel.GetLength() / 2) * charWidth), maxHeight - 5 - 6, axisYLabel);
							
							pDC->SelectObject(pOldFont);

							//to allow scalability (height may be less than tickRange)
							tickScale = 0.00;
							seriesSpace = yAxisHeight / graphSeries->GetCount();

							//draw y axis ticks
							if(graphType != SCATTER_GRAPH)
							{
								tickScale = (yAxisHeight - 10) / seriesSize;
								POSITION pos;
								pos = graphSeries->GetHeadPosition();
								CGraphSeries* tmpSeries;
								for(y = 1; y <= graphSeries->GetCount(); y++)  
								{
									tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

									tickYLocation = yApexPoint - ((y * seriesSpace) - (seriesSpace / 2));

									//draw tick mark
									pDC->MoveTo(xApexPoint - 3, tickYLocation);
									pDC->LineTo(xApexPoint + 3, tickYLocation);

									if(!inRedraw)
									{
										//draw tick label
										CString tickLabel;
										tickLabel = tmpSeries->GetLabel();

										CFont yFont;
										yFont.CreateFont(yTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, FIXED_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&yFont);
										charWidth = tm.tmAveCharWidth;

										pDC->TextOut(xApexPoint - (xAxisWidth / 2) - (xAxisLabelLength * charWidth) - 5, tickYLocation - (tm.tmHeight / 2), tickLabel);
									}
									topYTick = tickYLocation;
									pDC->SelectObject(pOldFont);
								}
								topYTick = tickYLocation;
								pDC->SelectObject(pOldFont);
							}
							else
							{
								//scatter graphs will use the same tick marking on both axis lines
								int oldNumTicks;
								oldNumTicks = numTicks;
								numTicks /= 2;

								for(y = 0; y <= numTicks; y++)  
								{
									tickScale = (yAxisHeight - 10) / numTicks;
									tickYLocation = yApexPoint - (int)(y * tickScale);

									//draw tick mark
									pDC->MoveTo(xApexPoint - 3, tickYLocation);
									pDC->LineTo(xApexPoint + 3, tickYLocation);

									if(!inRedraw)
									{
										//draw tick label
										CString tickLabel;
										tickLabel.Format(_T("%d"), (y * tickSpace));

										CFont yFont;
										yFont.CreateFont(yTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&yFont);

										pDC->TextOut(xApexPoint - (xAxisWidth / 2) - (tickLabel.GetLength() * (yTickFontSize / 2)) - 6, tickYLocation - 6, tickLabel);
									}
									topYTick = tickYLocation;
									pDC->SelectObject(pOldFont);
								}
								numTicks = oldNumTicks;
							}

							//draw X axis tick marks
							tickScale = (xAxisWidth - 10) / numTicks;
							for(x = 0; x <= numTicks / 2; x++)
							{
								tickXLocation = xApexPoint - (xAxisWidth / 2) + (int)(x * tickScale) + 5;

								if(!inRedraw)
								{
									pDC->MoveTo(tickXLocation,yApexPoint - 3);
									pDC->LineTo(tickXLocation,yApexPoint + 3);

									if(graphHasGridLines)
									{
										//draw grid lines
										COLORREF gridLineColor;
										gridLineColor = DARK_GREY;
										CPen gridPen (PS_SOLID, 1, gridLineColor);
										CPen* pOldPen;
										pOldPen = pDC->SelectObject(&gridPen);
										pDC->MoveTo(tickXLocation, yApexPoint - yAxisHeight);
										pDC->LineTo(tickXLocation, yApexPoint);
										pDC->SelectObject(pOldPen);
									}

									//draw tick label
									CString tickLabel;
									tickLabel.Format(_T("%d"), minTick + (x * tickSpace));
									if(!xAxisAlign)  //horizontal
									{
										CFont xFont;
										xFont.CreateFont(xTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&xFont);
										charWidth = tm.tmAveCharWidth;
										pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), yApexPoint + 8, tickLabel);
										pDC->SelectObject(pOldFont);
									}
									else
									{
										CFont sideFont2;
										sideFont2.CreateFont(xTickFontSize, 0, (xAxisAlign * 10), 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&sideFont2);
										pDC->GetTextMetrics(&tm);
										charWidth = tm.tmAveCharWidth;
										if(xAxisAlign < 180)
											pDC->TextOut(tickXLocation - 8, yApexPoint + 8 + (xAxisLabelLength * charWidth), tickLabel);
										else
											pDC->TextOut(tickXLocation + 2, yApexPoint + 8, tickLabel);

										pDC->SelectObject(pOldFont);
									}
								}
								rightXTick = tickXLocation;
							}
							//draw the 0 location
							tickXLocation = xApexPoint - (xAxisWidth / 2) + (int)(x * tickScale);
							if(!inRedraw)
							{
								//draw tick label
								if(!xAxisAlign)  //horizontal
								{
									CFont xFont;
									xFont.CreateFont(xTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
										ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
										DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
									pDC->SelectObject(&xFont);
									charWidth = tm.tmAveCharWidth;
									pDC->TextOut(tickXLocation - (charWidth / 2), yApexPoint + 8, "0");
									pDC->SelectObject(pOldFont);
								}
								else
								{
									CFont sideFont2;
									sideFont2.CreateFont(xTickFontSize, 0, (xAxisAlign * 10), 0, 700, FALSE, FALSE, 0,
										ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
										DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
									pDC->SelectObject(&sideFont2);
									pDC->GetTextMetrics(&tm);
									charWidth = tm.tmAveCharWidth;
									if(xAxisAlign < 180)
										pDC->TextOut(tickXLocation - 8, yApexPoint + 8 + (xAxisLabelLength * charWidth), "0");
									else
										pDC->TextOut(tickXLocation + 2, yApexPoint + 8, "0");

									pDC->SelectObject(pOldFont);
								}
							}

							for(; x <= numTicks; x++)
							{
								tickXLocation = xApexPoint - (xAxisWidth / 2) + (int)(x * tickScale);

								pDC->MoveTo(tickXLocation,yApexPoint - 3);
								pDC->LineTo(tickXLocation,yApexPoint + 3);

								if(!inRedraw)
								{
									if(graphHasGridLines)
									{
										//draw grid lines
										COLORREF gridLineColor;
										gridLineColor = DARK_GREY;
										CPen gridPen (PS_SOLID, 1, gridLineColor);
										CPen* pOldPen;
										pOldPen = pDC->SelectObject(&gridPen);
										pDC->MoveTo(tickXLocation, yApexPoint - yAxisHeight);
										pDC->LineTo(tickXLocation, yApexPoint);
										pDC->SelectObject(pOldPen);
									}

									//draw tick label
									CString tickLabel;
									tickLabel.Format(_T("%d"), minTick + (x * tickSpace));
									if(!xAxisAlign)  //horizontal
									{
										CFont xFont;
										xFont.CreateFont(xTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&xFont);
										charWidth = tm.tmAveCharWidth;
										pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), yApexPoint + 8, tickLabel);
										pDC->SelectObject(pOldFont);
									}
									else
									{
										CFont sideFont2;
										sideFont2.CreateFont(xTickFontSize, 0, (xAxisAlign * 10), 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&sideFont2);
										pDC->GetTextMetrics(&tm);
										charWidth = tm.tmAveCharWidth;
										if(xAxisAlign < 180)
											pDC->TextOut(tickXLocation - 8, yApexPoint + 8 + (xAxisLabelLength * charWidth), tickLabel);
										else
											pDC->TextOut(tickXLocation + 2, yApexPoint + 8, tickLabel);

										pDC->SelectObject(pOldFont);
									}
								}
								rightXTick = tickXLocation;
							}
							break;
						case 3 :
							pDC->TextOut(10, yApexPoint - (axisYLabel.GetLength() / 2), axisXLabel);

							pDC->SelectObject(&axisFont);
							pDC->GetTextMetrics(&tm);
							charWidth = tm.tmAveCharWidth;
							labelHeight = tm.tmHeight + 10;
							if(axisYLabel.GetLength() == 0)
								labelHeight = 0;
							pDC->TextOut(xApexPoint + (xAxisWidth / 2) - ((axisXLabel.GetLength() / 2) * charWidth), maxHeight - 5 - 6, axisYLabel);
							
							pDC->SelectObject(pOldFont);

							//to allow scalability (height may be less than tickRange)
							tickScale = 0.00;
							seriesSpace = yAxisHeight / graphSeries->GetCount();

							//draw y axis ticks
							if(graphType != SCATTER_GRAPH)
							{
								tickScale = (yAxisHeight - 10) / seriesSize;
								POSITION pos;
								pos = graphSeries->GetHeadPosition();
								CGraphSeries* tmpSeries;
								for(y = 1; y <= graphSeries->GetCount() / 2; y++)  
								{
									tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

									tickYLocation = yApexPoint - (yAxisHeight / 2) + ((y * seriesSpace) - (seriesSpace / 2));

									//draw tick mark
									pDC->MoveTo(xApexPoint - 3, tickYLocation);
									pDC->LineTo(xApexPoint + 3, tickYLocation);

									if(!inRedraw)
									{
										//draw tick label
										CString tickLabel;
										tickLabel = tmpSeries->GetLabel();

										CFont yFont;
										yFont.CreateFont(yTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, FIXED_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&yFont);
										charWidth = tm.tmAveCharWidth;

										pDC->TextOut(xApexPoint - 5 - (tickLabel.GetLength() * (yTickFontSize / 2)), tickYLocation - 6, tickLabel);
									}
									topYTick = tickYLocation;
									pDC->SelectObject(pOldFont);
								}
								//draw the 0 location
								tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);
								tickYLocation = yApexPoint;
								//draw tick mark
								pDC->MoveTo(xApexPoint - 3, tickYLocation);
								pDC->LineTo(xApexPoint + 3, tickYLocation);

								if(!inRedraw)
								{
									//draw tick label
									CString tickLabel;
									tickLabel = tmpSeries->GetLabel();

									CFont yFont;
									yFont.CreateFont(yTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
										ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
										DEFAULT_QUALITY, FIXED_PITCH | FF_ROMAN,_T("Arial"));
									pDC->SelectObject(&yFont);
									charWidth = tm.tmAveCharWidth;

									pDC->TextOut(xApexPoint - 5 - (tickLabel.GetLength() * (yTickFontSize / 2)), tickYLocation - 6, tickLabel);
								}
								topYTick = tickYLocation;
								pDC->SelectObject(pOldFont);
								y++;

								for(; y <= graphSeries->GetCount(); y++)  
								{
									tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

									tickYLocation = yApexPoint - (yAxisHeight / 2) + ((y * seriesSpace) - (seriesSpace / 2));

									//draw tick mark
									pDC->MoveTo(xApexPoint - 3, tickYLocation);
									pDC->LineTo(xApexPoint + 3, tickYLocation);

									if(!inRedraw)
									{
										//draw tick label
										CString tickLabel;
										tickLabel = tmpSeries->GetLabel();

										CFont yFont;
										yFont.CreateFont(yTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, FIXED_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&yFont);
										charWidth = tm.tmAveCharWidth;

										pDC->TextOut(xApexPoint - 5 - (tickLabel.GetLength() * (yTickFontSize / 2)), tickYLocation - 6, tickLabel);
									}
									topYTick = tickYLocation;
									pDC->SelectObject(pOldFont);
								}
							}
							else
							{
								//scatter graphs will use the same tick marking on both axis lines
								for(y = 0; y < numTicks / 2; y++)  
								{
									tickScale = (yAxisHeight - 10) / numTicks;
									tickYLocation = yApexPoint - (yAxisHeight / 2) + (int)(y * tickScale) + 5;

									//draw tick mark
									pDC->MoveTo(xApexPoint - 3, tickYLocation);
									pDC->LineTo(xApexPoint + 3, tickYLocation);

									if(!inRedraw)
									{
										//draw tick label
										CString tickLabel;
										tickLabel.Format(_T("%d"), maxTick - (y * tickSpace));

										CFont yFont;
										yFont.CreateFont(yTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&yFont);

										pDC->TextOut(xApexPoint - (tickLabel.GetLength() * (yTickFontSize / 2)) - 8, tickYLocation - 6, tickLabel);
										pDC->SelectObject(pOldFont);
									}
								}
								//draw the 0 location
								tickYLocation = yApexPoint;
								//draw tick mark
								pDC->MoveTo(xApexPoint - 3, tickYLocation);
								pDC->LineTo(xApexPoint + 3, tickYLocation);

								if(!inRedraw)
								{
									//draw tick label
									CFont yFont;
									yFont.CreateFont(yTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
										ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
										DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
									pDC->SelectObject(&yFont);

									pDC->TextOut(xApexPoint - (yTickFontSize / 2) - 8, tickYLocation - 6, "0");
									pDC->SelectObject(pOldFont);
								}
								y++;

								for(; y <= numTicks; y++)  
								{
									tickScale = (yAxisHeight - 10) / numTicks;
									tickYLocation = yApexPoint - (yAxisHeight / 2) + (int)(y * tickScale) + 5;

									//draw tick mark
									pDC->MoveTo(xApexPoint - 3, tickYLocation);
									pDC->LineTo(xApexPoint + 3, tickYLocation);

									if(!inRedraw)
									{
										//draw tick label
										CString tickLabel;
										tickLabel.Format(_T("%d"), maxTick - (y * tickSpace));

										CFont yFont;
										yFont.CreateFont(yTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&yFont);

										pDC->TextOut(xApexPoint - (tickLabel.GetLength() * (yTickFontSize / 2)) - 5, tickYLocation - 6, tickLabel);
										pDC->SelectObject(pOldFont);
									}
									topYTick = tickYLocation;
								}
							}

							//draw X axis tick marks
							int oldNumTicks;
							oldNumTicks = numTicks;
							numTicks /= 2;
							tickScale = (xAxisWidth - 10) / numTicks;

							for(x = 0; x <= numTicks; x++)
							{
								tickXLocation = xApexPoint + (int)(x * tickScale);

								if(!inRedraw)
								{
									pDC->MoveTo(tickXLocation,yApexPoint - 3);
									pDC->LineTo(tickXLocation,yApexPoint + 3);

									if(graphHasGridLines)
									{
										//draw grid lines
										COLORREF gridLineColor;
										gridLineColor = DARK_GREY;
										CPen gridPen (PS_SOLID, 1, gridLineColor);
										CPen* pOldPen;
										pOldPen = pDC->SelectObject(&gridPen);
										pDC->MoveTo(tickXLocation, yApexPoint - (yAxisHeight / 2));
										pDC->LineTo(tickXLocation, yApexPoint + (yAxisHeight / 2));
										pDC->SelectObject(pOldPen);
									}

									//draw tick label
									CString tickLabel;
									tickLabel.Format(_T("%d"), (x * tickSpace * 2));
									if(!xAxisAlign)  //horizontal
									{
										CFont xFont;
										xFont.CreateFont(xTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&xFont);
										charWidth = tm.tmAveCharWidth;
										pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), yApexPoint + (yAxisHeight / 2) + 8, tickLabel);
										pDC->SelectObject(pOldFont);
									}
									else
									{
										CFont sideFont2;
										sideFont2.CreateFont(xTickFontSize, 0, (xAxisAlign * 10), 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&sideFont2);
										pDC->GetTextMetrics(&tm);
										charWidth = tm.tmAveCharWidth;
										if(xAxisAlign < 180)
											pDC->TextOut(tickXLocation - 8, yApexPoint + (yAxisHeight / 2) + 8 + (xAxisLabelLength * charWidth), tickLabel);
										else
											pDC->TextOut(tickXLocation + 2, yApexPoint + (yAxisHeight / 2) + 8, tickLabel);

										pDC->SelectObject(pOldFont);
									}
								}
							}
							numTicks = oldNumTicks;
							rightXTick = tickXLocation;
							break;
						case 4 :
							pDC->TextOut(10, yApexPoint - (axisYLabel.GetLength() / 2), axisXLabel);

							pDC->SelectObject(&axisFont);
							pDC->GetTextMetrics(&tm);
							charWidth = tm.tmAveCharWidth;
							labelHeight = tm.tmHeight + 10;
							if(axisYLabel.GetLength() == 0)
								labelHeight = 0;
							pDC->TextOut(xApexPoint - ((axisXLabel.GetLength() / 2) * charWidth), maxHeight - 5 - 6, axisYLabel);
							
							pDC->SelectObject(pOldFont);

							//to allow scalability (height may be less than tickRange)
							tickScale = 0.00;
							seriesSpace = yAxisHeight / graphSeries->GetCount();

							//draw y axis ticks
							if(graphType != SCATTER_GRAPH)
							{
								tickScale = (yAxisHeight - 10) / seriesSize;
								POSITION pos;
								pos = graphSeries->GetHeadPosition();
								CGraphSeries* tmpSeries;
								for(y = 1; y <= graphSeries->GetCount() / 2; y++)  
								{
									tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

									tickYLocation = yApexPoint - (yAxisHeight / 2) + ((y * seriesSpace) - (seriesSpace / 2));

									//draw tick mark
									pDC->MoveTo(xApexPoint - 3, tickYLocation);
									pDC->LineTo(xApexPoint + 3, tickYLocation);

									if(!inRedraw)
									{
										//draw tick label
										CString tickLabel;
										tickLabel = tmpSeries->GetLabel();

										CFont yFont;
										yFont.CreateFont(yTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, FIXED_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&yFont);
										charWidth = tm.tmAveCharWidth;

										pDC->TextOut(xApexPoint - (xAxisWidth / 2) - (xAxisLabelLength * charWidth) - 5, tickYLocation - (tm.tmHeight / 2), tickLabel);
									}
									topYTick = tickYLocation;
									pDC->SelectObject(pOldFont);
								}
								//draw the 0 location
								tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);
								tickYLocation = yApexPoint;
								//draw tick mark
								pDC->MoveTo(xApexPoint - 3, tickYLocation);
								pDC->LineTo(xApexPoint + 3, tickYLocation);

								if(!inRedraw)
								{
									//draw tick label
									CString tickLabel;
									tickLabel = tmpSeries->GetLabel();

									CFont yFont;
									yFont.CreateFont(yTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
										ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
										DEFAULT_QUALITY, FIXED_PITCH | FF_ROMAN,_T("Arial"));
									pDC->SelectObject(&yFont);
									charWidth = tm.tmAveCharWidth;

									pDC->TextOut(xApexPoint - (xAxisWidth / 2) - (xAxisLabelLength * charWidth) - 5, tickYLocation - (tm.tmHeight / 2), tickLabel);
								}
								topYTick = tickYLocation;
								pDC->SelectObject(pOldFont);
								y++;

								for(; y <= graphSeries->GetCount(); y++)  
								{
									tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

									tickYLocation = yApexPoint - (yAxisHeight / 2) + ((y * seriesSpace) - (seriesSpace / 2));

									//draw tick mark
									pDC->MoveTo(xApexPoint - 3, tickYLocation);
									pDC->LineTo(xApexPoint + 3, tickYLocation);

									if(!inRedraw)
									{
										//draw tick label
										CString tickLabel;
										tickLabel = tmpSeries->GetLabel();

										CFont yFont;
										yFont.CreateFont(yTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, FIXED_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&yFont);
										charWidth = tm.tmAveCharWidth;

										pDC->TextOut(xApexPoint - (xAxisWidth / 2) - (xAxisLabelLength * charWidth) - 5, tickYLocation - (tm.tmHeight / 2), tickLabel);
									}
									topYTick = tickYLocation;
									pDC->SelectObject(pOldFont);
								}
							}
							else
							{
								//scatter graphs will use the same tick marking on both axis lines
								for(y = 1; y <= numTicks; y++)  
								{
									tickScale = (yAxisHeight - 10) / numTicks;
									tickYLocation = yApexPoint - (yAxisHeight / 2) + (int)(y * tickScale);

									//draw tick mark
									pDC->MoveTo(xApexPoint - 3, tickYLocation);
									pDC->LineTo(xApexPoint + 3, tickYLocation);

									if(!inRedraw)
									{
										//draw tick label
										CString tickLabel;
										tickLabel.Format(_T("%d"), y * tickSpace);

										CFont yFont;
										yFont.CreateFont(yTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&yFont);

										pDC->TextOut(xApexPoint - (xAxisWidth / 2) - (tickLabel.GetLength() * (yTickFontSize / 2)), tickYLocation - 6, tickLabel);
									}
									topYTick = tickYLocation;
									pDC->SelectObject(pOldFont);
								}
							}

							//draw X axis tick marks
							tickScale = (xAxisWidth - 10) / numTicks;
							for(x = 0; x <= numTicks / 2; x++)
							{
								tickXLocation = xApexPoint - (xAxisWidth / 2) + (int)(x * tickScale) + 5;

								if(!inRedraw)
								{
									pDC->MoveTo(tickXLocation,yApexPoint - 3);
									pDC->LineTo(tickXLocation,yApexPoint + 3);

									if(graphHasGridLines)
									{
										//draw grid lines
										COLORREF gridLineColor;
										gridLineColor = DARK_GREY;
										CPen gridPen (PS_SOLID, 1, gridLineColor);
										CPen* pOldPen;
										pOldPen = pDC->SelectObject(&gridPen);
										pDC->MoveTo(tickXLocation, yApexPoint - (yAxisHeight / 2));
										pDC->LineTo(tickXLocation, yApexPoint + (yAxisHeight / 2));
										pDC->SelectObject(pOldPen);
									}

									//draw tick label
									CString tickLabel;
									tickLabel.Format(_T("%d"), minTick + (x * tickSpace));
									if(!xAxisAlign)  //horizontal
									{
										CFont xFont;
										xFont.CreateFont(xTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&xFont);
										charWidth = tm.tmAveCharWidth;
										pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), yApexPoint + (yAxisHeight / 2) + 8, tickLabel);
										pDC->SelectObject(pOldFont);
									}
									else
									{
										CFont sideFont2;
										sideFont2.CreateFont(xTickFontSize, 0, (xAxisAlign * 10), 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&sideFont2);
										pDC->GetTextMetrics(&tm);
										charWidth = tm.tmAveCharWidth;
										if(xAxisAlign < 180)
											pDC->TextOut(tickXLocation - 8, yApexPoint + (yAxisHeight / 2) + 8 + (xAxisLabelLength * charWidth), tickLabel);
										else
											pDC->TextOut(tickXLocation + 2, yApexPoint + (yAxisHeight / 2) + 8, tickLabel);

										pDC->SelectObject(pOldFont);
									}
								}
								rightXTick = tickXLocation;
							}
							//draw the 0 location
							tickXLocation = xApexPoint - (xAxisWidth / 2) + (int)(x * tickScale);
							if(!inRedraw)
							{
								if(graphHasGridLines)
								{
									//draw grid lines
									COLORREF gridLineColor;
									gridLineColor = DARK_GREY;
									CPen gridPen (PS_SOLID, 1, gridLineColor);
									CPen* pOldPen;
									pOldPen = pDC->SelectObject(&gridPen);
									pDC->MoveTo(tickXLocation, yApexPoint - (yAxisHeight / 2));
									pDC->LineTo(tickXLocation, yApexPoint + (yAxisHeight / 2));
									pDC->SelectObject(pOldPen);
								}

								//draw tick label
								if(!xAxisAlign)  //horizontal
								{
									CFont xFont;
									xFont.CreateFont(xTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
										ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
										DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
									pDC->SelectObject(&xFont);
									charWidth = tm.tmAveCharWidth;
									pDC->TextOut(tickXLocation - (charWidth / 2), yApexPoint + (yAxisHeight / 2) + 8, "0");
									pDC->SelectObject(pOldFont);
								}
								else
								{
									CFont sideFont2;
									sideFont2.CreateFont(xTickFontSize, 0, (xAxisAlign * 10), 0, 700, FALSE, FALSE, 0,
										ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
										DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
									pDC->SelectObject(&sideFont2);
									pDC->GetTextMetrics(&tm);
									charWidth = tm.tmAveCharWidth;
									if(xAxisAlign < 180)
										pDC->TextOut(tickXLocation - 8, yApexPoint + (yAxisHeight / 2) + 8 + (xAxisLabelLength * charWidth), "0");
									else
										pDC->TextOut(tickXLocation + 2, yApexPoint + (yAxisHeight / 2) + 8, "0");

									pDC->SelectObject(pOldFont);
								}
							}
							x++;

							for(; x <= numTicks; x++)
							{
								tickXLocation = xApexPoint - (xAxisWidth / 2) + (int)(x * tickScale);

								pDC->MoveTo(tickXLocation,yApexPoint - 3);
								pDC->LineTo(tickXLocation,yApexPoint + 3);

								if(!inRedraw)
								{
									if(graphHasGridLines)
									{
										//draw grid lines
										COLORREF gridLineColor;
										gridLineColor = DARK_GREY;
										CPen gridPen (PS_SOLID, 1, gridLineColor);
										CPen* pOldPen;
										pOldPen = pDC->SelectObject(&gridPen);
										pDC->MoveTo(tickXLocation, yApexPoint - (yAxisHeight / 2));
										pDC->LineTo(tickXLocation, yApexPoint + (yAxisHeight / 2));
										pDC->SelectObject(pOldPen);
									}

									//draw tick label
									CString tickLabel;
									tickLabel.Format(_T("%d"), (x / 2) * tickSpace);
									if(!xAxisAlign)  //horizontal
									{
										CFont xFont;
										xFont.CreateFont(xTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&xFont);
										charWidth = tm.tmAveCharWidth;
										pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), yApexPoint + (yAxisHeight / 2) + 8, tickLabel);
										pDC->SelectObject(pOldFont);
									}
									else
									{
										CFont sideFont2;
										sideFont2.CreateFont(xTickFontSize, 0, (xAxisAlign * 10), 0, 700, FALSE, FALSE, 0,
											ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
										pDC->SelectObject(&sideFont2);
										pDC->GetTextMetrics(&tm);
										charWidth = tm.tmAveCharWidth;
										if(xAxisAlign < 180)
											pDC->TextOut(tickXLocation - 8, yApexPoint + (yAxisHeight / 2) + 8 + (xAxisLabelLength * charWidth), tickLabel);
										else
											pDC->TextOut(tickXLocation + 2, yApexPoint + (yAxisHeight / 2) + 8, tickLabel);

										pDC->SelectObject(pOldFont);
									}
								}
								rightXTick = tickXLocation;
							}
							break;
					}
				}
				break;
		case BAR_GRAPH_3D :
		case LINE_GRAPH_3D :
		case STACKED_BAR_GRAPH_3D :
				depth = (int)(yAxisHeight * depthRatio);

				xApexPoint += depth;
				yApexPoint -= depth;
				xAxisWidth -= depth;
				yAxisHeight -= depth;
				//draw y axis
				pDC->MoveTo(xApexPoint, yApexPoint);  
				pDC->LineTo(xApexPoint, yApexPoint - yAxisHeight);
				//draw x axis
				pDC->MoveTo(xApexPoint, yApexPoint);  
				pDC->LineTo(xApexPoint + xAxisWidth, yApexPoint);

				//adjust graph settings for 3D look
				yApexPoint += depth;
				xApexPoint -= depth;
				//foreground axis
				pDC->MoveTo(xApexPoint, yApexPoint + (int)(depth * .2));  
				pDC->LineTo(xApexPoint, yApexPoint - yAxisHeight);
				pDC->MoveTo(xApexPoint - (int)(depth * .2), yApexPoint);  
				pDC->LineTo(xApexPoint + xAxisWidth, yApexPoint);
				//connecting line at apex
				pDC->MoveTo(xApexPoint, yApexPoint);  
				pDC->LineTo(xApexPoint + depth, yApexPoint - depth);
				//connecting line at height
				pDC->MoveTo(xApexPoint, yApexPoint - yAxisHeight);  
				pDC->LineTo(xApexPoint + depth, yApexPoint - yAxisHeight - depth);
				//connecting line at width
				pDC->MoveTo(xApexPoint + xAxisWidth, yApexPoint);  
				pDC->LineTo(xApexPoint + xAxisWidth + depth, yApexPoint - depth);
				//top and right border lines
				pDC->MoveTo(xApexPoint + depth, yApexPoint - yAxisHeight - depth);
				pDC->LineTo(xApexPoint + xAxisWidth + depth, yApexPoint - yAxisHeight - depth);
				pDC->LineTo(xApexPoint + xAxisWidth + depth, yApexPoint - depth);

				//Tick marks ... and labels too
				//draw labels
				tFontSize = 16;
				if(maxWidth > maxHeight)
				{
					while((axisYLabel.GetLength() * (tFontSize / 2)) > maxHeight)
					{
						tFontSize -= 2;
					}
				}
				else
				{
					while((axisXLabel.GetLength() * (tFontSize / 2)) > maxWidth)
					{
						tFontSize -= 2;
					}
				}
				axisFont.CreateFont(tFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
					ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
				sideFont.CreateFont(tFontSize, 0, 900, 0, 700, FALSE, FALSE, 0,
					ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));

				pDC->SelectObject(&sideFont);
				pDC->GetTextMetrics(&tm);
				charWidth = tm.tmAveCharWidth;

				if(graphAlignment)
				{
					pDC->TextOut(10, (yApexPoint - (yAxisHeight / 2)) + ((axisYLabel.GetLength() * charWidth) / 2), axisYLabel);

					pDC->SelectObject(&axisFont);
					pDC->GetTextMetrics(&tm);
					charWidth = tm.tmAveCharWidth;
					pDC->TextOut(xApexPoint + (xAxisWidth / 2) - ((axisXLabel.GetLength() / 2) * charWidth), maxHeight - 5 - 6, axisXLabel);
					
					pDC->SelectObject(pOldFont);

					tickScale = 0.00;
					tickScale = yAxisHeight / numTicks;

					//draw y axis ticks
					for(int y = 1; y <= numTicks; y++)  
					{
						tickYLocation = yApexPoint - (int)(y * tickScale);

						//draw tick mark
						pDC->MoveTo(xApexPoint - (int)(depth * .2), tickYLocation);
						pDC->LineTo(xApexPoint, tickYLocation);

						COLORREF gridLineColor;
						CPen* pOldPen;

						//draw grid lines
						gridLineColor = DARK_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(xApexPoint, tickYLocation);
						pDC->LineTo(xApexPoint + depth, tickYLocation - depth);
						if(graphHasGridLines)
						{
							pDC->LineTo(xApexPoint + xAxisWidth + depth, tickYLocation - depth);
						}
						pDC->SelectObject(pOldPen);

						//draw tick label
						CString tickLabel;
						tickLabel.Format(_T("%d"), minTick + (y * tickSpace));

						CFont yFont;
						yFont.CreateFont(yTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
							ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
							DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
						pDC->SelectObject(&yFont);

						pDC->TextOut(xApexPoint - 10 - (tickLabel.GetLength() * (yTickFontSize / 2)), tickYLocation - 6, tickLabel);
						topYTick = tickYLocation;
						pDC->SelectObject(pOldFont);

						//draw 1/2 tick line (grey)
						int midTickYLocation;
						midTickYLocation = yApexPoint - (int)(y * tickScale * .5) - (int)((y - 1) * tickScale * .5);

						//draw tick mark
						pDC->MoveTo(xApexPoint - (int)(depth * .2), midTickYLocation);
						pDC->LineTo(xApexPoint, midTickYLocation);

						//draw grid lines
						gridLineColor = LIGHT_GREY;
						CPen midGridPen (PS_SOLID, 1, gridLineColor);
						pOldPen = pDC->SelectObject(&midGridPen);
						pDC->MoveTo(xApexPoint, midTickYLocation);
						pDC->LineTo(xApexPoint + depth, midTickYLocation - depth);
						if(graphHasGridLines)
						{
							pDC->LineTo(xApexPoint + xAxisWidth + depth, midTickYLocation - depth);
						}
						pDC->SelectObject(pOldPen);
					}

					//draw X axis tick marks
					POSITION pos;
					pos = graphSeries->GetHeadPosition();
					CGraphSeries* tmpSeries;
					for(int x = 1; x <= graphSeries->GetCount(); x++)
					{
						tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

						seriesSpace = xAxisWidth / graphSeries->GetCount();
						tickXLocation = xApexPoint + ((x * seriesSpace) - (seriesSpace / 2));

						//unlike normal series, we put ticks here
						//on the outsides of the series
						pDC->MoveTo(xApexPoint + (x * seriesSpace),yApexPoint + (int)(depth * .2));
						pDC->LineTo(xApexPoint + (x * seriesSpace),yApexPoint);
						pDC->LineTo(xApexPoint + (x * seriesSpace) + depth,yApexPoint - depth);

						//draw tick label
						CString tickLabel;
						tickLabel = tmpSeries->GetLabel();
						if(!xAxisAlign)  //horizontal
							pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * 8) / 2), yApexPoint + 8, tickLabel);
						else
						{
							CFont sideFont2;
							sideFont2.CreateFont(xTickFontSize, 0, (xAxisAlign * 10), 0, 700, FALSE, FALSE, 0,
								ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
								DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
							pDC->SelectObject(&sideFont2);
							pDC->GetTextMetrics(&tm);
							charWidth = tm.tmAveCharWidth;
							if(xAxisAlign < 180)
								pDC->TextOut(tickXLocation - 8, yApexPoint + 8 + (xAxisLabelLength * charWidth), tickLabel);
							else
								pDC->TextOut(tickXLocation + 2, yApexPoint + 8, tickLabel);

							pDC->SelectObject(pOldFont);
						}
						rightXTick = tickXLocation;
					}
				}
				else
				{
					//reverse above stuff and treat as horizontal graph
					pDC->TextOut(10, (yApexPoint - (yAxisHeight / 2)) + (axisYLabel.GetLength() / 2), axisXLabel);

					pDC->SelectObject(&axisFont);
					pDC->GetTextMetrics(&tm);
					charWidth = tm.tmAveCharWidth;
					labelHeight = tm.tmHeight + 10;
					if(axisYLabel.GetLength() == 0)
						labelHeight = 0;
					pDC->TextOut(xApexPoint + (xAxisWidth / 2) - ((axisXLabel.GetLength() / 2) * charWidth), maxHeight - 5 - 6, axisYLabel);
					
					pDC->SelectObject(pOldFont);

					tickScale = 0.00;

					//draw y axis ticks
					tickScale = (yAxisHeight - 10) / seriesSize;
					POSITION pos;
					pos = graphSeries->GetHeadPosition();
					CGraphSeries* tmpSeries;
					for(int y = 1; y <= graphSeries->GetCount(); y++)  
					{
						tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

						seriesSpace = yAxisHeight / graphSeries->GetCount();
						tickYLocation = yApexPoint - (y * seriesSpace);

						//draw tick mark
						//unlike normal series, we put ticks here on the 
						//outside of the series
						pDC->MoveTo(xApexPoint - (int)(depth * .2), tickYLocation);
						pDC->LineTo(xApexPoint, tickYLocation);
						pDC->LineTo(xApexPoint + depth,yApexPoint - (y * seriesSpace) - depth);

						//draw tick label
						CString tickLabel;
						tickLabel = tmpSeries->GetLabel();

						CFont yFont;
						yFont.CreateFont(yTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
							ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
							DEFAULT_QUALITY, FIXED_PITCH | FF_ROMAN,_T("Arial"));
						pDC->SelectObject(&yFont);
						charWidth = tm.tmAveCharWidth;

						pDC->TextOut(xApexPoint - (xAxisLabelLength * charWidth) - 5, tickYLocation - (tm.tmHeight / 2), tickLabel);

						topYTick = tickYLocation;
						pDC->SelectObject(pOldFont);
					}

					//draw X axis tick marks
					tickScale = (xAxisWidth - 10) / numTicks;
					for(int x = 1; x <= numTicks; x++)
					{
						tickXLocation = xApexPoint + (int)(x * tickScale);

						pDC->MoveTo(tickXLocation,yApexPoint + (int)(depth * .2));
						pDC->LineTo(tickXLocation,yApexPoint);

						COLORREF gridLineColor;
						CPen* pOldPen;

						//draw grid lines
						gridLineColor = DARK_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(tickXLocation, yApexPoint);
						pDC->LineTo(tickXLocation + depth, yApexPoint - depth);
						if(graphHasGridLines)
						{
							pDC->LineTo(tickXLocation + depth, yApexPoint - yAxisHeight - depth);
						}
						pDC->SelectObject(pOldPen);

						//draw tick label
						CString tickLabel;
						tickLabel.Format(_T("%d"), minTick + (x * tickSpace));
						if(!xAxisAlign)  //horizontal
						{
							CFont xFont;
							xFont.CreateFont(xTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
								ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
								DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
							pDC->SelectObject(&xFont);
							charWidth = tm.tmAveCharWidth;
							pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), yApexPoint + 8, tickLabel);
							pDC->SelectObject(pOldFont);
						}
						else
						{
							CFont sideFont2;
							sideFont2.CreateFont(xTickFontSize, 0, (xAxisAlign * 10), 0, 700, FALSE, FALSE, 0,
								ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
								DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
							pDC->SelectObject(&sideFont2);
							pDC->GetTextMetrics(&tm);
							charWidth = tm.tmAveCharWidth;
							if(xAxisAlign < 180)
								pDC->TextOut(tickXLocation - 8, yApexPoint + 8 + (xAxisLabelLength * charWidth), tickLabel);
							else
								pDC->TextOut(tickXLocation + 2, yApexPoint + 8, tickLabel);

							pDC->SelectObject(pOldFont);
						}
						rightXTick = tickXLocation;

						//draw 1/2 tick line (grey)
						int midTickXLocation;
						midTickXLocation = xApexPoint + (int)(x * tickScale * .5) + (int)((x - 1) * tickScale * .5);

						//draw tick mark
						pDC->MoveTo(midTickXLocation,yApexPoint + (int)(depth * .2));
						pDC->LineTo(midTickXLocation,yApexPoint);

						//draw grid lines
						gridLineColor = LIGHT_GREY;
						CPen midGridPen (PS_SOLID, 1, gridLineColor);
						pDC->SelectObject(&midGridPen);
						pDC->MoveTo(midTickXLocation, yApexPoint);
						pDC->LineTo(midTickXLocation + depth, yApexPoint - depth);
						if(graphHasGridLines)
						{
							pDC->LineTo(midTickXLocation + depth, yApexPoint - yAxisHeight - depth);
						}
						pDC->SelectObject(pOldPen);
					}
				}

				break;
	}
}

void CGraph::AddSeries(CGraphSeries* dataSet)
{
	int numData = 0;

	numData = dataSet->GetDataCount();

	if(numData > seriesSize)
		seriesSize = numData;

	graphSeries->AddTail(dataSet);
}

void CGraph::SetXAxisLabel(CString label)
{
	axisXLabel = label;
	xAxisLabelLength = axisXLabel.GetLength();
}

void CGraph::SetYAxisLabel(CString label)
{
	axisYLabel = label;
	yAxisLabelLength = axisYLabel.GetLength();
}

void CGraph::DrawSeries(CDC* pDC)
{
	switch(graphType)
	{
		case BAR_GRAPH :
				 DrawBarSeries(pDC);
				 break;
		case LINE_GRAPH :
				 DrawLineSeries(pDC);
				 break;
		case PIE_GRAPH :
				 DrawPieSeries(pDC);
				 break;
		case SCATTER_GRAPH :
				 DrawScatterSeries(pDC);
				 break;
		case BOX_WHISKER_GRAPH :
				 DrawBoxWhiskerSeries(pDC);
				 break;
		case STACKED_BAR_GRAPH :
				 DrawStackedBarSeries(pDC);
				 break;
		case XY_LINE_GRAPH :
				 DrawXYLineSeries(pDC);
				 break;
//		case RADAR_GRAPH :
//				 DrawRadarSeries(pDC);
//				 break;
		case BAR_GRAPH_3D :
				 Draw3DBarSeries(pDC);
				 break;
		case LINE_GRAPH_3D :
				 Draw3DLineSeries(pDC);
				 break;
		case STACKED_BAR_GRAPH_3D :
				 Draw3DStackedBarSeries(pDC);
				 break;
		case PIE_GRAPH_3D :
				 Draw3DPieSeries(pDC);
				 break;
		default :
				 AfxMessageBox(_T("No graph type to display"));
				 break;
	}

}

void CGraph::SetLegend(int datagroup, CString label)
{
	if(graphType != SCATTER_GRAPH)	//scatter graphs don't need legends
	{
		graphLegend.SetLegendText(datagroup, label);
		graphHasLegend = TRUE;
		if(legendMaxText < label.GetLength())
			legendMaxText = label.GetLength();
	}
}

int CGraph::DrawLegend(CDC* pDC)
{
	//determine size of legend
	//12 chars per seriesSize + 6 for spacing (3 top and bottom) 
	//+ 1 set for label title(3+12+6) + rectangle (2 chars) + 3 for final bottom buffer
	int legendHeight = 23 + (seriesSize * 18) + 3;
	int legendL, legendT, legendR, legendB;
	int barL, barT, barR, barB;
	TEXTMETRIC tm;

	legendT = (maxHeight / 2) - (legendHeight / 2);
	legendB = legendT + legendHeight;
	legendR = maxWidth - 5;
	legendL = legendR - ((graphLegend.GetLegendLength() * (int)(legendFontSize * .66)) + 25);
		//allows 50 pixels for display of legend bar 45 + 5 space.
	legendWidth = legendR - legendL;

	pDC->Rectangle(legendL, legendT, legendR, legendB);
	CFont legendTitleFont;
	legendTitleFont.CreateFont(16, 0, 0, 0, 700, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
	CFont* pOldFont = (CFont*) pDC->SelectObject(&legendTitleFont);
	pDC->GetTextMetrics(&tm);
	int charWidth = tm.tmAveCharWidth;
	pDC->TextOut(legendL + (legendWidth / 2) - (3 * charWidth), 
					legendT + 3, "");
	pDC->SelectObject(pOldFont);
	for(int i = 0; i < seriesSize; i++)
	{
		//top "Legend" text will use 12 + 3 top + 6 bottom (21 total)
		//each legend label will need 3 chars on top, so the 24 in the offset
		//each label than uses 12 + 3 below + 3 above next label, so 18
		// in the i * offset.  
		CFont legendFont;
		legendFont.CreateFont(legendFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
			ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
		pOldFont = (CFont*) pDC->SelectObject(&legendFont);

		pDC->TextOut(legendL + 5, legendT + 24 + (i * 18) + 1, graphLegend.GetLegendText(i));

		pDC->SelectObject(pOldFont);

		//draw bar
		COLORREF barColor;
		barColor = GetColor(i);
		CBrush brush (barColor);
		CBrush* pOldBrush;
		pOldBrush = pDC->SelectObject(&brush);

		barL = legendL + 5 + (legendMaxText * (legendFontSize / 2)) + 5;
		barT = legendT + 24 + (i * 18) + 1, graphLegend.GetLegendText(i);
		barR = legendR - 5;
		barB = barT + 12;
		pDC->Rectangle(barL, barT, barR, barB);

		pDC->SelectObject(pOldBrush);
	}

	return legendL;
}

void CGraph::SetGraphTitle(CString title)
{
	graphTitle = title;
}

void CGraph::SetXTickFontSize(int size)
{
	xTickFontSize = size;
}

void CGraph::SetYTickFontSize(int size)
{
	yTickFontSize = size;
}

void CGraph::SetLegendFontSize(int size)
{
	legendFontSize = size;
}


int CGraph::PrintGraph(CDC* pDC, CPrintInfo* pInfo)
{
	CString		str;
	CFont		graphTitleFont, textFont;
	TEXTMETRIC	tm;
	CString tickLabel;

	maxHeight = pGraphT - pGraphB; 
	maxWidth = pGraphR - pGraphL; 

    //titleFont is 24 point, bold
	graphTitleFont.CreateFont(-480, 0, 0, 0, 700, FALSE, FALSE, 0, ANSI_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH || FF_ROMAN,
			_T("Times New Roman"));

	//textFont is 12 point
	textFont.CreateFont(-240, 0, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH || FF_ROMAN,
			_T("Times New Roman"));

	pDC->SetMapMode(MM_TWIPS);
	CFont* pOldFont = (CFont*) pDC->SelectObject(&graphTitleFont);
	pDC->GetTextMetrics(&tm);

	int charWidth = tm.tmAveCharWidth;
	int charHeight = tm.tmHeight;
	//next line is centered....trust me !!
	pDC->TextOut(pGraphL + (maxWidth / 2) - ((graphTitle.GetLength() / 2) * charWidth), pGraphT - 50, graphTitle);

	pDC->SelectObject(pOldFont);
	
	if((graphType == PIE_GRAPH) || (graphType == PIE_GRAPH_3D))
	{
		xAxisWidth = pGraphR - pGraphL - 200;
		yAxisHeight = maxHeight - 600 - charHeight;
		xApexPoint = pGraphL + 500;
		yApexPoint = pGraphB + 500;
	}
	else
	{
		//compute label size for axis alignment
		CFont sideFont, axisFont;
		int tFontSize;
		tFontSize = 16;
		if(maxWidth > maxHeight)
		{
			while((axisYLabel.GetLength() * (tFontSize / 2)) > maxHeight)
			{
				tFontSize -= 2;
			}
		}
		else
		{
			while((axisXLabel.GetLength() * (tFontSize / 2)) > maxWidth)
			{
				tFontSize -= 2;
			}
		}
		//set tFontSize for printer
		tFontSize *= -20;
		axisFont.CreateFont(tFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
			ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
		sideFont.CreateFont(tFontSize, 0, 900, 0, 700, FALSE, FALSE, 0,
			ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));

		CFont* pOldFont = (CFont*) pDC->SelectObject(&sideFont);
		pDC->GetTextMetrics(&tm);
		int charWidth = tm.tmAveCharWidth;
		int labelHeight = tm.tmHeight + 50;
		if(graphAlignment)
		{
			if(axisXLabel.GetLength() == 0)
				labelHeight = 0;
		}
		else
		{
			if(axisYLabel.GetLength() == 0)
				labelHeight = 0;
		}
		pDC->SelectObject(pOldFont);
		
		tickLabel.Format(_T("%d"), maxTick);


		//determine axis specifications 
		if(graphAlignment)
		{
			xApexPoint = pGraphL + (tickLabel.GetLength() * charWidth) + labelHeight + 100; 

			if(!xAxisAlign)  //horizontal
			{
				yApexPoint = pGraphB + 50 + (labelHeight * 2);		
			}
			else
			{
				yApexPoint = pGraphB + labelHeight + (xAxisLabelLength * labelHeight);
			}
			yAxisHeight = pGraphT - 200 - charHeight/*this is title height*/ - yApexPoint;
			xAxisWidth = (maxWidth - 100) - xApexPoint;

		}
		else
		{
			xApexPoint = pGraphL + (xAxisLabelLength * charWidth) + labelHeight; 

			yApexPoint = pGraphB + 50 + (labelHeight * 2);		
			yAxisHeight = pGraphT - 200 - charHeight/*this is title height*/ - yApexPoint;
			xAxisWidth = (maxWidth - 100) - xApexPoint;
		}
	}

	//draw legend
	if(graphHasLegend)
	{
		int legendL = PrintLegend(pDC);
		xAxisWidth = legendL - 200 - xApexPoint;
	}

	if(graphType != 2)  //not pie
	{
		//draw axis lines
		PrintAxis(pDC);
	}

	//draw series data and labels
	PrintSeries(pDC);

	
	//update pGraphB value to pad some space below the graph
	pGraphB -= 90;
	return pGraphB;
}


int CGraph::PrintLegend(CDC *pDC)
{
	CFont		legendLabelFont, legendTitleFont;
	TEXTMETRIC	tm;

	int legendL, legendT, legendR, legendB;
	int barL, barT, barR, barB;

	//legendLabelFont is 10 point
	legendLabelFont.CreateFont(-200, 0, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH || FF_ROMAN,
			_T("Times New Roman"));

	//legendTitleFont is 12 point
	legendTitleFont.CreateFont(-240, 0, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH || FF_ROMAN,
			_T("Times New Roman"));

	CFont* pOldFont = (CFont*) pDC->SelectObject(&legendTitleFont);
	pDC->GetTextMetrics(&tm);
	int legendCharWidth = tm.tmAveCharWidth;
	legendT = (((pGraphB - pGraphT) / 2) + pGraphT) + (((seriesSize + 1) / 2) * 240 + 300);
	legendB = (((pGraphB - pGraphT) / 2) + pGraphT) - (((seriesSize + 1) / 2) * 240 + 300);
	legendR = pGraphR - 100;
	legendL = legendR - ((graphLegend.GetLegendLength() * legendCharWidth) + 1000);
	legendWidth = legendR - legendL;

	pDC->Rectangle(legendL, legendT, legendR, legendB);
	pDC->TextOut(legendL + (legendWidth / 2) - 360, 
					legendT - 160, "");
	pDC->SelectObject(pOldFont);
	for(int i = 0; i < seriesSize; i++)
	{
		pOldFont = (CFont*) pDC->SelectObject(&legendLabelFont);
		pDC->GetTextMetrics(&tm);

		int charWidth = tm.tmMaxCharWidth;
		int charHeight = tm.tmHeight;

		CString valuStr;
		pDC->TextOut(legendL + 100, legendT - 500 - (i * charHeight), graphLegend.GetLegendText(i));
		pDC->SelectObject(pOldFont);

		//draw bar
		COLORREF barColor;
		barColor = GetColor(i);
		CBrush brush (barColor);
		CBrush* pOldBrush;
		pOldBrush = pDC->SelectObject(&brush);

		//barT uses a 560 offset below,...why?
		//would be 500, but barB is only using 120 height,
		//when it should be 240 (but 240 puts bars too close
		//together, so I'm setting them to 1/2 size.
		//therefore, I want to adjust my barT by half of that
		//adjustment ( 1/2 of 120 is 60).  This basically centers
		//the bar 1/2 way in text, and makes it 1/2 the height
		//of text
		barL = legendL + 50 + (legendMaxText * charWidth / 2);
		barT = legendT - 560 - (i * charHeight);
		barR = legendR - 100;
		barB = barT - 120;
		pDC->Rectangle(barL, barT, barR, barB);

		pDC->SelectObject(pOldBrush);
	
	}
	return legendL;
}

void CGraph::PrintAxis(CDC *pDC)
{
	TEXTMETRIC tm;
	pDC->SetTextColor(BLACK);
	CFont sideFont, axisFont;
	int charWidth;
	int charHeight;
	int tFontSize;
	double tickScale;
	int tickYLocation;
	int seriesSpace;
	int tickXLocation;

	switch(graphType)
	{
		case BAR_GRAPH :
		case LINE_GRAPH :
		case SCATTER_GRAPH :
		case BOX_WHISKER_GRAPH :
//		case RADAR_GRAPH :
		case STACKED_BAR_GRAPH :
		case XY_LINE_GRAPH :
				//draw y axis
				pDC->MoveTo(xApexPoint, yApexPoint);  
					//unlike drawAxis, the lineto below uses a + in the y
					//coordinate because printer uses negative locations
					//when going down a page, so use a + to move the line
					//upwards...
				pDC->LineTo(xApexPoint, yApexPoint + yAxisHeight);

				//draw x axis
				pDC->MoveTo(xApexPoint, yApexPoint);  
				pDC->LineTo(xApexPoint + xAxisWidth, yApexPoint);

				//draw labels
				tFontSize = 16;
				if(maxWidth > maxHeight)
				{
					while((axisYLabel.GetLength() * (tFontSize / 2)) > maxHeight)
					{
						tFontSize -= 2;
					}
				}
				else
				{
					while((axisXLabel.GetLength() * (tFontSize / 2)) > maxWidth)
					{
						tFontSize -= 2;
					}
				}
				tFontSize *= 20;
				axisFont.CreateFont(tFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
					ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
				sideFont.CreateFont(tFontSize, 0, 2700, 0, 700, FALSE, FALSE, 0,
					ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
				CFont* pOldFont;
				pOldFont = (CFont*) pDC->SelectObject(&sideFont);
				pDC->GetTextMetrics(&tm);
				charWidth = tm.tmAveCharWidth;
				charHeight = tm.tmHeight;

				if(graphAlignment)
				{
					pDC->TextOut(pGraphL + 10, pGraphT - (maxHeight / 2) - ((axisYLabel.GetLength() / 2) * charHeight), axisYLabel);

					pDC->SelectObject(&axisFont);
					pDC->GetTextMetrics(&tm);
					charWidth = tm.tmAveCharWidth;

					pDC->TextOut(xApexPoint + (xAxisWidth / 2) - ((axisXLabel.GetLength() / 2) * charWidth), pGraphB + 50 + charHeight, axisXLabel);

					pDC->SelectObject(pOldFont);

					tickScale = 0.00;
					tickScale = (yAxisHeight - 100) / numTicks;

					//draw y axis ticks
					for(int y = 1; y <= numTicks; y++)  
					{
						tickYLocation = yApexPoint + (int)(y * tickScale);

						//draw tick mark
						pDC->MoveTo(xApexPoint - 60, tickYLocation);
						pDC->LineTo(xApexPoint + 60, tickYLocation);

						if(graphHasGridLines)
						{
							//draw grid lines
							COLORREF gridLineColor;
							gridLineColor = DARK_GREY;
							CBrush gridBrush (gridLineColor);
							CBrush* pOldBrush;
							pOldBrush = pDC->SelectObject(&gridBrush);
							pDC->MoveTo(xApexPoint, tickYLocation);
							pDC->LineTo(xApexPoint + xAxisWidth, tickYLocation);
							pDC->SelectObject(pOldBrush);
						}

						//draw tick label
						CString tickLabel;
						tickLabel.Format(_T("%d"), minTick + (y * tickSpace));

						CFont yFont;
						yFont.CreateFont(yTickFontSize * 20, 0, 0, 0, 700, FALSE, FALSE, 0,
							ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
							DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
						pOldFont = (CFont*) pDC->SelectObject(&yFont);

						pDC->TextOut(xApexPoint - 200 - (tickLabel.GetLength() * (yTickFontSize / 2) * 20), tickYLocation + 120, tickLabel);

						p_topYTick = tickYLocation;
						pDC->SelectObject(pOldFont);

					}

					//draw X axis tick marks
					if(graphType != SCATTER_GRAPH)
					{
						POSITION pos;
						pos = graphSeries->GetHeadPosition();
						CGraphSeries* tmpSeries;
						for(int x = 1; x <= graphSeries->GetCount(); x++)
						{
							tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

							int seriesSpace;
							int tickXLocation;
							seriesSpace = xAxisWidth / graphSeries->GetCount();
							tickXLocation = xApexPoint + ((x * seriesSpace) - (seriesSpace / 2));

							pDC->MoveTo(tickXLocation,yApexPoint - 60);
							pDC->LineTo(tickXLocation,yApexPoint + 60);

							//draw tick label
							CString tickLabel;
							tickLabel = tmpSeries->GetLabel();
							if(!xAxisAlign)  //horizontal
								pDC->TextOut(tickXLocation - ((tickLabel.GetLength() / 2) * (xTickFontSize * 10)), yApexPoint - 160, tickLabel);
							else
							{
								CFont sideFont2;
								sideFont2.CreateFont(xTickFontSize * 20, 0, ((360 - xAxisAlign) * 10), 0, 700, FALSE, FALSE, 0,
									ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
									DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
								pOldFont = (CFont*) pDC->SelectObject(&sideFont2);
								pDC->GetTextMetrics(&tm);
								charWidth = tm.tmAveCharWidth;
								if(xAxisAlign < 180)
									pDC->TextOut(tickXLocation - (xTickFontSize * 10), yApexPoint - (xTickFontSize * 10) - (xAxisLabelLength * charWidth), tickLabel);
								else
									pDC->TextOut(tickXLocation + (xTickFontSize * 10), yApexPoint - 160, tickLabel);

							}
							p_rightXTick = tickXLocation;
							pDC->SelectObject(pOldFont);
						}
					}
					else
					{
						//scatter graphs will use the same tick marking on both axis lines
						int seriesSpace;
						seriesSpace = (xAxisWidth - 200) / numTicks;
						for(int x = 1; x <= numTicks; x++)  
						{
							int tickXLocation;
							tickXLocation = xApexPoint + (x * seriesSpace);

							//draw tick mark
							pDC->MoveTo(tickXLocation, yApexPoint - 60);
							pDC->LineTo(tickXLocation, yApexPoint + 60);

							//draw tick label
							CString tickLabel;
							tickLabel.Format(_T("%d"), x * tickSpace);

							CFont xFont;
							xFont.CreateFont(xTickFontSize * 20, 0, 0, 0, 700, FALSE, FALSE, 0,
								ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
								DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
							pOldFont = (CFont*) pDC->SelectObject(&xFont);
							pDC->GetTextMetrics(&tm);
							charWidth = tm.tmAveCharWidth;

							pDC->TextOut(tickXLocation - (tickLabel.GetLength() * charWidth / 2), yApexPoint - 160, tickLabel);

							p_rightXTick = tickXLocation;
							pDC->SelectObject(pOldFont);
						}
					}
				}
				else
				{
					//reverse above stuff and treat as horizontal graph
					pDC->TextOut(pGraphL + 10, pGraphT - (maxHeight / 2) - ((axisXLabel.GetLength() / 2) * charHeight), axisXLabel);

					pDC->SelectObject(&axisFont);
					pDC->GetTextMetrics(&tm);
					charWidth = tm.tmAveCharWidth;
					int labelHeight = tm.tmHeight + 50;
					if(axisYLabel.GetLength() == 0)
						labelHeight = 0;

					pDC->TextOut(xApexPoint + (xAxisWidth / 2) - ((axisYLabel.GetLength() / 2) * charWidth), pGraphB + 50 + labelHeight, axisYLabel);
					
					pDC->SelectObject(pOldFont);

					//to allow scalability (height may be less than tickRange)
					double tickScale;
					tickScale = 0.00;

					//draw y axis ticks
					if(graphType != SCATTER_GRAPH)
					{
						tickScale = (yAxisHeight - 100) / seriesSize;
						POSITION pos;
						pos = graphSeries->GetHeadPosition();
						CGraphSeries* tmpSeries;
						for(int y = 1; y <= graphSeries->GetCount(); y++)  
						{
							tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

							int seriesSpace;
							int tickYLocation;
							seriesSpace = (yAxisHeight - 50) / graphSeries->GetCount();
							tickYLocation = yApexPoint + ((y * seriesSpace) - (seriesSpace / 2));

							//draw tick mark
							pDC->MoveTo(xApexPoint - 60, tickYLocation);
							pDC->LineTo(xApexPoint + 60, tickYLocation);

							//draw tick label
							CString tickLabel;
							tickLabel = tmpSeries->GetLabel();

							CFont sideFont2;
							sideFont2.CreateFont(yTickFontSize * 20, 0, 0, 0, 700, FALSE, FALSE, 0,
								ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
								DEFAULT_QUALITY, FIXED_PITCH | FF_ROMAN,_T("Arial"));
							pOldFont = (CFont*) pDC->SelectObject(&sideFont2);
							pDC->GetTextMetrics(&tm);

							pDC->TextOut(xApexPoint - (tickLabel.GetLength() * (yTickFontSize * 10)) - 200, tickYLocation + 120, tickLabel);

							p_topYTick = tickYLocation;
							pDC->SelectObject(pOldFont);
						}
					}
					else
					{
						//scatter graphs will use the same tick marking on both axis lines
						for(int y = 1; y <= numTicks; y++)  
						{
							tickScale = (yAxisHeight - 100) / numTicks;
							int tickYLocation;
							tickYLocation = yApexPoint + (int)(y * tickScale);

							//draw tick mark
							pDC->MoveTo(xApexPoint - 60, tickYLocation);
							pDC->LineTo(xApexPoint + 60, tickYLocation);

							//draw tick label
							CString tickLabel;
							tickLabel.Format(_T("%d"), y * tickSpace);

							CFont yFont;
							yFont.CreateFont(yTickFontSize * 20, 0, 0, 0, 700, FALSE, FALSE, 0,
								ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
								DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
							pOldFont = (CFont*) pDC->SelectObject(&yFont);

							pDC->TextOut(xApexPoint - (tickLabel.GetLength() * (yTickFontSize * 10)) - 200, tickYLocation + 120, tickLabel);

							p_topYTick = tickYLocation;
							pDC->SelectObject(pOldFont);
						}
					}

					//draw X axis tick marks
					tickScale = (xAxisWidth - 200) / numTicks;
					for(int x = 1; x <= numTicks; x++)
					{
						int tickXLocation;
						tickXLocation = xApexPoint + (int)(x * tickScale);

						pDC->MoveTo(tickXLocation,yApexPoint - 60);
						pDC->LineTo(tickXLocation,yApexPoint + 60);

						if(graphHasGridLines)
						{
							//draw grid lines
							COLORREF gridLineColor;
							gridLineColor = DARK_GREY;
							CBrush gridBrush (gridLineColor);
							CBrush* pOldBrush;
							pOldBrush = pDC->SelectObject(&gridBrush);
							pDC->MoveTo(tickXLocation, yApexPoint);
							pDC->LineTo(tickXLocation, yApexPoint + yAxisHeight);
							pDC->SelectObject(pOldBrush);
						}

						//draw tick label
						CString tickLabel;
						tickLabel.Format(_T("%d"), x * tickSpace);
						if(!xAxisAlign)  //horizontal
						{
							CFont xFont;
							xFont.CreateFont(xTickFontSize * 20, 0, 0, 0, 700, FALSE, FALSE, 0,
								ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
								DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
							pOldFont = (CFont*) pDC->SelectObject(&xFont);
							charWidth = tm.tmAveCharWidth;
							pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), yApexPoint - 160, tickLabel);
							pDC->SelectObject(pOldFont);
						}
						else
						{
							CFont sideFont2;
							sideFont2.CreateFont(xTickFontSize * 20, 0, (xAxisAlign * 10), 0, 700, FALSE, FALSE, 0,
								ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
								DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
							pOldFont = (CFont*) pDC->SelectObject(&sideFont2);
							pDC->GetTextMetrics(&tm);
							charWidth = tm.tmAveCharWidth;
							if(xAxisAlign < 180)
								pDC->TextOut(tickXLocation - (xTickFontSize * 10), yApexPoint - (xTickFontSize * 10) - (xAxisLabelLength * charWidth), tickLabel);
							else
								pDC->TextOut(tickXLocation + (xTickFontSize * 10), yApexPoint - 160, tickLabel);

							pDC->SelectObject(pOldFont);
						}
						p_rightXTick = tickXLocation;
					}
				}
				break;
		case BAR_GRAPH_3D :
		case LINE_GRAPH_3D :
		case STACKED_BAR_GRAPH_3D :
				if(graphAlignment)
					depth = (int)(yAxisHeight * .05);
				else
					depth = (int)(xAxisWidth * .05);
				xApexPoint += depth;
				yApexPoint += depth;
				xAxisWidth -= depth;
				yAxisHeight -= depth;
				//draw y axis
				pDC->MoveTo(xApexPoint, yApexPoint);  
				pDC->LineTo(xApexPoint, yApexPoint + yAxisHeight);
				//draw x axis
				pDC->MoveTo(xApexPoint, yApexPoint);  
				pDC->LineTo(xApexPoint + xAxisWidth, yApexPoint);

				//adjust graph settings for 3D look
				yApexPoint -= depth;
				xApexPoint -= depth;
				//foreground axis
				pDC->MoveTo(xApexPoint, yApexPoint - (int)(depth * .5));  
				pDC->LineTo(xApexPoint, yApexPoint + yAxisHeight);
				pDC->MoveTo(xApexPoint - (int)(depth * .5), yApexPoint);  
				pDC->LineTo(xApexPoint + xAxisWidth, yApexPoint);
				//connecting line at apex
				pDC->MoveTo(xApexPoint, yApexPoint);  
				pDC->LineTo(xApexPoint + depth, yApexPoint + depth);
				//connecting line at height
				pDC->MoveTo(xApexPoint, yApexPoint + yAxisHeight);  
				pDC->LineTo(xApexPoint + depth, yApexPoint + yAxisHeight + depth);
				//connecting line at width
				pDC->MoveTo(xApexPoint + xAxisWidth, yApexPoint);  
				pDC->LineTo(xApexPoint + xAxisWidth + depth, yApexPoint + depth);
				//top and right border lines
				pDC->MoveTo(xApexPoint + depth, yApexPoint + yAxisHeight + depth);
				pDC->LineTo(xApexPoint + xAxisWidth + depth, yApexPoint + yAxisHeight + depth);
				pDC->LineTo(xApexPoint + xAxisWidth + depth, yApexPoint + depth);

				//Tick marks ... and labels too
				//draw labels
				tFontSize = 16;
				if(maxWidth > maxHeight)
				{
					while((axisYLabel.GetLength() * (tFontSize / 2)) > maxHeight)
					{
						tFontSize -= 2;
					}
				}
				else
				{
					while((axisXLabel.GetLength() * (tFontSize / 2)) > maxWidth)
					{
						tFontSize -= 2;
					}
				}
				axisFont.CreateFont(tFontSize * 20, 0, 0, 0, 700, FALSE, FALSE, 0,
					ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
				sideFont.CreateFont(tFontSize * 20, 0, 2700, 0, 700, FALSE, FALSE, 0,
					ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));

				pOldFont = (CFont*) pDC->SelectObject(&sideFont);
				pDC->GetTextMetrics(&tm);
				charWidth = tm.tmAveCharWidth;
				charHeight = tm.tmHeight;

				if(graphAlignment)
				{
					pDC->TextOut(pGraphL + 10, pGraphT - (maxHeight / 2) - ((axisYLabel.GetLength() * charWidth) / 2), axisYLabel);

					pDC->SelectObject(&axisFont);
					pDC->GetTextMetrics(&tm);
					charWidth = tm.tmAveCharWidth;
					pDC->TextOut(xApexPoint + (xAxisWidth / 2) - ((axisXLabel.GetLength() / 2) * charWidth), pGraphB + 50 + charHeight, axisXLabel);
					
					pDC->SelectObject(pOldFont);

					//to allow scalability (height may be less than tickRange)
					tickScale = 0.00;
					tickScale = yAxisHeight / numTicks;

					//draw y axis ticks
					for(int y = 1; y <= numTicks; y++)  
					{
						tickYLocation = yApexPoint + (int)(y * tickScale);

						//draw tick mark
						pDC->MoveTo(xApexPoint - (int)(depth * .5), tickYLocation);
						pDC->LineTo(xApexPoint, tickYLocation);

						COLORREF gridLineColor;
						CPen* pOldPen;
						if(graphHasGridLines)
						{
							//draw grid lines
							gridLineColor = DARK_GREY;
							CPen gridPen (PS_SOLID, 1, gridLineColor);
							pOldPen = pDC->SelectObject(&gridPen);
							pDC->MoveTo(xApexPoint, tickYLocation);
							pDC->LineTo(xApexPoint + depth, tickYLocation + depth);
							pDC->LineTo(xApexPoint + xAxisWidth + depth, tickYLocation + depth);
							pDC->SelectObject(pOldPen);
						}

						//draw tick label
						CString tickLabel;
						tickLabel.Format(_T("%d"), y * tickSpace);

						CFont yFont;
						yFont.CreateFont(yTickFontSize * 20, 0, 0, 0, 700, FALSE, FALSE, 0,
							ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
							DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
						pOldFont = (CFont*) pDC->SelectObject(&yFont);

						pDC->TextOut(xApexPoint - 200 - (tickLabel.GetLength() * ((yTickFontSize / 2) * 20)), tickYLocation + 120, tickLabel);
						topYTick = tickYLocation;
						pDC->SelectObject(pOldFont);

						//draw 1/2 tick line (grey)
						int midTickYLocation;
						midTickYLocation = yApexPoint + (int)(y * tickScale * .5) + (int)((y - 1) * tickScale * .5);

						//draw tick mark
						pDC->MoveTo(xApexPoint - (int)(depth * .5), midTickYLocation);
						pDC->LineTo(xApexPoint, midTickYLocation);

						if(graphHasGridLines)
						{
							//draw grid lines
							gridLineColor = LIGHT_GREY;
							CPen midGridPen (PS_SOLID, 1, gridLineColor);
							pOldPen = pDC->SelectObject(&midGridPen);
							pDC->MoveTo(xApexPoint, midTickYLocation);
							pDC->LineTo(xApexPoint + depth, midTickYLocation + depth);
							pDC->LineTo(xApexPoint + xAxisWidth + depth, midTickYLocation + depth);
							pDC->SelectObject(pOldPen);
						}
					}

					//draw X axis tick marks
					POSITION pos;
					pos = graphSeries->GetHeadPosition();
					CGraphSeries* tmpSeries;
					for(int x = 1; x <= graphSeries->GetCount(); x++)
					{
						tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

						seriesSpace = xAxisWidth / graphSeries->GetCount();
						tickXLocation = xApexPoint + ((x * seriesSpace) - (seriesSpace / 2));

						//unlike normal series, we put ticks here
						//on the outsides of the series
						pDC->MoveTo(xApexPoint + (x * seriesSpace),yApexPoint - (int)(depth * .5));
						pDC->LineTo(xApexPoint + (x * seriesSpace),yApexPoint);
						pDC->LineTo(xApexPoint + (x * seriesSpace) + depth,yApexPoint + depth);

						//draw tick label
						CString tickLabel;
						tickLabel = tmpSeries->GetLabel();
						if(!xAxisAlign)  //horizontal
							pDC->TextOut(tickXLocation - ((tickLabel.GetLength() / 2) * (xTickFontSize * 10)), yApexPoint - 160, tickLabel);
						else
						{
							CFont sideFont2;
							sideFont2.CreateFont(xTickFontSize * 20, 0, (xAxisAlign * 10), 0, 700, FALSE, FALSE, 0,
								ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
								DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
							pOldFont = (CFont*) pDC->SelectObject(&sideFont2);
							pDC->GetTextMetrics(&tm);
							charWidth = tm.tmAveCharWidth;
							if(xAxisAlign < 180)
								pDC->TextOut(tickXLocation - (xTickFontSize * 10), yApexPoint - (xTickFontSize * 10) - (xAxisLabelLength * charWidth), tickLabel);
							else
								pDC->TextOut(tickXLocation + (xTickFontSize * 10), yApexPoint - 160, tickLabel);

							pDC->SelectObject(pOldFont);
						}
						p_rightXTick = tickXLocation;
					}
				}
				else
				{
					//reverse above stuff and treat as horizontal graph
					pDC->TextOut(pGraphL + 10, pGraphT - (maxHeight / 2) - ((axisXLabel.GetLength() / 2) * charHeight), axisXLabel);

					pDC->SelectObject(&axisFont);
					pDC->GetTextMetrics(&tm);
					charWidth = tm.tmAveCharWidth;
					int labelHeight;
					labelHeight = tm.tmHeight + 50;
					if(axisYLabel.GetLength() == 0)
						labelHeight = 0;
					pDC->TextOut(xApexPoint + (xAxisWidth / 2) - ((axisXLabel.GetLength() / 2) * charWidth), pGraphB + 50 + labelHeight, axisYLabel);
					
					pDC->SelectObject(pOldFont);

					//to allow scalability (height may be less than tickRange)
					tickScale = 0.00;

					//draw y axis ticks
					tickScale = (yAxisHeight - 100) / seriesSize;
					POSITION pos;
					pos = graphSeries->GetHeadPosition();
					CGraphSeries* tmpSeries;
					for(int y = 1; y <= graphSeries->GetCount(); y++)  
					{
						tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

						seriesSpace = (yAxisHeight - 50) / graphSeries->GetCount();
						tickYLocation = yApexPoint + (y * seriesSpace);

						//draw tick mark
						//unlike normal series, we put ticks here on the 
						//outside of the series
						pDC->MoveTo(xApexPoint - (int)(depth * .5), tickYLocation);
						pDC->LineTo(xApexPoint, tickYLocation);

						//draw tick label
						CString tickLabel;
						tickLabel = tmpSeries->GetLabel();

						CFont yFont;
						yFont.CreateFont(yTickFontSize * 20, 0, 0, 0, 700, FALSE, FALSE, 0,
							ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
							DEFAULT_QUALITY, FIXED_PITCH | FF_ROMAN,_T("Arial"));
						pOldFont = (CFont*) pDC->SelectObject(&yFont);
						pDC->GetTextMetrics(&tm);

						pDC->TextOut(xApexPoint - (tickLabel.GetLength() * (yTickFontSize * 10)), tickYLocation + 120, tickLabel);

						p_topYTick = tickYLocation;
						pDC->SelectObject(pOldFont);
					}

					//draw X axis tick marks
					tickScale = (xAxisWidth - 200) / numTicks;
					for(int x = 1; x <= numTicks; x++)
					{
						tickXLocation = xApexPoint + (int)(x * tickScale);

						pDC->MoveTo(tickXLocation,yApexPoint - (int)(depth * .25));
						pDC->LineTo(tickXLocation,yApexPoint);

						COLORREF gridLineColor;
						CPen* pOldPen;
						if(graphHasGridLines)
						{
							//draw grid lines
							gridLineColor = DARK_GREY;
							CPen gridPen (PS_SOLID, 1, gridLineColor);
							pOldPen = pDC->SelectObject(&gridPen);
							pDC->MoveTo(tickXLocation, yApexPoint);
							pDC->LineTo(tickXLocation + depth, yApexPoint + depth);
							pDC->LineTo(tickXLocation + depth, yApexPoint + yAxisHeight + depth);
							pDC->SelectObject(pOldPen);
						}

						//draw tick label
						CString tickLabel;
						tickLabel.Format(_T("%d"), x * tickSpace);
						if(!xAxisAlign)  //horizontal
						{
							CFont xFont;
							xFont.CreateFont(xTickFontSize * 20, 0, 0, 0, 700, FALSE, FALSE, 0,
								ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
								DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
							pOldFont = (CFont*) pDC->SelectObject(&xFont);
							charWidth = tm.tmAveCharWidth;
							pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), yApexPoint - 160, tickLabel);
							pDC->SelectObject(pOldFont);
						}
						else
						{
							CFont sideFont2;
							sideFont2.CreateFont(xTickFontSize * 20, 0, (xAxisAlign * 10), 0, 700, FALSE, FALSE, 0,
								ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
								DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
							pOldFont = (CFont*) pDC->SelectObject(&sideFont2);
							pDC->GetTextMetrics(&tm);
							charWidth = tm.tmAveCharWidth;
							if(xAxisAlign < 180)
								pDC->TextOut(tickXLocation - (xTickFontSize * 10), yApexPoint - (xTickFontSize * 10) - (xAxisLabelLength * charWidth), tickLabel);
							else
								pDC->TextOut(tickXLocation + (xTickFontSize * 10), yApexPoint - 160, tickLabel);

							pDC->SelectObject(pOldFont);
						}
						p_rightXTick = tickXLocation;

						//draw 1/2 tick line (grey)
						int midTickXLocation;
						midTickXLocation = xApexPoint + (int)(x * tickScale * .5) + (int)((x - 1) * tickScale * .5);

						//draw tick mark
						pDC->MoveTo(midTickXLocation,yApexPoint - (int)(depth * .25));
						pDC->LineTo(midTickXLocation,yApexPoint);

						if(graphHasGridLines)
						{
							//draw grid lines
							gridLineColor = LIGHT_GREY;
							CPen midGridPen (PS_SOLID, 1, gridLineColor);
							pDC->SelectObject(&midGridPen);
							pDC->MoveTo(midTickXLocation, yApexPoint);
							pDC->LineTo(midTickXLocation + depth, yApexPoint + depth);
							pDC->LineTo(midTickXLocation + depth, yApexPoint + yAxisHeight + depth);
							pDC->SelectObject(pOldPen);
						}
					}

				}
				break;
	}
}

void CGraph::PrintSeries(CDC *pDC)
{
	switch(graphType)
	{
		case BAR_GRAPH :
				 PrintBarSeries(pDC);
				 break;
		case LINE_GRAPH :
				 PrintLineSeries(pDC);
				 break;
		case PIE_GRAPH :
				 PrintPieSeries(pDC);
				 break;
		case SCATTER_GRAPH :
				 PrintScatterSeries(pDC);
				 break;
		case BOX_WHISKER_GRAPH :
				 PrintBoxWhiskerSeries(pDC);
				 break;
		case STACKED_BAR_GRAPH :
				 PrintStackedBarSeries(pDC);
				 break;
		case XY_LINE_GRAPH :
				 PrintXYLineSeries(pDC);
				 break;
//		case RADAR_GRAPH :
//				 PrintRadarSeries(pDC);
//				 break;
		case BAR_GRAPH_3D :
				 Print3DBarSeries(pDC);
				 break;
		case LINE_GRAPH_3D :
				 Print3DLineSeries(pDC);
				 break;
		case STACKED_BAR_GRAPH_3D :
				 Print3DStackedBarSeries(pDC);
				 break;
		case PIE_GRAPH_3D :
				 Print3DPieSeries(pDC);
				 break;
		default :
				 AfxMessageBox(_T("No graph type to print"));
				 break;
	}
}

void CGraph::SetMargins(int top, int bottom, int left, int right, int graphTop)
{
	//page margins for portrait or landscape distinction
	topMargin = top;
	bottomMargin = bottom;
	leftMargin = left;
	rightMargin = right;

	//set up rectangle area for showing the graph
	pGraphL = leftMargin;
	pGraphT = graphTop;
	pGraphB = pGraphT - 5760/*7200*/;
	pGraphR = rightMargin;

	if(pGraphB < bottomMargin)
		pGraphB = bottomMargin; 

}

void CGraph::SetGridLines(BOOL hasGridLines)
{
	graphHasGridLines = hasGridLines;
}

void CGraph::SetGraphAlignment(int alignment)
{
	graphAlignment = alignment;
	//because of how it ends up looking, I'm forcing 3D line graphs
	//to always display as if in vertical mode graph
	if(graphType == LINE_GRAPH_3D)
		graphAlignment = VERTICAL_ALIGN;  
}

void CGraph::SetTickLimits(int minTickValue, int maxTickValue, int tickStep)
{
	maxTick = maxTickValue;
	minTick = minTickValue;
	tickSpace = tickStep;
	numTicks = (maxTick - minTick) / tickStep;
	if((numTicks * tickStep) < (maxTick - minTick))
		numTicks++;
}

void CGraph::DrawBarSeries(CDC* pDC)
{
	int barWidth;
	int dataPlotSize;   //used to plot rects of data
	int barL, barT, barR, barB;
	int tickXLocation, tickYLocation;
	int seriesSpace;
	int barHeight;
	POSITION pos;
	CGraphSeries* tmpSeries;
	int x, y, s;

	if(graphAlignment)
	{
		//determine width of barchart data blocks
		seriesSpace = xAxisWidth / graphSeries->GetCount();
		barWidth = (int)((seriesSpace * .6) / seriesSize);
		dataPlotSize = seriesSize * barWidth;

		pos = graphSeries->GetHeadPosition();

		switch(graphQuadType)
		{
			case 1 :
				for(x = 1; x <= graphSeries->GetCount(); x++)
				{
					tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

					tickXLocation = xApexPoint + ((x * seriesSpace) - (seriesSpace / 2));

					for(s = 0; s < seriesSize; s++)
					{
						if(tmpSeries->GetData(s) > -1)
						{
							double dataScale = 0.00;
							dataScale = ((yAxisHeight - 10) * 1.00) / (maxTick - minTick);
							barHeight = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
							if(((tmpSeries->GetData(s) - minTick) > 0) && (barHeight < 1))
								barHeight = (int)dataScale;

							barL = tickXLocation - (dataPlotSize / 2) + (s * barWidth);
							if(barHeight > yAxisHeight)
								barT = yApexPoint - yAxisHeight;
							else
								barT = yApexPoint - barHeight;
							barR = barL + barWidth;
							barB = yApexPoint;
							
							COLORREF barColor;
							barColor = GetColor(s);
							CBrush brush (barColor);
							CBrush* pOldBrush;
							pOldBrush = pDC->SelectObject(&brush);
							pDC->Rectangle(barL, barT, barR, barB);
							pDC->SelectObject(pOldBrush);
						}
					}
				}
				break;
			case 2 :
				for(x = 1; x <= graphSeries->GetCount() / 2; x++)
				{
					tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

					tickXLocation = xApexPoint - (xAxisWidth / 2) + ((x * seriesSpace) - (seriesSpace / 2));

					for(s = 0; s < seriesSize; s++)
					{
						double dataScale = 0.00;
						dataScale = ((yAxisHeight - 10) * 1.00) / (maxTick - minTick);
						barHeight = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
						if(barHeight < 1)
							barHeight = (int)dataScale;

						barL = tickXLocation - (dataPlotSize / 2) + (s * barWidth);
						if(barHeight > (yAxisHeight / 2))
							barT = yApexPoint - (yAxisHeight / 2);
						else
							barT = yApexPoint - barHeight;
						barR = barL + barWidth;
						barB = yApexPoint;
						
						COLORREF barColor;
						barColor = GetColor(s);
						CBrush brush (barColor);
						CBrush* pOldBrush;
						pOldBrush = pDC->SelectObject(&brush);
						pDC->Rectangle(barL, barT, barR, barB);
						pDC->SelectObject(pOldBrush);
					}
				}
				//draw at the 0 location
				tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);
				tickXLocation = xApexPoint;
				for(s = 0; s < seriesSize; s++)
				{
					double dataScale = 0.00;
					dataScale = ((yAxisHeight - 10) * 1.00) / (maxTick - minTick);
					barHeight = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
					if(((tmpSeries->GetData(s) - minTick) > 0) && (barHeight < 1))
						barHeight = (int)dataScale;

					barL = tickXLocation - (dataPlotSize / 2) + (s * barWidth);
					if(barHeight > yAxisHeight)
						barT = yApexPoint - yAxisHeight;
					else
						barT = yApexPoint - barHeight;
					barR = barL + barWidth;
					barB = yApexPoint;
					
					COLORREF barColor;
					barColor = GetColor(s);
					CBrush brush (barColor);
					CBrush* pOldBrush;
					pOldBrush = pDC->SelectObject(&brush);
					pDC->Rectangle(barL, barT, barR, barB);
					pDC->SelectObject(pOldBrush);
				}
				x++;

				for(; x <= graphSeries->GetCount(); x++)
				{
					tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

					tickXLocation = xApexPoint - (xAxisWidth / 2) + ((x * seriesSpace) - (seriesSpace / 2));

					for(s = 0; s < seriesSize; s++)
					{
						double dataScale = 0.00;
						dataScale = ((yAxisHeight - 10) * 1.00) / (maxTick - minTick);
						barHeight = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
						if(((tmpSeries->GetData(s) - minTick) > 0) && (barHeight < 1))
							barHeight = (int)dataScale;

						barL = tickXLocation - (dataPlotSize / 2) + (s * barWidth);
						if(barHeight > (yAxisHeight / 2))
							barT = yApexPoint - (yAxisHeight / 2);
						else
							barT = yApexPoint - barHeight;
						barR = barL + barWidth;
						barB = yApexPoint;
						
						COLORREF barColor;
						barColor = GetColor(s);
						CBrush brush (barColor);
						CBrush* pOldBrush;
						pOldBrush = pDC->SelectObject(&brush);
						pDC->Rectangle(barL, barT, barR, barB);
						pDC->SelectObject(pOldBrush);
					}
				}
				break;
			case 3 :
				for(x = 1; x <= graphSeries->GetCount(); x++)
				{
					tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

					tickXLocation = xApexPoint + ((x * seriesSpace) - (seriesSpace / 2));

					for(s = 0; s < seriesSize; s++)
					{
						double dataScale = 0.00;
						dataScale = ((yAxisHeight - 10) * 1.00) / (maxTick - minTick);
						barHeight = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);

						barL = tickXLocation - (dataPlotSize / 2) + (s * barWidth);
						if(barHeight < 0)
						{
							if((0 - barHeight) > (yAxisHeight / 2))
								barT = yApexPoint - (yAxisHeight / 2);
							else
								barT = yApexPoint - barHeight;
						}
						else
						{
							if(barHeight > (yAxisHeight / 2))
								barT = yApexPoint - (yAxisHeight / 2);
							else
								barT = yApexPoint - barHeight;
						}
						barR = barL + barWidth;
						barB = yApexPoint;
						
						COLORREF barColor;
						barColor = GetColor(s);
						CBrush brush (barColor);
						CBrush* pOldBrush;
						pOldBrush = pDC->SelectObject(&brush);
						pDC->Rectangle(barL, barT, barR, barB);
						pDC->SelectObject(pOldBrush);
					}
				}
				break;
			case 4 :
				for(x = 1; x <= graphSeries->GetCount() / 2; x++)
				{
					tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

					tickXLocation = xApexPoint - (xAxisWidth / 2) + ((x * seriesSpace) - (seriesSpace / 2));

					for(s = 0; s < seriesSize; s++)
					{
						double dataScale = 0.00;
						dataScale = ((yAxisHeight - 10) * 1.00) / (maxTick - minTick);
						barHeight = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);

						barL = tickXLocation - (dataPlotSize / 2) + (s * barWidth);
						if(barHeight < 0)
						{
							if((0 - barHeight) > (yAxisHeight / 2))
								barT = yApexPoint - (yAxisHeight / 2);
							else
								barT = yApexPoint - barHeight;
						}
						else
						{
							if(barHeight > (yAxisHeight / 2))
								barT = yApexPoint - (yAxisHeight / 2);
							else
								barT = yApexPoint - barHeight;
						}
						barR = barL + barWidth;
						barB = yApexPoint;
						
						COLORREF barColor;
						barColor = GetColor(s);
						CBrush brush (barColor);
						CBrush* pOldBrush;
						pOldBrush = pDC->SelectObject(&brush);
						pDC->Rectangle(barL, barT, barR, barB);
						pDC->SelectObject(pOldBrush);
					}
				}
				//draw at the 0 location
				tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);
				tickXLocation = xApexPoint;
				for(s = 0; s < seriesSize; s++)
				{
					double dataScale = 0.00;
					dataScale = ((yAxisHeight - 10) * 1.00) / (maxTick - minTick);
					barHeight = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
					if(((tmpSeries->GetData(s) - minTick) > 0) && (barHeight < 1))
						barHeight = (int)dataScale;

					barL = tickXLocation - (dataPlotSize / 2) + (s * barWidth);
					if(barHeight < 0)
					{
						if((0 - barHeight) > (yAxisHeight / 2))
							barT = yApexPoint - (yAxisHeight / 2);
						else
							barT = yApexPoint - barHeight;
					}
					else
					{
						if(barHeight > (yAxisHeight / 2))
							barT = yApexPoint - (yAxisHeight / 2);
						else
							barT = yApexPoint - barHeight;
					}
					barR = barL + barWidth;
					barB = yApexPoint;
					
					COLORREF barColor;
					barColor = GetColor(s);
					CBrush brush (barColor);
					CBrush* pOldBrush;
					pOldBrush = pDC->SelectObject(&brush);
					pDC->Rectangle(barL, barT, barR, barB);
					pDC->SelectObject(pOldBrush);
				}
				x++;

				for(; x <= graphSeries->GetCount(); x++)
				{
					tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

					tickXLocation = xApexPoint - (xAxisWidth / 2) + ((x * seriesSpace) - (seriesSpace / 2));

					for(s = 0; s < seriesSize; s++)
					{
						double dataScale = 0.00;
						dataScale = ((yAxisHeight - 10) * 1.00) / (maxTick - minTick);
						barHeight = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
						if(((tmpSeries->GetData(s) - minTick) > 0) && (barHeight < 1))
							barHeight = (int)dataScale;

						barL = tickXLocation - (dataPlotSize / 2) + (s * barWidth);
						if(barHeight < 0)
						{
							if((0 - barHeight) > (yAxisHeight / 2))
								barT = yApexPoint - (yAxisHeight / 2);
							else
								barT = yApexPoint - barHeight;
						}
						else
						{
							if(barHeight > (yAxisHeight / 2))
								barT = yApexPoint - (yAxisHeight / 2);
							else
								barT = yApexPoint - barHeight;
						}
						barR = barL + barWidth;
						barB = yApexPoint;
						
						COLORREF barColor;
						barColor = GetColor(s);
						CBrush brush (barColor);
						CBrush* pOldBrush;
						pOldBrush = pDC->SelectObject(&brush);
						pDC->Rectangle(barL, barT, barR, barB);
						pDC->SelectObject(pOldBrush);
					}
				}
				break;
		}

	}
	else
	{
		//determine width of barchart data blocks
		seriesSpace = yAxisHeight / graphSeries->GetCount();
		barHeight = (int)((seriesSpace * .6) / seriesSize);
		dataPlotSize = seriesSize * barHeight;

		pos = graphSeries->GetHeadPosition();

		switch(graphQuadType)
		{
			case 1 :
				for(y = 1; y <= graphSeries->GetCount(); y++)
				{
					tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

					tickYLocation = yApexPoint - ((y * seriesSpace) - (seriesSpace / 2));

					for(s = 0; s < seriesSize; s++)
					{
						double dataScale = 0.00;
						dataScale = ((xAxisWidth - 10) * 1.00) / (maxTick - minTick);

						barWidth = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
						if(((tmpSeries->GetData(s) - minTick) > 0) && (barWidth < 1))
							barWidth = (int)dataScale;

						barL = xApexPoint;
						barT = tickYLocation - (dataPlotSize / 2) + (s * barHeight);
						barB = barT + barHeight;
						if(barWidth > xAxisWidth)
							barR = xApexPoint + xAxisWidth;
						else
							barR = xApexPoint + barWidth;

						COLORREF barColor;
						barColor = GetColor(s);
						CBrush brush (barColor);
						CBrush* pOldBrush;
						pOldBrush = pDC->SelectObject(&brush);
						pDC->Rectangle(barL, barT, barR, barB);
						pDC->SelectObject(pOldBrush);
					}
				}
				break;
			case 2 :
				for(y = 1; y <= graphSeries->GetCount(); y++)
				{
					tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

					tickYLocation = yApexPoint - ((y * seriesSpace) - (seriesSpace / 2));

					for(s = 0; s < seriesSize; s++)
					{
						double dataScale = 0.00;
						dataScale = ((xAxisWidth - 10) * 1.00) / (maxTick - minTick);

						barWidth = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
						if(((tmpSeries->GetData(s) - minTick) > 0) && (barWidth < 1))
							barWidth = (int)dataScale;

						barL = xApexPoint;
						barT = tickYLocation - (dataPlotSize / 2) + (s * barHeight);
						barB = barT + barHeight;
						if(barWidth < 0)
						{
							if((0 - barWidth) > (xAxisWidth / 2))
								barR = xApexPoint + (xAxisWidth / 2);
							else
								barR = xApexPoint + barWidth;
						}
						else
						{
							if(barWidth > (xAxisWidth / 2))
								barR = xApexPoint + (xAxisWidth / 2);
							else
								barR = xApexPoint + barWidth;
						}

						COLORREF barColor;
						barColor = GetColor(s);
						CBrush brush (barColor);
						CBrush* pOldBrush;
						pOldBrush = pDC->SelectObject(&brush);
						pDC->Rectangle(barL, barT, barR, barB);
						pDC->SelectObject(pOldBrush);
					}
				}
				break;
			case 3 :
				for(y = 1; y <= graphSeries->GetCount(); y++)
				{
					tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

					tickYLocation = yApexPoint - (yAxisHeight / 2) + ((y * seriesSpace) - (seriesSpace / 2));

					for(s = 0; s < seriesSize; s++)
					{
						double dataScale = 0.00;
						dataScale = ((xAxisWidth - 10) * 1.00) / (maxTick - minTick);

						barWidth = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
						if(((tmpSeries->GetData(s) - minTick) > 0) && (barWidth < 1))
							barWidth = (int)dataScale;

						barL = xApexPoint;
						barT = tickYLocation - (dataPlotSize / 2) + (s * barHeight);
						barB = barT + barHeight;
						if(barWidth > xAxisWidth)
							barR = xApexPoint + xAxisWidth;
						else
							barR = xApexPoint + barWidth;

						COLORREF barColor;
						barColor = GetColor(s);
						CBrush brush (barColor);
						CBrush* pOldBrush;
						pOldBrush = pDC->SelectObject(&brush);
						pDC->Rectangle(barL, barT, barR, barB);
						pDC->SelectObject(pOldBrush);
					}
				}
				break;
			case 4 :
				for(y = 1; y <= graphSeries->GetCount() / 2; y++)
				{
					tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

					tickYLocation = yApexPoint - (yAxisHeight / 2) + ((y * seriesSpace) - (seriesSpace / 2));

					for(s = 0; s < seriesSize; s++)
					{
						double dataScale = 0.00;
						dataScale = ((xAxisWidth - 10) * 1.00) / (maxTick - minTick);

						barWidth = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
						if(((tmpSeries->GetData(s) - minTick) > 0) && (barWidth < 1))
							barWidth = (int)dataScale;

						barL = xApexPoint;
						barT = tickYLocation - (dataPlotSize / 2) + (s * barHeight);
						barB = barT + barHeight;
						if(barWidth < 0)
						{
							if((0 - barWidth) > (xAxisWidth / 2))
								barR = xApexPoint + (xAxisWidth / 2);
							else
								barR = xApexPoint + barWidth;
						}
						else
						{
							if(barWidth > (xAxisWidth / 2))
								barR = xApexPoint + (xAxisWidth / 2);
							else
								barR = xApexPoint + barWidth;
						}

						COLORREF barColor;
						barColor = GetColor(s);
						CBrush brush (barColor);
						CBrush* pOldBrush;
						pOldBrush = pDC->SelectObject(&brush);
						pDC->Rectangle(barL, barT, barR, barB);
						pDC->SelectObject(pOldBrush);
					}
				}
				//draw the 0 location
				tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);
				tickYLocation = yApexPoint;
				for(s = 0; s < seriesSize; s++)
				{
					double dataScale = 0.00;
					dataScale = ((xAxisWidth - 10) * 1.00) / (maxTick - minTick);

					barWidth = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
					if(((tmpSeries->GetData(s) - minTick) > 0) && (barWidth < 1))
						barWidth = (int)dataScale;

					barL = xApexPoint;
					barT = tickYLocation - (dataPlotSize / 2) + (s * barHeight);
					barB = barT + barHeight;
					if(barWidth < 0)
					{
						if((0 - barWidth) > (xAxisWidth / 2))
							barR = xApexPoint + (xAxisWidth / 2);
						else
							barR = xApexPoint + barWidth;
					}
					else
					{
						if(barWidth > (xAxisWidth / 2))
							barR = xApexPoint + (xAxisWidth / 2);
						else
							barR = xApexPoint + barWidth;
					}

					COLORREF barColor;
					barColor = GetColor(s);
					CBrush brush (barColor);
					CBrush* pOldBrush;
					pOldBrush = pDC->SelectObject(&brush);
					pDC->Rectangle(barL, barT, barR, barB);
					pDC->SelectObject(pOldBrush);
				}
				y++;
	
				for(; y <= graphSeries->GetCount(); y++)
				{
					tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

					tickYLocation = yApexPoint - (yAxisHeight / 2) + ((y * seriesSpace) - (seriesSpace / 2));

					for(s = 0; s < seriesSize; s++)
					{
						double dataScale = 0.00;
						dataScale = ((xAxisWidth - 10) * 1.00) / (maxTick - minTick);

						barWidth = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
						if(((tmpSeries->GetData(s) - minTick) > 0) && (barWidth < 1))
							barWidth = (int)dataScale;

						barL = xApexPoint;
						barT = tickYLocation - (dataPlotSize / 2) + (s * barHeight);
						barB = barT + barHeight;
						if(barWidth < 0)
						{
							if((0 - barWidth) > (xAxisWidth / 2))
								barR = xApexPoint + (xAxisWidth / 2);
							else
								barR = xApexPoint + barWidth;
						}
						else
						{
							if(barWidth > (xAxisWidth / 2))
								barR = xApexPoint + (xAxisWidth / 2);
							else
								barR = xApexPoint + barWidth;
						}

						COLORREF barColor;
						barColor = GetColor(s);
						CBrush brush (barColor);
						CBrush* pOldBrush;
						pOldBrush = pDC->SelectObject(&brush);
						pDC->Rectangle(barL, barT, barR, barB);
						pDC->SelectObject(pOldBrush);
					}
				}
				break;
		}

	}
}

void CGraph::DrawLineSeries(CDC* pDC)
{
	int barWidth;
	int dataPlotSize;   //used to plot rects of data
	int tickXLocation, tickYLocation;
	int seriesSpace;
	int barHeight;
	POSITION pos;
	CGraphSeries* tmpSeries;

	int lastXLoc, lastYLoc;
	if(graphAlignment)
	{
		for(int s = 0; s < seriesSize; s++)
		{
			//determine width of barchart data blocks
			seriesSpace = xAxisWidth / graphSeries->GetCount();
			barWidth = (int)((seriesSpace * .6) / seriesSize);
			dataPlotSize = seriesSize * barWidth;

			pos = graphSeries->GetHeadPosition();

			for(int x = 1; x <= graphSeries->GetCount(); x++)
			{
				tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

				if(tmpSeries->GetData(s) > -1)
				{
					tickXLocation = xApexPoint + ((x * seriesSpace) - (seriesSpace / 2));
					
					barHeight = 0;
					double dataScale = 0.00;
					
					dataScale = ((yAxisHeight - 10) * 1.00) / (maxTick - minTick);
					barHeight = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
					if(((tmpSeries->GetData(s) - minTick) > 0) && (barHeight < 1))
						barHeight = (int)dataScale;

					if(barHeight > yApexPoint)
						barHeight = (int)((maxTick - minTick) * dataScale) + 5;

					int yDataLocation = yApexPoint - barHeight;
					//now have x and y location of center of ellipse
					COLORREF barColor;
					barColor = GetColor(s);
					CBrush brush (barColor);
					CBrush* pOldBrush;
					pOldBrush = pDC->SelectObject(&brush);
					//draw line back to last data member
					if(x > 1)
					{
						CPen* pOldPen;
						CPen linePen (PS_SOLID, 1, barColor);
						pOldPen = pDC->SelectObject(&linePen);
						pDC->MoveTo(lastXLoc + 2, lastYLoc - 1);
						pDC->LineTo(tickXLocation - 3, yDataLocation - 1);
						pDC->SelectObject(pOldPen);
					}
					//now draw ellipse...
					pDC->Ellipse(tickXLocation - 3, yDataLocation - 3,
						tickXLocation + 3, yDataLocation + 3);
					lastXLoc = tickXLocation;
					lastYLoc = yDataLocation;
					pDC->SelectObject(pOldBrush);
				}
			}
		}
	}
	else
	{
		for(int s = 0; s < seriesSize; s++)
		{
			//determine width of barchart data blocks
			seriesSpace = yAxisHeight / graphSeries->GetCount();
			barHeight = (int)((seriesSpace * .6) / seriesSize);
			dataPlotSize = seriesSize * barHeight;

			pos = graphSeries->GetHeadPosition();

			for(int x = 1; x <= graphSeries->GetCount(); x++)
			{
				tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

				if(tmpSeries->GetData(s) > -1)
				{
					tickYLocation = yApexPoint - ((x * seriesSpace) - (seriesSpace / 2));
					
					barWidth = 0;
					double dataScale = 0.00;

					if((maxTick - minTick) != xAxisWidth)
						dataScale = (xAxisWidth * 1.00) / ((maxTick - minTick) * 1.00);
					else dataScale = 1;
					barWidth = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
					if(((tmpSeries->GetData(s) - minTick) > 0) && (barWidth < 1))
						barWidth = (int)dataScale;

					int xDataLocation = xApexPoint + barWidth;
					
					//now have x and y location of center of ellipse
					COLORREF barColor;
					barColor = GetColor(s);
					CBrush brush (barColor);
					CBrush* pOldBrush;
					pOldBrush = pDC->SelectObject(&brush);
					//draw line back to last data member
					if(x > 1)
					{
						CPen* pOldPen;
						CPen linePen (PS_SOLID, 1, barColor);
						pOldPen = pDC->SelectObject(&linePen);
						pDC->MoveTo(lastXLoc + 2, lastYLoc - 1);
						pDC->LineTo(xDataLocation - 3, tickYLocation - 1);
						pDC->SelectObject(pOldPen);
					}
					//now draw ellipse...
					pDC->Ellipse(xDataLocation - 3, tickYLocation - 3,
						xDataLocation + 3, tickYLocation + 3);
					lastYLoc = tickYLocation;
					lastXLoc = xDataLocation;
					pDC->SelectObject(pOldBrush);
				}
			}
		}
	}
}

void CGraph::DrawPieSeries(CDC* pDC)
{
	double dataSum = 0.00;  //for storing cumulative sum
	double labelData = 0.00;
	int lastXLocation, lastYLocation;
	int newXLocation, newYLocation;
	int labelXLocation, labelYLocation;
	double percent = 0.00;
	double labelPercent = 0.00;
	int degrees;
	int labelDegrees;
	double totalSum = 0.00;
	int deltaXY;
	int labelDeltaXY;
	int radius;
	POSITION pos;
	CGraphSeries* tmpSeries;
	int seriesSpace;
	int labelLineXStart, labelLineYStart;
	int labelLineXEnd, labelLineYEnd;
	int maxLabelWidth;
	int maxLabelHeight;
	TEXTMETRIC tm;
		
	lastXLocation = 0;
	lastYLocation = 0;

	pDC->GetTextMetrics(&tm);
	maxLabelWidth = tm.tmMaxCharWidth + 10;
	maxLabelHeight = tm.tmHeight + 6;
	
	//pie labels will be stored in a list and drawn after entire pie
	//is completed.
	CObList *pieLabels;
	CGraphPieLabel *pieLabel;

	//determine width of pie display area
	if(xAxisWidth > yAxisHeight)
		seriesSpace = yAxisHeight / graphSeries->GetCount();
	else
		seriesSpace = xAxisWidth / graphSeries->GetCount();
	seriesSpace -= (3 * maxLabelWidth);  //allows text like 25%  (3 chars)

	//to plot a pie plus labels inside of series space, use the following :
	//(3 * radius) + (2 * label width) = series space 
	//so, 3*radius = series space - (2 * label width)
	// 1 radius = (series space - (2 * label width)) / 3
	radius = seriesSpace / 3;  //pie needs 2 radius, + 1 extra for line to labels = 3 for my divisor

	int centerYPie = (yAxisHeight + 60) / 2;

	pos = graphSeries->GetHeadPosition();
	for(int x = 1; x <= graphSeries->GetCount(); x++)
	{
		pieLabels = new CObList();

		tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);
		totalSum = 0;
		int s = 0;
		for(s = 0; s < seriesSize; s++)
			totalSum += tmpSeries->GetData(s);
		int pieLeft, pieRight;
		if(graphSeries->GetCount() == 1)
		{
			pieLeft = xApexPoint + (xAxisWidth / 2) - radius;
		}
		else
		{
			pieLeft = xApexPoint + 15 + ((x * 2 - 1) * ((xAxisWidth / graphSeries->GetCount()) / 2)) - radius;
		}
		pieRight = pieLeft + (2 * radius);
		CRect pieRect (pieLeft, 
				centerYPie - radius,
				pieRight, 
				centerYPie + radius);
		int centerXPie = pieLeft + radius;

		//plot series label
		pDC->TextOut(centerXPie - ((tmpSeries->GetLabel().GetLength() * 8) / 2),
					centerYPie + (int)(radius * 1.5) + maxLabelHeight + 15, tmpSeries->GetLabel());
		lastXLocation = pieLeft;
		lastYLocation = centerYPie;

		dataSum = 0;
		for(s = 0; s < seriesSize; s++)
		{
			if(tmpSeries->GetData(s) > 0)
			{
				int seriesDataValue;
				seriesDataValue = tmpSeries->GetData(s);
				labelData = seriesDataValue / 2;
				dataSum += seriesDataValue;
				percent = (dataSum / totalSum) * 100;
				labelPercent = ((dataSum - labelData) / totalSum) * 100;
				degrees = (int)((360 * percent) / 100);
				labelDegrees = (int)((360 * labelPercent) / 100);

				deltaXY = (degrees * radius) / 90;
				labelDeltaXY = (labelDegrees * radius) / 90;
					//deltaXY is change from start point of pie 0
					//this value is total change.  so if radius is 300
					//and degrees is 270, delta is 300.  The change 
					//would move both x and y 300 pixels.  For x, 100
					//to right is center, another 100 to right edge,
					//100 back to center.  For y, 100 to bottom, 100
					//back to center, 100 to top. thus gives 270 degree
					//rotation.

				//determine destination quadrant...
					//and set newXLocation and newYLocation values...
					//degress / 90 will never exceed 4.
					//this can tell us the quadrant of destination
				int quadrant = degrees / 90;  //truncates decimal
				int labelQuadrant = labelDegrees / 90;

				switch(quadrant)
				{
					//in the computations below, the remarked line is
					//the original computation.  The line above it, for
					//actual use, is the simplified line, and gives the
					//exact same result
					case 0 : newXLocation = pieLeft + deltaXY;
							 newYLocation = centerYPie + deltaXY;
							 break;
					case 1 : newXLocation = pieLeft + deltaXY;
							 newYLocation = centerYPie + (2 * radius) - deltaXY;
							 break;
					case 2 : newXLocation = pieLeft + (4 * radius) - deltaXY;
							 newYLocation = centerYPie + (2 * radius) - deltaXY;
							 break;
					case 3 : newXLocation = pieLeft + (4 * radius) - deltaXY;
							 newYLocation = centerYPie - (4 * radius) + deltaXY;
							 break;
					case 4 : newXLocation = pieLeft;
							 newYLocation = centerYPie - 1;
							 break;
				}
				
				switch(labelQuadrant)
				{
					//after getting X & Y location for label, we take
					//1/2 the delta between x y locations and center pie
					case 0 : labelXLocation = pieLeft + labelDeltaXY;
							 labelYLocation = centerYPie + labelDeltaXY;
							 labelLineXStart = labelXLocation + ((centerXPie - labelXLocation) / 2);
							 labelLineYStart = labelYLocation - ((labelYLocation - centerYPie) / 2);
							 labelLineXEnd = labelXLocation - (radius / 2);
							 labelLineYEnd = labelYLocation + (radius / 2);
							 break;
					case 1 : labelXLocation = pieLeft + labelDeltaXY;
							 labelYLocation = centerYPie + (2 * radius) - labelDeltaXY;
							 labelLineXStart = labelXLocation - ((labelXLocation - centerXPie) / 2);
							 labelLineYStart = labelYLocation - ((labelYLocation - centerYPie) / 2);
							 labelLineXEnd = labelXLocation + (radius / 2);
							 labelLineYEnd = labelYLocation + (radius / 2);
							 break;
					case 2 : labelXLocation = pieLeft + (4 * radius) - labelDeltaXY;
							 labelYLocation = centerYPie + (2 * radius) - labelDeltaXY;
							 labelLineXStart = labelXLocation - ((labelXLocation - centerXPie) / 2);
							 labelLineYStart = labelYLocation + ((centerYPie - labelYLocation) / 2);
							 labelLineXEnd = labelXLocation + (radius / 2);
							 labelLineYEnd = labelYLocation - (radius / 2);
							 break;
					case 3 : labelXLocation = pieLeft + (4 * radius) - labelDeltaXY;
							 labelYLocation = centerYPie - (4 * radius) + labelDeltaXY;
							 labelLineXStart = labelXLocation + ((centerXPie - labelXLocation) / 2);
							 labelLineYStart = labelYLocation + ((centerYPie - labelYLocation) / 2);
							 labelLineXEnd = labelXLocation - (radius / 2);
							 labelLineYEnd = labelYLocation - (radius / 2);
							 break;
//there should never be a half point ending on 4, so leave it out
				}

				pieLabel = new CGraphPieLabel();

				pieLabel->lineXStart = labelLineXStart;
				pieLabel->lineYStart = labelLineYStart;
				pieLabel->lineXEnd = labelLineXEnd;
				pieLabel->lineYEnd = labelLineYEnd;
				switch(labelQuadrant)
				{
					case 0 : //label to left of line
							 pieLabel->topLeftX = labelLineXEnd - maxLabelWidth;
							 pieLabel->topLeftY = labelLineYEnd;
							 break;
					case 1 : //label to right of line
							 pieLabel->topLeftX = labelLineXEnd + 5;
							 pieLabel->topLeftY = labelLineYEnd;
							 break;
					case 2 : //label to right of line
							 pieLabel->topLeftX = labelLineXEnd + 5;
							 pieLabel->topLeftY = labelLineYEnd - maxLabelHeight;
							 break;
					case 3 : //label to left of line
							 pieLabel->topLeftX = labelLineXEnd - maxLabelWidth;
							 pieLabel->topLeftY = labelLineYEnd - maxLabelHeight;
							 break;
				}
				pieLabel->labelQuadrant = labelQuadrant;
				int roundPercent;
				roundPercent = (int)((seriesDataValue * 100) / totalSum);
				pieLabel->pieLabel.Format(_T("%d%%"), roundPercent);
				pieLabels->AddTail(pieLabel);

				if(s == 0)
					lastYLocation -= 1;

				CPoint p1 (lastXLocation, lastYLocation);
				CPoint p2 (newXLocation, newYLocation);
				COLORREF barColor;
				barColor = GetColor(s);
				CBrush brush (barColor);
				CBrush* pOldBrush;
				CPen piePen(PS_SOLID, 1, barColor);
				CPen* pOldPen;
				pOldBrush = pDC->SelectObject(&brush);
				pOldPen = pDC->SelectObject(&piePen);
				pDC->Pie(pieRect, p1, p2); 
				pDC->SelectObject(pOldBrush);
				pDC->SelectObject(pOldPen);

				lastXLocation = newXLocation;
				lastYLocation = newYLocation;
			}
		}
		//draw lines and labels for pies slices
		POSITION labelPos;

		CBrush lineBrush (BLACK);
		CBrush* pOldBrush;
		pOldBrush = pDC->SelectObject(&lineBrush);
		POSITION mainLinePos, checkLinePos;
		mainLinePos = pieLabels->GetHeadPosition();
		int numLinesDrawn = 0;
		CGraphPieLabel* currentLabel;
		CGraphPieLabel* tmpLabel;
		while(mainLinePos != NULL)
		{
			currentLabel = (CGraphPieLabel*)pieLabels->GetNext(mainLinePos);
			int r = 0;
			checkLinePos = pieLabels->GetHeadPosition();
			while(r < numLinesDrawn)
			{
				//check if any overlap in label areas
				tmpLabel = (CGraphPieLabel*)pieLabels->GetAt(checkLinePos);
				if((currentLabel->topLeftX > tmpLabel->topLeftX) &&
					(currentLabel->topLeftX < (tmpLabel->topLeftX + maxLabelWidth)) &&
					(currentLabel->topLeftY > tmpLabel->topLeftY) &&
					(currentLabel->topLeftY < (tmpLabel->topLeftY + maxLabelHeight)))
				{
					//OVERLAP !!!
					//move current label top left position up or down
					//depending on its quadrant
					if(currentLabel->labelQuadrant < 2)
					{
						//move label down to tmplabel topleft + height
						currentLabel->topLeftY = tmpLabel->topLeftY + maxLabelHeight;
						currentLabel->lineYEnd = tmpLabel->lineYEnd + maxLabelHeight;
					}
					else
					{
						//move label up to tmpLabel topleft - height
						currentLabel->topLeftY = tmpLabel->topLeftY - maxLabelHeight;
						currentLabel->lineYEnd = tmpLabel->lineYEnd - maxLabelHeight;
					}
					//reset r value to 0 so it starts over, just in
					//case we moved the label and it overlaps another
					r = 0;
					checkLinePos = pieLabels->GetHeadPosition();
				}
				else
				{
					r++;
					pieLabels->GetNext(checkLinePos);
				}

			}
			//draw the line and label
			pDC->MoveTo(currentLabel->lineXStart, currentLabel->lineYStart);
			pDC->LineTo(currentLabel->lineXEnd, currentLabel->lineYEnd);

			//write the label
			pDC->TextOut(currentLabel->topLeftX, currentLabel->topLeftY,
					currentLabel->pieLabel);
			numLinesDrawn++;
		}

		pDC->SelectObject(pOldBrush);

		//now done, remove everything inside the label list
		labelPos = pieLabels->GetHeadPosition();
		while(labelPos != NULL)
		{
			pieLabel = (CGraphPieLabel*)pieLabels->GetNext(labelPos);
			delete pieLabel;
		}
		delete pieLabels;
	}
}

void CGraph::DrawScatterSeries(CDC* pDC)
{
	//rightXTick and topYTick contain the outer bounds of the axis ticks.
	//So, if maxTick is 100 on both axis lines, then 100, 100 would be top
	//right.  We will use the bounds to see spacing from apex points and then
	//scale so any point can be plotted inside

	double yAxisScale, xAxisScale;
	int tickXLocation, tickYLocation;

	//multiply each value by 1.00 to force result into a double value, and therefore
	//make it more accurate in it's plot location.
	switch(graphQuadType)
	{
		case 1 :
			yAxisScale = ((yApexPoint - topYTick) / (maxTick * 1.00));
			xAxisScale = ((rightXTick - xApexPoint) / (maxTick * 1.00));
			break;
		case 2 :
			yAxisScale = ((yApexPoint - topYTick) / (maxTick * 1.00));
			xAxisScale = ((rightXTick - xApexPoint) / (maxTick * 1.00));
			break;
		case 3 :
			yAxisScale = ((topYTick - yApexPoint) / (maxTick * 1.00));
			xAxisScale = ((rightXTick - xApexPoint) / (maxTick * 1.00));
			break;
		case 4 :
			yAxisScale = ((topYTick - yApexPoint) / (maxTick * 1.00));
			xAxisScale = ((rightXTick - xApexPoint) / (maxTick * 1.00));
			break;
	}

	//points will now plot as ((value * scale) + apex point)
	COLORREF barColor;
	barColor = BLACK;
	CBrush brush (barColor);
	CBrush* pOldBrush;
	pOldBrush = pDC->SelectObject(&brush);

	POSITION pos;
	CGraphSeries* tmpSeries;
	for(pos = graphSeries->GetHeadPosition(); pos != NULL; graphSeries->GetNext(pos))
	{
		tmpSeries = (CGraphSeries*)graphSeries->GetAt(pos);

		//multiply each value by 1.00 to force result into a double value, and therefore
		//make it more accurate in it's plot location.
		tickXLocation = (int)((tmpSeries->GetXDataValue() * 1.00) * xAxisScale) + xApexPoint;
		tickYLocation = yApexPoint - (int)((tmpSeries->GetYDataValue() * 1.00) * yAxisScale);

		//draw ellipse...
		pDC->Ellipse(tickXLocation - 3, tickYLocation - 3,
			tickXLocation + 3, tickYLocation + 3);

	}
	pDC->SelectObject(pOldBrush);


}

void CGraph::DrawBoxWhiskerSeries(CDC* pDC)
{
	int upperQuartile;
	int lowerQuartile;
	int median;
	int highValue;
	int lowValue;
	
	COLORREF barColor;
	barColor = BLACK;
	CBrush brush (barColor);
	CBrush* pOldBrush;
	pOldBrush = pDC->SelectObject(&brush);

	int barWidth;
	int barL, barT, barR, barB;
	int tickXLocation, tickYLocation;
	int seriesSpace;
	int barHeight;
	POSITION pos;
	CGraphSeries* tmpSeries;

	pos = graphSeries->GetHeadPosition();
	CUIntArray dataArray;
	CUIntArray sortedArray;

	for(int x = 1; x <= graphSeries->GetCount(); x++)
	{
		dataArray.RemoveAll();
		sortedArray.RemoveAll();

		CObList *sortedData;
		sortedData = new CObList();
		tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);
		seriesSize = tmpSeries->GetDataCount();

		int dataValue;
		for(int s = 0; s < seriesSize; s++)
		{
			dataValue = 0;
			if(tmpSeries->GetData(s) > -1)
				dataValue = tmpSeries->GetData(s);
			dataArray.Add(dataValue);
		}

		//sort the array
		dataValue = dataArray.GetAt(0);
		sortedArray.Add(dataValue);
		for(int d = 1; d < dataArray.GetSize(); d++)
		{
			dataValue = dataArray.GetAt(d);
			BOOL placed = FALSE;
			for(int s = 0; s < sortedArray.GetSize(); s++)
			{
				int tmpI = sortedArray.GetAt(s);
				if(dataValue <= tmpI)
				{
					sortedArray.InsertAt(s, dataValue);
					placed = TRUE;
					break;
				}
			}
			if(!placed)	//add at end
				sortedArray.Add(dataValue);
		}
		
		//sortedArray now contains the sorted list of all data in this
		//series.  From here, we derive the other box-whisker data
		
		int medialElement;
		div_t div_result;
		div_result = div(sortedArray.GetSize(), 2);

		if(div_result.rem == 0)	//even number of elements
		{
			//size is not 0 base, so below, I subtract 1 to 0 base it.
			medialElement = (sortedArray.GetSize() / 2) - 1;
			median = sortedArray.GetAt(medialElement) + 
						((sortedArray.GetAt(medialElement + 1) - 
						sortedArray.GetAt(medialElement)) / 2);
			highValue = sortedArray.GetAt(sortedArray.GetSize() - 1);
			lowValue = sortedArray.GetAt(0);
			div_t div2Result;
			div2Result = div(medialElement + 1, 2);
			if(div2Result.rem == 0)	//even again
			{
				upperQuartile = sortedArray.GetAt(medialElement + 1 + (medialElement / 2)) +
									(sortedArray.GetAt(medialElement + (medialElement / 2) + 2) -
									sortedArray.GetAt(medialElement + (medialElement / 2))) / 2;
				lowerQuartile = sortedArray.GetAt(medialElement / 2) +
									((sortedArray.GetAt((medialElement / 2) + 1) -
									sortedArray.GetAt(medialElement / 2)) / 2);
			}
			else	//odd
			{
				upperQuartile = sortedArray.GetAt(medialElement + 1 + (medialElement / 2));
				lowerQuartile = sortedArray.GetAt(medialElement / 2);
			}
		}
		else	//odd number of elements
		{
			//size is not 0 base, so below, I subtract 1 to 0 base it.
			medialElement = sortedArray.GetSize() / 2;
			median = sortedArray.GetAt(medialElement);
			highValue = sortedArray.GetAt(sortedArray.GetSize() - 1);
			lowValue = sortedArray.GetAt(0);
			div_t div2Result;
			div2Result = div(medialElement, 2);
			if(div2Result.rem == 0)	//even 
			{
				upperQuartile = sortedArray.GetAt(medialElement + (medialElement / 2)) +
									(sortedArray.GetAt(medialElement + (medialElement / 2) + 1) -
									sortedArray.GetAt(medialElement + (medialElement / 2))) / 2;
				lowerQuartile = sortedArray.GetAt((medialElement - 1) / 2) +
									((sortedArray.GetAt((medialElement + 1) / 2) -
									sortedArray.GetAt((medialElement - 1) / 2)) / 2);
			}
			else	//odd
			{
				upperQuartile = sortedArray.GetAt(medialElement + 1 + (medialElement / 2));
				lowerQuartile = sortedArray.GetAt(medialElement / 2);
			}
		}

		//data has been computed for median, high, low, and interquartile range
		//now we can draw the series

		if(graphAlignment)
		{
			seriesSpace = xAxisWidth / graphSeries->GetCount();

			barWidth = (int)(seriesSpace * .8);

			double tickScale = 0.00;
			tickScale = (yAxisHeight - 10) / numTicks;

			tickXLocation = xApexPoint + ((x * seriesSpace) - (seriesSpace / 2));

			barL = tickXLocation - (barWidth / 2);
			barR = barL + barWidth;
			double dataScale = 0.00;
			dataScale = ((yAxisHeight - 10) * 1.00) / (maxTick - minTick);

			//top cross bar (max value)
			barHeight = (int)((highValue * 1.00) * tickScale);
			barT = yApexPoint - barHeight;
			barB = barT + 3;
			pDC->Rectangle(barL, barT, barR, barB);

			//bottom cross bar (min value)
			barHeight = (int)((lowValue * 1.00) * tickScale);
			barT = yApexPoint - barHeight;
			barB = barT - 3;
			pDC->Rectangle(barL, barT, barR, barB);

			//vertical line (whisker)
			barHeight = (int)((highValue * 1.00) * tickScale);
			barT = yApexPoint - barHeight;
			//barB = ?;	//this will be left over from bottom cross bar
			pDC->Rectangle(tickXLocation - 2, barT, tickXLocation + 2, barB);

			//box (interquartile range, from upper quartile to lower quartile)
			barHeight = (int)(upperQuartile * tickScale);
			barT = yApexPoint - barHeight;
			barHeight = (int)(lowerQuartile * tickScale);
			barB = yApexPoint - barHeight;
			pDC->Rectangle(barL, barT, barR, barB);

			//draw median line (in RED)
			CPen* pOldPen;
			CPen linePen (PS_SOLID, 1, RED);
			pOldPen = pDC->SelectObject(&linePen);
			barHeight = (int)((median * 1.00) * tickScale);
			pDC->MoveTo(barL, yApexPoint - barHeight);
			pDC->LineTo(barR, yApexPoint - barHeight);
			pDC->SelectObject(pOldPen);
		}
		else
		{
			seriesSpace = yAxisHeight / graphSeries->GetCount();

			barHeight = (int)(seriesSpace * .8);

			double tickScale = 0.00;
			tickScale = (xAxisWidth - 10) / numTicks;

			tickYLocation = yApexPoint - ((x * seriesSpace) - (seriesSpace / 2));

			barT = tickYLocation - (barHeight / 2);
			barB = barT + barHeight;
			double dataScale = 0.00;
			dataScale = ((xAxisWidth - 10) * 1.00) / (maxTick - minTick);

			//top cross bar (max value)
			barWidth = (int)((highValue * 1.00) * tickScale);
			barR = xApexPoint + barWidth;
			barL = barR - 3;
			pDC->Rectangle(barL, barT, barR, barB);

			//bottom cross bar (min value)
			barWidth = (int)((lowValue * 1.00) * tickScale);
			barR = xApexPoint + barWidth;
			barL = barR + 3;
			pDC->Rectangle(barL, barT, barR, barB);

			//vertical line (whisker)
			barWidth = (int)((highValue * 1.00) * tickScale);
			barR = xApexPoint + barWidth;
			pDC->Rectangle(barL, tickYLocation - 2, barR, tickYLocation + 2);

			//box (interquartile range, from upper quartile to lower quartile)
			barWidth = (int)(upperQuartile * tickScale);
			barL = xApexPoint + barWidth;
			barWidth = (int)(lowerQuartile * tickScale);
			barR = xApexPoint + barWidth;
			pDC->Rectangle(barL, barT, barR, barB);

			//draw median line (in RED)
			CPen* pOldPen;
			CPen linePen (PS_SOLID, 1, RED);
			pOldPen = pDC->SelectObject(&linePen);
			barWidth = (int)((median * 1.00) * tickScale);
			pDC->MoveTo(xApexPoint + barWidth, barT);
			pDC->LineTo(xApexPoint + barWidth, barB);
			pDC->SelectObject(pOldPen);
		}
	}

	pDC->SelectObject(pOldBrush);
}

void CGraph::DrawStackedBarSeries(CDC* pDC)
{
	int barWidth;
	int barL, barT, barR, barB;
	int tickXLocation, tickYLocation;
	int seriesSpace;
	int barHeight;
	POSITION pos;
	CGraphSeries* tmpSeries;

	if(graphAlignment)
	{
		//determine width of barchart data blocks
		seriesSpace = xAxisWidth / graphSeries->GetCount();
		barWidth = (int)(seriesSpace * .6);

		pos = graphSeries->GetHeadPosition();

		double dataScale = 0.00;
		dataScale = ((yAxisHeight - 10) * 1.00) / (maxTick - minTick);
		for(int x = 1; x <= graphSeries->GetCount(); x++)
		{
			tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

			tickXLocation = xApexPoint + ((x * seriesSpace) - (seriesSpace / 2));
			
			barT = yApexPoint;	//this is a running total;
								//it starts at yApexPoint so barB can
								//start there, then moves up by barHeight
			for(int s = 0; s < seriesSize; s++)
			{
				if(tmpSeries->GetData(s) > -1)
				{
					barHeight = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
					if(((tmpSeries->GetData(s) - minTick) > 0) && (barHeight < 1))
						barHeight = (int)dataScale;

					barB = barT;
					barL = tickXLocation - (barWidth / 2);
					if(barHeight > yAxisHeight)
						barT = barB - yAxisHeight;
					else
						barT = barB - barHeight;
					barR = barL + barWidth;
					
					COLORREF barColor;
					barColor = GetColor(s);
					CBrush brush (barColor);
					CBrush* pOldBrush;
					pOldBrush = pDC->SelectObject(&brush);
					pDC->Rectangle(barL, barT, barR, barB);
					pDC->SelectObject(pOldBrush);
				}
			}
		}
	}
	else
	{
		//determine width of barchart data blocks
		seriesSpace = yAxisHeight / graphSeries->GetCount();
		barHeight = (int)(seriesSpace * .6);

		pos = graphSeries->GetHeadPosition();

		double dataScale = 0.00;
		dataScale = ((xAxisWidth - 10) * 1.00) / (maxTick - minTick);
		for(int y = 1; y <= graphSeries->GetCount(); y++)
		{
			tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

			tickYLocation = yApexPoint - ((y * seriesSpace) - (seriesSpace / 2));

			barR = xApexPoint;	//this is a running total;
								//it starts at xApexPoint so barL can
								//start there, then moves right by barWidth
			for(int s = 0; s < seriesSize; s++)
			{
				if(tmpSeries->GetData(s) > -1)
				{

					barWidth = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
					if(((tmpSeries->GetData(s) - minTick) > 0) && (barWidth < 1))
						barWidth = (int)dataScale;

					barL = barR;
					barT = tickYLocation - (barHeight / 2);
					barB = barT + barHeight;
					if(barWidth > xAxisWidth)
						barR = barL + xAxisWidth;
					else
						barR = barL + barWidth;

					COLORREF barColor;
					barColor = GetColor(s);
					CBrush brush (barColor);
					CBrush* pOldBrush;
					pOldBrush = pDC->SelectObject(&brush);
					pDC->Rectangle(barL, barT, barR, barB);
					pDC->SelectObject(pOldBrush);
				}
			}
		}
	}
}

void CGraph::DrawXYLineSeries(CDC* pDC)
{
	//XY Line graph is same as Line graph, but has no circles at endpoints
	int barWidth;
	int dataPlotSize;   //used to plot rects of data
	int tickXLocation, tickYLocation;
	int seriesSpace;
	int barHeight;
	POSITION pos;
	CGraphSeries* tmpSeries;

	int lastXLoc, lastYLoc;
	if(graphAlignment)
	{
		for(int s = 0; s < seriesSize; s++)
		{
			//determine width of barchart data blocks
			seriesSpace = xAxisWidth / graphSeries->GetCount();
			barWidth = (int)((seriesSpace * .6) / seriesSize);
			dataPlotSize = seriesSize * barWidth;

			pos = graphSeries->GetHeadPosition();

			for(int x = 1; x <= graphSeries->GetCount(); x++)
			{
				tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

				if(tmpSeries->GetData(s) > -1)
				{
					tickXLocation = xApexPoint + ((x * seriesSpace) - (seriesSpace / 2));
					
					barHeight = 0;
					double dataScale = 0.00;
					
					if((maxTick - minTick) != yAxisHeight)
						dataScale = (yAxisHeight * 1.00) / ((maxTick - minTick) * 1.00);
					else dataScale = 1;
					barHeight = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
					if(((tmpSeries->GetData(s) - minTick) > 0) && (barHeight < 1))
						barHeight = (int)dataScale;

					if(barHeight > yApexPoint)
						barHeight = (int)((maxTick - minTick) * dataScale) + 5;

					int yDataLocation = yApexPoint - barHeight;
					//now have x and y location of center of ellipse
					COLORREF barColor;
					barColor = GetColor(s);
					//draw line back to last data member
					if(x > 1)
					{
						CPen* pOldPen;
						CPen linePen (PS_SOLID, 2, barColor);
						pOldPen = pDC->SelectObject(&linePen);
						pDC->MoveTo(lastXLoc, lastYLoc);
						pDC->LineTo(tickXLocation, yDataLocation);
						pDC->SelectObject(pOldPen);
					}
					lastXLoc = tickXLocation;
					lastYLoc = yDataLocation;
				}
			}
		}
	}
	else
	{
		for(int s = 0; s < seriesSize; s++)
		{
			//determine width of barchart data blocks
			seriesSpace = yAxisHeight / graphSeries->GetCount();
			barHeight = (int)((seriesSpace * .6) / seriesSize);
			dataPlotSize = seriesSize * barHeight;

			pos = graphSeries->GetHeadPosition();

			for(int x = 1; x <= graphSeries->GetCount(); x++)
			{
				tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

				if(tmpSeries->GetData(s) > -1)
				{
					tickYLocation = yApexPoint - ((x * seriesSpace) - (seriesSpace / 2));
					
					barWidth = 0;
					double dataScale = 0.00;

					if((maxTick - minTick) != xAxisWidth)
						dataScale = (xAxisWidth * 1.00) / ((maxTick - minTick) * 1.00);
					else dataScale = 1;
					barWidth = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
					if(((tmpSeries->GetData(s) - minTick) > 0) && (barWidth < 1))
						barWidth = (int)dataScale;

					int xDataLocation = xApexPoint + barWidth;
					
					//now have x and y location of center of ellipse
					COLORREF barColor;
					barColor = GetColor(s);
					//draw line back to last data member
					if(x > 1)
					{
						CPen* pOldPen;
						CPen linePen (PS_SOLID, 2, barColor);
						pOldPen = pDC->SelectObject(&linePen);
						pDC->MoveTo(lastXLoc, lastYLoc);
						pDC->LineTo(xDataLocation, tickYLocation);
						pDC->SelectObject(pOldPen);
					}
					lastYLoc = tickYLocation;
					lastXLoc = xDataLocation;
				}
			}
		}
	}
}

//I may implement radar graphs in the future, after I really determine
//what the purpose is and what the dynamics of it involves

//void CGraph::DrawRadarSeries(CDC* pDC)
//{
//}

void CGraph::Draw3DBarSeries(CDC* pDC)
{
	int barWidth;
	int dataPlotSize;   //used to plot rects of data
	int barL, barT, barR, barB;
	int tickXLocation, tickYLocation;
	int seriesSpace;
	int barHeight;
	POSITION pos;
	CGraphSeries* tmpSeries;
	int blue;
	int red;
	int green;

	if(graphAlignment)
	{
		//determine width of barchart data blocks
		seriesSpace = xAxisWidth  / graphSeries->GetCount();
		barWidth = (int)((seriesSpace * .8) / seriesSize);
		dataPlotSize = seriesSize * barWidth;

		pos = graphSeries->GetHeadPosition();

		for(int x = 1; x <= graphSeries->GetCount(); x++)
		{
			tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

			tickXLocation = xApexPoint + ((x * seriesSpace) - (seriesSpace / 2));

			for(int s = 0; s < seriesSize; s++)
			{
				if(tmpSeries->GetData(s) > -1)
				{
					double dataScale = 0.00;
					dataScale = (yAxisHeight * 1.00) / (maxTick - minTick);
					barHeight = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
					if(((tmpSeries->GetData(s) - minTick) > 0) && (barHeight < 1))
						barHeight = (int)dataScale;

					barL = tickXLocation - (dataPlotSize / 2) + (s * barWidth);
					if(barHeight > yAxisHeight)
						barT = yApexPoint - yAxisHeight;
					else
						barT = yApexPoint - barHeight;
					barR = barL + barWidth;
					barB = yApexPoint;
					
					COLORREF barColor;
					barColor = GetColor(s);
					CBrush brush (barColor);
					CPen rectPen (PS_SOLID, 1, barColor);
					CPen* pOldPen;
					pOldPen = pDC->SelectObject(&rectPen);
					CBrush* pOldBrush;
					pOldBrush = pDC->SelectObject(&brush);
					pDC->Rectangle(barL, barT, barR, barB);

					//now, we do the side
					//side is darker than front, so subtract
					//from color to make closer to black
					red = GetRValue(barColor);
					green = GetGValue(barColor);
					blue = GetBValue(barColor);
					int sideRed = red - 35;
					int sideGreen = green - 35;
					int sideBlue = blue - 35;
					if(sideRed < 0) sideRed = 0;
					if(sideGreen < 0) sideGreen = 0;
					if(sideBlue < 0) sideBlue = 0;
					COLORREF sideColor;
					sideColor = RGB(sideRed, sideGreen, sideBlue);
					CBrush sideBrush (sideColor);
					CPen sidePen (PS_SOLID, 1, sideColor);
					pDC->SelectObject(&sideBrush);
					pDC->SelectObject(&sidePen);
					CPoint sidePolyArray[4];
					CPoint sp1(barR, barT);
					CPoint sp2(barR, barB);
					CPoint sp3(barR + depth, barB - depth);
					CPoint sp4(barR + depth, barT - depth);
					sidePolyArray[0] = sp1;
					sidePolyArray[1] = sp2;
					sidePolyArray[2] = sp3;
					sidePolyArray[3] = sp4;

					pDC->Polygon(sidePolyArray, 4);
					
					//finally, the top
					int topRed = red + 35;
					int topGreen = green + 35;
					int topBlue = blue + 35;
					if(topRed > 255) topRed = 255;
					if(topGreen > 255) topGreen = 255;
					if(topBlue > 255) topBlue = 255;
					COLORREF topColor;
					topColor = RGB(topRed, topGreen, topBlue);
					CBrush topBrush (topColor);
					CPen topPen (PS_SOLID, 1, topColor);
					pDC->SelectObject(&topBrush);
					pDC->SelectObject(&topPen);
					CPoint topPolyArray[4];
					CPoint tp1(barL, barT);
					CPoint tp2(barR, barT);
					CPoint tp3(barR + depth, barT - depth);
					CPoint tp4(barL + depth, barT - depth);
					topPolyArray[0] = tp1;
					topPolyArray[1] = tp2;
					topPolyArray[2] = tp3;
					topPolyArray[3] = tp4;

					pDC->Polygon(topPolyArray, 4);

					pDC->SelectObject(pOldBrush);
					pDC->SelectObject(pOldPen);
				}
			}
		}
	}
	else
	{
		//determine width of barchart data blocks
		seriesSpace = yAxisHeight / graphSeries->GetCount();
		barHeight = (int)((seriesSpace * .8) / seriesSize);
		dataPlotSize = seriesSize * barHeight;

		pos = graphSeries->GetHeadPosition();

		for(int y = 1; y <= graphSeries->GetCount(); y++)
		{
			tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

			tickYLocation = yApexPoint - ((y * seriesSpace) - (seriesSpace / 2));

			for(int s = 0; s < seriesSize; s++)
			{
				if(tmpSeries->GetData(s) > -1)
				{
					double dataScale = 0.00;
					dataScale = ((xAxisWidth - 10) * 1.00) / (maxTick - minTick);

					barWidth = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
					if(((tmpSeries->GetData(s) - minTick) > 0) && (barWidth < 1))
						barWidth = (int)dataScale;

					barL = xApexPoint;
					barB = tickYLocation + (dataPlotSize / 2) - (s * barHeight);
					barT = barB - barHeight;
					if(barWidth > xAxisWidth)
						barR = xApexPoint + xAxisWidth;
					else
						barR = xApexPoint + barWidth;

					COLORREF barColor;
					barColor = GetColor(s);
					CBrush brush (barColor);
					CBrush* pOldBrush;
					pOldBrush = pDC->SelectObject(&brush);
					CPen rectPen (PS_SOLID, 1, barColor);
					CPen* pOldPen;
					pOldPen = pDC->SelectObject(&rectPen);
					pDC->Rectangle(barL, barT, barR, barB);

					//now, we do the side
					//side is darker than front, so subtract
					//from color to make closer to black
					red = GetRValue(barColor);
					green = GetGValue(barColor);
					blue = GetBValue(barColor);
					int sideRed = red - 35;
					int sideGreen = green - 35;
					int sideBlue = blue - 35;
					if(sideRed < 0) sideRed = 0;
					if(sideGreen < 0) sideGreen = 0;
					if(sideBlue < 0) sideBlue = 0;
					COLORREF sideColor;
					sideColor = RGB(sideRed, sideGreen, sideBlue);
					CBrush sideBrush (sideColor);
					CPen sidePen (PS_SOLID, 1, sideColor);
					pDC->SelectObject(&sideBrush);
					pDC->SelectObject(&sidePen);
					CPoint sidePolyArray[4];
					CPoint sp1(barL, barT);
					CPoint sp2(barR, barT);
					CPoint sp3(barR + depth, barT - depth);
					CPoint sp4(barL + depth, barT - depth);
					sidePolyArray[0] = sp1;
					sidePolyArray[1] = sp2;
					sidePolyArray[2] = sp3;
					sidePolyArray[3] = sp4;

					pDC->Polygon(sidePolyArray, 4);
					
					//finally, the top
					int topRed = red + 35;
					int topGreen = green + 35;
					int topBlue = blue + 35;
					if(topRed > 255) topRed = 255;
					if(topGreen > 255) topGreen = 255;
					if(topBlue > 255) topBlue = 255;
					COLORREF topColor;
					topColor = RGB(topRed, topGreen, topBlue);
					CBrush topBrush (topColor);
					CPen topPen (PS_SOLID, 1, topColor);
					pDC->SelectObject(&topBrush);
					pDC->SelectObject(&topPen);
					CPoint topPolyArray[4];
					CPoint tp1(barR, barT);
					CPoint tp2(barR, barB);
					CPoint tp3(barR + depth, barB - depth);
					CPoint tp4(barR + depth, barT - depth);
					topPolyArray[0] = tp1;
					topPolyArray[1] = tp2;
					topPolyArray[2] = tp3;
					topPolyArray[3] = tp4;

					pDC->Polygon(topPolyArray, 4);

					pDC->SelectObject(pOldBrush);
					pDC->SelectObject(pOldPen);
				}
			}
		}
	}
}

void CGraph::Draw3DLineSeries(CDC* pDC)
{
	int barHeight;
	POSITION pos;
	CGraphSeries* tmpSeries;
	int blue;
	int red;
	int green;
	CBrush* pOldBrush;
	CPen* pOldPen;
	int deltaX, deltaY;

	double yAxisScale, xAxisScale;
	int tickXLocation;

	//multiply each value by 1.00 to force result into a double value, and therefore
	//make it more accurate in it's plot location.
	switch(graphQuadType)
	{
		case 1 :
			yAxisScale = ((yApexPoint - topYTick) / (maxTick * 1.00));
			xAxisScale = ((rightXTick - xApexPoint) / (maxTick * 1.00));
			break;
		case 2 :
			yAxisScale = ((yApexPoint - topYTick) / (maxTick * 1.00));
			xAxisScale = ((rightXTick - xApexPoint) / (maxTick * 1.00));
			break;
		case 3 :
			yAxisScale = ((topYTick - yApexPoint) / (maxTick * 1.00));
			xAxisScale = ((rightXTick - xApexPoint) / (maxTick * 1.00));
			break;
		case 4 :
			yAxisScale = ((topYTick - yApexPoint) / (maxTick * 1.00));
			xAxisScale = ((rightXTick - xApexPoint) / (maxTick * 1.00));
			break;
	}

	int lastXLoc, lastYLoc;
	//because of how it ends up looking, I'm forcing 3D line graphs
	//to always display as if in vertical mode graph
//	if(graphAlignment)
//	{
		for(int s = 0; s < seriesSize; s++)
		{
			lastXLoc = xApexPoint + line3DXBase;
			lastYLoc = yApexPoint - line3DYBase;
			//determine width of barchart data blocks

			pos = graphSeries->GetHeadPosition();
			int yDataLocation;

			for(int x = 1; x <= graphSeries->GetCount(); x++)
			{
				tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

				if(tmpSeries->GetData(s) > -1)
				{
					tickXLocation = xApexPoint + (int)(x * (xAxisWidth / graphSeries->GetCount() + 1));
					
					barHeight = 0;
					double dataScale = 0.00;
					
					if((maxTick - minTick) != yAxisHeight)
						dataScale = (yAxisHeight * 1.00) / ((maxTick - minTick) * 1.00);
					else dataScale = 1;
					barHeight = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
					if(((tmpSeries->GetData(s) - minTick) > 0) && (barHeight < 1))
						barHeight = (int)dataScale;

					if(barHeight > yApexPoint)
						barHeight = (int)((maxTick - minTick) * dataScale) + 5;

					yDataLocation = yApexPoint - barHeight;
					//now have x and y location of center of ellipse
					COLORREF barColor;
					barColor = GetColor(s);
					red = GetRValue(barColor);
					green = GetGValue(barColor);
					blue = GetBValue(barColor);
					CBrush brush (barColor);
					pOldBrush = pDC->SelectObject(&brush);

					//draw line back to last data member
					CPen linePen (PS_SOLID, 2, BLACK);
					pOldPen = pDC->SelectObject(&linePen);
					//front side of line
					pDC->MoveTo(lastXLoc + ((seriesSize - s) * (depth / seriesSize)), lastYLoc - ((seriesSize - s) * (depth / seriesSize)));
					pDC->LineTo(tickXLocation + ((seriesSize - s) * (depth / seriesSize)), yDataLocation - ((seriesSize - s) * (depth / seriesSize)));
					pDC->MoveTo(lastXLoc + ((seriesSize - s) * (depth / seriesSize)) - (depth / seriesSize), lastYLoc - ((seriesSize - s) * (depth / seriesSize)) + (depth / seriesSize));
					pDC->LineTo(tickXLocation + ((seriesSize - s) * (depth / seriesSize)) - (depth / seriesSize), yDataLocation - ((seriesSize - s) * (depth / seriesSize)) + (depth / seriesSize));
					pDC->MoveTo(lastXLoc + ((seriesSize - s) * (depth / seriesSize)) - (depth / seriesSize), lastYLoc - ((seriesSize - s) * (depth / seriesSize)) + (depth / seriesSize));
					pDC->LineTo(lastXLoc + ((seriesSize - s) * (depth / seriesSize)), lastYLoc - ((seriesSize - s) * (depth / seriesSize)));
					pDC->MoveTo(tickXLocation + ((seriesSize - s) * (depth / seriesSize)) - (depth / seriesSize), yDataLocation - ((seriesSize - s) * (depth / seriesSize)) + (depth / seriesSize));
					pDC->LineTo(tickXLocation + ((seriesSize - s) * (depth / seriesSize)), yDataLocation - ((seriesSize - s) * (depth / seriesSize)));

					//top part of line, lighter than front
					int topRed = red + 35;
					int topGreen = green + 35;
					int topBlue = blue + 35;
					if(topRed > 255) topRed = 255;
					if(topGreen > 255) topGreen = 255;
					if(topBlue > 255) topBlue = 255;
					COLORREF topColor;
					topColor = RGB(topRed, topGreen, topBlue);
					int bottomRed = red - 55;
					int bottomGreen = green - 55;
					int bottomBlue = blue - 55;
					if(bottomRed < 0) bottomRed = 0;
					if(bottomGreen < 0) bottomGreen = 0;
					if(bottomBlue < 0) bottomBlue = 0;
					COLORREF bottomColor;
					bottomColor = RGB(bottomRed, bottomGreen, bottomBlue);
	
					deltaX = tickXLocation - lastXLoc;
					deltaY = lastYLoc - yDataLocation;

					if(deltaX == deltaY)
					{
						pDC->MoveTo(tickXLocation + ((seriesSize - s) * (depth / seriesSize)), yDataLocation - ((seriesSize - s) * (depth / seriesSize)));
						pDC->LineTo(tickXLocation + ((seriesSize - s) * (depth / seriesSize)) - (depth / seriesSize), yDataLocation - ((seriesSize - s) * (depth / seriesSize)) + (depth / seriesSize));
					}
					else
					{
						CPoint d3linePolyArray[4];
						if(  
							((deltaX > deltaY) && (deltaX < 0)) ||
							((deltaX < deltaY) && (deltaX > 0))
						  )
						{
							CPen bottomPen(PS_SOLID, 1, BLACK);
							CBrush bottomBrush(bottomColor);
							pDC->SelectObject(&bottomPen);
							pDC->SelectObject(&bottomBrush);
							CPoint bp1(lastXLoc + ((seriesSize - s) * (depth / seriesSize)), lastYLoc - ((seriesSize - s) * (depth / seriesSize)));
							CPoint bp2(lastXLoc + ((seriesSize - s) * (depth / seriesSize)) - (depth / seriesSize), lastYLoc - ((seriesSize - s) * (depth / seriesSize)) + (depth / seriesSize));
							CPoint bp3(tickXLocation + ((seriesSize - s) * (depth / seriesSize)) - (depth / seriesSize), yDataLocation - ((seriesSize - s) * (depth / seriesSize)) + (depth / seriesSize));
							CPoint bp4(tickXLocation + ((seriesSize - s) * (depth / seriesSize)), yDataLocation - ((seriesSize - s) * (depth / seriesSize)));
							d3linePolyArray[0] = bp1;
							d3linePolyArray[1] = bp2;
							d3linePolyArray[2] = bp3;
							d3linePolyArray[3] = bp4;
							pDC->Polygon(d3linePolyArray, 4);
						}
						else
						{
							CPen topPen(PS_SOLID, 1, BLACK);
							CBrush topBrush(topColor);
							pDC->SelectObject(&topPen);
							pDC->SelectObject(&topBrush);
							CPoint tp1(lastXLoc + ((seriesSize - s) * (depth / seriesSize)), lastYLoc - ((seriesSize - s) * (depth / seriesSize)));
							CPoint tp2(lastXLoc + ((seriesSize - s) * (depth / seriesSize)) - (depth / seriesSize), lastYLoc - ((seriesSize - s) * (depth / seriesSize)) + (depth / seriesSize));
							CPoint tp3(tickXLocation + ((seriesSize - s) * (depth / seriesSize)) - (depth / seriesSize), yDataLocation - ((seriesSize - s) * (depth / seriesSize)) + (depth / seriesSize));
							CPoint tp4(tickXLocation + ((seriesSize - s) * (depth / seriesSize)), yDataLocation - ((seriesSize - s) * (depth / seriesSize)));
							d3linePolyArray[0] = tp1;
							d3linePolyArray[1] = tp2;
							d3linePolyArray[2] = tp3;
							d3linePolyArray[3] = tp4;
							pDC->Polygon(d3linePolyArray, 4);
						}
					}

					pDC->SelectObject(pOldBrush);
					pDC->SelectObject(pOldPen);

					lastXLoc = tickXLocation;
					lastYLoc = yDataLocation;
				}
			}
		}
/*	}
	else
	{
		for(int s = 0; s < seriesSize; s++)
		{
			//determine width of barchart data blocks
			seriesSpace = yAxisHeight / graphSeries->GetCount();

			pos = graphSeries->GetHeadPosition();
			int xDataLocation;

			for(int x = 1; x <= graphSeries->GetCount(); x++)
			{
				tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

				if(tmpSeries->GetData(s) > -1)
				{
					tickYLocation = yApexPoint - ((x * seriesSpace) - (seriesSpace / 2));
					
					int barWidth;
					double dataScale = 0.00;

					if((maxTick - minTick) != xAxisWidth)
						dataScale = (xAxisWidth * 1.00) / ((maxTick - minTick) * 1.00);
					else dataScale = 1;
					barWidth = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
					if(((tmpSeries->GetData(s) - minTick) > 0) && (barWidth < 1))
						barWidth = (int)dataScale;

					xDataLocation = xApexPoint + barWidth;
					
					//now have x and y location of center of ellipse
					COLORREF barColor;
					barColor = GetColor(s);
					red = GetRValue(barColor);
					green = GetGValue(barColor);
					blue = GetBValue(barColor);
					CBrush brush (barColor);
					pOldBrush = pDC->SelectObject(&brush);
					//draw line back to last data member
					if(x > 1)
					{
						CPen linePen (PS_SOLID, 1, barColor);
						pOldPen = pDC->SelectObject(&linePen);
						//front side of line
						pDC->MoveTo(lastXLoc, lastYLoc);
						pDC->LineTo(xDataLocation, tickYLocation);

						//top part of line, lighter than front
						int topRed = red + 35;
						int topGreen = green + 35;
						int topBlue = blue + 35;
						if(topRed > 255) topRed = 255;
						if(topGreen > 255) topGreen = 255;
						if(topBlue > 255) topBlue = 255;
						COLORREF topColor;
						topColor = RGB(topRed, topGreen, topBlue);
						int bottomRed = red - 55;
						int bottomGreen = green - 55;
						int bottomBlue = blue - 55;
						if(bottomRed < 0) bottomRed = 0;
						if(bottomGreen < 0) bottomGreen = 0;
						if(bottomBlue < 0) bottomBlue = 0;
						COLORREF bottomColor;
						bottomColor = RGB(bottomRed, bottomGreen, bottomBlue);
						
						deltaX = xDataLocation - lastXLoc;
						deltaY = lastYLoc - tickYLocation;

						if(deltaX == deltaY)
						{
							pDC->MoveTo(xDataLocation, tickYLocation);
							pDC->LineTo(xDataLocation + depth, tickYLocation - depth);
						}
						else
						{
							CPoint d3linePolyArray[4];
							if(  
								((deltaY > deltaX) && (deltaY < 0)) ||
								((deltaY < deltaX) && (deltaY > 0))
							  )
							{
								CPen bottomPen(PS_SOLID, 1, BLACK);
								CBrush bottomBrush(bottomColor);
								pDC->SelectObject(&bottomPen);
								pDC->SelectObject(&bottomBrush);
								CPoint bp1(lastXLoc, lastYLoc);
								CPoint bp2(lastXLoc + depth, lastYLoc - depth);
								CPoint bp3(xDataLocation + depth, tickYLocation - depth);
								CPoint bp4(xDataLocation, tickYLocation);
								d3linePolyArray[0] = bp1;
								d3linePolyArray[1] = bp2;
								d3linePolyArray[2] = bp3;
								d3linePolyArray[3] = bp4;
								pDC->Polygon(d3linePolyArray, 4);
							}
							else
							{
								CPen topPen(PS_SOLID, 1, BLACK);
								CBrush topBrush(topColor);
								pDC->SelectObject(&topPen);
								pDC->SelectObject(&topBrush);
								CPoint tp1(lastXLoc, lastYLoc);
								CPoint tp2(lastXLoc + depth, lastYLoc - depth);
								CPoint tp3(xDataLocation + depth, tickYLocation - depth);
								CPoint tp4(xDataLocation, tickYLocation);
								d3linePolyArray[0] = tp1;
								d3linePolyArray[1] = tp2;
								d3linePolyArray[2] = tp3;
								d3linePolyArray[3] = tp4;
								pDC->Polygon(d3linePolyArray, 4);
							}
						}

						pDC->SelectObject(pOldBrush);
						pDC->SelectObject(pOldPen);
					}
					lastXLoc = xDataLocation;
					lastYLoc = tickYLocation;
				}
			}
		}
	}*/
}

void CGraph::Draw3DPieSeries(CDC* pDC)
{
	double dataSum = 0.00;  //for storing cumulative sum
	double labelData = 0.00;
	int lastXLocation, lastYLocation;
	int newXLocation, newYLocation;
	int labelXLocation, labelYLocation;
	double percent = 0.00;
	double labelPercent = 0.00;
	int degrees;
	int labelDegrees;
	double totalSum = 0.00;
	int radius;
	POSITION pos;
	CGraphSeries* tmpSeries;
	int seriesSpace;
	int labelLineXStart, labelLineYStart;
	int labelLineXEnd, labelLineYEnd;
	int maxLabelWidth;
	int maxLabelHeight;
	TEXTMETRIC tm;
	CPen* pOldPen;
	CBrush* pOldBrush;
		
	CPen tmpPen(PS_SOLID, 1, BLACK);
	CBrush tmpBrush(WHITE);
	pOldPen = pDC->SelectObject(&tmpPen);
	pOldBrush = pDC->SelectObject(&tmpBrush);

	//deltaX and deltaY will be based on distance from x and y
	//axis for the new endpoint of the pie.  These values can 
	//then be used to find the true distance between starting 
	//line and ending line of pie boundary.
	double deltaX, deltaY;
	double degreeRadians, degreeRadians2;
	double labelDeltaX, labelDeltaY;
	double labelDegreeRadians, labelDegreeRadians2;

	lastXLocation = 0;
	lastYLocation = 0;

	pDC->GetTextMetrics(&tm);
	maxLabelWidth = tm.tmMaxCharWidth + 10;
	maxLabelHeight = tm.tmHeight + 6;
	
	//pie labels will be stored in a list and drawn after entire pie
	//is completed.
	CObList *pieLabels;
	CGraphPieLabel *pieLabel;

	//determine width of pie display area
	if(xAxisWidth > yAxisHeight)
		seriesSpace = yAxisHeight / graphSeries->GetCount();
	else
		seriesSpace = xAxisWidth / graphSeries->GetCount();
	seriesSpace -= (3 * maxLabelWidth);  //allows text like 25%  (3 chars)

	//to plot a pie plus labels inside of series space, use the following :
	//(3 * radius) + (2 * label width) = series space 
	//so, 3*radius = series space - (2 * label width)
	// 1 radius = (series space - (2 * label width)) / 3
	radius = seriesSpace / 3;  //pie needs 2 radius, + 1 extra for line to labels = 3 for my divisor
	int depth = (int)(radius * depthRatio);	//for shadow pie

	int centerYPie = (yAxisHeight + 60) / 2;

	pos = graphSeries->GetHeadPosition();
	for(int x = 1; x <= graphSeries->GetCount(); x++)
	{
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
		pieLabels = new CObList();

		tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);
		totalSum = 0;
		int s = 0;
		for(s = 0; s < seriesSize; s++)
			totalSum += tmpSeries->GetData(s);
		int pieLeft, pieRight;
		if(graphSeries->GetCount() == 1)
		{
			pieLeft = xApexPoint + (xAxisWidth / 2) - radius;
		}
		else
		{
			pieLeft = xApexPoint + 15 + ((x * 2 - 1) * ((xAxisWidth / graphSeries->GetCount()) / 2)) - radius;
		}
		pieRight = pieLeft + (2 * radius);
		CRect pieRect (pieLeft, 
				centerYPie - radius,
				pieRight, 
				centerYPie + radius);
		CRect shadowRect (pieLeft + depth,
				centerYPie - radius + depth,
				pieRight + depth,
				centerYPie + radius + depth);
		int centerXPie = pieLeft + radius;

		//plot series label
		pDC->TextOut(centerXPie - ((tmpSeries->GetLabel().GetLength() * 8) / 2),
					centerYPie + (int)(radius * 1.5) + maxLabelHeight + 15, tmpSeries->GetLabel());

		int centerShadowXPie;
		int centerShadowYPie;

		//draw shadow pie first
		centerShadowYPie = centerYPie + depth;
		centerShadowXPie = centerXPie + depth;
		lastXLocation = centerShadowXPie - radius;
		lastYLocation = centerShadowYPie;

		int lastTopX = centerXPie - radius;
		int lastTopY = centerYPie;
		int newTopX, newTopY;
		dataSum = 0;
		int lastQuad = 0;
		CPoint lastSidePolyArray[4];
		COLORREF lastColor;
		CPoint spa1;
		CPoint spa2;
		CPoint spa3;
		CPoint spa4;
		for(s = 0; s < seriesSize; s++)
		{
			if(tmpSeries->GetData(s) > 0)
			{
				int seriesDataValue;
				seriesDataValue = tmpSeries->GetData(s);
				dataSum += seriesDataValue;
				percent = (dataSum / totalSum) * 100;
				degrees = (int)((360 * percent) / 100);

				//determine destination quadrant...
					//and set newXLocation and newYLocation values...
					//degrees / 90 will never exceed 4.
					//this can tell us the quadrant of destination
				int quadrant = degrees / 90;  //truncates decimal

				//using the law of sines to determine the deltas :
				//deltaX = radius * sin(90 - degrees)
				//deltaY = radius * sin(degrees)
				//we convert degrees into radians so sin() function works right
				//note :  in quad 1 and 3, we reverse the angle used to compute
				//			the deltas, since the triangle plots reversed.
				switch(quadrant)
				{
					case 0 : //this is the base quadrant, so no manipulation needed
							 degreeRadians = degrees * (3.14159 / 180);
							 degreeRadians2 = (90 - degrees) * (3.14159 / 180);
							 deltaX = radius * sin(degreeRadians2);
							 deltaY = radius * sin(degreeRadians);
							 newXLocation = (int)(centerShadowXPie - deltaX);
							 newYLocation = (int)(centerShadowYPie + deltaY);
							 newTopX = (int)(centerXPie - deltaX);
							 newTopY = (int)(centerYPie + deltaY);
							 break;
					case 1 : //bottom right quadrant, subtract 90 from angle
							 degreeRadians = (degrees - 90) * (3.14159 / 180);
							 degreeRadians2 = (90 - (degrees - 90)) * (3.14159 / 180);
							 deltaX = radius * sin(degreeRadians);
							 deltaY = radius * sin(degreeRadians2);
							 newXLocation = (int)(centerShadowXPie + deltaX);
							 newYLocation = (int)(centerShadowYPie + deltaY);
							 newTopX = (int)(centerXPie + deltaX);
							 newTopY = (int)(centerYPie + deltaY);
							 break;
					case 2 : //top right quadrant, subtract 180 from angle
							 degreeRadians = (degrees - 180) * (3.14159 / 180);
							 degreeRadians2 = (90 - (degrees - 180)) * (3.14159 / 180);
							 deltaX = radius * sin(degreeRadians2);
							 deltaY = radius * sin(degreeRadians);
							 newXLocation = (int)(centerShadowXPie + deltaX);
							 newYLocation = (int)(centerShadowYPie - deltaY);
							 newTopX = (int)(centerXPie + deltaX);
							 newTopY = (int)(centerYPie - deltaY);
							 break;
					case 3 : //upper left quadrant, subtract 270 from angle
							 degreeRadians = (degrees - 270) * (3.14159 / 180);
							 degreeRadians2 = (90 - (degrees - 270)) * (3.14159 / 180);
							 deltaX = radius * sin(degreeRadians);
							 deltaY = radius * sin(degreeRadians2);
							 newXLocation = (int)(centerShadowXPie - deltaX);
							 newYLocation = (int)(centerShadowYPie - deltaY);
							 newTopX = (int)(centerXPie - deltaX);
							 newTopY = (int)(centerYPie - deltaY);
							 break;
					case 4 : //straight line to left of center
							 deltaX = radius;
							 deltaY = 1;
							 newXLocation = (int)(centerShadowXPie - deltaX);
							 newYLocation = (int)(centerShadowYPie - deltaY);
							 newTopX = (int)(centerXPie - deltaX);
							 newTopY = (int)(centerYPie - deltaY);
							 break;
				}
				
				if(s == 0)
				{
					lastYLocation -= 1;
					lastTopY -= 1;
				}

				COLORREF barColor;
				barColor = GetColor(s);
				int red, green, blue;
				red = GetRValue(barColor) - 35;
				green = GetGValue(barColor) - 35;
				blue = GetBValue(barColor) - 35;
				if(red < 0) red = 0;
				if(green < 0) green = 0;
				if(blue < 0) blue = 0;
				CPoint sp1 (lastXLocation, lastYLocation);
				CPoint sp2 (newXLocation, newYLocation);
				COLORREF shadowColor(RGB(red, green, blue));
				CPen shadowPen (PS_SOLID, 1, shadowColor);
				CBrush shadowBrush(shadowColor);
				pDC->SelectObject(&shadowPen);
				pDC->SelectObject(&shadowBrush);
				if((quadrant < 2) || (lastQuad < 2))
				{
					CPoint sidePolyArray[4];
					spa1.x = lastXLocation;
					spa1.y = lastYLocation;
					spa2.x = newXLocation;
					spa2.y = newYLocation;
					spa3.x = newTopX;
					spa3.y = newTopY;
					spa4.x = lastTopX;
					spa4.y = lastTopY;
					sidePolyArray[0] = spa1;
					sidePolyArray[1] = spa2;
					sidePolyArray[2] = spa3;
					sidePolyArray[3] = spa4;
					pDC->Polygon(sidePolyArray, 4);
					pDC->Pie(shadowRect, sp1, sp2);
				}
				else
				{
					CPoint sidePolyArray[3];
					spa1.x = newXLocation;
					spa1.y = newYLocation;
					spa2.x = newTopX;
					spa2.y = newTopY;
					spa3.x = lastTopX;
					spa3.y = lastTopY;
					sidePolyArray[0] = spa1;
					sidePolyArray[1] = spa2;
					sidePolyArray[2] = spa3;
					CPen oldShadowPen (PS_SOLID, 1, lastColor);
					pDC->SelectObject(&oldShadowPen);
					CBrush oldShadowBrush(lastColor);
					pDC->SelectObject(&oldShadowBrush);
					pDC->Pie(shadowRect, sp1, sp2);
					pDC->Polygon(sidePolyArray, 3);
					pDC->SelectObject(&shadowPen);
					pDC->SelectObject(&shadowBrush);
				}

				lastXLocation = newXLocation;
				lastYLocation = newYLocation;
				lastTopX = newTopX;
				lastTopY = newTopY;
				if((quadrant > 1) && (lastQuad < 2))
				{
					lastSidePolyArray[0] = spa1;
					lastSidePolyArray[1] = spa2;
					lastSidePolyArray[2] = spa3;
					lastSidePolyArray[3] = spa4;
					lastColor = shadowColor;
				}
				lastQuad = quadrant;
				pDC->SelectObject(pOldBrush);
				pDC->SelectObject(pOldPen);
			}
		}

		//draw normal pie
		lastXLocation = pieLeft;
		lastYLocation = centerYPie;

		dataSum = 0;
		for(s = 0; s < seriesSize; s++)
		{
			if(tmpSeries->GetData(s) > 0)
			{
				int seriesDataValue;
				seriesDataValue = tmpSeries->GetData(s);
				labelData = seriesDataValue / 2;
				dataSum += seriesDataValue;
				percent = (dataSum / totalSum) * 100;
				labelPercent = ((dataSum - labelData) / totalSum) * 100;
				degrees = (int)((360 * percent) / 100);
				labelDegrees = (int)((360 * labelPercent) / 100);

				//determine destination quadrant...
					//and set newXLocation and newYLocation values...
					//degress / 90 will never exceed 4.
					//this can tell us the quadrant of destination
				int quadrant = degrees / 90;  //truncates decimal
				int labelQuadrant = labelDegrees / 90;

				//using the law of sines to determine the deltas :
				//deltaX = radius * sin(90 - degrees)
				//deltaY = radius * sin(degrees)
				//we convert degrees into radians so sin() function works right
				//note :  in quad 1 and 3, we reverse the angle used to compute
				//			the deltas, since the triangle plots reversed.
				switch(quadrant)
				{
					case 0 : //this is the base quadrant, so no manipulation needed
							 degreeRadians = degrees * (3.14159 / 180);
							 degreeRadians2 = (90 - degrees) * (3.14159 / 180);
							 deltaX = radius * sin(degreeRadians2);
							 deltaY = radius * sin(degreeRadians);
							 newXLocation = (int)(centerXPie - deltaX);
							 newYLocation = (int)(centerYPie + deltaY);
							 break;
					case 1 : //bottom right quadrant, subtract 90 from angle
							 degreeRadians = (degrees - 90) * (3.14159 / 180);
							 degreeRadians2 = (90 - (degrees - 90)) * (3.14159 / 180);
							 deltaX = radius * sin(degreeRadians);
							 deltaY = radius * sin(degreeRadians2);
							 newXLocation = (int)(centerXPie + deltaX);
							 newYLocation = (int)(centerYPie + deltaY);
							 break;
					case 2 : //top right quadrant, subtract 180 from angle
							 degreeRadians = (degrees - 180) * (3.14159 / 180);
							 degreeRadians2 = (90 - (degrees - 180)) * (3.14159 / 180);
							 deltaX = radius * sin(degreeRadians2);
							 deltaY = radius * sin(degreeRadians);
							 newXLocation = (int)(centerXPie + deltaX);
							 newYLocation = (int)(centerYPie - deltaY);
							 break;
					case 3 : //upper left quadrant, subtract 270 from angle
							 degreeRadians = (degrees - 270) * (3.14159 / 180);
							 degreeRadians2 = (90 - (degrees - 270)) * (3.14159 / 180);
							 deltaX = radius * sin(degreeRadians);
							 deltaY = radius * sin(degreeRadians2);
							 newXLocation = (int)(centerXPie - deltaX);
							 newYLocation = (int)(centerYPie - deltaY);
							 break;
					case 4 : //straight line to left of center
							 deltaX = radius;
							 deltaY = 1;
							 newXLocation = (int)(centerXPie - deltaX);
							 newYLocation = (int)(centerYPie - deltaY);
							 break;
				}
				
				switch(labelQuadrant)
				{
					//after getting X & Y location for label, we take
					//1/2 the delta between x y locations and center pie
					case 0 : 
							 labelDegreeRadians = labelDegrees * (3.14159 / 180);
							 labelDegreeRadians2 = (90 - labelDegrees) * (3.14159 / 180);
							 labelDeltaX = radius * sin(labelDegreeRadians2);
							 labelDeltaY = radius * sin(labelDegreeRadians);
							 labelXLocation = (int)(centerXPie - labelDeltaX);
							 labelYLocation = (int)(centerYPie + labelDeltaY);
							 labelLineXStart = labelXLocation + ((centerXPie - labelXLocation) / 2);
							 labelLineYStart = labelYLocation - ((labelYLocation - centerYPie) / 2);
							 labelLineXEnd = labelXLocation - (radius / 2);
							 labelLineYEnd = labelYLocation + (radius / 2);
							 break;
					case 1 : 
							 labelDegreeRadians = (labelDegrees - 90) * (3.14159 / 180);
							 labelDegreeRadians2 = (90 - (labelDegrees - 90)) * (3.14159 / 180);
							 labelDeltaX = radius * sin(labelDegreeRadians);
							 labelDeltaY = radius * sin(labelDegreeRadians2);
							 labelXLocation = (int)(centerXPie + labelDeltaX);
							 labelYLocation = (int)(centerYPie + labelDeltaY);
							 labelLineXStart = labelXLocation - ((labelXLocation - centerXPie) / 2);
							 labelLineYStart = labelYLocation - ((labelYLocation - centerYPie) / 2);
							 labelLineXEnd = labelXLocation + (radius / 2);
							 labelLineYEnd = labelYLocation + (radius / 2);
							 break;
					case 2 : 
							 labelDegreeRadians = (labelDegrees - 180) * (3.14159 / 180);
							 labelDegreeRadians2 = (90 - (labelDegrees - 180)) * (3.14159 / 180);
							 labelDeltaX = radius * sin(labelDegreeRadians2);
							 labelDeltaY = radius * sin(labelDegreeRadians);
							 labelXLocation = (int)(centerXPie + labelDeltaX);
							 labelYLocation = (int)(centerYPie - labelDeltaY);
							 labelLineXStart = labelXLocation - ((labelXLocation - centerXPie) / 2);
							 labelLineYStart = labelYLocation + ((centerYPie - labelYLocation) / 2);
							 labelLineXEnd = labelXLocation + (radius / 2);
							 labelLineYEnd = labelYLocation - (radius / 2);
							 break;
					case 3 : 
							 labelDegreeRadians = (labelDegrees - 270) * (3.14159 / 180);
							 labelDegreeRadians2 = (90 - (labelDegrees - 270)) * (3.14159 / 180);
							 labelDeltaX = radius * sin(labelDegreeRadians);
							 labelDeltaY = radius * sin(labelDegreeRadians2);
							 labelXLocation = (int)(centerXPie - labelDeltaX);
							 labelYLocation = (int)(centerYPie - labelDeltaY);
							 labelLineXStart = labelXLocation + ((centerXPie - labelXLocation) / 2);
							 labelLineYStart = labelYLocation + ((centerYPie - labelYLocation) / 2);
							 labelLineXEnd = labelXLocation - (radius / 2);
							 labelLineYEnd = labelYLocation - (radius / 2);
							 break;
//there should never be a half point ending on 4, so leave it out
				}

				pieLabel = new CGraphPieLabel();

				pieLabel->lineXStart = labelLineXStart;
				pieLabel->lineYStart = labelLineYStart;
				pieLabel->lineXEnd = labelLineXEnd;
				pieLabel->lineYEnd = labelLineYEnd;
				switch(labelQuadrant)
				{
					case 0 : //label to left of line
							 pieLabel->topLeftX = labelLineXEnd - maxLabelWidth;
							 pieLabel->topLeftY = labelLineYEnd;
							 break;
					case 1 : //label to right of line
							 pieLabel->topLeftX = labelLineXEnd + 5;
							 pieLabel->topLeftY = labelLineYEnd;
							 break;
					case 2 : //label to right of line
							 pieLabel->topLeftX = labelLineXEnd + 5;
							 pieLabel->topLeftY = labelLineYEnd - maxLabelHeight;
							 break;
					case 3 : //label to left of line
							 pieLabel->topLeftX = labelLineXEnd - maxLabelWidth;
							 pieLabel->topLeftY = labelLineYEnd - maxLabelHeight;
							 break;
				}
				pieLabel->labelQuadrant = labelQuadrant;
				int roundPercent;
				roundPercent = (int)((seriesDataValue * 100) / totalSum);
				pieLabel->pieLabel.Format(_T("%d%%"), roundPercent);
				pieLabels->AddTail(pieLabel);

				if(s == 0)
					lastYLocation -= 1;

				COLORREF barColor;
				barColor = GetColor(s);
				CPoint p1 (lastXLocation, lastYLocation);
				CPoint p2 (newXLocation, newYLocation);
				CBrush brush (barColor);
				CPen piePen (PS_SOLID, 1, barColor);
				pDC->SelectObject(&piePen);
				pDC->SelectObject(&brush);
				pDC->Pie(pieRect, p1, p2); 

				lastXLocation = newXLocation;
				lastYLocation = newYLocation;
			}
			pDC->SelectObject(pOldBrush);
			pDC->SelectObject(pOldPen);
		}

		//draw lines and labels for pies slices
		POSITION labelPos;

		CBrush lineBrush (BLACK);
		CPen linePen (PS_SOLID, 1, BLACK);
		pDC->SelectObject(&lineBrush);
		pDC->SelectObject(&linePen);
		POSITION mainLinePos, checkLinePos;
		mainLinePos = pieLabels->GetHeadPosition();
		int numLinesDrawn = 0;
		CGraphPieLabel* currentLabel;
		CGraphPieLabel* tmpLabel;
		while(mainLinePos != NULL)
		{
			currentLabel = (CGraphPieLabel*)pieLabels->GetNext(mainLinePos);
			int r = 0;
			checkLinePos = pieLabels->GetHeadPosition();
			while(r < numLinesDrawn)
			{
				//check if any overlap in label areas
				tmpLabel = (CGraphPieLabel*)pieLabels->GetAt(checkLinePos);
				if((currentLabel->topLeftX > tmpLabel->topLeftX) &&
					(currentLabel->topLeftX < (tmpLabel->topLeftX + maxLabelWidth)) &&
					(currentLabel->topLeftY > tmpLabel->topLeftY) &&
					(currentLabel->topLeftY < (tmpLabel->topLeftY + maxLabelHeight)))
				{
					//OVERLAP !!!
					//move current label top left position up or down
					//depending on its quadrant
					if(currentLabel->labelQuadrant < 2)
					{
						//move label down to tmplabel topleft + height
						currentLabel->topLeftY = tmpLabel->topLeftY + maxLabelHeight;
						currentLabel->lineYEnd = tmpLabel->lineYEnd + maxLabelHeight;
					}
					else
					{
						//move label up to tmpLabel topleft - height
						currentLabel->topLeftY = tmpLabel->topLeftY - maxLabelHeight;
						currentLabel->lineYEnd = tmpLabel->lineYEnd - maxLabelHeight;
					}
					//reset r value to 0 so it starts over, just in
					//case we moved the label and it overlaps another
					r = 0;
					checkLinePos = pieLabels->GetHeadPosition();
				}
				else
				{
					r++;
					pieLabels->GetNext(checkLinePos);
				}

			}
			//draw the line and label
			pDC->MoveTo(currentLabel->lineXStart, currentLabel->lineYStart);
			pDC->LineTo(currentLabel->lineXEnd, currentLabel->lineYEnd);

			//write the label
			pDC->TextOut(currentLabel->topLeftX, currentLabel->topLeftY,
					currentLabel->pieLabel);
			numLinesDrawn++;
		}

		//now done, remove everything inside the label list
		labelPos = pieLabels->GetHeadPosition();
		while(labelPos != NULL)
		{
			pieLabel = (CGraphPieLabel*)pieLabels->GetNext(labelPos);
			delete pieLabel;
		}
		delete pieLabels;
	}
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
}

void CGraph::Draw3DStackedBarSeries(CDC* pDC)
{
	int barWidth;
	int barL, barT, barR, barB;
	int tickXLocation, tickYLocation;
	int seriesSpace;
	int barHeight;
	POSITION pos;
	CGraphSeries* tmpSeries;
	int blue;
	int red;
	int green;

	if(graphAlignment)
	{
		//determine width of barchart data blocks
		seriesSpace = xAxisWidth  / graphSeries->GetCount();
		barWidth = (int)(seriesSpace * .8);

		pos = graphSeries->GetHeadPosition();

		double dataScale = 0.00;
		dataScale = (yAxisHeight * 1.00) / (maxTick - minTick);
		for(int x = 1; x <= graphSeries->GetCount(); x++)
		{
			tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

			tickXLocation = xApexPoint + ((x * seriesSpace) - (seriesSpace / 2));

			barT = yApexPoint;	//this is a running total;
								//it starts at yApexPoint so barB can
								//start there, then moves up by barHeight
			for(int s = 0; s < seriesSize; s++)
			{
				if(tmpSeries->GetData(s) > -1)
				{
					barHeight = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
					if(((tmpSeries->GetData(s) - minTick) > 0) && (barHeight < 1))
						barHeight = (int)dataScale;

					barB = barT;
					barL = tickXLocation - (barWidth / 2);
					if(barHeight > yAxisHeight)
						barT = barB - yAxisHeight;
					else
						barT = barB - barHeight;
					barR = barL + barWidth;

					COLORREF barColor;
					barColor = GetColor(s);
					CBrush brush (barColor);
					CPen rectPen (PS_SOLID, 1, barColor);
					CPen* pOldPen;
					pOldPen = pDC->SelectObject(&rectPen);
					CBrush* pOldBrush;
					pOldBrush = pDC->SelectObject(&brush);
					pDC->Rectangle(barL, barT, barR, barB);

					//now, we do the side
					//side is darker than front, so subtract
					//from color to make closer to black
					red = GetRValue(barColor);
					green = GetGValue(barColor);
					blue = GetBValue(barColor);
					int sideRed = red - 35;
					int sideGreen = green - 35;
					int sideBlue = blue - 35;
					if(sideRed < 0) sideRed = 0;
					if(sideGreen < 0) sideGreen = 0;
					if(sideBlue < 0) sideBlue = 0;
					COLORREF sideColor;
					sideColor = RGB(sideRed, sideGreen, sideBlue);
					CBrush sideBrush (sideColor);
					CPen sidePen (PS_SOLID, 1, sideColor);
					pDC->SelectObject(&sideBrush);
					pDC->SelectObject(&sidePen);
					CPoint sidePolyArray[4];
					CPoint sp1(barR, barT);
					CPoint sp2(barR, barB);
					CPoint sp3(barR + depth, barB - depth);
					CPoint sp4(barR + depth, barT - depth);
					sidePolyArray[0] = sp1;
					sidePolyArray[1] = sp2;
					sidePolyArray[2] = sp3;
					sidePolyArray[3] = sp4;

					pDC->Polygon(sidePolyArray, 4);
					
					//finally, the top
					int topRed = red + 35;
					int topGreen = green + 35;
					int topBlue = blue + 35;
					if(topRed > 255) topRed = 255;
					if(topGreen > 255) topGreen = 255;
					if(topBlue > 255) topBlue = 255;
					COLORREF topColor;
					topColor = RGB(topRed, topGreen, topBlue);
					CBrush topBrush (topColor);
					CPen topPen (PS_SOLID, 1, topColor);
					pDC->SelectObject(&topBrush);
					pDC->SelectObject(&topPen);
					CPoint topPolyArray[4];
					CPoint tp1(barL, barT);
					CPoint tp2(barR, barT);
					CPoint tp3(barR + depth, barT - depth);
					CPoint tp4(barL + depth, barT - depth);
					topPolyArray[0] = tp1;
					topPolyArray[1] = tp2;
					topPolyArray[2] = tp3;
					topPolyArray[3] = tp4;

					pDC->Polygon(topPolyArray, 4);

					pDC->SelectObject(pOldBrush);
					pDC->SelectObject(pOldPen);
				}
			}
		}
	}
	else
	{
		//determine width of barchart data blocks
		seriesSpace = yAxisHeight / graphSeries->GetCount();
		barHeight = (int)(seriesSpace * .8);

		pos = graphSeries->GetHeadPosition();

		double dataScale = 0.00;
		dataScale = ((xAxisWidth - 10) * 1.00) / (maxTick - minTick);
		for(int y = 1; y <= graphSeries->GetCount(); y++)
		{
			tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

			tickYLocation = yApexPoint - ((y * seriesSpace) - (seriesSpace / 2));

			barR = xApexPoint;	//this is a running total;
								//it starts at xApexPoint so barL can
								//start there, then moves right by barWidth
			for(int s = 0; s < seriesSize; s++)
			{
				if(tmpSeries->GetData(s) > -1)
				{

					barWidth = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
					if(((tmpSeries->GetData(s) - minTick) > 0) && (barWidth < 1))
						barWidth = (int)dataScale;

					barL = barR;
					barT = tickYLocation - (barHeight / 2);
					barB = barT + barHeight;
					if(barWidth > xAxisWidth)
						barR = barL + xAxisWidth;
					else
						barR = barL + barWidth;

					COLORREF barColor;
					barColor = GetColor(s);
					CBrush brush (barColor);
					CBrush* pOldBrush;
					pOldBrush = pDC->SelectObject(&brush);
					CPen rectPen (PS_SOLID, 1, barColor);
					CPen* pOldPen;
					pOldPen = pDC->SelectObject(&rectPen);
					pDC->Rectangle(barL, barT, barR, barB);

					//now, we do the side
					//side is darker than front, so subtract
					//from color to make closer to black
					red = GetRValue(barColor);
					green = GetGValue(barColor);
					blue = GetBValue(barColor);
					int sideRed = red - 35;
					int sideGreen = green - 35;
					int sideBlue = blue - 35;
					if(sideRed < 0) sideRed = 0;
					if(sideGreen < 0) sideGreen = 0;
					if(sideBlue < 0) sideBlue = 0;
					COLORREF sideColor;
					sideColor = RGB(sideRed, sideGreen, sideBlue);
					CBrush sideBrush (sideColor);
					CPen sidePen (PS_SOLID, 1, sideColor);
					pDC->SelectObject(&sideBrush);
					pDC->SelectObject(&sidePen);
					CPoint sidePolyArray[4];
					CPoint sp1(barL, barT);
					CPoint sp2(barR, barT);
					CPoint sp3(barR + depth, barT - depth);
					CPoint sp4(barL + depth, barT - depth);
					sidePolyArray[0] = sp1;
					sidePolyArray[1] = sp2;
					sidePolyArray[2] = sp3;
					sidePolyArray[3] = sp4;

					pDC->Polygon(sidePolyArray, 4);
					
					//finally, the top
					int topRed = red + 35;
					int topGreen = green + 35;
					int topBlue = blue + 35;
					if(topRed > 255) topRed = 255;
					if(topGreen > 255) topGreen = 255;
					if(topBlue > 255) topBlue = 255;
					COLORREF topColor;
					topColor = RGB(topRed, topGreen, topBlue);
					CBrush topBrush (topColor);
					CPen topPen (PS_SOLID, 1, topColor);
					pDC->SelectObject(&topBrush);
					pDC->SelectObject(&topPen);
					CPoint topPolyArray[4];
					CPoint tp1(barR, barT);
					CPoint tp2(barR, barB);
					CPoint tp3(barR + depth, barB - depth);
					CPoint tp4(barR + depth, barT - depth);
					topPolyArray[0] = tp1;
					topPolyArray[1] = tp2;
					topPolyArray[2] = tp3;
					topPolyArray[3] = tp4;

					pDC->Polygon(topPolyArray, 4);

					pDC->SelectObject(pOldBrush);
					pDC->SelectObject(pOldPen);
				}
			}
		}
	}
}


void CGraph::PrintBarSeries(CDC* pDC)
{
	int barWidth;
	int dataPlotSize;   //used to plot rects of data
	int barL, barT, barR, barB;
	int tickXLocation, tickYLocation;
	int seriesSpace;
	int barHeight;  //for scalability
	POSITION pos;
	CGraphSeries* tmpSeries;

	if(graphAlignment)
	{
		//determine width of barchart data blocks
		seriesSpace = xAxisWidth / graphSeries->GetCount();
		barWidth = (int)((seriesSpace * .6) / seriesSize);
		dataPlotSize = seriesSize * barWidth;

		pos = graphSeries->GetHeadPosition();

		for(int x = 1; x <= graphSeries->GetCount(); x++)
		{
			tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

			tickXLocation = xApexPoint + ((x * seriesSpace) - (seriesSpace / 2));

			for(int s = 0; s < seriesSize; s++)
			{
				if(tmpSeries->GetData(s) > -1)
				{
					double dataScale = 0.00;
					dataScale = ((yAxisHeight - 100) * 1.00) / (maxTick - minTick);
					barHeight = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
					if(((tmpSeries->GetData(s) - minTick) > 0) && (barHeight < 1))
						barHeight = (int)dataScale;

					barL = tickXLocation - (dataPlotSize / 2) + (s * barWidth);
					if(barHeight > yAxisHeight)
						barT = yApexPoint + yAxisHeight;
					else
						barT = yApexPoint + barHeight;
					barR = barL + barWidth;
					barB = yApexPoint;
					
					COLORREF barColor;
					barColor = GetColor(s);
					CBrush brush (barColor);
					CBrush* pOldBrush;
					pOldBrush = pDC->SelectObject(&brush);
					pDC->Rectangle(barL, barT, barR, barB);
					pDC->SelectObject(pOldBrush);
				}
			}
		}
	}
	else
	{
		//determine width of barchart data blocks
		seriesSpace = yAxisHeight / graphSeries->GetCount();
		barHeight = (int)((seriesSpace * .6) / seriesSize);
		dataPlotSize = seriesSize * barHeight;

		pos = graphSeries->GetHeadPosition();

		for(int y = 1; y <= graphSeries->GetCount(); y++)
		{
			tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

			tickYLocation = yApexPoint + ((y * seriesSpace) - (seriesSpace / 2));

			for(int s = 0; s < seriesSize; s++)
			{
				if(tmpSeries->GetData(s) > -1)
				{
					double dataScale = 0.00;
					dataScale = ((xAxisWidth - 100) * 1.00) / (maxTick - minTick);

					barWidth = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
					if(((tmpSeries->GetData(s) - minTick) > 0) && (barWidth < 1))
						barWidth = (int)dataScale;

					barL = xApexPoint;
					barT = tickYLocation - (dataPlotSize / 2) + (s * barHeight);
					barB = barT + barHeight;
					if(barWidth > xAxisWidth)
						barR = xApexPoint + xAxisWidth;
					else
						barR = xApexPoint + barWidth;
					
					COLORREF barColor;
					barColor = GetColor(s);
					CBrush brush (barColor);
					CBrush* pOldBrush;
					pOldBrush = pDC->SelectObject(&brush);
					pDC->Rectangle(barL, barT, barR, barB);
					pDC->SelectObject(pOldBrush);
				}
			}
		}
	}
}

void CGraph::PrintLineSeries(CDC* pDC)
{
	int barWidth;
	int dataPlotSize;   //used to plot rects of data
	int tickXLocation, tickYLocation;
	int seriesSpace;
	int barHeight;  //for scalability
	POSITION pos;
	CGraphSeries* tmpSeries;

	int lastXLoc, lastYLoc;
	if(graphAlignment)
	{
		for(int s = 0; s < seriesSize; s++)
		{
			//determine width of barchart data blocks
			seriesSpace = xAxisWidth / graphSeries->GetCount();
			barWidth = (int)((seriesSpace * .6) / seriesSize);
			dataPlotSize = seriesSize * barWidth;

			pos = graphSeries->GetHeadPosition();

			for(int x = 1; x <= graphSeries->GetCount(); x++)
			{
				tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

				if(tmpSeries->GetData(s) > -1)
				{
					tickXLocation = xApexPoint + ((x * seriesSpace) - (seriesSpace / 2));
					
					barHeight = 0;
					double dataScale = 0.00;
					
					if((maxTick - minTick) != yAxisHeight)
						dataScale = ((yAxisHeight * 1.00) / ((maxTick - minTick) * 1.00)) * tickSpace;	
					else dataScale = tickSpace * 15.00;

					int axisHeight = (int)(dataScale * numTicks);
					double barPercent = 0.00;
					barPercent = ((tmpSeries->GetData(s) - minTick) * 1.00) / maxTick;
					barHeight = (int)(barPercent * axisHeight);
					if(((tmpSeries->GetData(s) - minTick) > 0) && (barHeight < 1))
						barHeight = (int)dataScale;

					int yDataLocation = yApexPoint + barHeight;
					
					//now have x and y location of center of ellipse
					COLORREF barColor;
					barColor = GetColor(s);
					CBrush brush (barColor);
					CBrush* pOldBrush;
					pOldBrush = pDC->SelectObject(&brush);
					//draw line back to last data member
					if(x > 1)
					{
						CPen* pOldPen;
						CPen linePen (PS_SOLID, 20, barColor);
						pOldPen = pDC->SelectObject(&linePen);
						pDC->MoveTo(lastXLoc, lastYLoc);
						pDC->LineTo(tickXLocation, yDataLocation);
						pDC->SelectObject(pOldPen);
					}
					//now draw ellipse...
					pDC->Ellipse(tickXLocation + 60, yDataLocation + 60,
						tickXLocation - 60, yDataLocation - 60);
					lastXLoc = tickXLocation;
					lastYLoc = yDataLocation;
					pDC->SelectObject(pOldBrush);
				}
			}
		}
	}
	else
	{
		for(int s = 0; s < seriesSize; s++)
		{
			//determine width of barchart data blocks
			seriesSpace = yAxisHeight / graphSeries->GetCount();
			barHeight = (int)((seriesSpace * .6) / seriesSize);
			dataPlotSize = seriesSize * barHeight;

			pos = graphSeries->GetHeadPosition();

			for(int x = 1; x <= graphSeries->GetCount(); x++)
			{
				tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

				if(tmpSeries->GetData(s) > -1)
				{
					tickYLocation = yApexPoint + ((x * seriesSpace) - (seriesSpace / 2));
					
					barWidth = 0;
					double dataScale = 0.00;

					dataScale = ((xAxisWidth - 100) * 1.00) / (maxTick - minTick);

					int axisWidth = (int)(dataScale * (maxTick - minTick));
					double barPercent = 0.00;
					barPercent = ((tmpSeries->GetData(s) - minTick) * 1.00) / ((maxTick - minTick) * 1.00);
					barWidth = (int)(barPercent * axisWidth);
					if(((tmpSeries->GetData(s) - minTick) > 0) && (barHeight < 1))
						barWidth = (int)dataScale;

					int xDataLocation = xApexPoint + barWidth;
					
					//now have x and y location of center of ellipse
					COLORREF barColor;
					barColor = GetColor(s);
					CBrush brush (barColor);
					CBrush* pOldBrush;
					pOldBrush = pDC->SelectObject(&brush);
					//draw line back to last data member
					if(x > 1)
					{
						CPen* pOldPen;
						CPen linePen (PS_SOLID, 20, barColor);
						pOldPen = pDC->SelectObject(&linePen);
						pDC->MoveTo(lastXLoc, lastYLoc);
						pDC->LineTo(xDataLocation, tickYLocation);
						pDC->SelectObject(pOldPen);
					}
					//now draw ellipse...
					pDC->Ellipse(xDataLocation + 60, tickYLocation + 60,
						xDataLocation - 60, tickYLocation - 60);
					lastYLoc = tickYLocation;
					lastXLoc = xDataLocation;
					pDC->SelectObject(pOldBrush);
				}
			}
		}
	}
}

void CGraph::PrintPieSeries(CDC* pDC)
{
	int seriesSpace;
	POSITION pos;
	CGraphSeries* tmpSeries;

	double dataSum = 0.00;  //for storing cumulative sum
	int lastXLocation, lastYLocation;
	int newXLocation, newYLocation;
	double percent = 0.00;
	int degrees;
	double totalSum = 0.00;
	int deltaXY;
	int radius;

	double labelData = 0.00;
	int labelXLocation, labelYLocation;
	double labelPercent = 0.00;
	int labelDegrees;
	int labelDeltaXY;
	int labelLineXStart, labelLineYStart;
	int labelLineXEnd, labelLineYEnd;
	int maxLabelWidth;
	int maxLabelHeight;
	TEXTMETRIC tm;
		
	lastXLocation = 0;
	lastYLocation = 0;

	pDC->GetTextMetrics(&tm);
	maxLabelWidth = tm.tmMaxCharWidth + 50;
	maxLabelHeight = tm.tmHeight + 30;

	//pie labels are stored in a list and drawn after entire pie
	//is completed.
	CObList *pieLabels;
	CGraphPieLabel *pieLabel;

	//determine width of pie display area
	if(xAxisWidth > yAxisHeight)
		seriesSpace = yAxisHeight / graphSeries->GetCount();
	else
		seriesSpace = xAxisWidth / graphSeries->GetCount();
	seriesSpace -= (3 * maxLabelWidth);  //allows text like 25%  (3 chars)

	//to plot a pie plus labels inside of series space, use the following :
	//(3 * radius) + (2 * label width) = series space 
	//so, 3*radius = series space - (2 * label width)
	// 1 radius = (series space - (2 * label width)) / 3
	radius = seriesSpace / 3;  //pie needs 2 radius, + 1 extra for line to labels = 3 for my divisor

	int centerYPie = pGraphT - (yAxisHeight / 2) - 500;
	
	pos = graphSeries->GetHeadPosition();
	for(int x = 1; x <= graphSeries->GetCount(); x++)
	{
		pieLabels = new CObList();

		tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);
		totalSum = 0;
		for(int s = 0; s < seriesSize; s++)
			totalSum += tmpSeries->GetData(s);
		int pieLeft, pieRight;
		if(graphSeries->GetCount() == 1)
		{
			pieLeft = xApexPoint + (xAxisWidth / 2) - radius;
		}
		else
		{
			pieLeft = xApexPoint + 15 + ((x * 2 - 1) * ((xAxisWidth / graphSeries->GetCount()) / 2)) - radius;
		}
		pieRight = pieLeft + (2 * radius);
		CRect pieRect (pieLeft, 
				centerYPie + radius,
				pieRight, 
				centerYPie - radius);
		int centerXPie = pieLeft + radius;

		CFont pieFont;
		pieFont.CreateFont(xTickFontSize * 20, 0, 0, 0, 700, FALSE, FALSE, 0,
			ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,_T("Arial"));
		CFont* pOldFont = (CFont*) pDC->SelectObject(&pieFont);

		//plot series label
		pDC->TextOut(centerXPie - ((tmpSeries->GetLabel().GetLength() / 2) * ((xTickFontSize / 2) * 20)),
					centerYPie - radius - maxLabelHeight - 600, tmpSeries->GetLabel());
		pDC->SelectObject(pOldFont);

		lastXLocation = pieLeft;
		lastYLocation = centerYPie;
		
		dataSum = 0;
		int s = 0;
		for(s = 0; s < seriesSize; s++)
		{
			if(tmpSeries->GetData(s) > 0)
			{
				int seriesDataValue;
				seriesDataValue = tmpSeries->GetData(s);
				dataSum += seriesDataValue;
				percent = (dataSum * 100) / totalSum;
				degrees = (int)((360 * percent) / 100);

				labelData = tmpSeries->GetData(s) / 2;
				labelPercent = ((dataSum - labelData) / totalSum) * 100;
				labelDegrees = (int)((360 * labelPercent) / 100);

					//degress / 90 will never exceed 4.
					//this can tell us the quadrant of destination
				deltaXY = (degrees * radius) / 90;
				labelDeltaXY = (labelDegrees * radius) / 90;
					//deltaXY is change from start point of pie 0
					//this value is total change.  so if radius is 300
					//and degrees is 270, delta is 300.  The change 
					//would move both x and y 300 pixels.  For x, 100
					//to right is center, another 100 to right edge,
					//100 back to center.  For y, 100 to bottom, 100
					//back to center, 100 to top. thus gives 270 degree
					//rotation.

				//determine destination quadrant...
					//and set newXLocation and newYLocation values...
				int quadrant = degrees / 90;  //truncates decimal
				int labelQuadrant = labelDegrees / 90;
				switch(quadrant)
				{
					//in the computations below, the remarked line is
					//the original computation.  The line above it, for
					//actual use, is the simplified line, and gives the
					//exact same result
					case 0 : newXLocation = pieLeft + deltaXY;
							 newYLocation = centerYPie - deltaXY;
							 break;
					case 1 : newXLocation = pieLeft + deltaXY;
							 newYLocation = centerYPie - (2 * radius) + deltaXY;
							 break;
					case 2 : newXLocation = pieLeft + (4 * radius) - deltaXY;
							 newYLocation = centerYPie - (2 * radius) + deltaXY;
							 break;
					case 3 : newXLocation = pieLeft + (4 * radius) - deltaXY;
							 newYLocation = centerYPie + (4 * radius) - deltaXY;
							 break;
					case 4 : newXLocation = pieLeft;
							 newYLocation = centerYPie + 1;
							 break;
				}
				
				switch(labelQuadrant)
				{
					//after getting X & Y location for label, we take
					//1/2 the delta between x y locations and center pie
					case 0 : labelXLocation = pieLeft + labelDeltaXY;
							 labelYLocation = centerYPie - labelDeltaXY;
							 labelLineXStart = labelXLocation + ((centerXPie - labelXLocation) / 2);
							 labelLineYStart = labelYLocation - ((labelYLocation - centerYPie) / 2);
							 labelLineXEnd = labelXLocation - (radius / 2);
							 labelLineYEnd = labelYLocation - (radius / 2);
							 break;
					case 1 : labelXLocation = pieLeft + labelDeltaXY;
							 labelYLocation = centerYPie - (2 * radius) + labelDeltaXY;
							 labelLineXStart = labelXLocation - ((labelXLocation - centerXPie) / 2);
							 labelLineYStart = labelYLocation - ((labelYLocation - centerYPie) / 2);
							 labelLineXEnd = labelXLocation + (radius / 2);
							 labelLineYEnd = labelYLocation - (radius / 2);
							 break;
					case 2 : labelXLocation = pieLeft + (4 * radius) - labelDeltaXY;
							 labelYLocation = centerYPie - (2 * radius) + labelDeltaXY;
							 labelLineXStart = labelXLocation - ((labelXLocation - centerXPie) / 2);
							 labelLineYStart = labelYLocation - ((labelYLocation - centerYPie) / 2);
							 labelLineXEnd = labelXLocation + (radius / 2);
							 labelLineYEnd = labelYLocation + (radius / 2);
							 break;
					case 3 : labelXLocation = pieLeft + (4 * radius) - labelDeltaXY;
							 labelYLocation = centerYPie + (4 * radius) - labelDeltaXY;
							 labelLineXStart = labelXLocation + ((centerXPie - labelXLocation) / 2);
							 labelLineYStart = labelYLocation - ((labelYLocation - centerYPie) / 2);
							 labelLineXEnd = labelXLocation - (radius / 2);
							 labelLineYEnd = labelYLocation + (radius / 2);
							 break;
//there should never be a half point ending on 4, so leave it out
				}

				pieLabel = new CGraphPieLabel();

				pieLabel->lineXStart = labelLineXStart;
				pieLabel->lineYStart = labelLineYStart;
				pieLabel->lineXEnd = labelLineXEnd;
				pieLabel->lineYEnd = labelLineYEnd;
				switch(labelQuadrant)
				{
					case 0 : //label to left of line
							 pieLabel->topLeftX = labelLineXEnd - maxLabelWidth;
							 pieLabel->topLeftY = labelLineYEnd;
							 break;
					case 1 : //label to right of line
							 pieLabel->topLeftX = labelLineXEnd + 50;
							 pieLabel->topLeftY = labelLineYEnd;
							 break;
					case 2 : //label to right of line
							 pieLabel->topLeftX = labelLineXEnd + 50;
							 pieLabel->topLeftY = labelLineYEnd + maxLabelHeight;
							 break;
					case 3 : //label to left of line
							 pieLabel->topLeftX = labelLineXEnd - maxLabelWidth;
							 pieLabel->topLeftY = labelLineYEnd + maxLabelHeight;
							 break;
				}
				pieLabel->labelQuadrant = labelQuadrant;
				int roundPercent;
				roundPercent = (int)((seriesDataValue * 100) / totalSum);
				pieLabel->pieLabel.Format(_T("%d%%"), roundPercent);
				pieLabels->AddTail(pieLabel);

				if(s == 0)
					lastYLocation -= 1;

				CPoint p1 (lastXLocation, lastYLocation);
				CPoint p2 (newXLocation, newYLocation);
				COLORREF barColor;
				barColor = GetColor(s);
				CBrush brush (barColor);
				CBrush* pOldBrush;
				CPen piePen(PS_SOLID, 1, barColor);
				CPen* pOldPen;
				pOldBrush = pDC->SelectObject(&brush);
				pOldPen = pDC->SelectObject(&piePen);
				pDC->Pie(pieRect, p1, p2); 
				pDC->SelectObject(pOldBrush);
				pDC->SelectObject(pOldPen);

				lastXLocation = newXLocation;
				lastYLocation = newYLocation;
			}
		}
		//draw lines and labels for pies slices
		POSITION labelPos;

		CBrush lineBrush (BLACK);
		CBrush* pOldBrush;
		pOldBrush = pDC->SelectObject(&lineBrush);
		POSITION mainLinePos, checkLinePos;
		mainLinePos = pieLabels->GetHeadPosition();
		int numLinesDrawn = 0;
		CGraphPieLabel* currentLabel;
		CGraphPieLabel* tmpLabel;
		while(mainLinePos != NULL)
		{
			currentLabel = (CGraphPieLabel*)pieLabels->GetNext(mainLinePos);
			int r = 0;
			checkLinePos = pieLabels->GetHeadPosition();
			while(r < numLinesDrawn)
			{
				//check if any overlap in label areas
				tmpLabel = (CGraphPieLabel*)pieLabels->GetAt(checkLinePos);
				if((currentLabel->topLeftX > tmpLabel->topLeftX) &&
					(currentLabel->topLeftX < (tmpLabel->topLeftX + maxLabelWidth)) &&
					(currentLabel->topLeftY < tmpLabel->topLeftY) &&
					(currentLabel->topLeftY > (tmpLabel->topLeftY - maxLabelHeight)))
				{
					//OVERLAP !!!
					//move current label top left position up or down
					//depending on its quadrant
					if(currentLabel->labelQuadrant < 2)
					{
						//move label down to tmplabel topleft + height
						currentLabel->topLeftY = tmpLabel->topLeftY - maxLabelHeight;
						currentLabel->lineYEnd = tmpLabel->lineYEnd - maxLabelHeight;
					}
					else
					{
						//move label up to tmpLabel topleft - height
						currentLabel->topLeftY = tmpLabel->topLeftY + maxLabelHeight;
						currentLabel->lineYEnd = tmpLabel->lineYEnd + maxLabelHeight;
					}
					//reset r value to 0 so it starts over, just in
					//case we moved the label and it overlaps another
					r = 0;
					checkLinePos = pieLabels->GetHeadPosition();
				}
				else
				{
					r++;
					pieLabels->GetNext(checkLinePos);
				}

			}
			//draw the line and label
			pDC->MoveTo(currentLabel->lineXStart, currentLabel->lineYStart);
			pDC->LineTo(currentLabel->lineXEnd, currentLabel->lineYEnd);

			//write the label
			pDC->TextOut(currentLabel->topLeftX, currentLabel->topLeftY,
					currentLabel->pieLabel);
			numLinesDrawn++;
		}

		pDC->SelectObject(pOldBrush);

		//now done, remove everything inside the label list
		labelPos = pieLabels->GetHeadPosition();
		while(labelPos != NULL)
		{
			pieLabel = (CGraphPieLabel*)pieLabels->GetNext(labelPos);
			delete pieLabel;
		}
		delete pieLabels;
	}
}


void CGraph::PrintScatterSeries(CDC* pDC)
{
	//rightXTick and topYTick contain the outer bounds of the axis ticks.
	//So, if maxTick is 100 on both axis lines, then 100, 100 would be top
	//right.  We will use the bounds to see spacing from apex points and then
	//scale so any point can be plotted inside

	double yAxisScale, xAxisScale;
	int tickXLocation, tickYLocation;

	//multiply each value by 1.00 to force result into a double value, and therefore
	//make it more accurate in it's plot location.
	yAxisScale = ((p_topYTick - yApexPoint) / (maxTick * 1.00));
	xAxisScale = ((p_rightXTick - xApexPoint) / (maxTick * 1.00));

	//points will now plot as ((value * scale) + apex point)
	COLORREF barColor;
	barColor = BLACK;
	CBrush brush (barColor);
	CBrush* pOldBrush;
	pOldBrush = pDC->SelectObject(&brush);

	POSITION pos;
	CGraphSeries* tmpSeries;
	for(pos = graphSeries->GetHeadPosition(); pos != NULL; graphSeries->GetNext(pos))
	{
		tmpSeries = (CGraphSeries*)graphSeries->GetAt(pos);

		//multiply each value by 1.00 to force result into a double value, and therefore
		//make it more accurate in it's plot location.
		tickXLocation = (int)((tmpSeries->GetXDataValue() * 1.00) * xAxisScale) + xApexPoint;
		tickYLocation = yApexPoint + (int)((tmpSeries->GetYDataValue() * 1.00) * yAxisScale);

		//draw ellipse...
		pDC->Ellipse(tickXLocation - 60, tickYLocation - 60,
			tickXLocation + 60, tickYLocation + 60);

	}
	pDC->SelectObject(pOldBrush);

}

void CGraph::PrintBoxWhiskerSeries(CDC* pDC)
{
	int upperQuartile;
	int lowerQuartile;
	int median;
	int highValue;
	int lowValue;
	
	COLORREF barColor;
	barColor = BLACK;
	CBrush brush (barColor);
	CBrush* pOldBrush;
	pOldBrush = pDC->SelectObject(&brush);

	int barWidth;
	int barL, barT, barR, barB;
	int tickXLocation, tickYLocation;
	int seriesSpace;
	int barHeight;
	POSITION pos;
	CGraphSeries* tmpSeries;

	pos = graphSeries->GetHeadPosition();
	CUIntArray dataArray;
	CUIntArray sortedArray;

	for(int x = 1; x <= graphSeries->GetCount(); x++)
	{
		dataArray.RemoveAll();
		sortedArray.RemoveAll();

		CObList *sortedData;
		sortedData = new CObList();
		tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);
		seriesSize = tmpSeries->GetDataCount();

		int dataValue;
		for(int s = 0; s < seriesSize; s++)
		{
			dataValue = 0;
			if(tmpSeries->GetData(s) > -1)
				dataValue = tmpSeries->GetData(s);
			dataArray.Add(dataValue);
		}

		//sort the array
		dataValue = dataArray.GetAt(0);
		sortedArray.Add(dataValue);
		for(int d = 1; d < dataArray.GetSize(); d++)
		{
			dataValue = dataArray.GetAt(d);
			BOOL placed = FALSE;
			for(int s = 0; s < sortedArray.GetSize(); s++)
			{
				int tmpI = sortedArray.GetAt(s);
				if(dataValue <= tmpI)
				{
					sortedArray.InsertAt(s, dataValue);
					placed = TRUE;
					break;
				}
			}
			if(!placed)	//add at end
				sortedArray.Add(dataValue);
		}
		
		//sortedArray now contains the sorted list of all data in this
		//series.  From here, we derive the other box-whisker data
		
		int medialElement;
		div_t div_result;
		div_result = div(sortedArray.GetSize(), 2);

		if(div_result.rem == 0)	//even number of elements
		{
			//size is not 0 base, so below, I subtract 1 to 0 base it.
			medialElement = (sortedArray.GetSize() / 2) - 1;
			median = sortedArray.GetAt(medialElement) + 
						((sortedArray.GetAt(medialElement + 1) - 
						sortedArray.GetAt(medialElement)) / 2);
			highValue = sortedArray.GetAt(sortedArray.GetSize() - 1);
			lowValue = sortedArray.GetAt(0);
			div_t div2Result;
			div2Result = div(medialElement + 1, 2);
			if(div2Result.rem == 0)	//even again
			{
				upperQuartile = sortedArray.GetAt(medialElement + 1 + (medialElement / 2)) +
									(sortedArray.GetAt(medialElement + (medialElement / 2) + 2) -
									sortedArray.GetAt(medialElement + (medialElement / 2))) / 2;
				lowerQuartile = sortedArray.GetAt(medialElement / 2) +
									((sortedArray.GetAt((medialElement / 2) + 1) -
									sortedArray.GetAt(medialElement / 2)) / 2);
			}
			else	//odd
			{
				upperQuartile = sortedArray.GetAt(medialElement + 1 + (medialElement / 2));
				lowerQuartile = sortedArray.GetAt(medialElement / 2);
			}
		}
		else	//odd number of elements
		{
			//size is not 0 base, so below, I subtract 1 to 0 base it.
			medialElement = sortedArray.GetSize() / 2;
			median = sortedArray.GetAt(medialElement);
			highValue = sortedArray.GetAt(sortedArray.GetSize() - 1);
			lowValue = sortedArray.GetAt(0);
			div_t div2Result;
			div2Result = div(medialElement, 2);
			if(div2Result.rem == 0)	//even 
			{
				upperQuartile = sortedArray.GetAt(medialElement + (medialElement / 2)) +
									(sortedArray.GetAt(medialElement + (medialElement / 2) + 1) -
									sortedArray.GetAt(medialElement + (medialElement / 2))) / 2;
				lowerQuartile = sortedArray.GetAt((medialElement - 1) / 2) +
									((sortedArray.GetAt((medialElement + 1) / 2) -
									sortedArray.GetAt((medialElement - 1) / 2)) / 2);
			}
			else	//odd
			{
				upperQuartile = sortedArray.GetAt(medialElement + 1 + (medialElement / 2));
				lowerQuartile = sortedArray.GetAt(medialElement / 2);
			}
		}

		//data has been computed for median, high, low, and interquartile range
		//now we can draw the series

		if(graphAlignment)
		{
			seriesSpace = xAxisWidth / graphSeries->GetCount();

			barWidth = (int)(seriesSpace * .8);

			double tickScale = 0.00;
			tickScale = (yAxisHeight - 100) / numTicks;	//this must be consistent with PrintAxis()

			tickXLocation = xApexPoint + ((x * seriesSpace) - (seriesSpace / 2));

			barL = tickXLocation - (barWidth / 2);
			barR = barL + barWidth;
			double dataScale = 0.00;
			dataScale = ((yAxisHeight - 60) * 1.00) / (maxTick - minTick);

			//top cross bar (max value)
			barHeight = (int)((highValue * 1.00) * tickScale);
			barT = yApexPoint + barHeight;
			barB = barT - 60;
			pDC->Rectangle(barL, barT, barR, barB);

			//bottom cross bar (min value)
			barHeight = (int)((lowValue * 1.00) * tickScale);
			barT = yApexPoint + barHeight;
			barB = barT + 60;
			pDC->Rectangle(barL, barT, barR, barB);

			//vertical line (whisker)
			barHeight = (int)((highValue * 1.00) * tickScale);
			barT = yApexPoint + barHeight;
			pDC->Rectangle(tickXLocation - 40, barT, tickXLocation + 40, barB);

			//box (interquartile range, from upper quartile to lower quartile)
			barHeight = (int)(upperQuartile * tickScale);
			barT = yApexPoint + barHeight;
			barHeight = (int)(lowerQuartile * tickScale);
			barB = yApexPoint + barHeight;
			pDC->Rectangle(barL, barT, barR, barB);

			//draw median line (in RED)
			CPen* pOldPen;
			CPen linePen (PS_SOLID, 1, RED);
			pOldPen = pDC->SelectObject(&linePen);
			barHeight = (int)((median * 1.00) * tickScale);
			pDC->MoveTo(barL, yApexPoint + barHeight);
			pDC->LineTo(barR, yApexPoint + barHeight);
			pDC->SelectObject(pOldPen);
		}
		else
		{
			seriesSpace = yAxisHeight / graphSeries->GetCount();

			barHeight = (int)(seriesSpace * .8);

			double tickScale = 0.00;
			tickScale = (xAxisWidth - 200) / numTicks;	//same as PrintAxis()

			tickYLocation = yApexPoint + ((x * seriesSpace) - (seriesSpace / 2));

			barT = tickYLocation + (barHeight / 2);
			barB = barT - barHeight;
			double dataScale = 0.00;
			dataScale = ((xAxisWidth - 60) * 1.00) / (maxTick - minTick);

			//top cross bar (max value)
			barWidth = (int)((highValue * 1.00) * tickScale);
			barR = xApexPoint + barWidth;
			barL = barR - 60;
			pDC->Rectangle(barL, barT, barR, barB);

			//bottom cross bar (min value)
			barWidth = (int)((lowValue * 1.00) * tickScale);
			barR = xApexPoint + barWidth;
			barL = barR + 60;
			pDC->Rectangle(barL, barT, barR, barB);

			//vertical line (whisker)
			barWidth = (int)((highValue * 1.00) * tickScale);
			barR = xApexPoint + barWidth;
			pDC->Rectangle(barL, tickYLocation + 40, barR, tickYLocation - 40);

			//box (interquartile range, from upper quartile to lower quartile)
			barWidth = (int)(upperQuartile * tickScale);
			barL = xApexPoint + barWidth;
			barWidth = (int)(lowerQuartile * tickScale);
			barR = xApexPoint + barWidth;
			pDC->Rectangle(barL, barT, barR, barB);

			//draw median line (in RED)
			CPen* pOldPen;
			CPen linePen (PS_SOLID, 1, RED);
			pOldPen = pDC->SelectObject(&linePen);
			barWidth = (int)((median * 1.00) * tickScale);
			pDC->MoveTo(xApexPoint + barWidth, barT);
			pDC->LineTo(xApexPoint + barWidth, barB);
			pDC->SelectObject(pOldPen);
		}
	}

	pDC->SelectObject(pOldBrush);
}

void CGraph::PrintStackedBarSeries(CDC* pDC)
{
	int barWidth;
	int barL, barT, barR, barB;
	int tickXLocation, tickYLocation;
	int seriesSpace;
	int barHeight;  //for scalability
	POSITION pos;
	CGraphSeries* tmpSeries;

	if(graphAlignment)
	{
		//determine width of barchart data blocks
		seriesSpace = xAxisWidth / graphSeries->GetCount();
		barWidth = (int)(seriesSpace * .6);

		pos = graphSeries->GetHeadPosition();

		double dataScale = 0.00;
		dataScale = ((yAxisHeight - 100) * 1.00) / (maxTick - minTick);
		for(int x = 1; x <= graphSeries->GetCount(); x++)
		{
			tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

			tickXLocation = xApexPoint + ((x * seriesSpace) - (seriesSpace / 2));
			
			barT = yApexPoint;	//this is a running total;
								//it starts at yApexPoint so barB can
								//start there, then moves up by barHeight
			for(int s = 0; s < seriesSize; s++)
			{
				if(tmpSeries->GetData(s) > -1)
				{
					barHeight = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
					if(((tmpSeries->GetData(s) - minTick) > 0) && (barHeight < 1))
						barHeight = (int)dataScale;

					barB = barT;
					barL = tickXLocation - (barWidth / 2);
					if(barHeight > yAxisHeight)
						barT = barB + yAxisHeight;
					else
						barT = barB + barHeight;
					barR = barL + barWidth;

					COLORREF barColor;
					barColor = GetColor(s);
					CBrush brush (barColor);
					CBrush* pOldBrush;
					pOldBrush = pDC->SelectObject(&brush);
					pDC->Rectangle(barL, barT, barR, barB);
					pDC->SelectObject(pOldBrush);
				}
			}
		}
	}
	else
	{
		//determine width of barchart data blocks
		seriesSpace = yAxisHeight / graphSeries->GetCount();
		barHeight = (int)(seriesSpace * .6);

		pos = graphSeries->GetHeadPosition();

		double dataScale = 0.00;
		dataScale = ((xAxisWidth - 100) * 1.00) / (maxTick - minTick);
		for(int y = 1; y <= graphSeries->GetCount(); y++)
		{
			tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

			tickYLocation = yApexPoint + ((y * seriesSpace) - (seriesSpace / 2));

			barR = xApexPoint;	//this is a running total;
								//it starts at xApexPoint so barL can
								//start there, then moves right by barWidth
			for(int s = 0; s < seriesSize; s++)
			{
				if(tmpSeries->GetData(s) > -1)
				{

					barWidth = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
					if(((tmpSeries->GetData(s) - minTick) > 0) && (barWidth < 1))
						barWidth = (int)dataScale;

					barL = barR;
					barT = tickYLocation + (barHeight / 2);
					barB = barT - barHeight;
					if(barWidth > xAxisWidth)
						barR = barL + xAxisWidth;
					else
						barR = barL + barWidth;
					
					COLORREF barColor;
					barColor = GetColor(s);
					CBrush brush (barColor);
					CBrush* pOldBrush;
					pOldBrush = pDC->SelectObject(&brush);
					pDC->Rectangle(barL, barT, barR, barB);
					pDC->SelectObject(pOldBrush);
				}
			}
		}
	}
}

void CGraph::PrintXYLineSeries(CDC* pDC)
{
	//same as Line graph, but without dots at the values
	int barWidth;
	int dataPlotSize;   //used to plot rects of data
	int tickXLocation, tickYLocation;
	int seriesSpace;
	int barHeight;  //for scalability
	POSITION pos;
	CGraphSeries* tmpSeries;

	int lastXLoc, lastYLoc;
	if(graphAlignment)
	{
		for(int s = 0; s < seriesSize; s++)
		{
			//determine width of barchart data blocks
			seriesSpace = xAxisWidth / graphSeries->GetCount();
			barWidth = (int)((seriesSpace * .6) / seriesSize);
			dataPlotSize = seriesSize * barWidth;

			pos = graphSeries->GetHeadPosition();

			for(int x = 1; x <= graphSeries->GetCount(); x++)
			{
				tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

				if(tmpSeries->GetData(s) > -1)
				{
					tickXLocation = xApexPoint + ((x * seriesSpace) - (seriesSpace / 2));
					
					barHeight = 0;
					double dataScale = 0.00;
					
					if((maxTick - minTick) != yAxisHeight)
						dataScale = ((yAxisHeight * 1.00) / ((maxTick - minTick) * 1.00)) * tickSpace;	
					else dataScale = tickSpace * 15.00;

					int axisHeight = (int)(dataScale * numTicks);
					double barPercent = 0.00;
					barPercent = ((tmpSeries->GetData(s) - minTick) * 1.00) / maxTick;
					barHeight = (int)(barPercent * axisHeight);
					if(((tmpSeries->GetData(s) - minTick) > 0) && (barHeight < 1))
						barHeight = (int)dataScale;

					int yDataLocation = yApexPoint + barHeight;
					
					//now have x and y location of center of ellipse
					COLORREF barColor;
					barColor = GetColor(s);
					CBrush brush (barColor);
					CBrush* pOldBrush;
					pOldBrush = pDC->SelectObject(&brush);
					//draw line back to last data member
					if(x > 1)
					{
						CPen* pOldPen;
						CPen linePen (PS_SOLID, 40, barColor);
						pOldPen = pDC->SelectObject(&linePen);
						pDC->MoveTo(lastXLoc, lastYLoc);
						pDC->LineTo(tickXLocation, yDataLocation);
						pDC->SelectObject(pOldPen);
					}
					lastXLoc = tickXLocation;
					lastYLoc = yDataLocation;
					pDC->SelectObject(pOldBrush);
				}
			}
		}
	}
	else
	{
		for(int s = 0; s < seriesSize; s++)
		{
			//determine width of barchart data blocks
			seriesSpace = yAxisHeight / graphSeries->GetCount();
			barHeight = (int)((seriesSpace * .6) / seriesSize);
			dataPlotSize = seriesSize * barHeight;

			pos = graphSeries->GetHeadPosition();

			for(int x = 1; x <= graphSeries->GetCount(); x++)
			{
				tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

				if(tmpSeries->GetData(s) > -1)
				{
					tickYLocation = yApexPoint + ((x * seriesSpace) - (seriesSpace / 2));
					
					barWidth = 0;
					double dataScale = 0.00;

					dataScale = ((xAxisWidth - 100) * 1.00) / (maxTick - minTick);

					int axisWidth = (int)(dataScale * (maxTick - minTick));
					double barPercent = 0.00;
					barPercent = ((tmpSeries->GetData(s) - minTick) * 1.00) / ((maxTick - minTick) * 1.00);
					barWidth = (int)(barPercent * axisWidth);
					if(((tmpSeries->GetData(s) - minTick) > 0) && (barHeight < 1))
						barWidth = (int)dataScale;

					int xDataLocation = xApexPoint + barWidth;
					
					//now have x and y location of center of ellipse
					COLORREF barColor;
					barColor = GetColor(s);
					CBrush brush (barColor);
					CBrush* pOldBrush;
					pOldBrush = pDC->SelectObject(&brush);
					//draw line back to last data member
					if(x > 1)
					{
						CPen* pOldPen;
						CPen linePen (PS_SOLID, 40, barColor);
						pOldPen = pDC->SelectObject(&linePen);
						pDC->MoveTo(lastXLoc, lastYLoc);
						pDC->LineTo(xDataLocation, tickYLocation);
						pDC->SelectObject(pOldPen);
					}
					//now draw ellipse...
					lastYLoc = tickYLocation;
					lastXLoc = xDataLocation;
					pDC->SelectObject(pOldBrush);
				}
			}
		}
	}
}

//void CGraph::PrintRadarSeries(CDC* pDC)
//{
//}

void CGraph::Print3DBarSeries(CDC* pDC)
{
	int barWidth;
	int dataPlotSize;   //used to plot rects of data
	int barL, barT, barR, barB;
	int tickXLocation, tickYLocation;
	int seriesSpace;
	int barHeight;
	POSITION pos;
	CGraphSeries* tmpSeries;
	int blue;
	int red;
	int green;

	if(graphAlignment)
	{
		//determine width of barchart data blocks
		seriesSpace = (xAxisWidth - 100) / graphSeries->GetCount();
		barWidth = (int)((seriesSpace * .8) / seriesSize);
		dataPlotSize = seriesSize * barWidth;

		pos = graphSeries->GetHeadPosition();

		for(int x = 1; x <= graphSeries->GetCount(); x++)
		{
			tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

			tickXLocation = xApexPoint + ((x * seriesSpace) - (seriesSpace / 2));

			for(int s = 0; s < seriesSize; s++)
			{
				if(tmpSeries->GetData(s) > -1)
				{
					double dataScale = 0.00;
					dataScale = (yAxisHeight * 1.00) / (maxTick - minTick);
					int dScale;
					dScale = (int)dataScale;
					barHeight = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dScale);
					if(((tmpSeries->GetData(s) - minTick) > 0) && (barHeight < 1))
						barHeight = 1 * dScale;

					barL = tickXLocation - (dataPlotSize / 2) + (s * barWidth);
					if(barHeight > yAxisHeight)
						barT = yApexPoint + yAxisHeight;
					else
						barT = yApexPoint + barHeight;
					barR = barL + barWidth;
					barB = yApexPoint;
					
					COLORREF barColor;
					barColor = GetColor(s);
					CBrush brush (barColor);
					CPen rectPen (PS_SOLID, 1, barColor);
					CPen* pOldPen;
					pOldPen = pDC->SelectObject(&rectPen);
					CBrush* pOldBrush;
					pOldBrush = pDC->SelectObject(&brush);
					pDC->Rectangle(barL, barT, barR, barB);

					//now, we do the side
					//side is darker than front, so subtract
					//from color to make closer to black
					red = GetRValue(barColor);
					green = GetGValue(barColor);
					blue = GetBValue(barColor);
					int sideRed = red - 35;
					int sideGreen = green - 35;
					int sideBlue = blue - 35;
					if(sideRed < 0) sideRed = 0;
					if(sideGreen < 0) sideGreen = 0;
					if(sideBlue < 0) sideBlue = 0;
					COLORREF sideColor;
					sideColor = RGB(sideRed, sideGreen, sideBlue);
					CBrush sideBrush (sideColor);
					CPen sidePen (PS_SOLID, 1, sideColor);
					pDC->SelectObject(&sideBrush);
					pDC->SelectObject(&sidePen);
					CPoint sidePolyArray[4];
					CPoint sp1(barR, barT);
					CPoint sp2(barR, barB);
					CPoint sp3(barR + depth, barB + depth);
					CPoint sp4(barR + depth, barT + depth);
					sidePolyArray[0] = sp1;
					sidePolyArray[1] = sp2;
					sidePolyArray[2] = sp3;
					sidePolyArray[3] = sp4;

					pDC->Polygon(sidePolyArray, 4);
					
					//finally, the top
					int topRed = red + 35;
					int topGreen = green + 35;
					int topBlue = blue + 35;
					if(topRed > 255) topRed = 255;
					if(topGreen > 255) topGreen = 255;
					if(topBlue > 255) topBlue = 255;
					COLORREF topColor;
					topColor = RGB(topRed, topGreen, topBlue);
					CBrush topBrush (topColor);
					CPen topPen (PS_SOLID, 1, topColor);
					pDC->SelectObject(&topBrush);
					pDC->SelectObject(&topPen);
					CPoint topPolyArray[4];
					CPoint tp1(barL, barT);
					CPoint tp2(barR, barT);
					CPoint tp3(barR + depth, barT + depth);
					CPoint tp4(barL + depth, barT + depth);
					topPolyArray[0] = tp1;
					topPolyArray[1] = tp2;
					topPolyArray[2] = tp3;
					topPolyArray[3] = tp4;

					pDC->Polygon(topPolyArray, 4);

					pDC->SelectObject(pOldBrush);
					pDC->SelectObject(pOldPen);
				}
			}
		}
	}
	else
	{
		//determine width of barchart data blocks
		seriesSpace = yAxisHeight / graphSeries->GetCount();
		barHeight = (int)((seriesSpace * .8) / seriesSize);
		dataPlotSize = seriesSize * barHeight;

		pos = graphSeries->GetHeadPosition();

		for(int y = 1; y <= graphSeries->GetCount(); y++)
		{
			tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

			tickYLocation = yApexPoint + ((y * seriesSpace) - (seriesSpace / 2));

			for(int s = 0; s < seriesSize; s++)
			{
				if(tmpSeries->GetData(s) > -1)
				{
					double dataScale = 0.00;
					dataScale = ((xAxisWidth - 200) * 1.00) / (maxTick - minTick);
					barWidth = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
					if(((tmpSeries->GetData(s) - minTick) > 0) && (barWidth < 1))
						barWidth = (int)dataScale;

					barL = xApexPoint;
					barB = tickYLocation - (dataPlotSize / 2) + (s * barHeight);
					barT = barB + barHeight;
					if(barWidth > xAxisWidth)
						barR = xApexPoint + xAxisWidth;
					else
						barR = xApexPoint + barWidth;

					COLORREF barColor;
					barColor = GetColor(s);
					CBrush brush (barColor);
					CBrush* pOldBrush;
					pOldBrush = pDC->SelectObject(&brush);
					CPen rectPen (PS_SOLID, 1, barColor);
					CPen* pOldPen;
					pOldPen = pDC->SelectObject(&rectPen);
					pDC->Rectangle(barL, barT, barR, barB);

					//now, we do the side
					//side is darker than front, so subtract
					//from color to make closer to black
					red = GetRValue(barColor);
					green = GetGValue(barColor);
					blue = GetBValue(barColor);
					int sideRed = red - 35;
					int sideGreen = green - 35;
					int sideBlue = blue - 35;
					if(sideRed < 0) sideRed = 0;
					if(sideGreen < 0) sideGreen = 0;
					if(sideBlue < 0) sideBlue = 0;
					COLORREF sideColor;
					sideColor = RGB(sideRed, sideGreen, sideBlue);
					CBrush sideBrush (sideColor);
					CPen sidePen (PS_SOLID, 1, sideColor);
					pDC->SelectObject(&sideBrush);
					pDC->SelectObject(&sidePen);
					CPoint sidePolyArray[4];
					CPoint sp1(barL, barT);
					CPoint sp2(barR, barT);
					//I'm subtracting below to shorten the width of the 
					//bar, since it was showing farther right than actual value
					CPoint sp3(barR + depth - (seriesSize - s), barT + depth);
					CPoint sp4(barL + depth - (seriesSize - s), barT + depth);
					sidePolyArray[0] = sp1;
					sidePolyArray[1] = sp2;
					sidePolyArray[2] = sp3;
					sidePolyArray[3] = sp4;

					pDC->Polygon(sidePolyArray, 4);
					
					//finally, the top
					int topRed = red + 35;
					int topGreen = green + 35;
					int topBlue = blue + 35;
					if(topRed > 255) topRed = 255;
					if(topGreen > 255) topGreen = 255;
					if(topBlue > 255) topBlue = 255;
					COLORREF topColor;
					topColor = RGB(topRed, topGreen, topBlue);
					CBrush topBrush (topColor);
					CPen topPen (PS_SOLID, 1, topColor);
					pDC->SelectObject(&topBrush);
					pDC->SelectObject(&topPen);
					CPoint topPolyArray[4];
					CPoint tp1(barR, barT);
					CPoint tp2(barR, barB);
					//I'm subtracting below to shorten the width of the 
					//bar, since it was showing farther right than actual value
					CPoint tp3(barR + depth - (seriesSize - s), barB + depth);
					CPoint tp4(barR + depth - (seriesSize - s), barT + depth);
					topPolyArray[0] = tp1;
					topPolyArray[1] = tp2;
					topPolyArray[2] = tp3;
					topPolyArray[3] = tp4;

					pDC->Polygon(topPolyArray, 4);

					pDC->SelectObject(pOldBrush);
					pDC->SelectObject(pOldPen);
				}
			}
		}
	}
}

void CGraph::Print3DLineSeries(CDC* pDC)
{
	int barWidth;
	int dataPlotSize;   //used to plot rects of data
	int tickXLocation, tickYLocation;
	int seriesSpace;
	int barHeight;
	POSITION pos;
	CGraphSeries* tmpSeries;
	int thick;
	int blue;
	int red;
	int green;
	CBrush* pOldBrush;
	CPen* pOldPen;

	thick = depth / 5;

	int lastXLoc, lastYLoc;
	if(graphAlignment)
	{
		for(int s = 0; s < seriesSize; s++)
		{
			//determine width of barchart data blocks
			seriesSpace = (xAxisWidth - 100) / graphSeries->GetCount();

			pos = graphSeries->GetHeadPosition();
			int yDataLocation;

			int x = 1;
			for(x = 1; x <= graphSeries->GetCount(); x++)
			{
				tickXLocation = xApexPoint + ((x * seriesSpace) - (seriesSpace / 2));
				tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

				if(tmpSeries->GetData(s) > -1)
				{
					double dataScale = 0.00;
					dataScale = (yAxisHeight * 1.00) / (maxTick - minTick);
					int dScale;
					dScale = (int)dataScale;
					barHeight = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dScale);
					if(((tmpSeries->GetData(s) - minTick) > 0) && (barHeight < 1))
						barHeight = 1 * dScale;

					yDataLocation = yApexPoint + barHeight;
					//now have x and y location of center of ellipse
					COLORREF barColor;
					barColor = GetColor(s);
					red = GetRValue(barColor);
					green = GetGValue(barColor);
					blue = GetBValue(barColor);
					CBrush brush (barColor);
					pOldBrush = pDC->SelectObject(&brush);
					//draw line back to last data member
					if(x > 1)
					{
						CPen linePen (PS_SOLID, 1, barColor);
						pOldPen = pDC->SelectObject(&linePen);
						//front side of line
						CPoint sidePolyArray[4];
						CPoint sp1(lastXLoc, lastYLoc);
						CPoint sp2(lastXLoc, lastYLoc - thick);
						CPoint sp3(tickXLocation, yDataLocation - thick);
						CPoint sp4(tickXLocation, yDataLocation);
						sidePolyArray[0] = sp1;
						sidePolyArray[1] = sp2;
						sidePolyArray[2] = sp3;
						sidePolyArray[3] = sp4;
						pDC->Polygon(sidePolyArray, 4);

						//top part of line, lighter than front
						int topRed = red + 35;
						int topGreen = green + 35;
						int topBlue = blue + 35;
						if(topRed > 255) topRed = 255;
						if(topGreen > 255) topGreen = 255;
						if(topBlue > 255) topBlue = 255;
						COLORREF topColor;
						topColor = RGB(topRed, topGreen, topBlue);
						
						CPen topPen(PS_SOLID, 1, topColor);
						CBrush topBrush(topColor);
						pDC->SelectObject(&topPen);
						pDC->SelectObject(&topBrush);
						CPoint topPolyArray[4];
						CPoint tp1(lastXLoc, lastYLoc);
						CPoint tp2(lastXLoc + depth, lastYLoc + depth);
						CPoint tp3(tickXLocation + depth, yDataLocation + depth);
						CPoint tp4(tickXLocation, yDataLocation);
						topPolyArray[0] = tp1;
						topPolyArray[1] = tp2;
						topPolyArray[2] = tp3;
						topPolyArray[3] = tp4;
						pDC->Polygon(topPolyArray, 4);

						pDC->SelectObject(pOldBrush);
						pDC->SelectObject(pOldPen);
					}
					lastXLoc = tickXLocation;
					lastYLoc = yDataLocation;
				}
			}
			if(x > 1)
			{
				//end point of line
				int endRed = red - 35;
				int endGreen = green - 35;
				int endBlue = blue - 35;
				if(endRed < 0) endRed = 0;
				if(endGreen < 0) endGreen = 0;
				if(endBlue < 0) endBlue = 0;
				COLORREF endColor;
				endColor = RGB(endRed, endGreen, endBlue);
				
				CPen endPen(PS_SOLID, 1, endColor);
				CBrush endBrush(endColor);
				pDC->SelectObject(&endPen);
				pDC->SelectObject(&endBrush);
				CPoint endPolyArray[4];
				CPoint ep1(tickXLocation, yDataLocation);
				CPoint ep2(tickXLocation + depth, yDataLocation + depth);
				CPoint ep3(tickXLocation + depth, yDataLocation + depth - thick);
				CPoint ep4(tickXLocation, yDataLocation + thick);
				endPolyArray[0] = ep1;
				endPolyArray[1] = ep2;
				endPolyArray[2] = ep3;
				endPolyArray[3] = ep4;
				pDC->Polygon(endPolyArray, 4);

				pDC->SelectObject(pOldBrush);
				pDC->SelectObject(pOldPen);
			}
		}
	}
	else
	{
		for(int s = 0; s < seriesSize; s++)
		{
			//determine width of barchart data blocks
			seriesSpace = yAxisHeight / graphSeries->GetCount();
			barHeight = (int)((seriesSpace * .6) / seriesSize);
			dataPlotSize = seriesSize * barHeight;

			pos = graphSeries->GetHeadPosition();
			int xDataLocation;

			int x = 1;
			for(x = 1; x <= graphSeries->GetCount(); x++)
			{
				tickYLocation = yApexPoint + ((x * seriesSpace) - (seriesSpace / 2));
				tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

				if(tmpSeries->GetData(s) > -1)
				{
					double dataScale = 0.00;
					dataScale = ((xAxisWidth - 200) * 1.00) / (maxTick - minTick);
					barWidth = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
					if(((tmpSeries->GetData(s) - minTick) > 0) && (barWidth < 1))
						barWidth = (int)dataScale;
					
					xDataLocation = xApexPoint + barWidth;
					
					//now have x and y location of center of ellipse
					COLORREF barColor;
					barColor = GetColor(s);
					red = GetRValue(barColor);
					green = GetGValue(barColor);
					blue = GetBValue(barColor);
					CBrush brush (barColor);
					pOldBrush = pDC->SelectObject(&brush);
					//draw line back to last data member
					if(x > 1)
					{
						CPen linePen (PS_SOLID, 1, barColor);
						pOldPen = pDC->SelectObject(&linePen);
						//front side of line
						CPoint sidePolyArray[4];
						CPoint sp1(lastXLoc, lastYLoc);
						CPoint sp2(lastXLoc - thick, lastYLoc);
						CPoint sp3(xDataLocation - thick, tickYLocation);
						CPoint sp4(xDataLocation, tickYLocation);
						sidePolyArray[0] = sp1;
						sidePolyArray[1] = sp2;
						sidePolyArray[2] = sp3;
						sidePolyArray[3] = sp4;
						pDC->Polygon(sidePolyArray, 4);

						//top part of line, lighter than front
						int topRed = red + 35;
						int topGreen = green + 35;
						int topBlue = blue + 35;
						if(topRed > 255) topRed = 255;
						if(topGreen > 255) topGreen = 255;
						if(topBlue > 255) topBlue = 255;
						COLORREF topColor;
						topColor = RGB(topRed, topGreen, topBlue);
						
						CPen topPen(PS_SOLID, 1, topColor);
						CBrush topBrush(topColor);
						pDC->SelectObject(&topPen);
						pDC->SelectObject(&topBrush);
						CPoint topPolyArray[4];
						CPoint tp1(lastXLoc, lastYLoc);
						CPoint tp2(lastXLoc + depth, lastYLoc + thick);
						CPoint tp3(xDataLocation + depth, tickYLocation + thick);
						CPoint tp4(xDataLocation, tickYLocation);
						topPolyArray[0] = tp1;
						topPolyArray[1] = tp2;
						topPolyArray[2] = tp3;
						topPolyArray[3] = tp4;
						pDC->Polygon(topPolyArray, 4);

						pDC->SelectObject(pOldBrush);
						pDC->SelectObject(pOldPen);
					}
					lastYLoc = tickYLocation;
					lastXLoc = xDataLocation;
				}
			}
			if(x > 1)
			{
				//end point of line
				int endRed = red - 35;
				int endGreen = green - 35;
				int endBlue = blue - 35;
				if(endRed < 0) endRed = 0;
				if(endGreen < 0) endGreen = 0;
				if(endBlue < 0) endBlue = 0;
				COLORREF endColor;
				endColor = RGB(endRed, endGreen, endBlue);
				
				CPen endPen(PS_SOLID, 1, endColor);
				CBrush endBrush(endColor);
				pDC->SelectObject(&endPen);
				pDC->SelectObject(&endBrush);
				CPoint endPolyArray[4];
				CPoint ep1(xDataLocation, tickYLocation);
				CPoint ep2(xDataLocation + depth, tickYLocation + thick);
				CPoint ep3(xDataLocation + depth - thick, tickYLocation + thick);
				CPoint ep4(xDataLocation - thick, tickYLocation);
				endPolyArray[0] = ep1;
				endPolyArray[1] = ep2;
				endPolyArray[2] = ep3;
				endPolyArray[3] = ep4;
				pDC->Polygon(endPolyArray, 4);

				pDC->SelectObject(pOldBrush);
				pDC->SelectObject(pOldPen);
			}
		}
	}
}

void CGraph::Print3DPieSeries(CDC* pDC)
{
	double dataSum = 0.00;  //for storing cumulative sum
	double labelData = 0.00;
	int lastXLocation, lastYLocation;
	int newXLocation, newYLocation;
	int labelXLocation, labelYLocation;
	double percent = 0.00;
	double labelPercent = 0.00;
	int degrees;
	int labelDegrees;
	double totalSum = 0.00;
	int radius;
	POSITION pos;
	CGraphSeries* tmpSeries;
	int seriesSpace;
	int labelLineXStart, labelLineYStart;
	int labelLineXEnd, labelLineYEnd;
	int maxLabelWidth;
	int maxLabelHeight;
	TEXTMETRIC tm;
	CPen* pOldPen;
	CBrush* pOldBrush;
		
	CPen tmpPen(PS_SOLID, 1, BLACK);
	CBrush tmpBrush(WHITE);
	pOldPen = pDC->SelectObject(&tmpPen);
	pOldBrush = pDC->SelectObject(&tmpBrush);

	//deltaX and deltaY will be based on distance from x and y
	//axis for the new endpoint of the pie.  These values can 
	//then be used to find the true distance between starting 
	//line and ending line of pie boundary.
	double deltaX, deltaY;
	double degreeRadians, degreeRadians2;
	double labelDeltaX, labelDeltaY;
	double labelDegreeRadians, labelDegreeRadians2;

	lastXLocation = 0;
	lastYLocation = 0;

	pDC->GetTextMetrics(&tm);
	maxLabelWidth = tm.tmMaxCharWidth + 10;
	maxLabelHeight = tm.tmHeight + 6;
	
	//pie labels will be stored in a list and drawn after entire pie
	//is completed.
	CObList *pieLabels;
	CGraphPieLabel *pieLabel;

	//determine width of pie display area
	if(xAxisWidth > yAxisHeight)
		seriesSpace = yAxisHeight / graphSeries->GetCount();
	else
		seriesSpace = xAxisWidth / graphSeries->GetCount();
	seriesSpace -= (3 * maxLabelWidth);  //allows text like 25%  (3 chars)

	//to plot a pie plus labels inside of series space, use the following :
	//(3 * radius) + (2 * label width) = series space 
	//so, 3*radius = series space - (2 * label width)
	// 1 radius = (series space - (2 * label width)) / 3
	radius = seriesSpace / 3;  //pie needs 2 radius, + 1 extra for line to labels = 3 for my divisor
	int depth = (int)(radius * depthRatio);	//for shadow pie

	int centerYPie = pGraphT - (yAxisHeight / 2) - 500 - depth;

	pos = graphSeries->GetHeadPosition();
	for(int x = 1; x <= graphSeries->GetCount(); x++)
	{
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
		pieLabels = new CObList();

		tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);
		totalSum = 0;
		for(int s = 0; s < seriesSize; s++)
			totalSum += tmpSeries->GetData(s);
		int pieLeft, pieRight;
		if(graphSeries->GetCount() == 1)
		{
			pieLeft = xApexPoint + (xAxisWidth / 2) - radius;
		}
		else
		{
			pieLeft = xApexPoint + 15 + ((x * 2 - 1) * ((xAxisWidth / graphSeries->GetCount()) / 2)) - radius;
		}
		pieRight = pieLeft + (2 * radius);
		CRect pieRect (pieLeft, 
				centerYPie + radius,
				pieRight, 
				centerYPie - radius);
		CRect shadowRect (pieLeft + depth,
				centerYPie + radius - depth,
				pieRight + depth,
				centerYPie - radius - depth);
		int centerXPie = pieLeft + radius;

		//plot series label
		pDC->TextOut(centerXPie - ((tmpSeries->GetLabel().GetLength() / 2) * ((xTickFontSize / 2) * 20)),
					centerYPie - radius - maxLabelHeight - 600 - depth, tmpSeries->GetLabel());

		int centerShadowXPie;
		int centerShadowYPie;

		//draw shadow pie first
		centerShadowYPie = centerYPie - depth;
		centerShadowXPie = centerXPie + depth;
		lastXLocation = centerShadowXPie - radius;
		lastYLocation = centerShadowYPie;

		int lastTopX = centerXPie - radius;
		int lastTopY = centerYPie;
		int newTopX, newTopY;
		dataSum = 0;
		int lastQuad = 0;
		CPoint lastSidePolyArray[4];
		COLORREF lastColor;
		CPoint spa1;
		CPoint spa2;
		CPoint spa3;
		CPoint spa4;
		int s = 0;
		for(s = 0; s < seriesSize; s++)
		{
			if(tmpSeries->GetData(s) > 0)
			{
				int seriesDataValue;
				seriesDataValue = tmpSeries->GetData(s);
				dataSum += seriesDataValue;
				percent = (dataSum / totalSum) * 100;
				degrees = (int)((360 * percent) / 100);

				//determine destination quadrant...
					//and set newXLocation and newYLocation values...
					//degrees / 90 will never exceed 4.
					//this can tell us the quadrant of destination
				int quadrant = degrees / 90;  //truncates decimal

				//using the law of sines to determine the deltas :
				//deltaX = radius * sin(90 - degrees)
				//deltaY = radius * sin(degrees)
				//we convert degrees into radians so sin() function works right
				//note :  in quad 1 and 3, we reverse the angle used to compute
				//			the deltas, since the triangle plots reversed.
				switch(quadrant)
				{
					case 0 : //this is the base quadrant, so no manipulation needed
							 degreeRadians = degrees * (3.14159 / 180);
							 degreeRadians2 = (90 - degrees) * (3.14159 / 180);
							 deltaX = radius * sin(degreeRadians2);
							 deltaY = radius * sin(degreeRadians);
							 newXLocation = (int)(centerShadowXPie - deltaX);
							 newYLocation = (int)(centerShadowYPie - deltaY);
							 newTopX = (int)(centerXPie - deltaX);
							 newTopY = (int)(centerYPie - deltaY);
							 break;
					case 1 : //bottom right quadrant, subtract 90 from angle
							 degreeRadians = (degrees - 90) * (3.14159 / 180);
							 degreeRadians2 = (90 - (degrees - 90)) * (3.14159 / 180);
							 deltaX = radius * sin(degreeRadians);
							 deltaY = radius * sin(degreeRadians2);
							 newXLocation = (int)(centerShadowXPie + deltaX);
							 newYLocation = (int)(centerShadowYPie - deltaY);
							 newTopX = (int)(centerXPie + deltaX);
							 newTopY = (int)(centerYPie - deltaY);
							 break;
					case 2 : //top right quadrant, subtract 180 from angle
							 degreeRadians = (degrees - 180) * (3.14159 / 180);
							 degreeRadians2 = (90 - (degrees - 180)) * (3.14159 / 180);
							 deltaX = radius * sin(degreeRadians2);
							 deltaY = radius * sin(degreeRadians);
							 newXLocation = (int)(centerShadowXPie + deltaX);
							 newYLocation = (int)(centerShadowYPie + deltaY);
							 newTopX = (int)(centerXPie + deltaX);
							 newTopY = (int)(centerYPie + deltaY);
							 break;
					case 3 : //upper left quadrant, subtract 270 from angle
							 degreeRadians = (degrees - 270) * (3.14159 / 180);
							 degreeRadians2 = (90 - (degrees - 270)) * (3.14159 / 180);
							 deltaX = radius * sin(degreeRadians);
							 deltaY = radius * sin(degreeRadians2);
							 newXLocation = (int)(centerShadowXPie - deltaX);
							 newYLocation = (int)(centerShadowYPie + deltaY);
							 newTopX = (int)(centerXPie - deltaX);
							 newTopY = (int)(centerYPie + deltaY);
							 break;
					case 4 : //straight line to left of center
							 deltaX = radius;
							 deltaY = 1;
							 newXLocation = (int)(centerShadowXPie - deltaX);
							 newYLocation = (int)(centerShadowYPie + deltaY);
							 newTopX = (int)(centerXPie - deltaX);
							 newTopY = (int)(centerYPie + deltaY);
							 break;
				}
				
				if(s == 0)
				{
					lastYLocation += 1;
					lastTopY += 1;
				}

				COLORREF barColor;
				barColor = GetColor(s);
				int red, green, blue;
				red = GetRValue(barColor) - 35;
				green = GetGValue(barColor) - 35;
				blue = GetBValue(barColor) - 35;
				if(red < 0) red = 0;
				if(green < 0) green = 0;
				if(blue < 0) blue = 0;
				CPoint sp1 (lastXLocation, lastYLocation);
				CPoint sp2 (newXLocation, newYLocation);
				COLORREF shadowColor(RGB(red, green, blue));
				CPen shadowPen (PS_SOLID, 1, shadowColor);
				CBrush shadowBrush(shadowColor);
				pDC->SelectObject(&shadowPen);
				pDC->SelectObject(&shadowBrush);
				if((quadrant < 2) || (lastQuad < 2))
				{
					CPoint sidePolyArray[4];
					spa1.x = lastXLocation;
					spa1.y = lastYLocation;
					spa2.x = newXLocation;
					spa2.y = newYLocation;
					spa3.x = newTopX;
					spa3.y = newTopY;
					spa4.x = lastTopX;
					spa4.y = lastTopY;
					sidePolyArray[0] = spa1;
					sidePolyArray[1] = spa2;
					sidePolyArray[2] = spa3;
					sidePolyArray[3] = spa4;
					pDC->Polygon(sidePolyArray, 4);
					pDC->Pie(shadowRect, sp1, sp2);
				}
				else
				{
					CPoint sidePolyArray[3];
					spa1.x = newXLocation;
					spa1.y = newYLocation;
					spa2.x = newTopX;
					spa2.y = newTopY;
					spa3.x = lastTopX;
					spa3.y = lastTopY;
					sidePolyArray[0] = spa1;
					sidePolyArray[1] = spa2;
					sidePolyArray[2] = spa3;
					CPen oldShadowPen (PS_SOLID, 1, lastColor);
					pDC->SelectObject(&oldShadowPen);
					CBrush oldShadowBrush(lastColor);
					pDC->SelectObject(&oldShadowBrush);
					pDC->Pie(shadowRect, sp1, sp2);
					pDC->Polygon(sidePolyArray, 3);
					pDC->SelectObject(&shadowPen);
					pDC->SelectObject(&shadowBrush);
				}

				lastXLocation = newXLocation;
				lastYLocation = newYLocation;
				lastTopX = newTopX;
				lastTopY = newTopY;
				if((quadrant > 1) && (lastQuad < 2))
				{
					lastSidePolyArray[0] = spa1;
					lastSidePolyArray[1] = spa2;
					lastSidePolyArray[2] = spa3;
					lastSidePolyArray[3] = spa4;
					lastColor = shadowColor;
				}
				lastQuad = quadrant;
				pDC->SelectObject(pOldBrush);
				pDC->SelectObject(pOldPen);
			}
		}

		//draw normal pie
		lastXLocation = pieLeft;
		lastYLocation = centerYPie;

		dataSum = 0;
		for(s = 0; s < seriesSize; s++)
		{
			if(tmpSeries->GetData(s) > 0)
			{
				int seriesDataValue;
				seriesDataValue = tmpSeries->GetData(s);
				labelData = seriesDataValue / 2;
				dataSum += seriesDataValue;
				percent = (dataSum / totalSum) * 100;
				labelPercent = ((dataSum - labelData) / totalSum) * 100;
				degrees = (int)((360 * percent) / 100);
				labelDegrees = (int)((360 * labelPercent) / 100);

				//determine destination quadrant...
					//and set newXLocation and newYLocation values...
					//degress / 90 will never exceed 4.
					//this can tell us the quadrant of destination
				int quadrant = degrees / 90;  //truncates decimal
				int labelQuadrant = labelDegrees / 90;

				//using the law of sines to determine the deltas :
				//deltaX = radius * sin(90 - degrees)
				//deltaY = radius * sin(degrees)
				//we convert degrees into radians so sin() function works right
				//note :  in quad 1 and 3, we reverse the angle used to compute
				//			the deltas, since the triangle plots reversed.
				switch(quadrant)
				{
					case 0 : //this is the base quadrant, so no manipulation needed
							 degreeRadians = degrees * (3.14159 / 180);
							 degreeRadians2 = (90 - degrees) * (3.14159 / 180);
							 deltaX = radius * sin(degreeRadians2);
							 deltaY = radius * sin(degreeRadians);
							 newXLocation = (int)(centerXPie - deltaX);
							 newYLocation = (int)(centerYPie - deltaY);
							 break;
					case 1 : //bottom right quadrant, subtract 90 from angle
							 degreeRadians = (degrees - 90) * (3.14159 / 180);
							 degreeRadians2 = (90 - (degrees - 90)) * (3.14159 / 180);
							 deltaX = radius * sin(degreeRadians);
							 deltaY = radius * sin(degreeRadians2);
							 newXLocation = (int)(centerXPie + deltaX);
							 newYLocation = (int)(centerYPie - deltaY);
							 break;
					case 2 : //top right quadrant, subtract 180 from angle
							 degreeRadians = (degrees - 180) * (3.14159 / 180);
							 degreeRadians2 = (90 - (degrees - 180)) * (3.14159 / 180);
							 deltaX = radius * sin(degreeRadians2);
							 deltaY = radius * sin(degreeRadians);
							 newXLocation = (int)(centerXPie + deltaX);
							 newYLocation = (int)(centerYPie + deltaY);
							 break;
					case 3 : //upper left quadrant, subtract 270 from angle
							 degreeRadians = (degrees - 270) * (3.14159 / 180);
							 degreeRadians2 = (90 - (degrees - 270)) * (3.14159 / 180);
							 deltaX = radius * sin(degreeRadians);
							 deltaY = radius * sin(degreeRadians2);
							 newXLocation = (int)(centerXPie - deltaX);
							 newYLocation = (int)(centerYPie + deltaY);
							 break;
					case 4 : //straight line to left of center
							 deltaX = radius;
							 deltaY = 1;
							 newXLocation = (int)(centerXPie - deltaX);
							 newYLocation = (int)(centerYPie + deltaY);
							 break;
				}
				
				switch(labelQuadrant)
				{
					//after getting X & Y location for label, we take
					//1/2 the delta between x y locations and center pie
					case 0 : 
							 labelDegreeRadians = labelDegrees * (3.14159 / 180);
							 labelDegreeRadians2 = (90 - labelDegrees) * (3.14159 / 180);
							 labelDeltaX = radius * sin(labelDegreeRadians2);
							 labelDeltaY = radius * sin(labelDegreeRadians);
							 labelXLocation = (int)(centerXPie - labelDeltaX);
							 labelYLocation = (int)(centerYPie - labelDeltaY);
							 labelLineXStart = labelXLocation + ((centerXPie - labelXLocation) / 2);
							 labelLineYStart = labelYLocation - ((labelYLocation - centerYPie) / 2);
							 labelLineXEnd = labelXLocation - (radius / 2);
							 labelLineYEnd = labelYLocation - (radius / 2);
							 break;
					case 1 : 
							 labelDegreeRadians = (labelDegrees - 90) * (3.14159 / 180);
							 labelDegreeRadians2 = (90 - (labelDegrees - 90)) * (3.14159 / 180);
							 labelDeltaX = radius * sin(labelDegreeRadians);
							 labelDeltaY = radius * sin(labelDegreeRadians2);
							 labelXLocation = (int)(centerXPie + labelDeltaX);
							 labelYLocation = (int)(centerYPie - labelDeltaY);
							 labelLineXStart = labelXLocation - ((labelXLocation - centerXPie) / 2);
							 labelLineYStart = labelYLocation - ((labelYLocation - centerYPie) / 2);
							 labelLineXEnd = labelXLocation + (radius / 2);
							 labelLineYEnd = labelYLocation - (radius / 2);
							 break;
					case 2 : 
							 labelDegreeRadians = (labelDegrees - 180) * (3.14159 / 180);
							 labelDegreeRadians2 = (90 - (labelDegrees - 180)) * (3.14159 / 180);
							 labelDeltaX = radius * sin(labelDegreeRadians2);
							 labelDeltaY = radius * sin(labelDegreeRadians);
							 labelXLocation = (int)(centerXPie + labelDeltaX);
							 labelYLocation = (int)(centerYPie + labelDeltaY);
							 labelLineXStart = labelXLocation - ((labelXLocation - centerXPie) / 2);
							 labelLineYStart = labelYLocation - ((labelYLocation - centerYPie) / 2);
							 labelLineXEnd = labelXLocation + (radius / 2);
							 labelLineYEnd = labelYLocation + (radius / 2);
							 break;
					case 3 : 
							 labelDegreeRadians = (labelDegrees - 270) * (3.14159 / 180);
							 labelDegreeRadians2 = (90 - (labelDegrees - 270)) * (3.14159 / 180);
							 labelDeltaX = radius * sin(labelDegreeRadians);
							 labelDeltaY = radius * sin(labelDegreeRadians2);
							 labelXLocation = (int)(centerXPie - labelDeltaX);
							 labelYLocation = (int)(centerYPie + labelDeltaY);
							 labelLineXStart = labelXLocation + ((centerXPie - labelXLocation) / 2);
							 labelLineYStart = labelYLocation - ((labelYLocation - centerYPie) / 2);
							 labelLineXEnd = labelXLocation - (radius / 2);
							 labelLineYEnd = labelYLocation + (radius / 2);
							 break;
//there should never be a half point ending on 4, so leave it out
				}

				pieLabel = new CGraphPieLabel();

				pieLabel->lineXStart = labelLineXStart;
				pieLabel->lineYStart = labelLineYStart;
				pieLabel->lineXEnd = labelLineXEnd;
				pieLabel->lineYEnd = labelLineYEnd;
				switch(labelQuadrant)
				{
					case 0 : //label to left of line
							 pieLabel->topLeftX = labelLineXEnd - maxLabelWidth;
							 pieLabel->topLeftY = labelLineYEnd;
							 break;
					case 1 : //label to right of line
							 pieLabel->topLeftX = labelLineXEnd + 50;
							 pieLabel->topLeftY = labelLineYEnd;
							 break;
					case 2 : //label to right of line
							 pieLabel->topLeftX = labelLineXEnd + 50;
							 pieLabel->topLeftY = labelLineYEnd + maxLabelHeight;
							 break;
					case 3 : //label to left of line
							 pieLabel->topLeftX = labelLineXEnd - maxLabelWidth;
							 pieLabel->topLeftY = labelLineYEnd + maxLabelHeight;
							 break;
				}
				pieLabel->labelQuadrant = labelQuadrant;
				int roundPercent;
				roundPercent = (int)((seriesDataValue * 100) / totalSum);
				pieLabel->pieLabel.Format(_T("%d%%"), roundPercent);
				pieLabels->AddTail(pieLabel);

				if(s == 0)
					lastYLocation += 1;

				COLORREF barColor;
				barColor = GetColor(s);
				CPoint p1 (lastXLocation, lastYLocation);
				CPoint p2 (newXLocation, newYLocation);
				CBrush brush (barColor);
				CPen piePen (PS_SOLID, 1, barColor);
				pDC->SelectObject(&piePen);
				pDC->SelectObject(&brush);
				pDC->Pie(pieRect, p1, p2); 

				lastXLocation = newXLocation;
				lastYLocation = newYLocation;
			}
			pDC->SelectObject(pOldBrush);
			pDC->SelectObject(pOldPen);
		}

		//draw lines and labels for pies slices
		POSITION labelPos;

		CBrush lineBrush (BLACK);
		CPen linePen (PS_SOLID, 1, BLACK);
		pDC->SelectObject(&lineBrush);
		pDC->SelectObject(&linePen);
		POSITION mainLinePos, checkLinePos;
		mainLinePos = pieLabels->GetHeadPosition();
		int numLinesDrawn = 0;
		CGraphPieLabel* currentLabel;
		CGraphPieLabel* tmpLabel;
		while(mainLinePos != NULL)
		{
			currentLabel = (CGraphPieLabel*)pieLabels->GetNext(mainLinePos);
			int r = 0;
			checkLinePos = pieLabels->GetHeadPosition();
			while(r < numLinesDrawn)
			{
				//check if any overlap in label areas
				tmpLabel = (CGraphPieLabel*)pieLabels->GetAt(checkLinePos);
				if((currentLabel->topLeftX > tmpLabel->topLeftX) &&
					(currentLabel->topLeftX < (tmpLabel->topLeftX - maxLabelWidth)) &&
					(currentLabel->topLeftY > tmpLabel->topLeftY) &&
					(currentLabel->topLeftY < (tmpLabel->topLeftY - maxLabelHeight)))
				{
					//OVERLAP !!!
					//move current label top left position up or down
					//depending on its quadrant
					if(currentLabel->labelQuadrant < 2)
					{
						//move label down to tmplabel topleft + height
						currentLabel->topLeftY = tmpLabel->topLeftY - maxLabelHeight;
						currentLabel->lineYEnd = tmpLabel->lineYEnd - maxLabelHeight;
					}
					else
					{
						//move label up to tmpLabel topleft - height
						currentLabel->topLeftY = tmpLabel->topLeftY + maxLabelHeight;
						currentLabel->lineYEnd = tmpLabel->lineYEnd + maxLabelHeight;
					}
					//reset r value to 0 so it starts over, just in
					//case we moved the label and it overlaps another
					r = 0;
					checkLinePos = pieLabels->GetHeadPosition();
				}
				else
				{
					r++;
					pieLabels->GetNext(checkLinePos);
				}

			}
			//draw the line and label
			pDC->MoveTo(currentLabel->lineXStart, currentLabel->lineYStart);
			pDC->LineTo(currentLabel->lineXEnd, currentLabel->lineYEnd);

			//write the label
			pDC->TextOut(currentLabel->topLeftX, currentLabel->topLeftY,
					currentLabel->pieLabel);
			numLinesDrawn++;
		}

		//now done, remove everything inside the label list
		labelPos = pieLabels->GetHeadPosition();
		while(labelPos != NULL)
		{
			pieLabel = (CGraphPieLabel*)pieLabels->GetNext(labelPos);
			delete pieLabel;
		}
		delete pieLabels;
	}
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
}

void CGraph::Print3DStackedBarSeries(CDC* pDC)
{
	int barWidth;
	int barL, barT, barR, barB;
	int tickXLocation, tickYLocation;
	int seriesSpace;
	int barHeight;
	POSITION pos;
	CGraphSeries* tmpSeries;
	int blue;
	int red;
	int green;

	if(graphAlignment)
	{
		//determine width of barchart data blocks
		seriesSpace = xAxisWidth  / graphSeries->GetCount();
		barWidth = (int)(seriesSpace * .8);

		pos = graphSeries->GetHeadPosition();

		double dataScale = 0.00;
		dataScale = (yAxisHeight * 1.00) / (maxTick - minTick);
		for(int x = 1; x <= graphSeries->GetCount(); x++)
		{
			tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

			tickXLocation = xApexPoint + ((x * seriesSpace) - (seriesSpace / 2));

			barT = yApexPoint;	//this is a running total;
								//it starts at yApexPoint so barB can
								//start there, then moves up by barHeight
			for(int s = 0; s < seriesSize; s++)
			{
				if(tmpSeries->GetData(s) > -1)
				{
					barHeight = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
					if(((tmpSeries->GetData(s) - minTick) > 0) && (barHeight < 1))
						barHeight = (int)dataScale;

					barB = barT;
					barL = tickXLocation - (barWidth / 2);
					if(barHeight > yAxisHeight)
						barT = barB + yAxisHeight;
					else
						barT = barB + barHeight;
					barR = barL + barWidth;

					COLORREF barColor;
					barColor = GetColor(s);
					CBrush brush (barColor);
					CPen rectPen (PS_SOLID, 1, barColor);
					CPen* pOldPen;
					pOldPen = pDC->SelectObject(&rectPen);
					CBrush* pOldBrush;
					pOldBrush = pDC->SelectObject(&brush);
					pDC->Rectangle(barL, barT, barR, barB);

					//now, we do the side
					//side is darker than front, so subtract
					//from color to make closer to black
					red = GetRValue(barColor);
					green = GetGValue(barColor);
					blue = GetBValue(barColor);
					int sideRed = red - 35;
					int sideGreen = green - 35;
					int sideBlue = blue - 35;
					if(sideRed < 0) sideRed = 0;
					if(sideGreen < 0) sideGreen = 0;
					if(sideBlue < 0) sideBlue = 0;
					COLORREF sideColor;
					sideColor = RGB(sideRed, sideGreen, sideBlue);
					CBrush sideBrush (sideColor);
					CPen sidePen (PS_SOLID, 1, sideColor);
					pDC->SelectObject(&sideBrush);
					pDC->SelectObject(&sidePen);
					CPoint sidePolyArray[4];
					CPoint sp1(barR, barT);
					CPoint sp2(barR, barB);
					CPoint sp3(barR + depth, barB + depth);
					CPoint sp4(barR + depth, barT + depth);
					sidePolyArray[0] = sp1;
					sidePolyArray[1] = sp2;
					sidePolyArray[2] = sp3;
					sidePolyArray[3] = sp4;

					pDC->Polygon(sidePolyArray, 4);
					
					//finally, the top
					int topRed = red + 35;
					int topGreen = green + 35;
					int topBlue = blue + 35;
					if(topRed > 255) topRed = 255;
					if(topGreen > 255) topGreen = 255;
					if(topBlue > 255) topBlue = 255;
					COLORREF topColor;
					topColor = RGB(topRed, topGreen, topBlue);
					CBrush topBrush (topColor);
					CPen topPen (PS_SOLID, 1, topColor);
					pDC->SelectObject(&topBrush);
					pDC->SelectObject(&topPen);
					CPoint topPolyArray[4];
					CPoint tp1(barL, barT);
					CPoint tp2(barR, barT);
					CPoint tp3(barR + depth, barT + depth);
					CPoint tp4(barL + depth, barT + depth);
					topPolyArray[0] = tp1;
					topPolyArray[1] = tp2;
					topPolyArray[2] = tp3;
					topPolyArray[3] = tp4;

					pDC->Polygon(topPolyArray, 4);

					pDC->SelectObject(pOldBrush);
					pDC->SelectObject(pOldPen);
				}
			}
		}
	}
	else
	{
		//determine width of barchart data blocks
		seriesSpace = yAxisHeight / graphSeries->GetCount();
		barHeight = (int)(seriesSpace * .8);

		pos = graphSeries->GetHeadPosition();

		double dataScale = 0.00;
		dataScale = ((xAxisWidth - 10) * 1.00) / (maxTick - minTick);
		for(int y = 1; y <= graphSeries->GetCount(); y++)
		{
			tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

			tickYLocation = yApexPoint + ((y * seriesSpace) - (seriesSpace / 2));

			barR = xApexPoint;	//this is a running total;
								//it starts at xApexPoint so barL can
								//start there, then moves right by barWidth
			for(int s = 0; s < seriesSize; s++)
			{
				if(tmpSeries->GetData(s) > -1)
				{

					barWidth = (int)(((tmpSeries->GetData(s) - minTick) * 1.00) * dataScale);
					if(((tmpSeries->GetData(s) - minTick) > 0) && (barWidth < 1))
						barWidth = (int)dataScale;

					barL = barR;
					barT = tickYLocation + (barHeight / 2);
					barB = barT - barHeight;
					if(barWidth > xAxisWidth)
						barR = barL + xAxisWidth;
					else
						barR = barL + barWidth;

					COLORREF barColor;
					barColor = GetColor(s);
					CBrush brush (barColor);
					CBrush* pOldBrush;
					pOldBrush = pDC->SelectObject(&brush);
					CPen rectPen (PS_SOLID, 1, barColor);
					CPen* pOldPen;
					pOldPen = pDC->SelectObject(&rectPen);
					pDC->Rectangle(barL, barT, barR, barB);

					//now, we do the side
					//side is darker than front, so subtract
					//from color to make closer to black
					red = GetRValue(barColor);
					green = GetGValue(barColor);
					blue = GetBValue(barColor);
					int sideRed = red - 35;
					int sideGreen = green - 35;
					int sideBlue = blue - 35;
					if(sideRed < 0) sideRed = 0;
					if(sideGreen < 0) sideGreen = 0;
					if(sideBlue < 0) sideBlue = 0;
					COLORREF sideColor;
					sideColor = RGB(sideRed, sideGreen, sideBlue);
					CBrush sideBrush (sideColor);
					CPen sidePen (PS_SOLID, 1, sideColor);
					pDC->SelectObject(&sideBrush);
					pDC->SelectObject(&sidePen);
					CPoint sidePolyArray[4];
					CPoint sp1(barL, barT);
					CPoint sp2(barR, barT);
					CPoint sp3(barR + depth, barT + depth);
					CPoint sp4(barL + depth, barT + depth);
					sidePolyArray[0] = sp1;
					sidePolyArray[1] = sp2;
					sidePolyArray[2] = sp3;
					sidePolyArray[3] = sp4;

					pDC->Polygon(sidePolyArray, 4);
					
					//finally, the top
					int topRed = red + 35;
					int topGreen = green + 35;
					int topBlue = blue + 35;
					if(topRed > 255) topRed = 255;
					if(topGreen > 255) topGreen = 255;
					if(topBlue > 255) topBlue = 255;
					COLORREF topColor;
					topColor = RGB(topRed, topGreen, topBlue);
					CBrush topBrush (topColor);
					CPen topPen (PS_SOLID, 1, topColor);
					pDC->SelectObject(&topBrush);
					pDC->SelectObject(&topPen);
					CPoint topPolyArray[4];
					CPoint tp1(barR, barT);
					CPoint tp2(barR, barB);
					CPoint tp3(barR + depth, barB + depth);
					CPoint tp4(barR + depth, barT + depth);
					topPolyArray[0] = tp1;
					topPolyArray[1] = tp2;
					topPolyArray[2] = tp3;
					topPolyArray[3] = tp4;

					pDC->Polygon(topPolyArray, 4);

					pDC->SelectObject(pOldBrush);
					pDC->SelectObject(pOldPen);
				}
			}
		}
	}
}

void CGraph::RemoveSeries(CString label)
{
	POSITION pos;
	CGraphSeries* pSeries;
	pos = graphSeries->GetHeadPosition();
	while(pos != NULL)
	{
		pSeries = (CGraphSeries*)graphSeries->GetAt(pos);
		if(pSeries->GetLabel().Compare(label) == 0)
		{
			graphSeries->RemoveAt(pos);
			delete pSeries;
			break;
		}
		graphSeries->GetNext(pos);
	}
}

void CGraph::RemoveColor(int dataGroup)
{
	POSITION pos;
	CGraphDataColor* dataColor;
	pos = colorList->GetHeadPosition();
	for(int i = 0; i < dataGroup; i++)
	{
		dataColor = (CGraphDataColor*)colorList->GetNext(pos);
	}
	colorList->RemoveAt(pos);
	delete dataColor;
}

void CGraph::RemoveLegend(int dataGroup)
{
	graphLegend.Remove(dataGroup);
}

void CGraph::RemoveAllSeries()
{
	POSITION pos;
	CGraphSeries* pSeries;
	pos = graphSeries->GetHeadPosition();
	while(pos != NULL)
	{
		pSeries = (CGraphSeries*) graphSeries->GetAt( pos );
		graphSeries->RemoveAt(pos);
		delete pSeries;
	}

	graphSeries->RemoveAll();
}

void CGraph::RemoveAllColors()
{
	POSITION pos;
	CGraphDataColor* dataColor;
	pos = colorList->GetHeadPosition();
	while(pos != NULL)
	{
		dataColor = (CGraphDataColor*) colorList->GetAt( pos );
		colorList->RemoveAt(pos);
		delete dataColor;
	}

	colorList->RemoveAll();
}

void CGraph::RemoveAllLegends()
{
	graphLegend.RemoveAll();
}

void CGraph::RemoveAllData()
{
	RemoveAllSeries();
	RemoveAllLegends();
	RemoveAllColors();
}

void CGraph::Set3DDepthRatio(double ratio)
{
	depthRatio = ratio;
}

void CGraph::SetGraphQuadType(int quads)
{
	if(graphType == BAR_GRAPH)
	{
		if((quads < 1) || (quads > 4))
			graphQuadType = 1;
		else
		{
			graphQuadType = quads;
			quadSetManually = TRUE;
		}
	}
}

void CGraph::Set3DLineBase(int x, int y)
{
	line3DXBase = x;
	line3DYBase = y;
}