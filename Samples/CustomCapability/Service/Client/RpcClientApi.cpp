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
#include "pch.h"

#pragma push_macro("WINAPI_FAMILY")
#undef WINAPI_FAMILY
#define WINAPI_FAMILY WINAPI_FAMILY_DESKTOP_APP
#include "RpcClient.h"
#pragma pop_macro("WINAPI_FAMILY")

#include "RpcClientApi.h"

extern "C" {
typedef struct _RPC_CLIENT {
    RpcClient* RpcClient;
} RPC_CLIENT, *PRPC_CLIENT;
}

RpcClient* RetriveRpcClientFromHandle(RPC_CLIENT_HANDLE RpcClientHandle);

// Opens metering RPC endpoint
__int64 RpcClientInitialize(RPC_CLIENT_HANDLE* RpcClientHandle) {
    __int64 ret;
    PRPC_CLIENT rpcClientStruct = NULL;
    if (RpcClientHandle == NULL) {
        ret = ERROR_INVALID_PARAMETER;
        goto error;
    }

    rpcClientStruct = new RPC_CLIENT();
    if (rpcClientStruct == NULL) {
        ret = ERROR_NOT_ENOUGH_MEMORY;
        goto error;
    }
    rpcClientStruct->RpcClient = new RpcClient();
    if (rpcClientStruct->RpcClient == NULL) {
        ret = ERROR_NOT_ENOUGH_MEMORY;
        goto error;
    }

    ret = rpcClientStruct->RpcClient->Initialize();

error:
    if (ret != 0) {
        if (rpcClientStruct != NULL) {
            if (rpcClientStruct->RpcClient != NULL) {
                delete rpcClientStruct->RpcClient;
            }
            delete rpcClientStruct;
        }
    }
    else {
        *RpcClientHandle = rpcClientStruct;
    }

    return ret;
}

// Starts metering service and blocks till metering is stopped
__int64 StartMeteringAndWaitForStop(RPC_CLIENT_HANDLE RpcClientHandle, __int64 samplePeriod) {
    RpcClient* client;
    if (RpcClientHandle == NULL) {
        return ERROR_INVALID_PARAMETER;
    } 

    client = RetriveRpcClientFromHandle(RpcClientHandle);
    if (client == NULL) {
        return ERROR_INVALID_HANDLE;
    }

    return client->StartMeteringAndWaitForStop(samplePeriod);
}

// Sends stop metering to RPC Service
__int64 StopMeteringData(RPC_CLIENT_HANDLE RpcClientHandle) {
    RpcClient* client;
    if (RpcClientHandle == NULL) {
        return ERROR_INVALID_PARAMETER;
    }

    client = RetriveRpcClientFromHandle(RpcClientHandle);
    if (client == NULL) {
        return ERROR_INVALID_HANDLE;
    }

    return client->StopMetering();
}

// Updates sample rate of metering RPC Service
__int64 SetSampleRate(RPC_CLIENT_HANDLE RpcClientHandle, int rate) {
    RpcClient* client;
    if (RpcClientHandle == NULL) {
        return ERROR_INVALID_PARAMETER;
    }

    client = RetriveRpcClientFromHandle(RpcClientHandle);
    if (client == NULL) {
        return ERROR_INVALID_HANDLE;
    }

    return client->SetSampleRate(rate);
}

// Get the most recent metering data received from the metering RPC service
__int64 GetMeteringData(RPC_CLIENT_HANDLE RpcClientHandle, __int64* MeteringData) {
    RpcClient* client;
    if (RpcClientHandle == NULL || MeteringData == NULL) {
        return ERROR_INVALID_PARAMETER;
    }

    client = RetriveRpcClientFromHandle(RpcClientHandle);
    if (client == NULL) {
        return ERROR_INVALID_HANDLE;
    }
    *MeteringData = client->MeteringData;
    return ERROR_SUCCESS;
}

// Closes metering RPC endpoint
__int64 RpcClientClose(RPC_CLIENT_HANDLE RpcClientHandle) {
    RpcClient* client;
    if (RpcClientHandle == NULL) {
        return ERROR_INVALID_PARAMETER;
    }

    client = RetriveRpcClientFromHandle(RpcClientHandle);
    if (client == NULL) {
        return ERROR_INVALID_HANDLE;
    }
    delete client;
    delete RpcClientHandle;

    return ERROR_SUCCESS;
}

// Retrieve the metering callback count for the current metering session
__int64 GetCallbackCount(RPC_CLIENT_HANDLE RpcClientHandle, __int64* MeteringData)
{
    RpcClient* client;
    if (RpcClientHandle == NULL || MeteringData == NULL) {
        return ERROR_INVALID_PARAMETER;
    }

    client = RetriveRpcClientFromHandle(RpcClientHandle);
    if (client == NULL) {
        return ERROR_INVALID_HANDLE;
    }

    *MeteringData = client->CallbackCount;
    return ERROR_SUCCESS;
}

RpcClient* RetriveRpcClientFromHandle(RPC_CLIENT_HANDLE RpcClientHandle) {
    return ((PRPC_CLIENT)RpcClientHandle)->RpcClient;
}