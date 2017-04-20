//
// RpcClient.h
//

#pragma once

#define RPC_STATIC_ENDPOINT L"HsaSampleRpcEndpoint"

#include "RpcInterface_h.h"

namespace SDKTemplate
{
    /// <summary>
    /// Client side RPC implementation
    /// </summary>
    private class RpcClient sealed
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
}
