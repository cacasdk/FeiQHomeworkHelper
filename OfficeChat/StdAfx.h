#pragma once


#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#import "..\tlb\FeiQ.tlb" no_namespace named_guids 

#include "..\include\EventSupport.h"
#include "..\include\mfcdual.h"
#include "..\include\FQMisc.h"
#include "..\include\FQDataKeys.h"


#define FQ_MODULE_IDENTIFIER_PLUGINFEEDBACK	_T("FeiQ.OfficeChat")
#define FQ_MODULE_NAME_PLUGINFEEDBACK		_T("办公模式对话")
#define FQ_MODULE_DESCRIPTION_PLUGINFEEDBACK	_T("界面简单,适用于在办公模式下进行对话")
#define FQ_MODULE_AUTHOR_PLUGINFEEDBACK	_T("卢本陶")

#define FQ_MODLE_CLSIDS _T("{3a2e9929-2633-42e7-8489-e39050823667}")//多个clsid之间用 | 隔开
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

