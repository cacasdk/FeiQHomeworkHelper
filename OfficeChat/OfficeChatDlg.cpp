// OfficeChatDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OfficeChat.h"
#include "OfficeChatDlg.h"
#include "SendFileDlg.h"
#include "SetDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COfficeChatDlg dialog
extern COfficeChatDlg *pOfficeChatDlg;
extern LOGFONT g_fontSend, g_fontRecv;
extern COLORREF g_clrSend, g_clrRecv;
extern BOOL g_bOfficeChat;
extern int g_BeforeMsgMode; 
extern BOOL g_bSendMsgEnter;
extern BOOL g_bSaveWndSize;
extern int g_nWndWidth, g_nWndHeight;
extern int g_nShowNameMode;
extern int g_nUserChangeTip;

COfficeChatDlg::COfficeChatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COfficeChatDlg::IDD, pParent)
{
	m_strSendFilePathArr.RemoveAll();
	m_strSendFileNameArr.RemoveAll();
	m_strOnlineIps = "";
	m_ptrRoot = NULL;
	m_bListBusy = FALSE;
	strSelfName = "";
	strOldSelUserIP = "";
	//{{AFX_DATA_INIT(COfficeChatDlg)
	m_strContent = _T("");
	m_strChatLog = _T("");
	m_strSendUsers = _T("要发送的用户:");
	m_strSendStatus = _T("状态>>:");
	m_strOnlineMembers = _T("");
	//}}AFX_DATA_INIT
}


void COfficeChatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COfficeChatDlg)
	DDX_Control(pDX, IDC_EDIT_SEND, m_sendEdit);
	DDX_Control(pDX, IDC_LIST1, m_usersList);
	DDX_Text(pDX, IDC_EDIT_SHOWALL, m_strChatLog);
	DDX_Text(pDX, IDC_EDIT_USER, m_strSendUsers);
	DDX_Text(pDX, IDC_EDIT_STATUS, m_strSendStatus);
	DDX_Text(pDX, IDC_EDIT1, m_strOnlineMembers);
	DDX_Text(pDX, IDC_EDIT_SEND, m_strContent);
	DDV_MaxChars(pDX, m_strContent, 7000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COfficeChatDlg, CDialog)
	//{{AFX_MSG_MAP(COfficeChatDlg)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_WM_TIMER()
	ON_NOTIFY(NM_CLICK, IDC_LIST1, OnClickList1)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, OnColumnclickList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
	ON_WM_SIZE()
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON_SENDFILE, OnButtonSendfile)
	ON_COMMAND(IDM_SEND_FILES, OnSendFiles)
	ON_COMMAND(IDM_SEND_DIRS, OnSendDirs)
	ON_BN_CLICKED(IDC_BUTTON_MORE, OnButtonMore)
	ON_COMMAND(IDM_SET, OnSet)
	ON_COMMAND(IDM_SEND_CTRLENTER, OnSendCtrlenter)
	ON_UPDATE_COMMAND_UI(IDM_SEND_CTRLENTER, OnUpdateSendCtrlenter)
	ON_COMMAND(IDM_SEND_ENTER, OnSendEnter)
	ON_UPDATE_COMMAND_UI(IDM_SEND_ENTER, OnUpdateSendEnter)
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(IDM_SEND_USERCHANGETIP, OnSendUserchangetip)
	ON_UPDATE_COMMAND_UI(IDM_SEND_USERCHANGETIP, OnUpdateSendUserchangetip)
	ON_COMMAND(IDM_SEND_MODE, OnSendMode)
	ON_UPDATE_COMMAND_UI(IDM_SEND_MODE, OnUpdateSendMode)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COfficeChatDlg message handlers

void COfficeChatDlg::OnOK() 
{
	
}


BOOL COfficeChatDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_usersList.SetDefaultTitle();
	//m_usersList.SetExtendedStyle(m_usersList.GetExtendedStyle() | LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);//完全行选择
	m_usersList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_UNDERLINEHOT|LVS_EX_ONECLICKACTIVATE);

	if(m_ptrRoot)
	{
		IFQBuddyCollectionPtr pFQBuddysCol = m_ptrRoot->OnlineBuddys;
		if(pFQBuddysCol)
		{
			for(int i = 0; i < pFQBuddysCol->Count; i++)
			{
				IFQBuddyPtr pFQBuddy = pFQBuddysCol->Item[i];
				if(pFQBuddy)
				{
					CString strMemoName = (LPCTSTR)pFQBuddy->MarkName;
					CString strHostName = (LPCTSTR)pFQBuddy->HostName;
					CString strLoginName = (LPCTSTR)pFQBuddy->LoginName;
					CString strGroupName = (LPCTSTR)pFQBuddy->GroupName;
					CString strMemoGroup = (LPCTSTR)pFQBuddy->MarkGroupName;
					CString strIP = (LPCTSTR)pFQBuddy->IPValue;
					AddBuddyToList(strMemoName, strHostName, strLoginName, strGroupName, strMemoGroup, strIP);
				}
			}
		}
		CComBSTR userName, hostName, IpValue, MacValue;
		long nPortNo = 0;
		m_ptrRoot->GetMySelfInfo(&userName, &hostName, &IpValue, &MacValue, &nPortNo);
		strSelfName = userName.m_str;
		strSelfName.TrimRight();
		if(strSelfName == "") strSelfName = "自己";
	}
	m_usersList.SetAutoSize();



	m_sendFont.CreateFontIndirect(&g_fontSend);
	m_recvFont.CreateFontIndirect(&g_fontRecv);
	((CEdit*)GetDlgItem(IDC_EDIT_SEND))->SetFont(&m_sendFont, TRUE);
	((CEdit*)GetDlgItem(IDC_EDIT_SHOWALL))->SetFont(&m_recvFont, TRUE);

	if(g_nWndHeight != 0 && g_nWndWidth != 0)
	{
		CRect dlgRc, windowRc;
		GetWindowRect(&dlgRc);
		dlgRc.right = dlgRc.left + g_nWndWidth;
		dlgRc.bottom = dlgRc.top + g_nWndHeight;
		SystemParametersInfo(SPI_GETWORKAREA,   0,   (LPVOID)&windowRc,   0); 
		SetWindowPos(NULL, windowRc.left + (int)(windowRc.Width() / 2.0 - dlgRc.Width() / 2.0), windowRc.top + int((windowRc.Height() - dlgRc.Height()) / 2.0), dlgRc.Width(), dlgRc.Height(), SWP_SHOWWINDOW & ~SWP_SHOWWINDOW);
	}
	UpdateSize();

	if(g_bOfficeChat)
	{
		SetWindowText("当前为办公对话模式--飞秋(通过发送下面箭头按钮可设置为普通对话模式");
	}
	else
	{
		SetWindowText("当前为普通对话模式--飞秋(通过发送下面箭头按钮可设置为办公对话模式");
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH COfficeChatDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	int  nID = pWnd->GetDlgCtrlID(); 
	if(nID == IDC_EDIT_SHOWALL) 
	{   
		pDC->SetBkColor(RGB(255,255,255));   
		pDC->SetTextColor(g_clrRecv);   
		return  ::CreateSolidBrush(RGB(255,255,255));   
	}   
	else if(nID == IDC_EDIT_SEND) 
	{   
		pDC->SetBkColor(RGB(255,255,255));   
		pDC->SetTextColor(g_clrSend);   
		return  ::CreateSolidBrush(RGB(255,255,255));   
	} 

	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void COfficeChatDlg::OnButton1() 
{
	if(m_ptrRoot == NULL) return;
	UpdateData();
	if(m_strContent == "" && m_strSendFilePathArr.GetSize() == 0)
	{
		GetDlgItem(IDC_EDIT_STATUS)->SetWindowText("状态>>:发送失败，发送内容和文件为空！");
		return;
	}
	CString strSendIps = "";
	CString strSendUsers = "";
	if(m_usersList.GetSelectedCount() > 1)
	{
		strSendUsers.Format("%d人", m_usersList.GetSelectedCount());
	}
	POSITION pos = m_usersList.GetFirstSelectedItemPosition();
	while (pos)
	{
		int nIndex = m_usersList.GetNextSelectedItem(pos);
		if(nIndex >= 0)
		{
			CString strItemIP;
			strItemIP = m_usersList.GetItemText(nIndex, 5);
			strSendIps += strItemIP + "|";//多个IP用"|"分开
			if(strSendUsers == "") strSendUsers = m_usersList.GetItemText(nIndex, 0);
		}
	}
	if(strSendIps == "")
	{
		GetDlgItem(IDC_EDIT_STATUS)->SetWindowText("状态>>:您还没有选择要发送的用户！");
		return;
	}
	if(g_nUserChangeTip && strOldSelUserIP != "" && strOldSelUserIP != strSendIps)
	{
		if(MessageBox("你是否要把信息发送给?\r\n" + m_strSendUsers, "提示", MB_YESNO|MB_ICONQUESTION) == IDNO)
		{
			return;
		}

	}
	CString strSendFiles;
	for(int i = 0; i < m_strSendFilePathArr.GetSize(); i++)
	{
		if(i != 0) strSendFiles += "|";//多个文件用"|"分开
		strSendFiles += m_strSendFilePathArr[i];
	}
	HRESULT hr = m_ptrRoot->SendBuddyMessage((LPCTSTR)strSendIps, (LPCTSTR)m_strContent, (LPCTSTR)strSendFiles);
	if(hr == S_FALSE)
	{
		GetDlgItem(IDC_EDIT_STATUS)->SetWindowText("状态>>:发送失败！网络问题或已下线,请刷新重试!");
		return;
	}
	else
	{
		CString strSendMsg;
		COleDateTime dt = COleDateTime::GetCurrentTime();
		if(m_strSendFilePathArr.GetSize() != 0)
		{
			CString strSendFiles;
			strSendFiles.Format("共发送了 %d 个文件(夹)", m_strSendFilePathArr.GetSize());
			if(m_strContent == "") m_strContent = strSendFiles;
			else m_strContent += "\r\n" + strSendFiles;
		}
		strSendMsg.Format("%s %s --> %s\r\n  %s\r\n", strSelfName, dt.Format(_T("%H:%M:%S")), strSendUsers, m_strContent);
		RecvMsg(strSendMsg, "");

		if(strSendFiles != "")
		{
			m_strSendFilePathArr.RemoveAll();
			m_strSendFileNameArr.RemoveAll();
			CWnd *pWnd = GetDlgItem(IDC_BUTTON_SENDFILE);
			pWnd->SetWindowText("");
			pWnd->ShowWindow(SW_HIDE);
			UpdateSize();
		}
		m_strContent = "";
		UpdateData(FALSE);
		
		GetDlgItem(IDC_EDIT_STATUS)->SetWindowText("状态>>:");

		GetDlgItem(IDC_EDIT_SEND)->SetFocus();

		strOldSelUserIP = strSendIps;
	}
}

void COfficeChatDlg::RecvMsg(CString strMsg, CString strIP, BOOL bInitSelUser)
{
	UpdateData();
	m_strChatLog += strMsg;
	UpdateData(FALSE);
	GetDlgItem(IDC_EDIT_SHOWALL)->PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	GetDlgItem(IDC_EDIT_SHOWALL)->Invalidate();
	
	if(strIP != "" && m_usersList.GetSelectedCount() == 0 && bInitSelUser)
	{
		for(int i = 0; i < m_usersList.GetItemCount() - 1; i++)
		{
			CString strItemIP = m_usersList.GetItemText(i, 5);
			if(strItemIP == strIP)
			{
				m_usersList.SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
				m_usersList.SetFocus();
				CString strSendBuddys;
				strSendBuddys.Format("要发送的用户(%d人):", m_usersList.GetSelectedCount());
				POSITION pos = m_usersList.GetFirstSelectedItemPosition();
				while (pos)
				{
					int nIndex = m_usersList.GetNextSelectedItem(pos);
					if(nIndex >= 0)
					{
						CString strItemName, strItemIP;
						strItemName = m_usersList.GetItemText(nIndex, 0);
						strItemIP = m_usersList.GetItemText(nIndex, 5);
						strSendBuddys += strItemName + "(" + strItemIP + ");";
					}
				}
				GetDlgItem(IDC_EDIT_USER)->SetWindowText(strSendBuddys);
				GetDlgItem(IDC_EDIT_SEND)->SetFocus();
				break;
			}
		}
	}
}

void COfficeChatDlg::OnDestroy() 
{
	CDialog::OnDestroy();

	if(m_ptrRoot)
	{
		IFQDataPtr cfgPtr = NULL;//得到配置信息
		m_ptrRoot->get_UserCustomConfig(&cfgPtr);
		if(cfgPtr)
		{
			if(g_bSaveWndSize)
			{
				CRect rc;
				GetWindowRect(&rc);
				g_nWndWidth = rc.Width();
				g_nWndHeight = rc.Height();
				cfgPtr->SetLong(L"BGCHAT_WNDWIDTH", g_nWndWidth);
				cfgPtr->SetLong(L"BGCHAT_WNDHEGHT", g_nWndHeight);
			}
		}
	}
	pOfficeChatDlg = NULL;
	delete this;
}

void COfficeChatDlg::OnClose() 
{	
	DestroyWindow();
}

void COfficeChatDlg::OnButton2() 
{
	if(m_ptrRoot == NULL) return;
	m_strOnlineIps = "";
	m_usersList.DeleteAllItems();
	CString strOnLine;
	strOnLine.Format("%d人在线", m_usersList.GetItemCount());
	GetDlgItem(IDC_EDIT1)->SetWindowText(strOnLine);
	GetDlgItem(IDC_EDIT_USER)->SetWindowText("要发送的用户:");
	m_ptrRoot->RefreshBuddys();
	SetTimer(1, 5000, NULL);
}


void COfficeChatDlg::AddBuddyToList(CString strMemoName, CString strHostName, CString strLoginName, CString strGroupName, CString strMemoGroupName, CString strIP)
{
	if(m_bListBusy) return;
	if(m_strOnlineIps.Find(strIP + ";") != -1) return;//如果此IP已在列表中
	CStringArray strItemArr;
	strItemArr.SetSize(6);

	strItemArr[0] = strMemoName;//用户名
	strItemArr[1] = strHostName;//主机
	strItemArr[2] =	strLoginName;//登录名
	strItemArr[3] = strGroupName;//组
	strItemArr[4] = strMemoGroupName;//自定义组
	strItemArr[5] = strIP;//IP
	int pos = m_usersList.GetItemCount();
	m_usersList.SetItemColumnValueC(pos, 5, strItemArr);
	
	m_strOnlineIps += strIP + ";";

	CString strOnLine;
	strOnLine.Format("%d人在线", m_usersList.GetItemCount());
	GetDlgItem(IDC_EDIT1)->SetWindowText(strOnLine);

	
}

void COfficeChatDlg::DelBuddyFromList(CString strIP)
{
	if(m_bListBusy) return;
	for(int i = 0; i < m_usersList.GetItemCount(); i++)
	{
		CString strItemIP = m_usersList.GetItemText(i, 5);
		if(strItemIP == strIP)
		{
			m_usersList.DeleteItem(i);
			CString strOnLine;
			strOnLine.Format("%d人在线", m_usersList.GetItemCount());
			GetDlgItem(IDC_EDIT1)->SetWindowText(strOnLine);
			m_strOnlineIps.Replace(strItemIP + ";", "");
			break;
		}
	}
}

void COfficeChatDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 1)
	{
		KillTimer(1);
		if(m_usersList.m_hWnd) m_usersList.SetAutoSize();
	}
	CDialog::OnTimer(nIDEvent);
}

void COfficeChatDlg::OnClickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CString strSendBuddys;
	strSendBuddys.Format("要发送的用户(%d人):", m_usersList.GetSelectedCount());
	POSITION pos = m_usersList.GetFirstSelectedItemPosition();
	while (pos)
	{
		int nIndex = m_usersList.GetNextSelectedItem(pos);
		if(nIndex >= 0)
		{
			CString strItemName, strItemIP;
			strItemName = m_usersList.GetItemText(nIndex, 0);
			strItemIP = m_usersList.GetItemText(nIndex, 5);
			strSendBuddys += strItemName + "(" + strItemIP + ");";
		}
	}
	GetDlgItem(IDC_EDIT_USER)->SetWindowText(strSendBuddys);
	*pResult = 0;
}

void COfficeChatDlg::OnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	int nSubItem = pNMListView->iSubItem;
	if(nSubItem < 0 || nSubItem >= m_usersList.GetHeaderCtrl()->GetItemCount()) return;
	static BOOL sortRev = FALSE;
	sortRev = !sortRev;

	CString strSortItem;
	int sortItem = 0;
	HDITEM hdi;
	TCHAR  lpBuffer[256];

	hdi.mask = HDI_TEXT;
	hdi.pszText = lpBuffer;
	hdi.cchTextMax = 256;

	m_usersList.GetHeaderCtrl()->GetItem(nSubItem, &hdi);

	strSortItem = lpBuffer;
	strSortItem.TrimLeft();
	strSortItem.TrimRight();
	if(strSortItem == "用户名")
	{
		sortItem = 0;
	}
	else if(strSortItem == "主机")
	{
		sortItem = 1;
	}
	else if(strSortItem == "登录名")
	{
		sortItem = 2;
	}
	else if(strSortItem == "组名")
	{
		sortItem = 3;
	}
	else if(strSortItem == "自定义组名")
	{
		sortItem = 4;
	}
	else if(strSortItem == "IP地址")
	{
		sortItem = 5;
	}

	m_bListBusy = TRUE;
	for(int i = 0; i < m_usersList.GetItemCount() - 1; i++)
	{
		for(int j = i + 1; j < m_usersList.GetItemCount(); j++)
		{
			CString strSortItem1 = m_usersList.GetItemText(i, sortItem);
		    CString strSortItem2 = m_usersList.GetItemText(j, sortItem);

			BOOL bSort = FALSE;
			if(sortRev)
			{
				if(strcmp(strSortItem1, strSortItem2) >= 0)
				{
					bSort = TRUE;
				}
			}
			else
			{
				if(strcmp(strSortItem1, strSortItem2) < 0)
				{
					bSort = TRUE;
				}
			}
			if(bSort)
			{
				CString strItem0, strItem1, strItem2, strItem3, strItem4, strItem5;
				strItem0 = m_usersList.GetItemText(i, 0);
				strItem1 = m_usersList.GetItemText(i, 1);
				strItem2 = m_usersList.GetItemText(i, 2);
				strItem3 = m_usersList.GetItemText(i, 3);
				strItem4 = m_usersList.GetItemText(i, 4);
				strItem5 = m_usersList.GetItemText(i, 5);
				m_usersList.SetItemText(i, 0, m_usersList.GetItemText(j, 0));
				m_usersList.SetItemText(i, 1, m_usersList.GetItemText(j, 1));
				m_usersList.SetItemText(i, 2, m_usersList.GetItemText(j, 2));
				m_usersList.SetItemText(i, 3, m_usersList.GetItemText(j, 3));
				m_usersList.SetItemText(i, 4, m_usersList.GetItemText(j, 4));
				m_usersList.SetItemText(i, 5, m_usersList.GetItemText(j, 5));

				m_usersList.SetItemText(j, 0, strItem0);
				m_usersList.SetItemText(j, 1, strItem1);
				m_usersList.SetItemText(j, 2, strItem2);
				m_usersList.SetItemText(j, 3, strItem3);
				m_usersList.SetItemText(j, 4, strItem4);
				m_usersList.SetItemText(j, 5, strItem5);
			}

		}
	}
	m_bListBusy = FALSE;
	*pResult = 0;
}

void COfficeChatDlg::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if(m_ptrRoot == NULL) return;
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	int nItem = pNMListView->iItem;
	if(nItem < 0) return;
	CString strIp = m_usersList.GetItemText(nItem, 5);

	m_ptrRoot->ShowBuddyChatDialog((LPCTSTR)strIp);
	*pResult = 0;
}

void COfficeChatDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	UpdateSize();
	
}

void COfficeChatDlg::UpdateSize()
{
	CRect dlgRect, rc;
	GetClientRect(&dlgRect);
	CWnd *pWnd = NULL;
	pWnd = GetDlgItem(IDC_STATIC_USERFRAME);
	if(pWnd)
	{
		rc = dlgRect;
		rc.InflateRect(-2, -2);
		rc.left = (int)(dlgRect.Width() * 7.0 / 10.0);
		pWnd->MoveWindow(rc);

		pWnd = GetDlgItem(IDC_LIST1);
		CRect rcList;
		if(pWnd)
		{
			rcList = rc;
			rcList.InflateRect(-5, -5);
			rcList.bottom -= 20;
			rcList.top += 10;
			pWnd->MoveWindow(rcList);
		}

		pWnd = GetDlgItem(IDC_EDIT1);
		CRect rcNum;
		if(pWnd)
		{
			rcNum.left = rcList.left;
			rcNum.right = rcNum.left + 70;
			rcNum.top = rcList.bottom + 2;
			rcNum.bottom = rcNum.top + 20;
			pWnd->MoveWindow(rcNum);
		}

		pWnd = GetDlgItem(IDC_BUTTON2);
		CRect rcRefresh;
		if(pWnd)
		{
			rcRefresh.right = rcList.right - 3;
			rcRefresh.left = rcRefresh.right - 63;
			rcRefresh.top = rcList.bottom + 2;
			rcRefresh.bottom = rcNum.top + 20;
			pWnd->MoveWindow(rcRefresh);
		}
	}

	pWnd = GetDlgItem(IDC_EDIT_SHOWALL);
	if(pWnd)
	{
		rc = dlgRect;
		rc.InflateRect(-2, -2);
		rc.right = (int)(dlgRect.Width() * 7.0 / 10.0) - 3;
		rc.bottom = (int)(dlgRect.Height() * 3.0 / 5.0);
		pWnd->MoveWindow(rc);

		CRect rcSends;
		pWnd = GetDlgItem(IDC_EDIT_USER);
		if(pWnd)
		{
			rcSends = rc;
			rcSends.top = rc.bottom + 1;
			rcSends.bottom = rcSends.top + 35;
			pWnd->MoveWindow(rcSends);
		}

		int nOffest = 0;
		pWnd = GetDlgItem(IDC_BUTTON_SENDFILE);
		if(pWnd->IsWindowVisible())
		{
			nOffest = 20;
			CRect rcFile = rcSends;
			rcFile.top = rcSends.bottom + 1;
			rcFile.bottom = rcFile.top + nOffest;
			pWnd->MoveWindow(rcFile);
		}

		int nSpace = (int)(dlgRect.Height() * 2.0 / 5.0) - 35 - nOffest - 20 - 3;

		pWnd = GetDlgItem(IDC_EDIT_SEND);
		CRect rcSendEdit;
		if(pWnd)
		{
			rcSendEdit = rcSends;
			rcSendEdit.right -= 30;
			rcSendEdit.top = rcSends.bottom + nOffest + 1 + (nOffest == 0 ? 0 : 1);
			rcSendEdit.bottom = rcSendEdit.top + nSpace;
			pWnd->MoveWindow(rcSendEdit);
		}

		pWnd = GetDlgItem(IDC_EDIT_STATUS);
		CRect rcStatus;
		if(pWnd)
		{
			rcStatus = rcSendEdit;
			rcSendEdit.right -= 30;
			rcStatus.top = rcSendEdit.bottom + 1;
			rcStatus.bottom = rcStatus.top + 20;
			pWnd->MoveWindow(rcStatus);
		}

		pWnd = GetDlgItem(IDC_BUTTON1);
		CRect rcSendBtn;
		if(pWnd)
		{
			rcSendBtn = rcSendEdit;
			rcSendBtn.left = rcSendEdit.right + 30 + 1;
			rcSendBtn.right = rcSendBtn.left + 30;
			rcSendBtn.bottom = rcSendEdit.bottom;
			pWnd->MoveWindow(rcSendBtn);
		}

		pWnd = GetDlgItem(IDC_BUTTON_MORE);
		CRect rcMoreBtn;
		if(pWnd)
		{
			rcMoreBtn = rcSendEdit;
			rcMoreBtn.left = rcSendEdit.right + 30 + 1;
			rcMoreBtn.right = rcMoreBtn.left + 30;
			rcMoreBtn.top = rcSendEdit.bottom + 1;
			rcMoreBtn.bottom = rcSendEdit.bottom + 20;
			pWnd->MoveWindow(rcMoreBtn);
		}

	}
}
void COfficeChatDlg::OnDropFiles(HDROP hDropInfo)
{
    HDROP m_hDropInfo = hDropInfo;
    CString Filename;

	CString strFiles;
    if (m_hDropInfo) {

        int iFiles = DragQueryFile(m_hDropInfo, (UINT)-1, NULL, 0);

        for (int i=0; i<iFiles; i++) {

            char* pFilename = Filename.GetBuffer(_MAX_PATH);
            DragQueryFile(m_hDropInfo, i, pFilename, _MAX_PATH);

            // do whatever...
			WIN32_FIND_DATA	fdat;

			if (GetFileInfomation(pFilename, &fdat))
			{
				m_strSendFilePathArr.Add(pFilename);
				m_strSendFileNameArr.Add(fdat.cFileName);
				strFiles += CString(fdat.cFileName) + " ";
			}
        }   // for each files...
    }       // if DropInfo

    DragFinish(m_hDropInfo);

    m_hDropInfo = 0;

	if(strFiles != "")
	{
		CWnd *pWnd = GetDlgItem(IDC_BUTTON_SENDFILE);
		pWnd->ShowWindow(SW_SHOW);
		CString strText;
		pWnd->GetWindowText(strText);
		if(strText != "")
		{
			strText += " ";
		}
		strText += strFiles;
		pWnd->SetWindowText(strText);
		UpdateSize();
	}

}

BOOL COfficeChatDlg::GetFileInfomation(const char *path, WIN32_FIND_DATA *fdata)
{
	HANDLE	fh;

	if ((fh = ::FindFirstFile(path, fdata)) != INVALID_HANDLE_VALUE)
	{
		::FindClose(fh);
		return	TRUE;
	}

	memset(fdata, 0, sizeof(WIN32_FIND_DATA));

	if ((fh = ::CreateFile(path, GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0)) != INVALID_HANDLE_VALUE)
	{
		BY_HANDLE_FILE_INFORMATION	info;
		BOOL	info_ret = ::GetFileInformationByHandle(fh, &info);
		::CloseHandle(fh);
		if (info_ret)
			return	memcpy(fdata, &info, (char *)&info.dwVolumeSerialNumber - (char *)&info), TRUE;
	}

	return	(fdata->dwFileAttributes = ::GetFileAttributes(path)) == 0xffffffff ? FALSE : TRUE;
}

void COfficeChatDlg::OnButtonSendfile() 
{
	CSendFileDlg sendFiledlg;
	sendFiledlg.m_strSendFilePathArr.Append(m_strSendFilePathArr);
	sendFiledlg.m_strSendFileNameArr.Append(m_strSendFileNameArr);
	if(sendFiledlg.DoModal() == IDOK)
	{
		m_strSendFilePathArr.RemoveAll();
		m_strSendFileNameArr.RemoveAll();
		m_strSendFilePathArr.Append(sendFiledlg.m_strSendFileNameArr);
		m_strSendFileNameArr.Append(sendFiledlg.m_strSendFileNameArr);

		if(m_strSendFilePathArr.GetSize() == 0)
		{
			CWnd *pWnd = GetDlgItem(IDC_BUTTON_SENDFILE);
			pWnd->SetWindowText("");
			pWnd->ShowWindow(SW_HIDE);
			UpdateSize();
		}
		else
		{
			CString strText;
			for(int i = 0; i < m_strSendFileNameArr.GetSize(); i++)
			{
				if(i != 0) strText += " ";
				strText += m_strSendFileNameArr[i];
			}
			CWnd *pWnd = GetDlgItem(IDC_BUTTON_SENDFILE);
			pWnd->SetWindowText(strText);
			pWnd->ShowWindow(SW_SHOW);
			UpdateSize();
		}
	}
}

void COfficeChatDlg::OnSendFiles() 
{
	// TODO: Add your control notification handler code here
	CFileDialog filedlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_EXPLORER,"All Files (*.*)|*.*||", this);
	//--------只要加上下面这段代码，就可以显示2000下的打开文件对话框-------------
	//------用来检测目前的操作系统的版本信息
	int nStructSize = 0;
	DWORD dwVersion, dwWindowsMajorVersion, dwWindwosMinorVersion;
	dwVersion = GetVersion();
	dwWindowsMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	dwWindwosMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));
	if(dwVersion < 0x80000000)
	{
		nStructSize = 88;   //2k,xp下显示新的版本
	}
	else
	{
		nStructSize = 76;  //98下显示老版本
	}
	filedlg.m_ofn.lStructSize = nStructSize;  //------用来检测目前的操作系统的版本信息
	filedlg.m_ofn.Flags|=OFN_ALLOWMULTISELECT;
	DWORD MAXFILE = 2562 * 50; //2562 is the max
	filedlg.m_ofn.nMaxFile = MAXFILE;//2562 is the max
	char* pc = new char[MAXFILE];
	filedlg.m_ofn.lpstrFile = pc;
	filedlg.m_ofn.lpstrFile[0] = NULL;
	if(filedlg.DoModal()==IDOK)
	{
		POSITION   pos=filedlg.GetStartPosition();   
		CStringArray   strFileNameArr;   
		strFileNameArr.RemoveAll();
		while(pos)   
		{   
			strFileNameArr.Add(filedlg.GetNextPathName(pos));        
		}   

		for(int i = 0; i < strFileNameArr.GetSize(); i++)
		{
			char  pFilename[MAX_PATH];  
			WIN32_FIND_DATA	fdat;
			
			strcpy(pFilename, strFileNameArr[i]);
			if (GetFileInfomation(pFilename, &fdat))
			{
				for(int k = 0; k < m_strSendFilePathArr.GetSize(); k++)
				{
					if(strcmp(m_strSendFilePathArr[k], pFilename) == 0)break;
				}
				if(k == m_strSendFilePathArr.GetSize())
				{
					m_strSendFilePathArr.Add(pFilename);
					m_strSendFileNameArr.Add(fdat.cFileName);
				}
			}
		}

	
		CString strText;
		for(i = 0; i < m_strSendFileNameArr.GetSize(); i++)
		{
			if(i != 0) strText += " ";
			strText += m_strSendFileNameArr[i];
		}
		CWnd *pWnd = GetDlgItem(IDC_BUTTON_SENDFILE);
		pWnd->SetWindowText(strText);
		pWnd->ShowWindow(SW_SHOW);
		UpdateSize();
	}
	delete []pc; 
}

void COfficeChatDlg::OnSendDirs() 
{
	// TODO: Add your command handler code here
	BROWSEINFO   bi;       
	char   dispname[MAX_PATH],   pFilename[MAX_PATH];       
	ITEMIDLIST   *   pidl;       
    
	bi.hwndOwner   =   m_hWnd;       
	bi.pidlRoot   =   0;       
	bi.pszDisplayName   =   dispname;       
	bi.lpszTitle   =   "增加文件夹:";       
	bi.ulFlags   =   BIF_RETURNONLYFSDIRS;       
	bi.lpfn   =   0;       
	bi.lParam   =   0;       
	bi.iImage   =   0;       
    
	if   (pidl   =   SHBrowseForFolder(&bi))   //弹出文件夹浏览窗口，并选取目录   
	{       
		SHGetPathFromIDList(pidl,   pFilename);       
		
		WIN32_FIND_DATA	fdat;
		
		if (GetFileInfomation(pFilename, &fdat))
		{
			for(int k = 0; k < m_strSendFilePathArr.GetSize(); k++)
			{
				if(strcmp(m_strSendFilePathArr[k], pFilename) == 0)break;
			}
			if(k == m_strSendFilePathArr.GetSize())
			{
				m_strSendFilePathArr.Add(pFilename);
				m_strSendFileNameArr.Add(fdat.cFileName);
			}
		}

		CString strText;
		for(int i = 0; i < m_strSendFileNameArr.GetSize(); i++)
		{
			if(i != 0) strText += " ";
			strText += m_strSendFileNameArr[i];
		}
		CWnd *pWnd = GetDlgItem(IDC_BUTTON_SENDFILE);
		pWnd->SetWindowText(strText);
		pWnd->ShowWindow(SW_SHOW);
		UpdateSize();
	}
}

void COfficeChatDlg::OnButtonMore() 
{
	CMenu menu;

	CMenu tmp;
	tmp.LoadMenu(IDR_MENU1);

	menu.Attach(*tmp.GetSubMenu(0));


	CPoint pt;
	GetCursorPos(&pt);
	menu.TrackPopupMenu(TPM_TOPALIGN,pt.x,pt.y,this);

	menu.Detach();
}

void COfficeChatDlg::OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu)
{
    ASSERT(pPopupMenu != NULL);
    // Check the enabled state of various menu items.

    CCmdUI state;
    state.m_pMenu = pPopupMenu;
    ASSERT(state.m_pOther == NULL);
    ASSERT(state.m_pParentMenu == NULL);

    // Determine if menu is popup in top-level menu and set m_pOther to
    // it if so (m_pParentMenu == NULL indicates that it is secondary popup).
    HMENU hParentMenu;
    if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
        state.m_pParentMenu = pPopupMenu;    // Parent == child for tracking popup.
    else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
    {
        CWnd* pParent = this;
           // Child windows don't have menus--need to go to the top!
        if (pParent != NULL &&
           (hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
        {
           int nIndexMax = ::GetMenuItemCount(hParentMenu);
           for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
           {
            if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu)
            {
                // When popup is found, m_pParentMenu is containing menu.
                state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
                break;
            }
           }
        }
    }

    state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
    for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
      state.m_nIndex++)
    {
        state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
        if (state.m_nID == 0)
           continue; // Menu separator or invalid cmd - ignore it.

        ASSERT(state.m_pOther == NULL);
        ASSERT(state.m_pMenu != NULL);
        if (state.m_nID == (UINT)-1)
        {
           // Possibly a popup menu, route to first item of that popup.
           state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
           if (state.m_pSubMenu == NULL ||
            (state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
            state.m_nID == (UINT)-1)
           {
            continue;       // First item of popup can't be routed to.
           }
           state.DoUpdate(this, TRUE);   // Popups are never auto disabled.
        }
        else
        {
           // Normal menu item.
           // Auto enable/disable if frame window has m_bAutoMenuEnable
           // set and command is _not_ a system command.
           state.m_pSubMenu = NULL;
           state.DoUpdate(this, FALSE);
        }

        // Adjust for menu deletions and additions.
        UINT nCount = pPopupMenu->GetMenuItemCount();
        if (nCount < state.m_nIndexMax)
        {
           state.m_nIndex -= (state.m_nIndexMax - nCount);
           while (state.m_nIndex < nCount &&
            pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
           {
            state.m_nIndex++;
           }
        }
        state.m_nIndexMax = nCount;
    }
}


void COfficeChatDlg::OnSet() 
{
	CSetDlg setDlg;
	setDlg.fontSend = g_fontSend;
	setDlg.fontRecv = g_fontRecv;
	setDlg.clrSend = g_clrSend;
	setDlg.clrRecv = g_clrRecv;
	setDlg.m_bOfficeChat = g_bOfficeChat;
	setDlg.m_nMsgMode = g_BeforeMsgMode;
	setDlg.m_bSendMsgEnter = g_bSendMsgEnter;
	setDlg.m_bSaveWndSize = g_bSaveWndSize;
	setDlg.m_nShowNameMode = g_nShowNameMode;
	setDlg.m_nUserChangeTip = g_nUserChangeTip;
	if(setDlg.DoModal() == IDOK)
	{
		g_fontSend = setDlg.fontSend;
		g_fontRecv = setDlg.fontRecv;
		g_clrSend = setDlg.clrSend;
		g_clrRecv = setDlg.clrRecv;
		g_BeforeMsgMode = setDlg.m_nMsgMode;
		g_bOfficeChat = setDlg.m_bOfficeChat;
		g_bSendMsgEnter = setDlg.m_bSendMsgEnter;
		g_bSaveWndSize = setDlg.m_bSaveWndSize;
		g_nShowNameMode = setDlg.m_nShowNameMode;
		g_nUserChangeTip = setDlg.m_nUserChangeTip;
		if(pOfficeChatDlg && pOfficeChatDlg->m_hWnd)
		{
			m_sendFont.DeleteObject();
			m_recvFont.DeleteObject();
			m_sendFont.CreateFontIndirect(&g_fontSend);
			m_recvFont.CreateFontIndirect(&g_fontRecv);
			((CEdit*)pOfficeChatDlg->GetDlgItem(IDC_EDIT_SEND))->SetFont(&m_sendFont, TRUE);
			((CEdit*)pOfficeChatDlg->GetDlgItem(IDC_EDIT_SHOWALL))->SetFont(&m_recvFont, TRUE);
		}
		if(m_ptrRoot)
		{
			IFQDataPtr cfgPtr = NULL;//得到配置信息
			m_ptrRoot->get_UserCustomConfig(&cfgPtr);
			if(cfgPtr)
			{
				cfgPtr->SetBuffer(L"BGCHAT_SENDFONT", (unsigned char *)&g_fontSend, sizeof(LOGFONT));
				cfgPtr->SetBuffer(L"BGCHAT_RECVFONT", (unsigned char *)&g_fontRecv, sizeof(LOGFONT));
				cfgPtr->SetLong(L"BGCHAT_SENDCLR", g_clrSend);
				cfgPtr->SetLong(L"BGCHAT_RECVCLR", g_clrRecv);
				cfgPtr->SetLong(L"BGCHAT_SENDMODE", g_bSendMsgEnter);
				cfgPtr->SetLong(L"BGCHAT_SAVEWNDSIZE", g_bSaveWndSize);
				cfgPtr->SetLong(L"BGCHAT_SHOWNAMEMODE", g_nShowNameMode);
				cfgPtr->SetLong(L"BGCHAT_USERCHANGETIP", g_nUserChangeTip);
				if(g_bSaveWndSize)
				{
					CRect rc;
					GetWindowRect(&rc);
					g_nWndWidth = rc.Width();
					g_nWndHeight = rc.Height();
					cfgPtr->SetLong(L"BGCHAT_WNDWIDTH", g_nWndWidth);
					cfgPtr->SetLong(L"BGCHAT_WNDHEGHT", g_nWndHeight);
				}
			}
		}
	}
}


void COfficeChatDlg::OnSendCtrlenter() 
{
	// TODO: Add your command handler code here
	g_bSendMsgEnter = FALSE;
}

void COfficeChatDlg::OnUpdateSendCtrlenter(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(g_bSendMsgEnter == FALSE);
}

void COfficeChatDlg::OnSendEnter() 
{
	// TODO: Add your command handler code here
	g_bSendMsgEnter = TRUE;
}

void COfficeChatDlg::OnUpdateSendEnter(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(g_bSendMsgEnter);
}


void COfficeChatDlg::OnSendUserchangetip() 
{
	// TODO: Add your command handler code here
	g_nUserChangeTip = !g_nUserChangeTip;
}

void COfficeChatDlg::OnUpdateSendUserchangetip(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(g_nUserChangeTip);
}

void COfficeChatDlg::OnSendMode() 
{
	// TODO: Add your command handler code here
	g_bOfficeChat = !g_bOfficeChat;
	IFQDataPtr cfgPtr;//得到配置信息
	m_ptrRoot->get_UserCustomConfig(&cfgPtr);
	if(g_bOfficeChat)
	{
		cfgPtr->SetLong(L"OFFICECHAT", 1);
		SetWindowText("当前为办公对话模式--飞秋(通过发送下面箭头按钮可设置为普通对话模式");
	}
	else
	{
		cfgPtr->SetLong(L"OFFICECHAT", 0);
		SetWindowText("当前为普通对话模式--飞秋(通过发送下面箭头按钮可设置为办公对话模式");
	}
}

void COfficeChatDlg::OnUpdateSendMode(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(g_bOfficeChat);
if(g_bOfficeChat)
	{
		SetWindowText("当前为办公对话模式--飞秋(通过发送下面箭头按钮可设置为普通对话模式");
	}
	else
	{
		SetWindowText("当前为普通对话模式--飞秋(通过发送下面箭头按钮可设置为办公对话模式");
	}
}
