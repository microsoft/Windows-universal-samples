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

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "RpcInterface_h.h"
#include <windows.h>

#include <sddl.h>
#include <securitybaseapi.h>
#include <AclAPI.h>
#include "RpcServer.h"

using namespace RpcServer;

#define DEFAULT_METERING_PERIOD 100

const WCHAR* CustomCapabilityName = L"microsoft.hsaTestCustomCapability_q536wpkpf5cy2";

bool ShutdownRequested;
static RPC_BINDING_VECTOR* BindingVector = nullptr;

void FreeSidArray(__inout_ecount(cSIDs) PSID* pSIDs, ULONG cSIDs)
{
    if (pSIDs != nullptr)
    {
        for (ULONG i = 0; i < cSIDs; i++)
        {
            LocalFree(pSIDs[i]);

            pSIDs[i] = nullptr;
        }

        LocalFree(pSIDs);

        pSIDs = nullptr;
        cSIDs = 0;
    }
}

//
// Routine to create RPC server and listen to incoming RPC calls
//
DWORD RpcServerStart()
{
    DWORD hResult = S_OK;
    WCHAR* protocolSequence = L"ncalrpc";
    unsigned int minCalls = 1;
    unsigned int dontWait = false;
    ShutdownRequested = false;

    SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
    PSID everyoneSid = nullptr;
    PSID* capabilitySids = nullptr;
    DWORD capabilitySidCount = 0;
    PSID* capabilityGroupSids = nullptr;
    DWORD capabilityGroupSidCount = 0;
    EXPLICIT_ACCESS ea[2] = {};
    PACL acl = nullptr;
    SECURITY_DESCRIPTOR rpcSecurityDescriptor = {};

    // When creating the RPC endpoint we want it to allow connections from any UWA that contains
    // the custom capability SID in its process token.  When a UWA declares the custom capability
    // in its app manifest, it will later contain the SID form of that custom capability in its
    // process token at runtime.  By default, RPC endpoints don't allow UWAs (AppContainer processes)
    // to connect to them, so we need to set the security on the endpoint to allow access to UWAs with the 
    // custom capability.
    //
    // To do this we'll perform the following steps:
    // 1) Convert the custom capability name to a SID
    // 2) Create a security descriptor using that SID, as well as other needed SIDs.  This sample shows how to allow
    //    all 'non UWAs' access as well as UWAs containing the custom capability SID. 
    // 3) Create the RPC endpoint using that security descriptor
    //
    // To create the security descriptor we're roughly following this MSDN sample: 
    // https://msdn.microsoft.com/en-us/library/windows/desktop/aa446595(v=vs.85).aspx

    // Get the SID form of the custom capability.  In this case we only expect one SID and
    // we don't care about the capability group. 
    if (!DeriveCapabilitySidsFromName(
        CustomCapabilityName,
        &capabilityGroupSids,
        &capabilityGroupSidCount,
        &capabilitySids,
        &capabilitySidCount))
    {
        hResult = GetLastError();
        goto end;
    }

    if (capabilitySidCount != 1)
    {
        // Unexpected sid count
        hResult = ERROR_INVALID_PARAMETER;
        goto end;
    }

    // Get the SID that represents 'everyone' (this doesn't include AppContainers)
    if (!AllocateAndInitializeSid(
        &SIDAuthWorld, 1,
        SECURITY_WORLD_RID,
        0, 0, 0, 0, 0, 0, 0,
        &everyoneSid))
    {
        hResult = GetLastError();
        goto end;
    }

    // Now create the Access Control List (ACL) for the Security descriptor

    // Everyone GENERIC_ALL access
    ea[0].grfAccessMode = SET_ACCESS;
    ea[0].grfAccessPermissions = GENERIC_ALL;
    ea[0].grfInheritance = NO_INHERITANCE;
    ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
    ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
    ea[0].Trustee.ptstrName = static_cast<LPWSTR>(everyoneSid);

    // Custom capability GENERIC_ALL access
    ea[1].grfAccessMode = SET_ACCESS;
    ea[1].grfAccessPermissions = GENERIC_ALL;
    ea[1].grfInheritance = NO_INHERITANCE;
    ea[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
    ea[1].Trustee.TrusteeType = TRUSTEE_IS_UNKNOWN;
    ea[1].Trustee.ptstrName = static_cast<LPWSTR>(capabilitySids[0]); // Earlier we ensured there was exactly 1

    hResult = SetEntriesInAcl(ARRAYSIZE(ea), ea, nullptr, &acl);

    if (hResult != ERROR_SUCCESS)
    {
        goto end;
    }

    // Initialize an empty security descriptor
    if (!InitializeSecurityDescriptor(&rpcSecurityDescriptor, SECURITY_DESCRIPTOR_REVISION))
    {
        hResult = GetLastError();
        goto end;
    }

    // Assign the ACL to the security descriptor
    if (!SetSecurityDescriptorDacl(&rpcSecurityDescriptor, TRUE, acl, FALSE))
    {
        hResult = GetLastError();
        goto end;
    }

    //
    // Bind to LRPC using dynamic endpoints
    //
    hResult = RpcServerUseProtseqEp(
        reinterpret_cast<RPC_WSTR>(protocolSequence),
        RPC_C_PROTSEQ_MAX_REQS_DEFAULT,
        reinterpret_cast<RPC_WSTR>(RPC_STATIC_ENDPOINT),
        &rpcSecurityDescriptor);

    if (hResult != S_OK)
    {
        goto end;
    }

    hResult = RpcServerRegisterIf3(
        RpcInterface_v1_0_s_ifspec,
        nullptr,
        nullptr,
        RPC_IF_AUTOLISTEN | RPC_IF_ALLOW_LOCAL_ONLY,
        RPC_C_LISTEN_MAX_CALLS_DEFAULT,
        0,
        nullptr,
        &rpcSecurityDescriptor);

    if (hResult != S_OK)
    {
        goto end;
    }

    hResult = RpcServerInqBindings(&BindingVector);

    if (hResult != S_OK)
    {
        goto end;
    }

    hResult = RpcEpRegister(
        RpcInterface_v1_0_s_ifspec,
        BindingVector,
        nullptr,
        nullptr);

    if (hResult != S_OK)
    {
        goto end;
    }

    hResult = RpcServerListen(
        minCalls,
        RPC_C_LISTEN_MAX_CALLS_DEFAULT,
        dontWait);

    if (hResult == RPC_S_ALREADY_LISTENING)
    {
        hResult = RPC_S_OK;
    }

end:

    // Cleanup sids
    FreeSidArray(capabilityGroupSids, capabilityGroupSidCount);
    FreeSidArray(capabilitySids, capabilitySidCount);

    if (everyoneSid != nullptr)
    {
        FreeSid(everyoneSid);
    }

    // cleanup acl
    if (acl != nullptr)
    {
        LocalFree(acl);
    }

    return hResult;
}

//
// Notify rpc server to stop listening to incoming rpc calls
//
void RpcServerDisconnect()
{
    DWORD hResult = S_OK;
    ShutdownRequested = true;
    hResult = RpcServerUnregisterIf(RpcInterface_v1_0_s_ifspec, nullptr, 0);

    RpcEpUnregister(RpcInterface_v1_0_s_ifspec, BindingVector, nullptr);

    if (BindingVector != nullptr)
    {
        RpcBindingVectorFree(&BindingVector);
        BindingVector = nullptr;
    }
}

//
// Rpc method to retrieve client context handle
//
void RemoteOpen(
    _In_ handle_t hBinding,
    _Out_ PPCONTEXT_HANDLE_TYPE pphContext)
{
    std::cout << "opening rpc endpoint" << std::endl;
    *pphContext = static_cast<PCONTEXT_HANDLE_TYPE *>(midl_user_allocate(sizeof(METERING_CONTEXT)));
    METERING_CONTEXT* meteringContext = static_cast<METERING_CONTEXT *>(*pphContext);
    meteringContext->metering = new Metering(DEFAULT_METERING_PERIOD);
}

//
// Rpc method to close the client context handle
//
void RemoteClose(_Inout_ PPCONTEXT_HANDLE_TYPE pphContext)
{
    std::cout << "closing rpc endpoint" << std::endl;
    if (*pphContext == nullptr)
    {
        //Log error, client tried to close a NULL handle.
        return;
    }

    METERING_CONTEXT* meteringContext = static_cast<METERING_CONTEXT *>(*pphContext);
    delete meteringContext->metering;
    MIDL_user_free(meteringContext);

    // This tells the run-time, when it is marshalling the out 
    // parameters, that the context handle has been closed normally.
    *pphContext = nullptr;
}

//
// Routine to cleanup client context when client has died with active 
// connection with server
//
void __RPC_USER PCONTEXT_HANDLE_TYPE_rundown(
    _In_ PCONTEXT_HANDLE_TYPE phContext)
{
    std::cout << "client died" << std::endl;
    StopMetering(phContext);
    RemoteClose(&phContext);
}

#pragma region METERING_RPCROUTINES

void StartMetering(
    _In_ PCONTEXT_HANDLE_TYPE phContext,
    _In_ __int64 period,
    _In_ __int64 context)
{
    std::cout << "start metering" << std::endl;
    METERING_CONTEXT* meteringContext = static_cast<METERING_CONTEXT *>(phContext);
    meteringContext->metering->StartMetering(period, context);
    std::cout << "done metering" << std::endl;
}

void SetSamplePeriod(
    _In_ PCONTEXT_HANDLE_TYPE phContext,
    _In_ __int64 period)
{
    METERING_CONTEXT* meteringContext = static_cast<METERING_CONTEXT *>(phContext);
    meteringContext->metering->SetSamplePeriod(period);
}


void StopMetering(_In_ PCONTEXT_HANDLE_TYPE phContext)
{
    std::cout << "stop metering" << std::endl;
    METERING_CONTEXT* meteringContext = static_cast<METERING_CONTEXT *>(phContext);
    meteringContext->metering->StopMetering();
}

#pragma endregion METERING_RPCROUTINES

/******************************************************/
/*         MIDL allocate and free                     */
/******************************************************/

void __RPC_FAR * __RPC_USER midl_user_allocate(_In_ size_t len)
{
    return(malloc(len));
}

void __RPC_USER midl_user_free(_In_ void __RPC_FAR* ptr)
{
    free(ptr);
}
