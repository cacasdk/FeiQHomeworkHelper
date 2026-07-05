// FeiQVerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FeiQVersionView.h"
#include "FeiQVerDlg.h"
#include "afxtempl.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFeiQVerDlg dialog


CFeiQVerDlg::CFeiQVerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFeiQVerDlg::IDD, pParent)
{
	m_Graph = NULL;
	//{{AFX_DATA_INIT(CFeiQVerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFeiQVerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFeiQVerDlg)
	DDX_Control(pDX, IDC_STATIC_PIE, m_ctrlGraphFrame);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFeiQVerDlg, CDialog)
	//{{AFX_MSG_MAP(CFeiQVerDlg)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFeiQVerDlg message handlers

BOOL CFeiQVerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	IFQUiPtr UiPtr(CLSID_FQUi);
	UiPtr->SkinDialog((LONG)this->m_hWnd, NULL, NULL, NULL, NULL);


	CStringArray verInfoArr;
	CArray<int, int> sortArr;
	CArray<int, int> numArr;
	int nUserCnt = 0;
	if(m_ptrRoot)
	{
		CComBSTR userName, hostName, IpValue, MacValue;
	
		IFQBuddyCollectionPtr pFQBuddysCol = m_ptrRoot->OnlineBuddys;
		if(pFQBuddysCol)
		{
			for(int i = 0; i < pFQBuddysCol->Count; i++)
			{
				IFQBuddyPtr pFQBuddy = pFQBuddysCol->Item[i];
				if(pFQBuddy)
				{
					CString strVersion = (LPCTSTR)pFQBuddy->Version;
					int nSort = 0;
					CString ver = GetVersionStr(strVersion, nSort);
					BOOL bFind = FALSE;
					for(int t = 0; t < verInfoArr.GetSize(); t++)
					{
						if(verInfoArr[t] == ver)
						{
							numArr[t]++;
							bFind = TRUE;
							break;
						}
					}
					if(!bFind)
					{
						verInfoArr.Add(ver);
						sortArr.Add(nSort);
						numArr.Add(1);
					}
					nUserCnt++;
				}
			}
		}
	}

	if(sortArr.GetSize() == 0 || nUserCnt == 0) return TRUE;
	//ÅÅÐò
	int k = 0;
	for(k = 0; k < sortArr.GetSize() - 1; k++)
	{
		for(int t = k + 1; t < sortArr.GetSize(); t++)
		{
			if(sortArr[k] > sortArr[t])
			{
				CString strTemp = verInfoArr[k];
				verInfoArr[k] = verInfoArr[t];
				verInfoArr[t] = strTemp;

				int nTemp = sortArr[k];
				sortArr[k] = sortArr[t];
				sortArr[t] = nTemp;


				nTemp = numArr[k];
				numArr[k] = numArr[t];
				numArr[t] = nTemp;
			}
		}
	}
	m_Graph = new CGraph(PIE_GRAPH_3D);
	CString strTitle;
	strTitle.Format(_T("·ÉÇïÓÃ»§°æ±¾±ý×´Í¼[×Ü¹²%d¸öÓÃ»§]"), nUserCnt);
	m_Graph->SetGraphTitle(strTitle);
	m_Graph->Set3DDepthRatio(0.1);
	
	//set up legend
	int nIndex = 0;
	for(k = 0; k < sortArr.GetSize(); k++)
	{
		CString strTip;
		strTip.Format(_T("%s---%d¸öÓÃ»§(Õ¼%d"), verInfoArr[k], numArr[k], (int)((double)numArr[k] * 100.0/ (double)nUserCnt));
		strTip += _T("%)");
		m_Graph->SetLegend(nIndex++, strTip);
	}

	//set up some series
	CGraphSeries* series1 = new CGraphSeries();
	series1->SetLabel("");
	nIndex = 0;
	for(k = 0; k < sortArr.GetSize(); k++)
	{
		series1->SetData(nIndex++, (int)((double)numArr[k] * 100.0/ (double)nUserCnt));
	}


	m_Graph->AddSeries(series1);

	//set the colors of my bars
	m_Graph->SetColor(0, FOREST_GREEN);
	m_Graph->SetColor(1, SKY_BLUE);
	m_Graph->SetColor(2, DUSK);
	m_Graph->SetColor(3, HOT_PINK);
	m_Graph->SetColor(4, LAVENDER);
	m_Graph->SetColor(5, ROYAL_BLUE);
	m_Graph->SetColor(6, BROWN);
	m_Graph->SetColor(7, MAROON);
	m_Graph->SetColor(8, GREY);
	m_Graph->SetColor(9, TAN);

	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFeiQVerDlg::PostNcDestroy() 
{
	
	CDialog::PostNcDestroy();
	if(m_Graph)
	{
		delete m_Graph;
		m_Graph = NULL;
	}
	delete this;
}

CString CFeiQVerDlg::GetVersionStr(CString strVer, int& nSort)
{
	nSort = 0;
	strVer.TrimRight();
	if(strVer == _T("1.0"))
	{
		nSort = 1;
		return _T("·É¸ë´«Êé");
	}
	else if(strVer == _T(""))
	{
		nSort = 2;
		return _T("·ÉÇï2.4»ò¸üÔç°æ±¾");
	}
	if(strVer == _T("2.4"))
	{
		nSort = 3;
		return _T("·ÉÇï2.4°æ±¾");
	}
	else if(strVer == _T("2.5a"))
	{
		nSort = 4;
		return _T("·ÉÇï2.5°æ±¾");
	}
	else if(strVer == _T("3.0a"))
	{
		nSort = 5;
		return _T("·ÉÇï2012Beta°æ±¾(Ê±¼äÔçÓÚ2012-09-20)");
	}
	else if(strVer == _T("301a"))
	{
		nSort = 6;
		return _T("·ÉÇï2012Beta°æ±¾(2012-09-21)");
	}
	else if(strVer == _T("302a"))
	{
		nSort = 7;
		return _T("·ÉÇï2012Beta°æ±¾(2012-09-22)");
	}
	else if(strVer == _T("303a"))
	{
		nSort = 8;
		return _T("·ÉÇï2012Beta°æ±¾(2012-09-27)");
	}
	else if(strVer == _T("303b"))
	{
		nSort = 9;
		return _T("·ÉÇï2012Beta°æ±¾(2012-09-28)");
	}
	else if(strVer == _T("304b"))
	{
		nSort = 10;
		return _T("·ÉÇï2012Beta°æ±¾(2012-10-19)");
	}
	else if(strVer == _T("305b"))
	{
		nSort = 11;
		return _T("·ÉÇï2012Beta°æ±¾(2012-10-20)");
	}
	else if(strVer == _T("305c"))
	{
		nSort = 12;
		return _T("·ÉÇï2013RC°æ±¾(2013-2-21)");
	}
	else if(strVer == _T("306c"))
	{
		nSort = 13;
		return _T("·ÉÇï2013RC°æ±¾(2013-2-22)");
	}
	else if(strVer == _T("307c"))
	{
		nSort = 14;
		return _T("·ÉÇï2013RC°æ±¾(2013-3-11)");
	}
	else if(strVer == _T("308c"))
	{
		nSort = 15;
		return _T("·ÉÇï2013RC°æ±¾(2013-3-15)");
	}
	else if(strVer == _T("309c"))
	{
		nSort = 16;
		return _T("·ÉÇï2013RC°æ±¾(2013-3-23)");
	}
	else if(strVer == _T("4000"))
	{
		nSort = 17;
		return _T("·ÉÇï2013ÕýÊ½°æ±¾");
	}
	else
	{
		if(strVer.GetLength() == 4)
		{
			int nVer = _ttoi(strVer);
			int nCurVer = atoi("4000");
			if(nVer > nCurVer)
			{
				nSort = 18;
				return _T("·ÉÇï(¸ß°æ±¾)") + strVer;
			}
			else 
			{
				nSort = 19;
				return strVer;
			}
		}
		nSort = 20;
		return _T("Î´Öª°æ±¾");
	}
}

void CFeiQVerDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	KillTimer(1);
	SetTimer(1, 200, NULL);
}	
	

void CFeiQVerDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 1)
	{
		CWnd* graphFrame = (CWnd*)GetDlgItem(IDC_STATIC_PIE);
		if(graphFrame)
		{
			CDC* pDC = graphFrame->GetDC();
			
			m_Graph->DrawGraph(pDC);
			ReleaseDC(pDC);
		}
		KillTimer(1);
	}

	CDialog::OnTimer(nIDEvent);
}

void CFeiQVerDlg::OnClose() 
{	
	DestroyWindow();
}
