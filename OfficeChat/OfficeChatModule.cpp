// OfficeChatModule.cpp : implementation file
//

#include "stdafx.h"
#include "OfficeChatModule.h"
#include "OfficeChatDlg.h"
#include "SetDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "..\include\FQEventSinkImp.h"
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")

COfficeChatDlg *pOfficeChatDlg = NULL;

// HTTP POST 到 Python 服务器
void SendToPythonServer(LPCTSTR Memo, LPCTSTR Host, LPCTSTR Group, LPCTSTR IP, LPCTSTR MAC, LPCTSTR sendMsg)
{
    // 构建 POST 数据 (只发送发件人和消息内容)
    CString postData;
    postData.Format(
        _T("sender=%s&msg=%s"),
        CW2A(Memo), CW2A(sendMsg)
    );

    // 创建 HTTP 会话
    HINTERNET hSession = WinHttpOpen(L"FeiQPlugin/1.0", 
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME, 
        WINHTTP_NO_PROXY_BYPASS, 
        0);

    if (hSession)
    {
        // 连接到服务器
        HINTERNET hConnect = WinHttpConnect(hSession, L"127.0.0.1", 51914, 0);  // 0xCACA
        
        if (hConnect)
        {
            // 创建请求
            HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", L"/msg",
                NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
            
            if (hRequest)
            {
                // 发送请求
                CStringW headers = L"Content-Type: application/x-www-form-urlencoded\r\n";
                WinHttpSendRequest(hRequest, headers, -1, 
                    (LPVOID)(CW2A(postData)), postData.GetLength(), 
                    postData.GetLength(), 0);
                
                WinHttpReceiveResponse(hRequest, NULL);
                
                // 读取响应（可选）
                DWORD dwSize = 0;
                WinHttpQueryDataAvailable(hRequest, &dwSize);
                
                if (dwSize > 0)
                {
                    char* buffer = new char[dwSize + 1];
                    WinHttpReadData(hRequest, buffer, dwSize, &dwSize);
                    buffer[dwSize] = '\0';
                    // 可以处理响应内容
                    delete[] buffer;
                }
                
                WinHttpCloseHandle(hRequest);
            }
            WinHttpCloseHandle(hConnect);
        }
        WinHttpCloseHandle(hSession);
    }
}
LOGFONT g_fontSend, g_fontRecv;
COLORREF g_clrSend = RGB(0, 0, 0), g_clrRecv = RGB(0, 0, 0);
BOOL g_bOfficeChat = FALSE;
int g_BeforeMsgMode = 2;
BOOL g_bSendMsgEnter = FALSE;
BOOL g_bSaveWndSize = TRUE;
int g_nWndWidth = 0, g_nWndHeight = 0;
int g_nShowNameMode = 1;
BOOL g_nUserChangeTip = TRUE;

IMPLEMENT_DYNCREATE(COfficeChatModule, CCmdTarget)
DELEGATE_DUAL_INTERFACE(COfficeChatModule, FQModule)
DELEGATE_DUAL_INTERFACE(COfficeChatModule, MenuSink)

COfficeChatModule::COfficeChatModule()
{
	m_AddMenuID5 = 0;
	m_ptrMenu = NULL;
	m_ptrModuleSite = NULL;
	m_ptrRoot = NULL;

	memset(&g_fontSend,0,sizeof(LOGFONT));
	memset(&g_fontRecv,0,sizeof(LOGFONT));

	g_fontSend.lfHeight = -16;
	g_fontSend.lfWidth = 0;
	g_fontSend.lfEscapement = 0;
	g_fontSend.lfOrientation = 0;
	g_fontSend.lfWeight = FW_MEDIUM;
	g_fontSend.lfItalic = FALSE;
	g_fontSend.lfUnderline = FALSE;
	g_fontSend.lfStrikeOut = FALSE;
	g_fontSend.lfCharSet = GB2312_CHARSET;
	g_fontSend.lfOutPrecision = OUT_DEFAULT_PRECIS;
	g_fontSend.lfClipPrecision = CLIP_LH_ANGLES;
	g_fontSend.lfQuality = DEFAULT_QUALITY;
	g_fontSend.lfPitchAndFamily =  FF_SWISS;
	strcpy(g_fontSend.lfFaceName, "宋体");
	g_fontRecv = g_fontSend;

	EnableAutomation();
	
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	
	AfxOleLockApp();
}

COfficeChatModule::~COfficeChatModule()
{
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.
	if(pOfficeChatDlg)
	{
		delete pOfficeChatDlg;
		pOfficeChatDlg = NULL;
	}
	AfxOleUnlockApp();
}


void COfficeChatModule::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(COfficeChatModule, CCmdTarget)
	//{{AFX_MSG_MAP(COfficeChatModule)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(COfficeChatModule, CCmdTarget)
	//{{AFX_DISPATCH_MAP(COfficeChatModule)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IOfficeChatModule to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

static const IID IID_IOfficeChatModule =
{ 0xfd4c7e5c, 0x2c5, 0x49ae, { 0x90, 0x49, 0x7c, 0x14, 0xe2, 0x54, 0x9c, 0xc } };

BEGIN_INTERFACE_MAP(COfficeChatModule, CCmdTarget)
	INTERFACE_PART(COfficeChatModule, IID_IOfficeChatModule, Dispatch)
	INTERFACE_PART(COfficeChatModule, IID_IFQModule, FQModule)
	INTERFACE_PART(COfficeChatModule, IID_IFQUICommand, MenuSink)
END_INTERFACE_MAP()


IMPLEMENT_OLECREATE(COfficeChatModule, "FeiQ.OfficeChatModule", 0x3a2e9929, 0x2633, 0x42e7, 0x84, 0x89, 0xe3, 0x90, 0x50, 0x82, 0x36, 0x67)

STDMETHODIMP COfficeChatModule::XFQModule::raw_OnModuleEvent(BSTR eventType, BSTR Name,BSTR sParam, BSTR* pVal)
{
	METHOD_PROLOGUE(COfficeChatModule, FQModule)

	return pThis->OnModuleEvent(eventType, Name, sParam, pVal);
}

STDMETHODIMP COfficeChatModule::XFQModule::raw_OnLoad(IDispatch* FQModuleSite)
{
	METHOD_PROLOGUE(COfficeChatModule, FQModule)

	return pThis->OnLoad(FQModuleSite);
}

STDMETHODIMP COfficeChatModule::XFQModule::raw_OnUnload()
{
	METHOD_PROLOGUE(COfficeChatModule, FQModule)

	return pThis->OnUnload();
}

STDMETHODIMP COfficeChatModule::XFQModule::raw_OnConfig()
{
	METHOD_PROLOGUE(COfficeChatModule, FQModule)

	return pThis->OnConfig();
}

STDMETHODIMP COfficeChatModule::XFQModule::get_Identifier(BSTR* pVal)
{
	METHOD_PROLOGUE(COfficeChatModule, FQModule)

	return pThis->get_Identifier(pVal);
}


STDMETHODIMP COfficeChatModule::XFQModule::get_Name(BSTR* pVal)
{
	METHOD_PROLOGUE(COfficeChatModule, FQModule)

	return pThis->get_Name(pVal);
}

STDMETHODIMP COfficeChatModule::XFQModule::get_Description(BSTR* pVal)
{
	METHOD_PROLOGUE(COfficeChatModule, FQModule)

	return pThis->get_Description(pVal);
}

STDMETHODIMP COfficeChatModule::XFQModule::get_Author(BSTR* pVal)
{
	METHOD_PROLOGUE(COfficeChatModule, FQModule)

	return pThis->get_Author(pVal);
}

HRESULT COfficeChatModule::get_Identifier(BSTR* pVal)
{
	if (NULL == pVal)
	{
		ASSERT(FALSE);
		return E_INVALIDARG;
	}

	*pVal = CString(FQ_MODULE_IDENTIFIER_PLUGINFEEDBACK).AllocSysString();

	return S_OK;
}

HRESULT COfficeChatModule::get_Name(BSTR* pVal)
{
	if (NULL == pVal)
	{
		ASSERT(FALSE);
		return E_INVALIDARG;
	}

	*pVal = CString(FQ_MODULE_NAME_PLUGINFEEDBACK).AllocSysString();

	return S_OK;
}

HRESULT COfficeChatModule::get_Description(BSTR* pVal)
{
	if (NULL == pVal)
	{
		ASSERT(FALSE);
		return E_INVALIDARG;
	}

	*pVal = CString(FQ_MODULE_DESCRIPTION_PLUGINFEEDBACK).AllocSysString();

	return S_OK;
}


HRESULT COfficeChatModule::get_Author(BSTR* pVal)
{
	if (NULL == pVal)
	{
		ASSERT(FALSE);
		return E_INVALIDARG;
	}

	*pVal = CString(FQ_MODULE_AUTHOR_PLUGINFEEDBACK).AllocSysString();

	return S_OK;
}

HRESULT COfficeChatModule::OnModuleEvent(BSTR eventType, BSTR Name, BSTR sParam, BSTR* pVal)
{
	if(CString(eventType) == "appbox.click")
	{
		if(!pOfficeChatDlg)
		{
			pOfficeChatDlg = new COfficeChatDlg;
		}
		if(!pOfficeChatDlg)
		{
			return S_FALSE;
		}
		if(!pOfficeChatDlg->m_hWnd)
		{
			HWND hDesktopWnd = GetDesktopWindow();
			CWnd *pDesktopWnd = CWnd::FromHandle(hDesktopWnd);
			pOfficeChatDlg->m_ptrRoot = m_ptrRoot;
			pOfficeChatDlg->Create(IDD_DIALOG_OFFICECHAT, pDesktopWnd);
		}
		if(pOfficeChatDlg->m_hWnd) pOfficeChatDlg->ShowWindow(SW_SHOW);
		pOfficeChatDlg->SetForegroundWindow();
	}
	*pVal = CString("").AllocSysString();
	return S_OK;
}

HRESULT COfficeChatModule::OnLoad(IDispatch* FQModuleSite)
{

	if (NULL == FQModuleSite)
	{
		ASSERT(FALSE);
		return E_INVALIDARG;
	}

	FQ_TRY
	{
		
		m_ptrModuleSite = FQModuleSite;
		m_ptrModuleSite->get_FQRoot(&m_ptrRoot);


		CComBSTR identeriferMenu("FeiQ.Menu");
		m_ptrMenu =  m_ptrRoot->Module[(_bstr_t)identeriferMenu];
		IFQUICommand* pUICmd = dynamic_cast<IFQUICommand* >(&m_xMenuSink);
		m_AddMenuID2 = m_ptrMenu->AddItem(FQ_UI_TYPE_TRAY_MENU, "软件帮助", pUICmd, "设置成办公对话模式", 100);
		m_AddMenuID = m_ptrMenu->AddItem(FQ_UI_TYPE_TRAY_MENU, "软件帮助", pUICmd, "办公模式对话框||", 101);

		m_AddMenuID3 = m_ptrMenu->AddItem(FQ_UI_TYPE_CHAT_PLUGIN, "", pUICmd, "||设置成办公对话模式", 100);
		m_AddMenuID4 = m_ptrMenu->AddItem(FQ_UI_TYPE_CHAT_PLUGIN, "", pUICmd, "办公模式对话框||", 101);
		/*
		菜单Text说明:
		形式如 ||测试Group||||测试 其中 || 和  可选。|| 表示分隔符。在名称前加 || 表示在添加的菜单项上面添加分隔符. 
		在名称前后 || 表示在添加的菜单项下面添加分隔符.  是上下级菜单的关系。例子中测试菜单的父菜单是测试Group。当测试Group不存在时，程序会自动创建这组菜单
		*/
		

		m_RootEventSink.HookEvent(evt_OnUserEnter, this, &COfficeChatModule::OnUserEnter);
		m_RootEventSink.HookEvent(evt_OnUserExit, this, &COfficeChatModule::OnUserExit);
		m_RootEventSink.HookEvent(evt_OnBeforeRecvMsg, this, &COfficeChatModule::OnBeforeRecvMsg);
		m_RootEventSink.HookEvent(evt_OnRootEvent, this, &COfficeChatModule::OnRootEvent);
	
		if (!m_RootEventSink.Advise(m_ptrRoot))
		{
			throw FQ_UNSPECIFIC_ERROR;
		}
		
		IFQDataPtr cfgPtr;//得到配置信息
		m_ptrRoot->get_UserCustomConfig(&cfgPtr);
		if(cfgPtr)
		{
			int nOfficeChat = cfgPtr->GetLong(L"OFFICECHAT");
			if(nOfficeChat == 1)
			{
				g_bOfficeChat = TRUE;
			}
			else
			{
				g_bOfficeChat = FALSE;
			}
			int nBufferLen = cfgPtr->GetBufferLength(L"BGCHAT_SENDFONT");
			if(nBufferLen == sizeof(LOGFONT))
			{
				cfgPtr->GetBuffer(L"BGCHAT_SENDFONT", (unsigned char *)&g_fontSend, sizeof(LOGFONT));
			}
			nBufferLen = cfgPtr->GetBufferLength(L"BGCHAT_RECVFONT");
			if(nBufferLen == sizeof(LOGFONT))
			{
				cfgPtr->GetBuffer(L"BGCHAT_RECVFONT", (unsigned char *)&g_fontRecv, sizeof(LOGFONT));
			}
			long clrSend = 0, clrRecv = 0;
			if(cfgPtr->raw_GetLong(L"BGCHAT_SENDCLR", &clrSend) == S_OK)
			{
				g_clrSend = (COLORREF)clrSend;
			}
			if(cfgPtr->raw_GetLong(L"BGCHAT_RECVCLR", &clrRecv) == S_OK)
			{
				g_clrRecv = (COLORREF)clrRecv;
			}
			long nMsgMode = 2;
			if(cfgPtr->raw_GetLong(L"BGCHAT_MSGMODE", &nMsgMode) == S_OK)
			{
				g_BeforeMsgMode = nMsgMode;
			}
			long nSendMsgEnter = 0;
			if(cfgPtr->raw_GetLong(L"BGCHAT_SENDMODE", &nSendMsgEnter) == S_OK)
			{
				g_bSendMsgEnter = nSendMsgEnter;
			}
			long nSaveWndSize = 0;
			if(cfgPtr->raw_GetLong(L"BGCHAT_SAVEWNDSIZE", &nSaveWndSize) == S_OK)
			{
				g_bSaveWndSize = nSaveWndSize;
			}
			long nWndWidth = 0;
			if(cfgPtr->raw_GetLong(L"BGCHAT_WNDWIDTH", &nWndWidth) == S_OK)
			{
				g_nWndWidth = nWndWidth;
			}
			long nWndHeight = 0;
			if(cfgPtr->raw_GetLong(L"BGCHAT_WNDHEGHT", &nWndHeight) == S_OK)
			{
				g_nWndHeight = nWndHeight;
			}
			long nShowNameMode = 1;
			if(cfgPtr->raw_GetLong(L"BGCHAT_SHOWNAMEMODE", &nShowNameMode) == S_OK)
			{
				g_nShowNameMode = nShowNameMode;
				if(g_nShowNameMode == 0) g_nShowNameMode = 1;
			}
			long nUserChangeTip = 1;
			if(cfgPtr->raw_GetLong(L"BGCHAT_USERCHANGETIP", &nUserChangeTip) == S_OK)
			{
				g_nUserChangeTip = nUserChangeTip;
			}
			cfgPtr->SetLong(L"OFFICECHAT", g_bOfficeChat);//保存进配置
			cfgPtr->SetBuffer(L"BGCHAT_SENDFONT", (unsigned char *)&g_fontSend, sizeof(LOGFONT));
			cfgPtr->SetBuffer(L"BGCHAT_RECVFONT", (unsigned char *)&g_fontRecv, sizeof(LOGFONT));
			cfgPtr->SetLong(L"BGCHAT_SENDCLR", g_clrSend);
			cfgPtr->SetLong(L"BGCHAT_RECVCLR", g_clrRecv);
			cfgPtr->SetLong(L"BGCHAT_MSGMODE", g_BeforeMsgMode);
			cfgPtr->SetLong(L"BGCHAT_SENDMODE", g_bSendMsgEnter);
			cfgPtr->SetLong(L"BGCHAT_SAVEWNDSIZE", g_bSaveWndSize);
			cfgPtr->SetLong(L"BGCHAT_WNDWIDTH", g_nWndWidth);
			cfgPtr->SetLong(L"BGCHAT_WNDHEGHT", g_nWndHeight);
			cfgPtr->SetLong(L"BGCHAT_SHOWNAMEMODE", g_nShowNameMode);
			cfgPtr->SetLong(L"BGCHAT_USERCHANGETIP", g_nUserChangeTip);
		}
		return S_OK;
	}
	FQ_CATCH_ALL(return E_FAIL)
}

HRESULT COfficeChatModule::OnUnload()
{
	IFQUICommand* pUICmd = dynamic_cast<IFQUICommand* >(&m_xMenuSink);
	if(m_ptrMenu)
	{
		m_ptrMenu->DelItem(FQ_UI_TYPE_TRAY_MENU, m_AddMenuID, pUICmd);
		m_ptrMenu->DelItem(FQ_UI_TYPE_TRAY_MENU, m_AddMenuID2, pUICmd);
		m_ptrMenu->DelItem(FQ_UI_TYPE_CHAT_PLUGIN, m_AddMenuID3, pUICmd);
		m_ptrMenu->DelItem(FQ_UI_TYPE_CHAT_PLUGIN, m_AddMenuID4, pUICmd);
	}


	m_RootEventSink.Unadvise();
	m_RootEventSink.UnhookEvent(evt_OnUserEnter, this, &COfficeChatModule::OnUserEnter);
	m_RootEventSink.UnhookEvent(evt_OnUserExit, this, &COfficeChatModule::OnUserExit);
	m_RootEventSink.UnhookEvent(evt_OnBeforeRecvMsg, this, &COfficeChatModule::OnBeforeRecvMsg);
	m_RootEventSink.UnhookEvent(evt_OnRootEvent, this, &COfficeChatModule::OnRootEvent);
	return S_OK;
}

HRESULT COfficeChatModule::OnConfig()
{
	//在这里可实现对插件功能的设置，及设置参数的保存及读取

	CSetDlg setDlg;
	setDlg.fontSend = g_fontSend;
	setDlg.fontRecv = g_fontRecv;
	setDlg.clrSend = g_clrSend;
	setDlg.clrRecv = g_clrRecv;
	setDlg.m_bOfficeChat = g_bOfficeChat;
	setDlg.m_nMsgMode = g_BeforeMsgMode;
	setDlg.m_bSendMsgEnter = g_bSendMsgEnter;
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
			pOfficeChatDlg->m_sendFont.DeleteObject();
			pOfficeChatDlg->m_recvFont.DeleteObject();
			pOfficeChatDlg->m_sendFont.CreateFontIndirect(&g_fontSend);
			pOfficeChatDlg->m_recvFont.CreateFontIndirect(&g_fontRecv);
			((CEdit*)pOfficeChatDlg->GetDlgItem(IDC_EDIT_SEND))->SetFont(&pOfficeChatDlg->m_sendFont, TRUE);
			((CEdit*)pOfficeChatDlg->GetDlgItem(IDC_EDIT_SHOWALL))->SetFont(&pOfficeChatDlg->m_recvFont, TRUE);
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
				cfgPtr->SetLong(L"BGCHAT_MSGMODE", g_BeforeMsgMode);
				cfgPtr->SetLong(L"BGCHAT_SENDMODE", g_bSendMsgEnter);
				cfgPtr->SetLong(L"BGCHAT_SAVEWNDSIZE", g_bSaveWndSize);
				cfgPtr->SetLong(L"BGCHAT_SHOWNAMEMODE", g_nShowNameMode);
				cfgPtr->SetLong(L"BGCHAT_USERCHANGETIP", g_nUserChangeTip);
			}
		}
	}
	return S_OK;
}



HRESULT COfficeChatModule::XMenuSink::raw_OnInvoke(enum FQ_UI_TYPE Type, long Id, VARIANT Parameter)
{	
	METHOD_PROLOGUE(COfficeChatModule, MenuSink)
	pThis->MenuOnInvoke(Type, Id, Parameter);
	return S_OK;	
}


HRESULT COfficeChatModule::XMenuSink::raw_OnQueryState(enum FQ_UI_TYPE Type, long Id, VARIANT Parameter,
                                 BSTR * bstrText,
                                 enum FQ_UI_ITEM_STATE * State )
{
	METHOD_PROLOGUE(COfficeChatModule, MenuSink)
	
	if(Id == pThis->m_AddMenuID2)
	{
		if(pThis->m_ptrRoot)
		{
			if(g_bOfficeChat)
			{
				//如果是办公对话模式
				*bstrText = CString("设置成普通对话模式").AllocSysString();
			}
			else
			{
				//如果是办公对话模式
				*bstrText = CString("设置成办公对话模式").AllocSysString();
			}
		}
	}
	else if(Id == pThis->m_AddMenuID3)
	{
		if(pThis->m_ptrRoot)
		{
			if(g_bOfficeChat)
			{
				//如果是办公对话模式
				*bstrText = CString("设置成普通对话模式").AllocSysString();
			}
			else
			{
				//如果是办公对话模式
				*bstrText = CString("设置成办公对话模式").AllocSysString();
			}
		}
	}
	return S_OK;
}

HRESULT COfficeChatModule::MenuOnInvoke(enum FQ_UI_TYPE Type, long Id, VARIANT Parameter )
{


	//点击自定义的菜单后，会触发此事件
	if(Id == m_AddMenuID || Id == m_AddMenuID4)
	{
		if(!pOfficeChatDlg)
		{
			pOfficeChatDlg = new COfficeChatDlg;
		}
		if(!pOfficeChatDlg)
		{
			return S_FALSE;
		}
		if(!pOfficeChatDlg->m_hWnd)
		{
			HWND hDesktopWnd = GetDesktopWindow();
			CWnd *pDesktopWnd = CWnd::FromHandle(hDesktopWnd);
			pOfficeChatDlg->m_ptrRoot = m_ptrRoot;
			pOfficeChatDlg->Create(IDD_DIALOG_OFFICECHAT, pDesktopWnd);
		}
		if(pOfficeChatDlg->m_hWnd) pOfficeChatDlg->ShowWindow(SW_SHOW);
		pOfficeChatDlg->SetForegroundWindow();
	}
	else if(Id == m_AddMenuID2 || Id == m_AddMenuID3)
	{
		IFQDataPtr cfgPtr;//得到配置信息
		m_ptrRoot->get_UserCustomConfig(&cfgPtr);
		int nOfficeChat = cfgPtr->GetLong(L"OFFICECHAT");
		if(nOfficeChat == 1)
		{
			cfgPtr->SetLong(L"OFFICECHAT", 0);
			g_bOfficeChat = FALSE;
		}
		else
		{
			cfgPtr->SetLong(L"OFFICECHAT", 1);
			g_bOfficeChat = TRUE;
		}
		if(pOfficeChatDlg && pOfficeChatDlg->m_hWnd)
		{
			if(g_bOfficeChat)
			{
				pOfficeChatDlg->SetWindowText("当前为办公对话模式--飞秋(通过发送下面箭头按钮可设置为普通对话模式");
			}
			else
			{
				pOfficeChatDlg->SetWindowText("当前为普通对话模式--飞秋(通过发送下面箭头按钮可设置为办公对话模式");
			}
		}
	}
	else if(Id == m_AddMenuID5)
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
				pOfficeChatDlg->m_sendFont.DeleteObject();
				pOfficeChatDlg->m_recvFont.DeleteObject();
				pOfficeChatDlg->m_sendFont.CreateFontIndirect(&g_fontSend);
				pOfficeChatDlg->m_recvFont.CreateFontIndirect(&g_fontRecv);
				((CEdit*)pOfficeChatDlg->GetDlgItem(IDC_EDIT_SEND))->SetFont(&pOfficeChatDlg->m_sendFont, TRUE);
				((CEdit*)pOfficeChatDlg->GetDlgItem(IDC_EDIT_SHOWALL))->SetFont(&pOfficeChatDlg->m_recvFont, TRUE);
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
				
				}
			}
		}
	}
	
	return S_OK;
}

HRESULT COfficeChatModule::OnUserEnter(LPCTSTR userMemoName, LPCTSTR userNickName, LPCTSTR userHostName, LPCTSTR userLoginName, LPCTSTR userMemoGroupName, LPCTSTR userGroupName, LPCTSTR userIP, LPCTSTR userMAC)
{
	//用户上线事件
	if(pOfficeChatDlg && pOfficeChatDlg->m_hWnd)
	{
		pOfficeChatDlg->AddBuddyToList(userMemoName, userHostName, userLoginName, userMemoGroupName, userGroupName, userIP);
	}
	return S_OK;
}
HRESULT COfficeChatModule::OnUserExit(LPCTSTR userMemoName, LPCTSTR userNickName, LPCTSTR userHostName, LPCTSTR userLoginName, LPCTSTR userMemoGroupName, LPCTSTR userGroupName, LPCTSTR userIP, LPCTSTR userMAC)
{
	//用户下线事件
	if(pOfficeChatDlg && pOfficeChatDlg->m_hWnd)
	{
		pOfficeChatDlg->DelBuddyFromList(userIP);
	}
	return S_OK;
}

HRESULT COfficeChatModule::OnBeforeRecvMsg(LPCTSTR Memo, LPCTSTR Host, LPCTSTR Group, LPCTSTR IP, LPCTSTR MAC, LPCTSTR sendMsg, LPCTSTR font, enum FQ_BEFORERECVMSG_RESULT *pResult)
{
	//接收到消息之前触发此事
	if(!g_bOfficeChat)
	{
		*pResult = FQ_BEFORERECVMSG_RESULT_NORMAL;
		return S_OK;
	}
	if(sendMsg == "")
	{
		*pResult = FQ_BEFORERECVMSG_RESULT_NORMAL;
		return S_OK;
	}
	
	// 调用 Python 服务器
	SendToPythonServer(Memo, Host, Group, IP, MAC, sendMsg);
	if(!pOfficeChatDlg)
	{
		pOfficeChatDlg = new COfficeChatDlg;
	}
	if(!pOfficeChatDlg)
	{
		*pResult = FQ_BEFORERECVMSG_RESULT_NORMAL;
		return S_OK;
	}
	BOOL bInitSelUser = FALSE;
	if(!pOfficeChatDlg->m_hWnd)
	{
		HWND hDesktopWnd = GetDesktopWindow();
		CWnd *pDesktopWnd = CWnd::FromHandle(hDesktopWnd);
		pOfficeChatDlg->m_ptrRoot = m_ptrRoot;
		pOfficeChatDlg->Create(IDD_DIALOG_OFFICECHAT, pDesktopWnd);
		bInitSelUser = TRUE;
	}
	CString strRecvMsg;
	COleDateTime dt = COleDateTime::GetCurrentTime();
	CString strUserName;
	if(g_nShowNameMode & 0x1)
	{
		strUserName = Memo;
	}
	if(g_nShowNameMode & 0x10)
	{
		if(strUserName != "")
		{
			strUserName += "(" + CString(Host) + ")";
		}
		else
		{
			strUserName += Host;
		}
	}
	if(g_nShowNameMode & 0x100)
	{
		if(strUserName != "")
		{
			strUserName += "(" + CString(Group) + ")";
		}
		else
		{
			strUserName += Group;
		}
	}
	if(g_nShowNameMode & 0x1000)
	{
		if(strUserName != "")
		{
			strUserName += "(" + CString(IP) + ")";
		}
		else
		{
			strUserName += IP;
		}
	}
	if(g_nShowNameMode & 0x10000)
	{
		if(strUserName != "")
		{
			strUserName += "(" + CString(MAC) + ")";
		}
		else
		{
			strUserName += MAC;
		}
	}
	strRecvMsg.Format("%s %s\r\n  %s\r\n", strUserName, dt.Format(_T("%H:%M:%S")), sendMsg);
	pOfficeChatDlg->RecvMsg(strRecvMsg, IP, bInitSelUser);
	if(pOfficeChatDlg->m_hWnd)
	{
		if(pOfficeChatDlg->IsWindowVisible())
		{
			pOfficeChatDlg->FlashWindow(TRUE);
		}
		else
		{
			pOfficeChatDlg->ShowWindow(SW_SHOW);
			pOfficeChatDlg->SetForegroundWindow();
		}
	}
	if(g_BeforeMsgMode == 0)
	{
		*pResult = FQ_BEFORERECVMSG_RESULT_NORMAL;
	}
	else if(g_BeforeMsgMode == 1)
	{
		*pResult = FQ_BEFORERECVMSG_RESULT_BLOCK_ALL;
	}
	else if(g_BeforeMsgMode == 2)
	{
		*pResult = FQ_BEFORERECVMSG_RESULT_BLOCK_SAVELOG;
	}
	else if(g_BeforeMsgMode == 3)
	{
		*pResult = FQ_BEFORERECVMSG_RESULT_BLOCK_SHOWTIP;
	}
	else if(g_BeforeMsgMode == 4)
	{
		*pResult = FQ_BEFORERECVMSG_RESULT_BLOCK_SAVELOG_SHOWTIP;
	}
	else
	{
		*pResult = FQ_BEFORERECVMSG_RESULT_NORMAL;
	}
	return S_OK;
}


HRESULT COfficeChatModule::OnRootEvent(LPCTSTR eventType, LPCTSTR Name, LPCTSTR sParam, IFQData* pData)
{
//	IFQDataPtr pFQData(pData);

	if(CString(eventType) == "AppBox.Menu")
	{
		if(CString(Name) == "BeginPopup")
		{
			if(m_AddMenuID5 != 0)
			{
				IFQUICommand* pUICmd = dynamic_cast<IFQUICommand* >(&m_xMenuSink);
				if(m_ptrMenu)
				{
					m_ptrMenu->DelItem(FQ_UI_TYPE_APPBOX_PLUGIN, m_AddMenuID5, pUICmd);
				}
				m_AddMenuID5 = 0;
			}
			CString strParam(sParam);
			CString strClsID(FQ_MODLE_CLSIDS);
			strParam.MakeLower();
			strClsID.MakeLower();
			if(strParam == strClsID)
			{
				CComBSTR identeriferMenu("FeiQ.Menu");
				m_ptrMenu =  m_ptrRoot->Module[(_bstr_t)identeriferMenu];
				IFQUICommand* pUICmd = dynamic_cast<IFQUICommand* >(&m_xMenuSink);
				m_AddMenuID5 = m_ptrMenu->AddItem(FQ_UI_TYPE_APPBOX_PLUGIN, "打开程序所在目录", pUICmd, "办公对话模式应用设置", 50);
			}
		}
	}
	return S_OK;
}


