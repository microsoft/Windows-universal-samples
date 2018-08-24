#include "metering.h"

#define RPC_STATIC_ENDPOINT L"HsaSampleRpcEndpoint"

// Client context used for making rpc calls using context handle
// https://msdn.microsoft.com/en-us/library/windows/desktop/aa378674(v=vs.85).aspx
typedef struct
{
    RpcServer::Metering* metering;
} METERING_CONTEXT;

// Create a rpc server endpoint and listen to incoming rpc calls
DWORD RpcServerStart();

// Signal the rpc server to stop listening to incoming rpc calls
void RpcServerDisconnect();
