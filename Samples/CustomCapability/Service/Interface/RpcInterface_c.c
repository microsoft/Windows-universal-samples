

/* this ALWAYS GENERATED file contains the RPC client stubs */


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

#if !defined(_M_IA64) && !defined(_M_AMD64) && !defined(_ARM_)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */

#pragma optimize("", off ) 

#include <string.h>

#include "RpcInterface_h.h"

#define TYPE_FORMAT_STRING_SIZE   63                                
#define PROC_FORMAT_STRING_SIZE   395                               
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
                  ( unsigned char * )&hBinding);
    
}


void RemoteClose( 
    /* [out][in] */ PPCONTEXT_HANDLE_TYPE pphContext)
{

    NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&RpcInterface_StubDesc,
                  (PFORMAT_STRING) &RpcInterface__MIDL_ProcFormatString.Format[34],
                  ( unsigned char * )&pphContext);
    
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
                  (PFORMAT_STRING) &RpcInterface__MIDL_ProcFormatString.Format[70],
                  ( unsigned char * )&phContext);
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
                  (PFORMAT_STRING) &RpcInterface__MIDL_ProcFormatString.Format[154],
                  ( unsigned char * )&phContext);
    return ( __int64  )_RetVal.Simple;
    
}


void StartMetering( 
    /* [in] */ PCONTEXT_HANDLE_TYPE phContext,
    /* [in] */ __int64 samplePeriod,
    /* [optional][in] */ __int64 context)
{

    NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&RpcInterface_StubDesc,
                  (PFORMAT_STRING) &RpcInterface__MIDL_ProcFormatString.Format[232],
                  ( unsigned char * )&phContext);
    
}


void SetSamplePeriod( 
    /* [in] */ PCONTEXT_HANDLE_TYPE phContext,
    /* [in] */ __int64 samplePeriod)
{

    NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&RpcInterface_StubDesc,
                  (PFORMAT_STRING) &RpcInterface__MIDL_ProcFormatString.Format[280],
                  ( unsigned char * )&phContext);
    
}


void StopMetering( 
    /* [in] */ PCONTEXT_HANDLE_TYPE phContext)
{

    NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&RpcInterface_StubDesc,
                  (PFORMAT_STRING) &RpcInterface__MIDL_ProcFormatString.Format[322],
                  ( unsigned char * )&phContext);
    
}


#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT50_OR_LATER)
#error You need Windows 2000 or later to run this stub because it uses these features:
#error   /robust command line switch.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will fail with the RPC_X_WRONG_STUB_VERSION error.
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
/*  8 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 10 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 12 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 14 */	NdrFcShort( 0x0 ),	/* 0 */
/* 16 */	NdrFcShort( 0x38 ),	/* 56 */
/* 18 */	0x40,		/* Oi2 Flags:  has ext, */
			0x1,		/* 1 */
/* 20 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */
/* 26 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pphContext */

/* 28 */	NdrFcShort( 0x110 ),	/* Flags:  out, simple ref, */
/* 30 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 32 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Procedure RemoteClose */

/* 34 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 36 */	NdrFcLong( 0x0 ),	/* 0 */
/* 40 */	NdrFcShort( 0x1 ),	/* 1 */
/* 42 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 44 */	0x30,		/* FC_BIND_CONTEXT */
			0xe4,		/* Ctxt flags:  via ptr, in, out, no serialize, */
/* 46 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 48 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 50 */	NdrFcShort( 0x38 ),	/* 56 */
/* 52 */	NdrFcShort( 0x38 ),	/* 56 */
/* 54 */	0x40,		/* Oi2 Flags:  has ext, */
			0x1,		/* 1 */
/* 56 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 58 */	NdrFcShort( 0x0 ),	/* 0 */
/* 60 */	NdrFcShort( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pphContext */

/* 64 */	NdrFcShort( 0x118 ),	/* Flags:  in, out, simple ref, */
/* 66 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 68 */	NdrFcShort( 0xe ),	/* Type Offset=14 */

	/* Procedure RegistryGetValue */

/* 70 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 72 */	NdrFcLong( 0x0 ),	/* 0 */
/* 76 */	NdrFcShort( 0x2 ),	/* 2 */
/* 78 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 80 */	0x30,		/* FC_BIND_CONTEXT */
			0x44,		/* Ctxt flags:  in, no serialize, */
/* 82 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 84 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 86 */	NdrFcShort( 0x3c ),	/* 60 */
/* 88 */	NdrFcShort( 0x48 ),	/* 72 */
/* 90 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x9,		/* 9 */
/* 92 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 94 */	NdrFcShort( 0x1 ),	/* 1 */
/* 96 */	NdrFcShort( 0x0 ),	/* 0 */
/* 98 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter phContext */

/* 100 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 102 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 104 */	NdrFcShort( 0x12 ),	/* Type Offset=18 */

	/* Parameter knownHKey */

/* 106 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 108 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 110 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter subkey */

/* 112 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 114 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 116 */	NdrFcShort( 0x18 ),	/* Type Offset=24 */

	/* Parameter valueName */

/* 118 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 120 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 122 */	NdrFcShort( 0x18 ),	/* Type Offset=24 */

	/* Parameter buffsize */

/* 124 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 126 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 128 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter type */

/* 130 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 132 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 134 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter value */

/* 136 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 138 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 140 */	NdrFcShort( 0x22 ),	/* Type Offset=34 */

	/* Parameter valuesize */

/* 142 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 144 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 146 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 148 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 150 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 152 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Procedure RegistrySetValue */

/* 154 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 156 */	NdrFcLong( 0x0 ),	/* 0 */
/* 160 */	NdrFcShort( 0x3 ),	/* 3 */
/* 162 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 164 */	0x30,		/* FC_BIND_CONTEXT */
			0x44,		/* Ctxt flags:  in, no serialize, */
/* 166 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 168 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 170 */	NdrFcShort( 0x44 ),	/* 68 */
/* 172 */	NdrFcShort( 0x10 ),	/* 16 */
/* 174 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x8,		/* 8 */
/* 176 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 178 */	NdrFcShort( 0x0 ),	/* 0 */
/* 180 */	NdrFcShort( 0x1 ),	/* 1 */
/* 182 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter phContext */

/* 184 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 186 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 188 */	NdrFcShort( 0x12 ),	/* Type Offset=18 */

	/* Parameter hKey */

/* 190 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 192 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 194 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter subkey */

/* 196 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 198 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 200 */	NdrFcShort( 0x18 ),	/* Type Offset=24 */

	/* Parameter valuename */

/* 202 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 204 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 206 */	NdrFcShort( 0x18 ),	/* Type Offset=24 */

	/* Parameter type */

/* 208 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 210 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 212 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter value */

/* 214 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 216 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 218 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter valuesize */

/* 220 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 222 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 224 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 226 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 228 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 230 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Procedure StartMetering */

/* 232 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 234 */	NdrFcLong( 0x0 ),	/* 0 */
/* 238 */	NdrFcShort( 0x4 ),	/* 4 */
/* 240 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 242 */	0x30,		/* FC_BIND_CONTEXT */
			0x44,		/* Ctxt flags:  in, no serialize, */
/* 244 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 246 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 248 */	NdrFcShort( 0x44 ),	/* 68 */
/* 250 */	NdrFcShort( 0x0 ),	/* 0 */
/* 252 */	0x40,		/* Oi2 Flags:  has ext, */
			0x3,		/* 3 */
/* 254 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 256 */	NdrFcShort( 0x0 ),	/* 0 */
/* 258 */	NdrFcShort( 0x0 ),	/* 0 */
/* 260 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter phContext */

/* 262 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 264 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 266 */	NdrFcShort( 0x12 ),	/* Type Offset=18 */

	/* Parameter samplePeriod */

/* 268 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 270 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 272 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter context */

/* 274 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 276 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 278 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Procedure SetSamplePeriod */

/* 280 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 282 */	NdrFcLong( 0x0 ),	/* 0 */
/* 286 */	NdrFcShort( 0x5 ),	/* 5 */
/* 288 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 290 */	0x30,		/* FC_BIND_CONTEXT */
			0x44,		/* Ctxt flags:  in, no serialize, */
/* 292 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 294 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 296 */	NdrFcShort( 0x34 ),	/* 52 */
/* 298 */	NdrFcShort( 0x0 ),	/* 0 */
/* 300 */	0x40,		/* Oi2 Flags:  has ext, */
			0x2,		/* 2 */
/* 302 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 304 */	NdrFcShort( 0x0 ),	/* 0 */
/* 306 */	NdrFcShort( 0x0 ),	/* 0 */
/* 308 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter phContext */

/* 310 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 312 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 314 */	NdrFcShort( 0x12 ),	/* Type Offset=18 */

	/* Parameter samplePeriod */

/* 316 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 318 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 320 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Procedure StopMetering */

/* 322 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 324 */	NdrFcLong( 0x0 ),	/* 0 */
/* 328 */	NdrFcShort( 0x6 ),	/* 6 */
/* 330 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 332 */	0x30,		/* FC_BIND_CONTEXT */
			0x44,		/* Ctxt flags:  in, no serialize, */
/* 334 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 336 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 338 */	NdrFcShort( 0x24 ),	/* 36 */
/* 340 */	NdrFcShort( 0x0 ),	/* 0 */
/* 342 */	0x40,		/* Oi2 Flags:  has ext, */
			0x1,		/* 1 */
/* 344 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 346 */	NdrFcShort( 0x0 ),	/* 0 */
/* 348 */	NdrFcShort( 0x0 ),	/* 0 */
/* 350 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter phContext */

/* 352 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 354 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 356 */	NdrFcShort( 0x12 ),	/* Type Offset=18 */

	/* Procedure MeteringDataEvent */

/* 358 */	0x34,		/* FC_CALLBACK_HANDLE */
			0x48,		/* Old Flags:  */
/* 360 */	NdrFcLong( 0x0 ),	/* 0 */
/* 364 */	NdrFcShort( 0x0 ),	/* 0 */
/* 366 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 368 */	NdrFcShort( 0x20 ),	/* 32 */
/* 370 */	NdrFcShort( 0x0 ),	/* 0 */
/* 372 */	0x40,		/* Oi2 Flags:  has ext, */
			0x2,		/* 2 */
/* 374 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 376 */	NdrFcShort( 0x0 ),	/* 0 */
/* 378 */	NdrFcShort( 0x0 ),	/* 0 */
/* 380 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter data */

/* 382 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 384 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 386 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter context */

/* 388 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 390 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 392 */	0xb,		/* FC_HYPER */
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
/* 40 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
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
/* 56 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 58 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 60 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */

			0x0
        }
    };

static const unsigned short RpcInterface_FormatStringOffsetTable[] =
    {
    0,
    34,
    70,
    154,
    232,
    280,
    322,
    };


static const unsigned short _callbackRpcInterface_FormatStringOffsetTable[] =
    {
    358
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
    0x801026e, /* MIDL Version 8.1.622 */
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


#endif /* !defined(_M_IA64) && !defined(_M_AMD64) && !defined(_ARM_) */

