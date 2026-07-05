

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Sat Mar 23 22:39:18 2013
 */
/* Compiler settings for FeiQVersionView.odl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __FeiQVersionView_h_h__
#define __FeiQVersionView_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IFeiQVersionViewModule_FWD_DEFINED__
#define __IFeiQVersionViewModule_FWD_DEFINED__
typedef interface IFeiQVersionViewModule IFeiQVersionViewModule;
#endif 	/* __IFeiQVersionViewModule_FWD_DEFINED__ */


#ifndef __FeiQVersionViewModule_FWD_DEFINED__
#define __FeiQVersionViewModule_FWD_DEFINED__

#ifdef __cplusplus
typedef class FeiQVersionViewModule FeiQVersionViewModule;
#else
typedef struct FeiQVersionViewModule FeiQVersionViewModule;
#endif /* __cplusplus */

#endif 	/* __FeiQVersionViewModule_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __FeiQVersionView_LIBRARY_DEFINED__
#define __FeiQVersionView_LIBRARY_DEFINED__

/* library FeiQVersionView */
/* [version][uuid] */ 


DEFINE_GUID(LIBID_FeiQVersionView,0x7418a42c,0x589f,0x41e4,0xa5,0xf4,0xe0,0x8a,0xa5,0xa1,0x93,0x54);

#ifndef __IFeiQVersionViewModule_DISPINTERFACE_DEFINED__
#define __IFeiQVersionViewModule_DISPINTERFACE_DEFINED__

/* dispinterface IFeiQVersionViewModule */
/* [uuid] */ 


DEFINE_GUID(DIID_IFeiQVersionViewModule,0xe4ac2f5c,0x7066,0x466f,0x87,0xdf,0xaf,0x0e,0xf4,0x88,0x54,0x05);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("e4ac2f5c-7066-466f-87df-af0ef4885405")
    IFeiQVersionViewModule : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IFeiQVersionViewModuleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFeiQVersionViewModule * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFeiQVersionViewModule * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFeiQVersionViewModule * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IFeiQVersionViewModule * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IFeiQVersionViewModule * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IFeiQVersionViewModule * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IFeiQVersionViewModule * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IFeiQVersionViewModuleVtbl;

    interface IFeiQVersionViewModule
    {
        CONST_VTBL struct IFeiQVersionViewModuleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFeiQVersionViewModule_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFeiQVersionViewModule_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFeiQVersionViewModule_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFeiQVersionViewModule_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IFeiQVersionViewModule_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IFeiQVersionViewModule_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IFeiQVersionViewModule_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IFeiQVersionViewModule_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_FeiQVersionViewModule,0xfd8bc272,0x1b6d,0x4738,0x9d,0x25,0x8e,0xc0,0xdb,0x2e,0x78,0xd2);

#ifdef __cplusplus

class DECLSPEC_UUID("fd8bc272-1b6d-4738-9d25-8ec0db2e78d2")
FeiQVersionViewModule;
#endif
#endif /* __FeiQVersionView_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


