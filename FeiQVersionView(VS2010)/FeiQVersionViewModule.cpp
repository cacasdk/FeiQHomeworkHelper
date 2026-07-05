// FeiQVersionViewModule.cpp : implementation file
//

#include "stdafx.h"
#include "FeiQVersionViewModule.h"
#include "FeiQVerDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "..\include\FQEventSinkImp.h"



IMPLEMENT_DYNCREATE(CFeiQVersionViewModule, CCmdTarget)
DELEGATE_DUAL_INTERFACE(CFeiQVersionViewModule, FQModule)

CFeiQVersionViewModule::CFeiQVersionViewModule()
{
	m_ptrModuleSite = NULL;
	m_ptrRoot = NULL;

	EnableAutomation();
	
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	
	AfxOleLockApp();
}

CFeiQVersionViewModule::~CFeiQVersionViewModule()
{
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.
	
	AfxOleUnlockApp();
}


void CFeiQVersionViewModule::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CFeiQVersionViewModule, CCmdTarget)
	//{{AFX_MSG_MAP(CFeiQVersionViewModule)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CFeiQVersionViewModule, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CFeiQVersionViewModule)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IFeiQVersionViewModule to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

static const IID IID_IFeiQVersionViewModule =
{ 0xe4ac2f5c, 0x7066, 0x466f, { 0x87, 0xdf, 0xaf, 0xe, 0xf4, 0x88, 0x54, 0x5 } };

BEGIN_INTERFACE_MAP(CFeiQVersionViewModule, CCmdTarget)
	INTERFACE_PART(CFeiQVersionViewModule, IID_IFeiQVersionViewModule, Dispatch)
	INTERFACE_PART(CFeiQVersionViewModule, IID_IFQModule, FQModule)
END_INTERFACE_MAP()


IMPLEMENT_OLECREATE(CFeiQVersionViewModule, "FeiQ.FeiQVersionViewModule", 0xfd8bc272, 0x1b6d, 0x4738, 0x9d, 0x25, 0x8e, 0xc0, 0xdb, 0x2e, 0x78, 0xd2)

STDMETHODIMP CFeiQVersionViewModule::XFQModule::raw_OnModuleEvent(BSTR eventType, BSTR Name,BSTR sParam, BSTR* pVal)
{
	METHOD_PROLOGUE(CFeiQVersionViewModule, FQModule)

	return pThis->OnModuleEvent(eventType, Name, sParam, pVal);
}

STDMETHODIMP CFeiQVersionViewModule::XFQModule::raw_OnLoad(IDispatch* FQModuleSite)
{
	METHOD_PROLOGUE(CFeiQVersionViewModule, FQModule)

	return pThis->OnLoad(FQModuleSite);
}

STDMETHODIMP CFeiQVersionViewModule::XFQModule::raw_OnUnload()
{
	METHOD_PROLOGUE(CFeiQVersionViewModule, FQModule)

	return pThis->OnUnload();
}

STDMETHODIMP CFeiQVersionViewModule::XFQModule::raw_OnConfig()
{
	METHOD_PROLOGUE(CFeiQVersionViewModule, FQModule)

	return pThis->OnConfig();
}

STDMETHODIMP CFeiQVersionViewModule::XFQModule::get_Identifier(BSTR* pVal)
{
	METHOD_PROLOGUE(CFeiQVersionViewModule, FQModule)

	return pThis->get_Identifier(pVal);
}


STDMETHODIMP CFeiQVersionViewModule::XFQModule::get_Name(BSTR* pVal)
{
	METHOD_PROLOGUE(CFeiQVersionViewModule, FQModule)

	return pThis->get_Name(pVal);
}

STDMETHODIMP CFeiQVersionViewModule::XFQModule::get_Description(BSTR* pVal)
{
	METHOD_PROLOGUE(CFeiQVersionViewModule, FQModule)

	return pThis->get_Description(pVal);
}

STDMETHODIMP CFeiQVersionViewModule::XFQModule::get_Author(BSTR* pVal)
{
	METHOD_PROLOGUE(CFeiQVersionViewModule, FQModule)

	return pThis->get_Author(pVal);
}

HRESULT CFeiQVersionViewModule::get_Identifier(BSTR* pVal)
{
	if (NULL == pVal)
	{
		ASSERT(FALSE);
		return E_INVALIDARG;
	}

	*pVal = CString(FQ_MODULE_IDENTIFIER_PLUGINFEEDBACK).AllocSysString();

	return S_OK;
}

HRESULT CFeiQVersionViewModule::get_Name(BSTR* pVal)
{
	if (NULL == pVal)
	{
		ASSERT(FALSE);
		return E_INVALIDARG;
	}

	*pVal = CString(FQ_MODULE_NAME_PLUGINFEEDBACK).AllocSysString();

	return S_OK;
}

HRESULT CFeiQVersionViewModule::get_Description(BSTR* pVal)
{
	if (NULL == pVal)
	{
		ASSERT(FALSE);
		return E_INVALIDARG;
	}

	*pVal = CString(FQ_MODULE_DESCRIPTION_PLUGINFEEDBACK).AllocSysString();

	return S_OK;
}


HRESULT CFeiQVersionViewModule::get_Author(BSTR* pVal)
{
	if (NULL == pVal)
	{
		ASSERT(FALSE);
		return E_INVALIDARG;
	}

	*pVal = CString(FQ_MODULE_AUTHOR_PLUGINFEEDBACK).AllocSysString();

	return S_OK;
}

HRESULT CFeiQVersionViewModule::OnLoad(IDispatch* FQModuleSite)
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

		return S_OK;
	}
	FQ_CATCH_ALL(return E_FAIL)
}

HRESULT CFeiQVersionViewModule::OnUnload()
{

	return S_OK;
}

HRESULT CFeiQVersionViewModule::OnModuleEvent(BSTR eventType, BSTR Name, BSTR sParam, BSTR* pVal)
{
	//事件触发
	if(CString(eventType) == "appbox.click")
	{
		CFeiQVerDlg *pFeiQVerDlg = new CFeiQVerDlg;
		pFeiQVerDlg->m_ptrRoot = m_ptrRoot;
		pFeiQVerDlg->Create(IDD_DIALOG_FEIQVER, CWnd::FromHandle(GetDesktopWindow()));
		pFeiQVerDlg->ShowWindow(SW_SHOW);
	}

	*pVal = CString("").AllocSysString();
	
	return S_OK;
}

HRESULT CFeiQVersionViewModule::OnConfig()
{
	//在这里可实现对插件功能的设置，及设置参数的保存及读取
	return S_OK;
}




