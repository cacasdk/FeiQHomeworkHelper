

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0628 */
/* at Tue Jan 19 11:14:07 2038
 */
/* Compiler settings for CacaHomeworkHelper.odl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.01.0628 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */


#ifndef __CacaHomeworkHelper_h_h__
#define __CacaHomeworkHelper_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef DECLSPEC_XFGVIRT
#if defined(_CONTROL_FLOW_GUARD_XFG)
#define DECLSPEC_XFGVIRT(base, func) __declspec(xfg_virtual(base, func))
#else
#define DECLSPEC_XFGVIRT(base, func)
#endif
#endif

/* Forward Declarations */ 

#ifndef __ICacaHomeworkHelperModule_FWD_DEFINED__
#define __ICacaHomeworkHelperModule_FWD_DEFINED__
typedef interface ICacaHomeworkHelperModule ICacaHomeworkHelperModule;

#endif 	/* __ICacaHomeworkHelperModule_FWD_DEFINED__ */


#ifndef __CacaHomeworkHelperModule_FWD_DEFINED__
#define __CacaHomeworkHelperModule_FWD_DEFINED__

#ifdef __cplusplus
typedef class CacaHomeworkHelperModule CacaHomeworkHelperModule;
#else
typedef struct CacaHomeworkHelperModule CacaHomeworkHelperModule;
#endif /* __cplusplus */

#endif 	/* __CacaHomeworkHelperModule_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __CacaHomeworkHelper_LIBRARY_DEFINED__
#define __CacaHomeworkHelper_LIBRARY_DEFINED__

/* library CacaHomeworkHelper */
/* [version][uuid] */ 


DEFINE_GUID(LIBID_CacaHomeworkHelper,0x3317b89e,0x1ecd,0x4bf8,0xba,0x32,0xc7,0xbb,0x11,0x49,0xcb,0xce);

#ifndef __ICacaHomeworkHelperModule_DISPINTERFACE_DEFINED__
#define __ICacaHomeworkHelperModule_DISPINTERFACE_DEFINED__

/* dispinterface ICacaHomeworkHelperModule */
/* [uuid] */ 


DEFINE_GUID(DIID_ICacaHomeworkHelperModule,0xa63cb75b,0xd5bf,0x4a25,0xb4,0xa8,0x84,0x32,0x5c,0x3a,0x44,0x24);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("a63cb75b-d5bf-4a25-b4a8-84325c3a4424")
    ICacaHomeworkHelperModule : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct ICacaHomeworkHelperModuleVtbl
    {
        BEGIN_INTERFACE
        
        DECLSPEC_XFGVIRT(IUnknown, QueryInterface)
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICacaHomeworkHelperModule * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        DECLSPEC_XFGVIRT(IUnknown, AddRef)
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICacaHomeworkHelperModule * This);
        
        DECLSPEC_XFGVIRT(IUnknown, Release)
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICacaHomeworkHelperModule * This);
        
        DECLSPEC_XFGVIRT(IDispatch, GetTypeInfoCount)
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ICacaHomeworkHelperModule * This,
            /* [out] */ UINT *pctinfo);
        
        DECLSPEC_XFGVIRT(IDispatch, GetTypeInfo)
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ICacaHomeworkHelperModule * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        DECLSPEC_XFGVIRT(IDispatch, GetIDsOfNames)
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ICacaHomeworkHelperModule * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        DECLSPEC_XFGVIRT(IDispatch, Invoke)
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ICacaHomeworkHelperModule * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } ICacaHomeworkHelperModuleVtbl;

    interface ICacaHomeworkHelperModule
    {
        CONST_VTBL struct ICacaHomeworkHelperModuleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICacaHomeworkHelperModule_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICacaHomeworkHelperModule_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICacaHomeworkHelperModule_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICacaHomeworkHelperModule_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ICacaHomeworkHelperModule_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ICacaHomeworkHelperModule_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ICacaHomeworkHelperModule_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __ICacaHomeworkHelperModule_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_CacaHomeworkHelperModule,0x4ce3c885,0x1854,0x44af,0xba,0x63,0x7a,0x8c,0xec,0x4a,0xbc,0x41);

#ifdef __cplusplus

class DECLSPEC_UUID("4ce3c885-1854-44af-ba63-7a8cec4abc41")
CacaHomeworkHelperModule;
#endif
#endif /* __CacaHomeworkHelper_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


