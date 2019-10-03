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

/*
    WinRT wrapper component to access RpcClient.dll exports
    from JavaScript
*/
namespace RpcClientRt
{
    public ref class RpcClient sealed
    {
    public:
        RpcClient();

        // Starts metering service asynchronously. Use GetMeteringStatus
        // to retrieve the current status of the Metering
        __int64 StartMeteringAsync(__int64 samplePeriod);
        __int64 GetMeteringStatus();
        __int64 StopMeteringData();
        __int64 SetSampleRate(int rate);
        __int64 GetMeteringData();
        __int64 GetCallbackCount();
    private:
        bool meteringOn;
        __int64 meteringStatus;
        RPC_CLIENT_HANDLE RpcClientHandle;
        __int64 RpcClientClose();
    };
}
