//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

// There is an error in the system header files that incorrectly
// places RpcStringBindingCompose in the app partition.
// Work around it by changing the WINAPI_FAMILY to desktop temporarily.
#pragma push_macro("WINAPI_FAMILY")
#undef WINAPI_FAMILY
#define WINAPI_FAMILY WINAPI_FAMILY_DESKTOP_APP
#include "RpcClient.h"
#pragma pop_macro("WINAPI_FAMILY")

using namespace SDKTemplate;

__int64 RpcClient::Initialize()
{
    RPC_STATUS status;
    RPC_WSTR pszStringBinding = nullptr;

    status = RpcStringBindingCompose(
        NULL,
        reinterpret_cast<RPC_WSTR>(L"ncalrpc"),
        NULL,
        reinterpret_cast<RPC_WSTR>(RPC_STATIC_ENDPOINT),
        NULL,
        &pszStringBinding);

    if (status)
    {
        goto error_status;
    }

    status = RpcBindingFromStringBinding(
        pszStringBinding, 
        &hRpcBinding);

    if (status)
    {
        goto error_status;
    }

    status = RpcStringFree(&pszStringBinding);

    if (status)
    {
        goto error_status;
    }

    RpcTryExcept
    {
        ::RemoteOpen(hRpcBinding, &phContext);
    }
    RpcExcept(1)
    {
        status = RpcExceptionCode();
    }
    RpcEndExcept

error_status:

    return status;
}

//
// Make RPC call to start metering. This is a blocking call and 
// will return only after StopMetering is called.
//
__int64 RpcClient::StartMeteringAndWaitForStop(__int64 samplePeriod)
{
    __int64 ulCode = 0;
    CallbackCount = 0;
    MeteringData = 0;

    RpcTryExcept
    {
        ::StartMetering(phContext, samplePeriod, (__int64)this);
    }
    RpcExcept(1)
    {
        ulCode = RpcExceptionCode();
    }
    RpcEndExcept

    return ulCode;
}


//
// Make rpc call SetSampleRate
//
__int64 RpcClient::SetSampleRate(int rate)
{
    __int64 ulCode = 0;
    RpcTryExcept
    {
        ::SetSamplePeriod(phContext, rate);
    }
    RpcExcept(1)
    {
        ulCode = RpcExceptionCode();
    }
    RpcEndExcept
    return ulCode;
}

//
// Make rpc call StopMetering
//
__int64 RpcClient::StopMetering()
{
    __int64 ulCode = 0;
    RpcTryExcept
    {
        ::StopMetering(phContext);
    }
    RpcExcept(1)
    {
        ulCode = RpcExceptionCode();
    }
    RpcEndExcept
    return ulCode;
}

RpcClient::~RpcClient()
{
    RPC_STATUS status;

    if (hRpcBinding != NULL) 
    {
        RpcTryExcept
        {
            ::RemoteClose(&phContext);
        }
        RpcExcept(1)
        {
            // Ignoring the result of RemoteClose as nothing can be
            // done on the client side with this return code
            status = RpcExceptionCode();
        }
        RpcEndExcept

        status = RpcBindingFree(&hRpcBinding);
        hRpcBinding = NULL;
    }
}

//
// Metering rpc callback
//
void MeteringDataEvent(__int64 data, __int64 context)
{
    RpcClient* client = static_cast<RpcClient*>((PVOID)context);
    client->MeteringData = data;
    ++client->CallbackCount;
}

///******************************************************/
///*         MIDL allocate and free                     */
///******************************************************/

void __RPC_FAR * __RPC_USER midl_user_allocate(size_t len)
{
    return(malloc(len));
}

void __RPC_USER midl_user_free(void __RPC_FAR * ptr)
{
    free(ptr);
}