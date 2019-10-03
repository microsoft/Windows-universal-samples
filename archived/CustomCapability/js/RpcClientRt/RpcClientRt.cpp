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
#include "RpcClientApi.h"
#include "RpcClientRt.h"

using namespace RpcClientRt;
using namespace Platform;
using namespace Concurrency;

/*
WinRT wrapper component to access RpcClient.dll exports
from JavaScript
*/

RpcClient::RpcClient()
{
    RpcClientHandle = NULL;
    meteringStatus = 0;
}

__int64 RpcClient::StartMeteringAsync(__int64 samplePeriod) {
    // Create a asynchronous task that will block. This is used
    // in Javascript to implement a promise
    create_task([this, samplePeriod]
    {
        meteringStatus = ::RpcClientInitialize(&RpcClientHandle);
        if (meteringStatus != 0) {
            return;
        }

        meteringOn = true;
        meteringStatus = ::StartMeteringAndWaitForStop(RpcClientHandle, samplePeriod);
        RpcClientClose();
        meteringOn = false;
    });

    return ERROR_SUCCESS;
}

__int64 RpcClient::GetMeteringStatus() {
    return meteringStatus;
}

__int64 RpcClient::StopMeteringData() {
    if (RpcClientHandle == NULL) {
        return ERROR_INVALID_PARAMETER;
    }
    return ::StopMeteringData(RpcClientHandle);
}

__int64 RpcClient::SetSampleRate(int rate) {
    if (RpcClientHandle == NULL) {
        return ERROR_INVALID_PARAMETER;
    }
    return ::SetSampleRate(RpcClientHandle, rate);
}

__int64 RpcClient::GetMeteringData() {
    __int64 meteringData = 0;
    if (RpcClientHandle == NULL) {
        return NULL;
    }
    ::GetMeteringData(RpcClientHandle, &meteringData);
    return meteringData;
}

__int64 RpcClient::GetCallbackCount() {
    __int64 callbackCount = 0;
    if (RpcClientHandle == NULL) {
        return NULL;
    }
    ::GetCallbackCount(RpcClientHandle, &callbackCount);
    return callbackCount;
}

__int64 RpcClient::RpcClientClose() {
    __int64 ret;
    ret = ::RpcClientClose(RpcClientHandle);
    RpcClientHandle = NULL;
    return ret;
}