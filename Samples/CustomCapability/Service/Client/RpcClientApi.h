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

//
// Exported API's for RPC client that can be used in
// Win32/UWP app. This can be also consumed in managed 
// code using PInvoke
//
extern "C" {
typedef void * RPC_CLIENT_HANDLE;

// Opens metering RPC endpoint
__declspec(dllexport)
__int64 RpcClientInitialize(RPC_CLIENT_HANDLE* RpcClientHandle);

// Starts metering service and blocks till metering is stopped
__declspec(dllexport)
__int64 StartMeteringAndWaitForStop(RPC_CLIENT_HANDLE RpcClientHandle, __int64 samplePeriod);

// Sends stop metering to RPC Service
__declspec(dllexport)
__int64 StopMeteringData(RPC_CLIENT_HANDLE RpcClientHandle);

// Updates sample rate of metering RPC Service
__declspec(dllexport)
__int64 SetSampleRate(RPC_CLIENT_HANDLE RpcClientHandle, int rate);

// Get the most recent metering data received from the metering RPC service
__declspec(dllexport)
__int64 GetMeteringData(RPC_CLIENT_HANDLE RpcClientHandle, __int64* MeteringData);

// Retrieve the metering callback count for the current metering session
__declspec(dllexport)
__int64 GetCallbackCount(RPC_CLIENT_HANDLE RpcClientHandle, __int64* MeteringData);

// Closes metering RPC endpoint
__declspec(dllexport)
__int64 RpcClientClose(RPC_CLIENT_HANDLE RpcClientHandle);
}