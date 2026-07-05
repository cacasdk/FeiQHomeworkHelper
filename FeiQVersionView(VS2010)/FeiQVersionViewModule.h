#pragma once

// FeiQVersionViewModule.h : header file
//


#include "..\include\FQEventSinkInc.h"
/////////////////////////////////////////////////////////////////////////////
// CFeiQVersionViewModule command target

class CFeiQVersionViewModule : public CCmdTarget
{
	DECLARE_DYNCREATE(CFeiQVersionViewModule)
	DECLARE_EVENT_RECEIVER(CFeiQVersionViewModule)

	CFeiQVersionViewModule();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFeiQVersionViewModule)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CFeiQVersionViewModule();

	// Generated message map functions
	//{{AFX_MSG(CFeiQVersionViewModule)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	BEGIN_DUAL_INTERFACE_PART(FQModule, IFQModule)
	STDMETHOD(raw_OnModuleEvent)(/*[in]*/ BSTR eventType, /*[in]*/ BSTR Name, /*[in]*/ BSTR sParam, /*[out, retval]*/ BSTR* pVal);
	STDMETHOD(raw_OnLoad)(/*[in]*/ IDispatch* FQModuleSite);
	STDMETHOD(raw_OnUnload)();
	STDMETHOD(raw_OnConfig)();
	STDMETHOD(get_Identifier)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_Name)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_Description)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_Author)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_ModuleSite)(/*[out, retval]*/ IDispatch* *pVal)
	{
		METHOD_PROLOGUE(CFeiQVersionViewModule, FQModule)
		
		FQ_CHECK_INVALIDARG_NULL(pVal);
		
		*pVal = pThis->m_ptrModuleSite;

		if (*pVal != NULL)
		{
			(*pVal)->AddRef();
		}

		return *pVal != NULL ? S_OK : E_FAIL;
	}
	END_DUAL_INTERFACE_PART(FQModule)	


	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CFeiQVersionViewModule)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CFeiQVersionViewModule)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

private:
	IFQModuleSitePtr m_ptrModuleSite;
	IFQRootPtr m_ptrRoot;


public:
	HRESULT OnModuleEvent(BSTR eventType, BSTR Name, BSTR sParam, BSTR* pVal);
	HRESULT OnLoad(IDispatch* FQModuleSite);
	HRESULT OnUnload();
	HRESULT OnConfig();
	HRESULT get_Identifier(BSTR* pVal);
	HRESULT get_Name(BSTR* pVal);
	HRESULT get_Description(BSTR* pVal);
	HRESULT get_Author(BSTR* pVal);


};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

