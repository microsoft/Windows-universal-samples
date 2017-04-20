

/* this ALWAYS GENERATED file contains the RPC client stubs */


 /* File created by MIDL compiler version 8.01.0620 */
/* at Mon Jan 18 19:14:07 2038
 */
/* Compiler settings for RpcInterface.Idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0620 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#if defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/

#include <string.h>

#include "RpcInterface_h.h"

#define TYPE_FORMAT_STRING_SIZE   63                                
#define PROC_FORMAT_STRING_SIZE   411                               
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _RpcInterface_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } RpcInterface_MIDL_TYPE_FORMAT_STRING;

typedef struct _RpcInterface_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } RpcInterface_MIDL_PROC_FORMAT_STRING;

typedef struct _RpcInterface_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } RpcInterface_MIDL_EXPR_FORMAT_STRING;


static const RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const RpcInterface_MIDL_TYPE_FORMAT_STRING RpcInterface__MIDL_TypeFormatString;
extern const RpcInterface_MIDL_PROC_FORMAT_STRING RpcInterface__MIDL_ProcFormatString;
extern const RpcInterface_MIDL_EXPR_FORMAT_STRING RpcInterface__MIDL_ExprFormatString;

#define GENERIC_BINDING_TABLE_SIZE   0            


/* Standard interface: RpcInterface, ver. 1.0,
   GUID={0x906B0CE0,0xC70B,0x1067,{0xB3,0x17,0x00,0xDD,0x01,0x06,0x62,0xDA}} */


extern const MIDL_SERVER_INFO RpcInterface_ServerInfo;


extern const RPC_DISPATCH_TABLE RpcInterface_v1_0_DispatchTable;

static const RPC_CLIENT_INTERFACE RpcInterface___RpcClientInterface =
    {
    sizeof(RPC_CLIENT_INTERFACE),
    {{0x906B0CE0,0xC70B,0x1067,{0xB3,0x17,0x00,0xDD,0x01,0x06,0x62,0xDA}},{1,0}},
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    (RPC_DISPATCH_TABLE*)&RpcInterface_v1_0_DispatchTable,
    0,
    0,
    0,
    &RpcInterface_ServerInfo,
    0x04000000
    };
RPC_IF_HANDLE RpcInterface_v1_0_c_ifspec = (RPC_IF_HANDLE)& RpcInterface___RpcClientInterface;

extern const MIDL_STUB_DESC RpcInterface_StubDesc;

static RPC_BINDING_HANDLE RpcInterface__MIDL_AutoBindHandle;


void RemoteOpen( 
    /* [in] */ handle_t hBinding,
    /* [out] */ PPCONTEXT_HANDLE_TYPE pphContext)
{

    NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&RpcInterface_StubDesc,
                  (PFORMAT_STRING) &RpcInterface__MIDL_ProcFormatString.Format[0],
                  hBinding,
                  pphContext);
    
}


void RemoteClose( 
    /* [out][in] */ PPCONTEXT_HANDLE_TYPE pphContext)
{

    NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&RpcInterface_StubDesc,
                  (PFORMAT_STRING) &RpcInterface__MIDL_ProcFormatString.Format[36],
                  pphContext);
    
}


__int64 RegistryGetValue( 
    /* [in] */ PCONTEXT_HANDLE_TYPE phContext,
    /* [in] */ __int64 knownHKey,
    /* [string][in] */ wchar_t *subkey,
    /* [string][in] */ wchar_t *valueName,
    /* [in] */ unsigned long buffsize,
    /* [out] */ unsigned long *type,
    /* [size_is][out] */ byte *value,
    /* [out] */ unsigned long *valuesize)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&RpcInterface_StubDesc,
                  (PFORMAT_STRING) &RpcInterface__MIDL_ProcFormatString.Format[74],
                  phContext,
                  knownHKey,
                  subkey,
                  valueName,
                  buffsize,
                  type,
                  value,
                  valuesize);
    return ( __int64  )_RetVal.Simple;
    
}


__int64 RegistrySetValue( 
    /* [in] */ PCONTEXT_HANDLE_TYPE phContext,
    /* [in] */ __int64 hKey,
    /* [string][in] */ wchar_t *subkey,
    /* [string][in] */ wchar_t *valuename,
    /* [in] */ unsigned long type,
    /* [size_is][in] */ byte *value,
    /* [in] */ unsigned long valuesize)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&RpcInterface_StubDesc,
                  (PFORMAT_STRING) &RpcInterface__MIDL_ProcFormatString.Format[160],
                  phContext,
                  hKey,
                  subkey,
                  valuename,
                  type,
                  value,
                  valuesize);
    return ( __int64  )_RetVal.Simple;
    
}


void StartMetering( 
    /* [in] */ PCONTEXT_HANDLE_TYPE phContext,
    /* [in] */ __int64 samplePeriod,
    /* [optional][in] */ __int64 context)
{

    NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&RpcInterface_StubDesc,
                  (PFORMAT_STRING) &RpcInterface__MIDL_ProcFormatString.Format[240],
                  phContext,
                  samplePeriod,
                  context);
    
}


void SetSamplePeriod( 
    /* [in] */ PCONTEXT_HANDLE_TYPE phContext,
    /* [in] */ __int64 samplePeriod)
{

    NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&RpcInterface_StubDesc,
                  (PFORMAT_STRING) &RpcInterface__MIDL_ProcFormatString.Format[290],
                  phContext,
                  samplePeriod);
    
}


void StopMetering( 
    /* [in] */ PCONTEXT_HANDLE_TYPE phContext)
{

    NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&RpcInterface_StubDesc,
                  (PFORMAT_STRING) &RpcInterface__MIDL_ProcFormatString.Format[334],
                  phContext);
    
}


#if !defined(__RPC_WIN64__)
#error  Invalid build platform for this stub.
#endif

static const RpcInterface_MIDL_PROC_FORMAT_STRING RpcInterface__MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure RemoteOpen */

			0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x0 ),	/* 0 */
/*  8 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 10 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 12 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 14 */	NdrFcShort( 0x0 ),	/* 0 */
/* 16 */	NdrFcShort( 0x38 ),	/* 56 */
/* 18 */	0x40,		/* Oi2 Flags:  has ext, */
			0x1,		/* 1 */
/* 20 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */
/* 26 */	NdrFcShort( 0x0 ),	/* 0 */
/* 28 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pphContext */

/* 30 */	NdrFcShort( 0x110 ),	/* Flags:  out, simple ref, */
/* 32 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 34 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Procedure RemoteClose */

/* 36 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 38 */	NdrFcLong( 0x0 ),	/* 0 */
/* 42 */	NdrFcShort( 0x1 ),	/* 1 */
/* 44 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 46 */	0x30,		/* FC_BIND_CONTEXT */
			0xe4,		/* Ctxt flags:  via ptr, in, out, no serialize, */
/* 48 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 50 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 52 */	NdrFcShort( 0x38 ),	/* 56 */
/* 54 */	NdrFcShort( 0x38 ),	/* 56 */
/* 56 */	0x40,		/* Oi2 Flags:  has ext, */
			0x1,		/* 1 */
/* 58 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 60 */	NdrFcShort( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0x0 ),	/* 0 */
/* 64 */	NdrFcShort( 0x0 ),	/* 0 */
/* 66 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pphContext */

/* 68 */	NdrFcShort( 0x118 ),	/* Flags:  in, out, simple ref, */
/* 70 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 72 */	NdrFcShort( 0xe ),	/* Type Offset=14 */

	/* Procedure RegistryGetValue */

/* 74 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 76 */	NdrFcLong( 0x0 ),	/* 0 */
/* 80 */	NdrFcShort( 0x2 ),	/* 2 */
/* 82 */	NdrFcShort( 0x48 ),	/* X64 Stack size/offset = 72 */
/* 84 */	0x30,		/* FC_BIND_CONTEXT */
			0x44,		/* Ctxt flags:  in, no serialize, */
/* 86 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 88 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 90 */	NdrFcShort( 0x3c ),	/* 60 */
/* 92 */	NdrFcShort( 0x48 ),	/* 72 */
/* 94 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x9,		/* 9 */
/* 96 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 98 */	NdrFcShort( 0x1 ),	/* 1 */
/* 100 */	NdrFcShort( 0x0 ),	/* 0 */
/* 102 */	NdrFcShort( 0x0 ),	/* 0 */
/* 104 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter phContext */

/* 106 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 108 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 110 */	NdrFcShort( 0x12 ),	/* Type Offset=18 */

	/* Parameter knownHKey */

/* 112 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 114 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 116 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter subkey */

/* 118 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 120 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 122 */	NdrFcShort( 0x18 ),	/* Type Offset=24 */

	/* Parameter valueName */

/* 124 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 126 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 128 */	NdrFcShort( 0x18 ),	/* Type Offset=24 */

	/* Parameter buffsize */

/* 130 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 132 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 134 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter type */

/* 136 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 138 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 140 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter value */

/* 142 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 144 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 146 */	NdrFcShort( 0x22 ),	/* Type Offset=34 */

	/* Parameter valuesize */

/* 148 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 150 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 152 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 154 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 156 */	NdrFcShort( 0x40 ),	/* X64 Stack size/offset = 64 */
/* 158 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Procedure RegistrySetValue */

/* 160 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 162 */	NdrFcLong( 0x0 ),	/* 0 */
/* 166 */	NdrFcShort( 0x3 ),	/* 3 */
/* 168 */	NdrFcShort( 0x40 ),	/* X64 Stack size/offset = 64 */
/* 170 */	0x30,		/* FC_BIND_CONTEXT */
			0x44,		/* Ctxt flags:  in, no serialize, */
/* 172 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 174 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 176 */	NdrFcShort( 0x44 ),	/* 68 */
/* 178 */	NdrFcShort( 0x10 ),	/* 16 */
/* 180 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x8,		/* 8 */
/* 182 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 184 */	NdrFcShort( 0x0 ),	/* 0 */
/* 186 */	NdrFcShort( 0x1 ),	/* 1 */
/* 188 */	NdrFcShort( 0x0 ),	/* 0 */
/* 190 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter phContext */

/* 192 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 194 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 196 */	NdrFcShort( 0x12 ),	/* Type Offset=18 */

	/* Parameter hKey */

/* 198 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 200 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 202 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter subkey */

/* 204 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 206 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 208 */	NdrFcShort( 0x18 ),	/* Type Offset=24 */

	/* Parameter valuename */

/* 210 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 212 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 214 */	NdrFcShort( 0x18 ),	/* Type Offset=24 */

	/* Parameter type */

/* 216 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 218 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 220 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter value */

/* 222 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 224 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 226 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter valuesize */

/* 228 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 230 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 232 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 234 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 236 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 238 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Procedure StartMetering */

/* 240 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 242 */	NdrFcLong( 0x0 ),	/* 0 */
/* 246 */	NdrFcShort( 0x4 ),	/* 4 */
/* 248 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 250 */	0x30,		/* FC_BIND_CONTEXT */
			0x44,		/* Ctxt flags:  in, no serialize, */
/* 252 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 254 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 256 */	NdrFcShort( 0x44 ),	/* 68 */
/* 258 */	NdrFcShort( 0x0 ),	/* 0 */
/* 260 */	0x40,		/* Oi2 Flags:  has ext, */
			0x3,		/* 3 */
/* 262 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 264 */	NdrFcShort( 0x0 ),	/* 0 */
/* 266 */	NdrFcShort( 0x0 ),	/* 0 */
/* 268 */	NdrFcShort( 0x0 ),	/* 0 */
/* 270 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter phContext */

/* 272 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 274 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 276 */	NdrFcShort( 0x12 ),	/* Type Offset=18 */

	/* Parameter samplePeriod */

/* 278 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 280 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 282 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter context */

/* 284 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 286 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 288 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Procedure SetSamplePeriod */

/* 290 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 292 */	NdrFcLong( 0x0 ),	/* 0 */
/* 296 */	NdrFcShort( 0x5 ),	/* 5 */
/* 298 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 300 */	0x30,		/* FC_BIND_CONTEXT */
			0x44,		/* Ctxt flags:  in, no serialize, */
/* 302 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 304 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 306 */	NdrFcShort( 0x34 ),	/* 52 */
/* 308 */	NdrFcShort( 0x0 ),	/* 0 */
/* 310 */	0x40,		/* Oi2 Flags:  has ext, */
			0x2,		/* 2 */
/* 312 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 314 */	NdrFcShort( 0x0 ),	/* 0 */
/* 316 */	NdrFcShort( 0x0 ),	/* 0 */
/* 318 */	NdrFcShort( 0x0 ),	/* 0 */
/* 320 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter phContext */

/* 322 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 324 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 326 */	NdrFcShort( 0x12 ),	/* Type Offset=18 */

	/* Parameter samplePeriod */

/* 328 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 330 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 332 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Procedure StopMetering */

/* 334 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 336 */	NdrFcLong( 0x0 ),	/* 0 */
/* 340 */	NdrFcShort( 0x6 ),	/* 6 */
/* 342 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 344 */	0x30,		/* FC_BIND_CONTEXT */
			0x44,		/* Ctxt flags:  in, no serialize, */
/* 346 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 348 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 350 */	NdrFcShort( 0x24 ),	/* 36 */
/* 352 */	NdrFcShort( 0x0 ),	/* 0 */
/* 354 */	0x40,		/* Oi2 Flags:  has ext, */
			0x1,		/* 1 */
/* 356 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 358 */	NdrFcShort( 0x0 ),	/* 0 */
/* 360 */	NdrFcShort( 0x0 ),	/* 0 */
/* 362 */	NdrFcShort( 0x0 ),	/* 0 */
/* 364 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter phContext */

/* 366 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 368 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 370 */	NdrFcShort( 0x12 ),	/* Type Offset=18 */

	/* Procedure MeteringDataEvent */

/* 372 */	0x34,		/* FC_CALLBACK_HANDLE */
			0x48,		/* Old Flags:  */
/* 374 */	NdrFcLong( 0x0 ),	/* 0 */
/* 378 */	NdrFcShort( 0x0 ),	/* 0 */
/* 380 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 382 */	NdrFcShort( 0x20 ),	/* 32 */
/* 384 */	NdrFcShort( 0x0 ),	/* 0 */
/* 386 */	0x40,		/* Oi2 Flags:  has ext, */
			0x2,		/* 2 */
/* 388 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 390 */	NdrFcShort( 0x0 ),	/* 0 */
/* 392 */	NdrFcShort( 0x0 ),	/* 0 */
/* 394 */	NdrFcShort( 0x0 ),	/* 0 */
/* 396 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter data */

/* 398 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 400 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 402 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter context */

/* 404 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 406 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 408 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

			0x0
        }
    };

static const RpcInterface_MIDL_TYPE_FORMAT_STRING RpcInterface__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/*  4 */	NdrFcShort( 0x2 ),	/* Offset= 2 (6) */
/*  6 */	0x30,		/* FC_BIND_CONTEXT */
			0xa4,		/* Ctxt flags:  via ptr, out, no serialize, */
/*  8 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 10 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 12 */	NdrFcShort( 0x2 ),	/* Offset= 2 (14) */
/* 14 */	0x30,		/* FC_BIND_CONTEXT */
			0xe5,		/* Ctxt flags:  via ptr, in, out, no serialize, can't be null */
/* 16 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 18 */	0x30,		/* FC_BIND_CONTEXT */
			0x45,		/* Ctxt flags:  in, no serialize, can't be null */
/* 20 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 22 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 24 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 26 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 28 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 30 */	
			0x11, 0x0,	/* FC_RP */
/* 32 */	NdrFcShort( 0x2 ),	/* Offset= 2 (34) */
/* 34 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 36 */	NdrFcShort( 0x1 ),	/* 1 */
/* 38 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 40 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 42 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 44 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 46 */	
			0x11, 0x0,	/* FC_RP */
/* 48 */	NdrFcShort( 0x2 ),	/* Offset= 2 (50) */
/* 50 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 52 */	NdrFcShort( 0x1 ),	/* 1 */
/* 54 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 56 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 58 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 60 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */

			0x0
        }
    };

static const unsigned short RpcInterface_FormatStringOffsetTable[] =
    {
    0,
    36,
    74,
    160,
    240,
    290,
    334,
    };


static const unsigned short _callbackRpcInterface_FormatStringOffsetTable[] =
    {
    372
    };


static const MIDL_STUB_DESC RpcInterface_StubDesc = 
    {
    (void *)& RpcInterface___RpcClientInterface,
    MIDL_user_allocate,
    MIDL_user_free,
    &RpcInterface__MIDL_AutoBindHandle,
    0,
    0,
    0,
    0,
    RpcInterface__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
    0,
    0x801026c, /* MIDL Version 8.1.620 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };

static const RPC_DISPATCH_FUNCTION RpcInterface_table[] =
    {
    NdrServerCall2,
    0
    };
static const RPC_DISPATCH_TABLE RpcInterface_v1_0_DispatchTable = 
    {
    1,
    (RPC_DISPATCH_FUNCTION*)RpcInterface_table
    };

static const SERVER_ROUTINE RpcInterface_ServerRoutineTable[] = 
    {
    (SERVER_ROUTINE)MeteringDataEvent
    };

static const MIDL_SERVER_INFO RpcInterface_ServerInfo = 
    {
    &RpcInterface_StubDesc,
    RpcInterface_ServerRoutineTable,
    RpcInterface__MIDL_ProcFormatString.Format,
    _callbackRpcInterface_FormatStringOffsetTable,
    0,
    0,
    0,
    0};
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* defined(_M_AMD64)*/

