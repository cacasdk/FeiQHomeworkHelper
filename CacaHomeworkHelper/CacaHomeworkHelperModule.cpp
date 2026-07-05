// CacaHomeworkHelperModule.cpp : implementation file
//

#include "stdafx.h"
#include "CacaHomeworkHelperModule.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "..\include\FQEventSinkImp.h"



IMPLEMENT_DYNCREATE(CCacaHomeworkHelperModule, CCmdTarget)
DELEGATE_DUAL_INTERFACE(CCacaHomeworkHelperModule, FQModule)
DELEGATE_DUAL_INTERFACE(CCacaHomeworkHelperModule, MenuSink)

CCacaHomeworkHelperModule::CCacaHomeworkHelperModule()
{
	m_ptrMenu = NULL;
	m_ptrModuleSite = NULL;
	m_ptrRoot = NULL;

	EnableAutomation();
	
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	
	AfxOleLockApp();
}

CCacaHomeworkHelperModule::~CCacaHomeworkHelperModule()
{
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.
	
	AfxOleUnlockApp();
}


void CCacaHomeworkHelperModule::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CCacaHomeworkHelperModule, CCmdTarget)
	//{{AFX_MSG_MAP(CCacaHomeworkHelperModule)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CCacaHomeworkHelperModule, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CCacaHomeworkHelperModule)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ICacaHomeworkHelperModule to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

static const IID IID_ICacaHomeworkHelperModule =
{ 0xa63cb75b, 0xd5bf, 0x4a25, { 0xb4, 0xa8, 0x84, 0x32, 0x5c, 0x3a, 0x44, 0x24 } };

BEGIN_INTERFACE_MAP(CCacaHomeworkHelperModule, CCmdTarget)
	INTERFACE_PART(CCacaHomeworkHelperModule, IID_ICacaHomeworkHelperModule, Dispatch)
	INTERFACE_PART(CCacaHomeworkHelperModule, IID_IFQModule, FQModule)
	INTERFACE_PART(CCacaHomeworkHelperModule, IID_IFQUICommand, MenuSink)
END_INTERFACE_MAP()


IMPLEMENT_OLECREATE(CCacaHomeworkHelperModule, "FeiQ.CacaHomeworkHelperModule", 0x4ce3c885, 0x1854, 0x44af, 0xba, 0x63, 0x7a, 0x8c, 0xec, 0x4a, 0xbc, 0x41)

STDMETHODIMP CCacaHomeworkHelperModule::XFQModule::raw_OnModuleEvent(BSTR eventType, BSTR Name,BSTR sParam, BSTR* pVal)
{
	METHOD_PROLOGUE(CCacaHomeworkHelperModule, FQModule)

	return pThis->OnModuleEvent(eventType, Name, sParam, pVal);
}

STDMETHODIMP CCacaHomeworkHelperModule::XFQModule::raw_OnLoad(IDispatch* FQModuleSite)
{
	METHOD_PROLOGUE(CCacaHomeworkHelperModule, FQModule)

	return pThis->OnLoad(FQModuleSite);
}

STDMETHODIMP CCacaHomeworkHelperModule::XFQModule::raw_OnUnload()
{
	METHOD_PROLOGUE(CCacaHomeworkHelperModule, FQModule)

	return pThis->OnUnload();
}

STDMETHODIMP CCacaHomeworkHelperModule::XFQModule::raw_OnConfig()
{
	METHOD_PROLOGUE(CCacaHomeworkHelperModule, FQModule)

	return pThis->OnConfig();
}

STDMETHODIMP CCacaHomeworkHelperModule::XFQModule::get_Identifier(BSTR* pVal)
{
	METHOD_PROLOGUE(CCacaHomeworkHelperModule, FQModule)

	return pThis->get_Identifier(pVal);
}


STDMETHODIMP CCacaHomeworkHelperModule::XFQModule::get_Name(BSTR* pVal)
{
	METHOD_PROLOGUE(CCacaHomeworkHelperModule, FQModule)

	return pThis->get_Name(pVal);
}

STDMETHODIMP CCacaHomeworkHelperModule::XFQModule::get_Description(BSTR* pVal)
{
	METHOD_PROLOGUE(CCacaHomeworkHelperModule, FQModule)

	return pThis->get_Description(pVal);
}

STDMETHODIMP CCacaHomeworkHelperModule::XFQModule::get_Author(BSTR* pVal)
{
	METHOD_PROLOGUE(CCacaHomeworkHelperModule, FQModule)

	return pThis->get_Author(pVal);
}

HRESULT CCacaHomeworkHelperModule::get_Identifier(BSTR* pVal)
{
	if (NULL == pVal)
	{
		ASSERT(FALSE);
		return E_INVALIDARG;
	}

	*pVal = CString(FQ_MODULE_IDENTIFIER_PLUGINFEEDBACK).AllocSysString();

	return S_OK;
}

HRESULT CCacaHomeworkHelperModule::get_Name(BSTR* pVal)
{
	if (NULL == pVal)
	{
		ASSERT(FALSE);
		return E_INVALIDARG;
	}

	*pVal = CString(FQ_MODULE_NAME_PLUGINFEEDBACK).AllocSysString();

	return S_OK;
}

HRESULT CCacaHomeworkHelperModule::get_Description(BSTR* pVal)
{
	if (NULL == pVal)
	{
		ASSERT(FALSE);
		return E_INVALIDARG;
	}

	*pVal = CString(FQ_MODULE_DESCRIPTION_PLUGINFEEDBACK).AllocSysString();

	return S_OK;
}


HRESULT CCacaHomeworkHelperModule::get_Author(BSTR* pVal)
{
	if (NULL == pVal)
	{
		ASSERT(FALSE);
		return E_INVALIDARG;
	}

	*pVal = CString(FQ_MODULE_AUTHOR_PLUGINFEEDBACK).AllocSysString();

	return S_OK;
}

HRESULT CCacaHomeworkHelperModule::OnLoad(IDispatch* FQModuleSite)
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
		m_AddMenuID = m_ptrMenu->AddItem(FQ_UI_TYPE_TRAY_MENU, "退出(&C)", pUICmd, "CacaHomeworkHelper||", 0);
		/*
		菜单Text说明:
		形式如 ||测试Group||@@||测试 其中 || 和 @@ 可选。|| 表示分隔符。在名称前加 || 表示在添加的菜单项上面添加分隔符. 
		在名称前后 || 表示在添加的菜单项下面添加分隔符. @@ 是上下级菜单的关系。例子中测试菜单的父菜单是测试Group。当测试Group不存在时，程序会自动创建这组菜单
		*/
		
		m_ModuleSiteEventSink.HookEvent(evt_OnDataReceived, this, &CCacaHomeworkHelperModule::OnDataReceived);
		m_ModuleSiteEventSink.HookEvent(evt_OnSendDataResult, this, &CCacaHomeworkHelperModule::OnSendDataResult);
		m_ModuleSiteEventSink.HookEvent(evt_OnViewData, this, &CCacaHomeworkHelperModule::OnViewData);
	
		if (!m_ModuleSiteEventSink.Advise(m_ptrModuleSite))
		{
			throw FQ_UNSPECIFIC_ERROR;
		}

		m_RootEventSink.HookEvent(evt_OnUserEnter, this, &CCacaHomeworkHelperModule::OnUserEnter);
		m_RootEventSink.HookEvent(evt_OnUserExit, this, &CCacaHomeworkHelperModule::OnUserExit);
		m_RootEventSink.HookEvent(evt_OnBeforeRecvMsg, this, &CCacaHomeworkHelperModule::OnBeforeRecvMsg);
		m_RootEventSink.HookEvent(evt_OnSkinChange2, this, &CCacaHomeworkHelperModule::OnSkinChange2);
		m_RootEventSink.HookEvent(evt_OnMainPanelChange, this, &CCacaHomeworkHelperModule::OnMainPanelChange);
		m_RootEventSink.HookEvent(evt_OnCustomButtonClick, this, &CCacaHomeworkHelperModule::OnCustomButtonClick);
		m_RootEventSink.HookEvent(evt_OnRootEvent, this, &CCacaHomeworkHelperModule::OnRootEvent);

		if (!m_RootEventSink.Advise(m_ptrRoot))
		{
			throw FQ_UNSPECIFIC_ERROR;
		}
		
		return S_OK;
	}
	FQ_CATCH_ALL(return E_FAIL)
}

HRESULT CCacaHomeworkHelperModule::OnUnload()
{
	IFQUICommand* pUICmd = dynamic_cast<IFQUICommand* >(&m_xMenuSink);
	if(m_ptrMenu)
	{
		m_ptrMenu->DelItem(FQ_UI_TYPE_TRAY_MENU, m_AddMenuID, pUICmd);
		
	}

	m_ModuleSiteEventSink.Unadvise();
	m_ModuleSiteEventSink.UnhookEvent(evt_OnDataReceived, this, &CCacaHomeworkHelperModule::OnDataReceived);
	m_ModuleSiteEventSink.UnhookEvent(evt_OnViewData, this, &CCacaHomeworkHelperModule::OnViewData);

	m_RootEventSink.Unadvise();
	m_RootEventSink.UnhookEvent(evt_OnUserEnter, this, &CCacaHomeworkHelperModule::OnUserEnter);
	m_RootEventSink.UnhookEvent(evt_OnUserExit, this, &CCacaHomeworkHelperModule::OnUserExit);
	m_RootEventSink.UnhookEvent(evt_OnBeforeRecvMsg, this, &CCacaHomeworkHelperModule::OnBeforeRecvMsg);
	m_RootEventSink.UnhookEvent(evt_OnSkinChange2, this, &CCacaHomeworkHelperModule::OnSkinChange2);
	m_RootEventSink.UnhookEvent(evt_OnMainPanelChange, this, &CCacaHomeworkHelperModule::OnMainPanelChange);
	m_RootEventSink.UnhookEvent(evt_OnCustomButtonClick, this, &CCacaHomeworkHelperModule::OnCustomButtonClick);
	m_RootEventSink.UnhookEvent(evt_OnRootEvent, this, &CCacaHomeworkHelperModule::OnRootEvent);
	return S_OK;
}

HRESULT CCacaHomeworkHelperModule::OnModuleEvent(BSTR eventType, BSTR Name, BSTR sParam, BSTR* pVal)
{
	//事件触发
	*pVal = CString("").AllocSysString();
	return S_OK;
}

HRESULT CCacaHomeworkHelperModule::OnConfig()
{
	//在这里可实现对插件功能的设置，及设置参数的保存及读取
	return S_OK;
}


HRESULT CCacaHomeworkHelperModule::OnSendDataResult(LPCTSTR Sender, LPCTSTR key, enum FQ_SENDDATA_RESULT nResult)
{
	//当发送数据后，有返回结果的时候，激发此事件
	return S_OK;
}

HRESULT CCacaHomeworkHelperModule::OnDataReceived(LPCTSTR key)
{
	//当接收到数据包的时候，激发此事件
	return S_OK;
}

HRESULT CCacaHomeworkHelperModule::OnViewData(LPCTSTR key, LPCTSTR Result)
{
	//当在OnDataReceived 中，没有将Key中的数据包从队列中删除的话，将弹出一个提示框，用户点击提示框的按钮时，将触发OnViewData事件
	return S_OK;
}

HRESULT CCacaHomeworkHelperModule::XMenuSink::raw_OnInvoke(enum FQ_UI_TYPE Type, long Id, VARIANT Parameter)
{	
	METHOD_PROLOGUE(CCacaHomeworkHelperModule, MenuSink)
	pThis->MenuOnInvoke(Type, Id, Parameter);
	return S_OK;	
}


HRESULT CCacaHomeworkHelperModule::XMenuSink::raw_OnQueryState(enum FQ_UI_TYPE Type, long Id, VARIANT Parameter,
                                 BSTR * bstrText,
                                 enum FQ_UI_ITEM_STATE * State )
{
	METHOD_PROLOGUE(CCacaHomeworkHelperModule, MenuSink)
	
	return S_OK;
}

HRESULT CCacaHomeworkHelperModule::MenuOnInvoke(enum FQ_UI_TYPE Type, long Id, VARIANT Parameter )
{
	//点击自定义的菜单后，会触发此事件
	return S_OK;
}

HRESULT CCacaHomeworkHelperModule::OnUserEnter(LPCTSTR userMemoName, LPCTSTR userNickName, LPCTSTR userHostName, LPCTSTR userLoginName, LPCTSTR userMemoGroupName, LPCTSTR userGroupName, LPCTSTR userIP, LPCTSTR userMAC)
{
	//用户上线事件
	return S_OK;
}
HRESULT CCacaHomeworkHelperModule::OnUserExit(LPCTSTR userMemoName, LPCTSTR userNickName, LPCTSTR userHostName, LPCTSTR userLoginName, LPCTSTR userMemoGroupName, LPCTSTR userGroupName, LPCTSTR userIP, LPCTSTR userMAC)
{
	//用户下线事件
	return S_OK;
}

HRESULT CCacaHomeworkHelperModule::OnBeforeRecvMsg(LPCTSTR Memo, LPCTSTR Host, LPCTSTR Group, LPCTSTR IP, LPCTSTR MAC, LPCTSTR sendMsg, LPCTSTR font, enum FQ_BEFORERECVMSG_RESULT *pResult)
{
	//接收到消息之前触发此事
	*pResult = FQ_BEFORERECVMSG_RESULT_NORMAL;
	return S_OK;
}

HRESULT CCacaHomeworkHelperModule::OnSkinChange2(COLORREF clrRgb)
{
	//皮肤颜色改变事件
	return S_OK;
}

HRESULT CCacaHomeworkHelperModule::OnMainPanelChange(LPCTSTR HidePanelName, HWND hHideWnd, LPCTSTR ShowPanelName, HWND hShowWnd)
{
	//主对话框主面板改变事件
	return S_OK;
}

HRESULT CCacaHomeworkHelperModule::OnCustomButtonClick(LPCTSTR ButtonName)
{
	//主对话框中自定义按钮点击事件
	return S_OK;
}

HRESULT CCacaHomeworkHelperModule::OnRootEvent(LPCTSTR eventType, LPCTSTR Name, LPCTSTR sParam, IFQData* pData)
{
	//主对话框中自定义按钮点击事件
	return S_OK;
}



