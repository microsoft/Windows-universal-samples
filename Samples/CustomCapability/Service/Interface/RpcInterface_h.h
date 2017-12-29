

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Mon Jan 18 19:14:07 2038
 */
/* Compiler settings for RpcInterface.Idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.01.0622 
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
#endif /* __RPCNDR_H_VERSION__ */


#ifndef __RpcInterface_h_h__
#define __RpcInterface_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

/* header files for imported files */
#include "oaidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __RpcInterface_INTERFACE_DEFINED__
#define __RpcInterface_INTERFACE_DEFINED__

/* interface RpcInterface */
/* [unique][version][uuid] */ 

typedef /* [context_handle_noserialize][context_handle] */ void *PCONTEXT_HANDLE_TYPE;

typedef /* [ref] */ PCONTEXT_HANDLE_TYPE *PPCONTEXT_HANDLE_TYPE;

void RemoteOpen( 
    /* [in] */ handle_t hBinding,
    /* [out] */ PPCONTEXT_HANDLE_TYPE pphContext);

void RemoteClose( 
    /* [out][in] */ PPCONTEXT_HANDLE_TYPE pphContext);

__int64 RegistryGetValue( 
    /* [in] */ PCONTEXT_HANDLE_TYPE phContext,
    /* [in] */ __int64 knownHKey,
    /* [string][in] */ wchar_t *subkey,
    /* [string][in] */ wchar_t *valueName,
    /* [in] */ unsigned long buffsize,
    /* [out] */ unsigned long *type,
    /* [size_is][out] */ byte *value,
    /* [out] */ unsigned long *valuesize);

__int64 RegistrySetValue( 
    /* [in] */ PCONTEXT_HANDLE_TYPE phContext,
    /* [in] */ __int64 hKey,
    /* [string][in] */ wchar_t *subkey,
    /* [string][in] */ wchar_t *valuename,
    /* [in] */ unsigned long type,
    /* [size_is][in] */ byte *value,
    /* [in] */ unsigned long valuesize);

void StartMetering( 
    /* [in] */ PCONTEXT_HANDLE_TYPE phContext,
    /* [in] */ __int64 samplePeriod,
    /* [optional][in] */ __int64 context);

void SetSamplePeriod( 
    /* [in] */ PCONTEXT_HANDLE_TYPE phContext,
    /* [in] */ __int64 samplePeriod);

void StopMetering( 
    /* [in] */ PCONTEXT_HANDLE_TYPE phContext);

/* [callback] */ void MeteringDataEvent( 
    /* [in] */ __int64 data,
    /* [optional][in] */ __int64 context);



extern RPC_IF_HANDLE RpcInterface_v1_0_c_ifspec;
extern RPC_IF_HANDLE RpcInterface_v1_0_s_ifspec;
#endif /* __RpcInterface_INTERFACE_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

void __RPC_USER PCONTEXT_HANDLE_TYPE_rundown( PCONTEXT_HANDLE_TYPE );

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


