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
#pragma once

#define RPC_STATIC_ENDPOINT L"HsaSampleRpcEndpoint"

#include "RpcInterface_h.h"


/// <summary>
/// Client side RPC implementation
/// </summary>
class RpcClient sealed
{
public:
    ~RpcClient();
    __int64 Initialize();
    __int64 StartMeteringAndWaitForStop(__int64 samplePeriod);
    __int64 StopMetering();
    __int64 SetSampleRate(int rate);
    int CallbackCount;
    __int64 MeteringData;
private:
    handle_t hRpcBinding;
    PCONTEXT_HANDLE_TYPE phContext;
};
