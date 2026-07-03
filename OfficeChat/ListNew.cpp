// ListNew.cpp : implementation file
//

#include "stdafx.h"
#include "ListNew.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListNew

CListNew::CListNew()
{

}

CListNew::~CListNew()
{
}


BEGIN_MESSAGE_MAP(CListNew, CListCtrl)
	//{{AFX_MSG_MAP(CListNew)
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CListNew::SetCurrentRow(int nItem)
{
	m_nItem=nItem;
}

void CListNew::SetCurrentCol(int nCol)
{
	m_nSubItem=nCol;
}

int CListNew::GetCurrentRow()
{
	return m_nItem;
}

int CListNew::GetCurrentCol()
{
	return m_nSubItem;
}

void CListNew::SetItemValue(int nItem)
{
	LV_ITEM lItem;
	lItem.mask=LVIF_TEXT;
	lItem.iItem=nItem;
	lItem.iSubItem=0;
	SetItem(&lItem);
	//SetItemText(nItem,1,"");

}
void CListNew::SetItemColumnValue(int nItem,int nSubItem,LPCTSTR szText)
{
	SetItemText(nItem,nSubItem,szText);
}

void CListNew::SetDefaultTitle(CString strListColStyle)
{
	strColTitleArr.RemoveAll();

	if(strListColStyle == "123456")
	{
		InsertColumn(0,"用户名", 0, 100);
		InsertColumn(1,"主机", 0, 100);
		InsertColumn(2,"登录名", 0, 100);
		InsertColumn(3,"组名", 0, 150);
		InsertColumn(4,"自定义组名", 0, 150);
		InsertColumn(5,"IP地址", 0, 100);

		strColTitleArr.Add("用户名");
		strColTitleArr.Add("主机");
		strColTitleArr.Add("登录名");
		strColTitleArr.Add("组名");
		strColTitleArr.Add("自定义组名");
		strColTitleArr.Add("IP地址");
	}
	else
	{
		int nLen = strListColStyle.GetLength();
		int nCols = 0;
		for(int i = 0; i < nLen; i++)
		{
			CString strItem = strListColStyle.GetAt(i);
			if(strItem == "1")
			{
				InsertColumn(nCols,"用户名", 0, 100);
				strColTitleArr.Add("用户名");
				nCols++;
			}
			else if(strItem == "2")
			{
				InsertColumn(nCols,"主机", 0, 100);
				strColTitleArr.Add("主机");
				nCols++;
			}
			else if(strItem == "3")
			{
				InsertColumn(nCols,"登录名", 0, 100);
				strColTitleArr.Add("登录名");
				nCols++;
			}
			else if(strItem == "4")
			{
				InsertColumn(nCols,"组名", 0, 100);
				strColTitleArr.Add("组名");
				nCols++;
			}
			else if(strItem == "5")
			{
				InsertColumn(nCols,"自定义组名", 0, 100);
				strColTitleArr.Add("自定义组名");
				nCols++;
			}
			else if(strItem == "6")
			{
				InsertColumn(nCols,"IP地址", 0, 100);
				strColTitleArr.Add("IP地址");
				nCols++;
			}
		}
	}
}

void CListNew::SetItemColumnValueA(int nItem,int nSubItem,char *cText[])
{
	//cText[6]
	
	LV_ITEM lItem;
	lItem.mask=LVIF_TEXT;
	lItem.iItem=nItem;
	lItem.iSubItem=0;
	InsertItem(&lItem);

	for(int i=1;i<nSubItem;i++)
		SetItemText(nItem,i,cText[i-1]);


}

void CListNew::SetItemColumnValueB(int nItem,int nSubItem,LPTSTR cText[])
{
	//cText[6]
	
	LV_ITEM lItem;
	lItem.mask=LVIF_TEXT;
	lItem.iItem=nItem;
	lItem.iSubItem=0;
	InsertItem(&lItem);

	for(int i=0;i<=nSubItem;i++)
		SetItemText(nItem,i,cText[i]);


}
void CListNew::SetItemColumnValueC(int nItem,int nSubItem,CStringArray &textArr)
{
	if(textArr.GetSize() < nSubItem + 1) return;
	InsertItem(nItem, "");

	for(int i=0;i<=nSubItem;i++)
	{
		int nCurSubItem = GetCurColItemPos(i);
		if(nCurSubItem < 0 || nCurSubItem >= GetHeaderCtrl()->GetItemCount()) continue;
		SetItemText(nItem,nCurSubItem,textArr[i]);
	}

}
/*
void CListNew::SetAutoSize(int nStartNum,int nCount)
{

	for(int i=0;i<9;i++)
		SetColumnWidth(i,LVSCW_AUTOSIZE);
	nCount+=nStartNum;
	for(nStartNum;nStartNum<nCount;nStartNum++)
		SetColumnWidth(nStartNum,70);
   
}
*/
void CListNew::SetAutoSize()
{
	for(int i=0;i < GetHeaderCtrl()->GetItemCount(); i++)
		SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
	Invalidate();
}





void CListNew::OnSize(UINT nType, int cx, int cy) 
{
	CListCtrl::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	//SetRedraw(TRUE);
	Invalidate();
}



BOOL CListNew::SetListStyle()
{
	SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_UNDERLINEHOT|LVS_EX_ONECLICKACTIVATE);
	ShowWindow(SW_SHOW);

	return TRUE;
}

void CListNew::OnSetFocus(CWnd* pOldWnd) 
{
	CListCtrl::OnSetFocus(pOldWnd);
	
	// TODO: Add your message handler code here
	
}

void CListNew::OnKillFocus(CWnd* pNewWnd) 
{
	return;
	CListCtrl::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	
}



int CListNew::GetCurColItemPos(int nOldPos)
{
	if(nOldPos == 0)
	{
		for(int i = 0; i < strColTitleArr.GetSize(); i++)
		{
			if(strColTitleArr[i] == "用户名") return i;
		}
		return -1;
	}
	else if(nOldPos == 1)
	{
		for(int i = 0; i < strColTitleArr.GetSize(); i++)
		{
			if(strColTitleArr[i] == "主机") return i;
		}
		return -1;
	}
	else if(nOldPos == 2)
	{
		for(int i = 0; i < strColTitleArr.GetSize(); i++)
		{
			if(strColTitleArr[i] == "登录名") return i;
		}
		return -1;
	}
	else if(nOldPos == 3)
	{
		for(int i = 0; i < strColTitleArr.GetSize(); i++)
		{
			if(strColTitleArr[i] == "组名") return i;
		}
		return -1;
	}
	else if(nOldPos == 4)
	{
		for(int i = 0; i < strColTitleArr.GetSize(); i++)
		{
			if(strColTitleArr[i] == "自定义组名") return i;
		}
		return -1;
	}
	else if(nOldPos == 5)
	{
		for(int i = 0; i < strColTitleArr.GetSize(); i++)
		{
			if(strColTitleArr[i] == "IP地址") return i;
		}
		return -1;
	}
	return -1;
}

void CListNew::SetListColumnWidth(CArray<int, int> &colWidthArr)
{
	for(int i = 0; i < colWidthArr.GetSize(); i++)
	{
		int nSubItem = GetCurColItemPos(i);
		if(nSubItem < 0 || nSubItem >= GetHeaderCtrl()->GetItemCount()) continue;
		if(colWidthArr[i] < 0) continue;
		SetColumnWidth(nSubItem, colWidthArr[i]);
	}
}


