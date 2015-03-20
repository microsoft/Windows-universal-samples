//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"

using namespace concurrency;
using namespace Microsoft::WRL;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Devices::AllJoyn;
using namespace org::alljoyn::Bus::Samples::Secure;

std::map<alljoyn_interfacedescription, WeakReference*> SecureInterfaceConsumer::SourceInterfaces;

SecureInterfaceConsumer::SecureInterfaceConsumer(AllJoynBusAttachment^ busAttachment)
    : m_busAttachment(busAttachment),
    m_proxyBusObject(nullptr),
    m_busObject(nullptr),
    m_sessionListener(nullptr),
    m_sessionId(0)
{
    m_weak = new WeakReference(this);
    m_signals = ref new SecureInterfaceSignals();
}

SecureInterfaceConsumer::~SecureInterfaceConsumer()
{
    if (SessionListener != nullptr)
    {
        alljoyn_busattachment_leavesession(AllJoynHelpers::GetInternalBusAttachment(m_busAttachment), m_sessionId);
        alljoyn_sessionlistener_destroy(SessionListener);
    }
    if (nullptr != ProxyBusObject)
    {
        alljoyn_proxybusobject_destroy(ProxyBusObject);
    }
    if (nullptr != BusObject)
    {
        alljoyn_busobject_destroy(BusObject);
    }
    delete m_weak;
}

void SecureInterfaceConsumer::OnSessionLost(_In_ alljoyn_sessionid sessionId, _In_ alljoyn_sessionlostreason reason)
{
    if (sessionId == m_sessionId)
    {
        AllJoynSessionLostEventArgs^ args = ref new AllJoynSessionLostEventArgs(static_cast<AllJoynSessionLostReason>(reason));
        SessionLost(this, args);
    }
}

void SecureInterfaceConsumer::OnSessionMemberAdded(_In_ alljoyn_sessionid sessionId, _In_ PCSTR uniqueName)
{
    if (sessionId == m_sessionId)
    {
        auto args = ref new AllJoynSessionMemberAddedEventArgs(AllJoynHelpers::MultibyteToPlatformString(uniqueName));
        SessionMemberAdded(this, args);
    }
}

void SecureInterfaceConsumer::OnSessionMemberRemoved(_In_ alljoyn_sessionid sessionId, _In_ PCSTR uniqueName)
{
    if (sessionId == m_sessionId)
    {
        auto args = ref new AllJoynSessionMemberRemovedEventArgs(AllJoynHelpers::MultibyteToPlatformString(uniqueName));
        SessionMemberRemoved(this, args);
    }
}

QStatus SecureInterfaceConsumer::AddSignalHandler(_In_ alljoyn_busattachment busAttachment, _In_ alljoyn_interfacedescription interfaceDescription, _In_ PCSTR methodName, _In_ alljoyn_messagereceiver_signalhandler_ptr handler)
{
    alljoyn_interfacedescription_member member;
    if (!alljoyn_interfacedescription_getmember(interfaceDescription, methodName, &member))
    {
        return ER_BUS_INTERFACE_NO_SUCH_MEMBER;
    }

    return alljoyn_busattachment_registersignalhandler(busAttachment, handler, member, NULL);
}

IAsyncOperation<SecureInterfaceJoinSessionResult^>^ SecureInterfaceConsumer::JoinSessionAsync(
    _In_ AllJoynServiceInfo^ serviceInfo, _Inout_ SecureInterfaceWatcher^ watcher)
{
    return create_async([serviceInfo, watcher]() -> SecureInterfaceJoinSessionResult^
    {
        auto result = ref new SecureInterfaceJoinSessionResult();
        result->Status = AllJoynStatus::Ok;
        result->Consumer = nullptr;

        result->Consumer = ref new SecureInterfaceConsumer(watcher->BusAttachment);
        result->Status = result->Consumer->JoinSession(serviceInfo);

        return result;
    });
}

IAsyncOperation<SecureInterfaceCatResult^>^ SecureInterfaceConsumer::CatAsync(_In_ Platform::String^ inStr1, _In_ Platform::String^ inStr2)
{
    return create_async([this, inStr1, inStr2]() -> SecureInterfaceCatResult^
    {
        auto result = ref new SecureInterfaceCatResult();
        
        alljoyn_message message = alljoyn_message_create(AllJoynHelpers::GetInternalBusAttachment(m_busAttachment));
        size_t argCount = 2;
        alljoyn_msgarg inputs = alljoyn_msgarg_array_create(argCount);

        TypeConversionHelpers::SetAllJoynMessageArg(alljoyn_msgarg_array_element(inputs, 0), "s", inStr1);
        TypeConversionHelpers::SetAllJoynMessageArg(alljoyn_msgarg_array_element(inputs, 1), "s", inStr2);
        
        QStatus status = alljoyn_proxybusobject_methodcall(
            ProxyBusObject,
            "org.alljoyn.Bus.Samples.Secure.SecureInterface",
            "Cat",
            inputs,
            argCount,
            message,
            c_MessageTimeoutInMilliseconds,
            0);
        result->Status = static_cast<int>(status);
        if (ER_OK == status) 
        {
            result->Status = AllJoynStatus::Ok;
            Platform::String^ argument0;
            status = static_cast<QStatus>(TypeConversionHelpers::GetAllJoynMessageArg(alljoyn_message_getarg(message, 0), "s", &argument0));
            result->outStr = argument0;

            if (status != ER_OK)
            {
                result->Status = static_cast<int>(status);
            }
        }
        else if (ER_BUS_REPLY_IS_ERROR_MESSAGE == status)
        {
            alljoyn_msgarg errorArg = alljoyn_message_getarg(message, 1);
            if (nullptr != errorArg)
            {
                uint16 errorStatus;
                status = alljoyn_msgarg_get_uint16(errorArg, &errorStatus);
                if (ER_OK == status)
                {
                    status = static_cast<QStatus>(errorStatus);
                }
            }
            result->Status = static_cast<int>(status);
        }
        
        alljoyn_message_destroy(message);
        alljoyn_msgarg_destroy(inputs);

        return result;
    });
}

void SecureInterfaceConsumer::OnPropertyChanged(_In_ alljoyn_proxybusobject obj, _In_ PCSTR interfaceName, _In_ const alljoyn_msgarg changed, _In_ const alljoyn_msgarg invalidated)
{
    UNREFERENCED_PARAMETER(obj);
    UNREFERENCED_PARAMETER(interfaceName);
    UNREFERENCED_PARAMETER(changed);
    UNREFERENCED_PARAMETER(invalidated);
}

int32 SecureInterfaceConsumer::JoinSession(_In_ AllJoynServiceInfo^ serviceInfo)
{
    alljoyn_sessionlistener_callbacks callbacks =
    {
        AllJoynHelpers::SessionLostHandler<SecureInterfaceConsumer>,
        AllJoynHelpers::SessionMemberAddedHandler<SecureInterfaceConsumer>,
        AllJoynHelpers::SessionMemberRemovedHandler<SecureInterfaceConsumer>
    };

    alljoyn_busattachment_enableconcurrentcallbacks(AllJoynHelpers::GetInternalBusAttachment(m_busAttachment));

    SessionListener = alljoyn_sessionlistener_create(&callbacks, m_weak);
    alljoyn_sessionopts sessionOpts = alljoyn_sessionopts_create(ALLJOYN_TRAFFIC_TYPE_MESSAGES, true, ALLJOYN_PROXIMITY_ANY, ALLJOYN_TRANSPORT_ANY);

    std::vector<char> sessionNameUtf8 = AllJoynHelpers::PlatformToMultibyteString(serviceInfo->UniqueName);
    RETURN_IF_QSTATUS_ERROR(alljoyn_busattachment_joinsession(
        AllJoynHelpers::GetInternalBusAttachment(m_busAttachment),
        &sessionNameUtf8[0],
        serviceInfo->SessionPort,
        SessionListener,
        &m_sessionId,
        sessionOpts));

    ServiceObjectPath = serviceInfo->ObjectPath;
    std::vector<char> objectPath = AllJoynHelpers::PlatformToMultibyteString(ServiceObjectPath);

    if (objectPath.empty())
    {
        return AllJoynStatus::Fail;
    }

    ProxyBusObject = alljoyn_proxybusobject_create(AllJoynHelpers::GetInternalBusAttachment(m_busAttachment), &sessionNameUtf8[0], &objectPath[0], m_sessionId);
    if (nullptr == ProxyBusObject)
    {
        return AllJoynStatus::Fail;
    }

    RETURN_IF_QSTATUS_ERROR(AllJoynHelpers::CreateBusObject<SecureInterfaceConsumer>(m_weak));

    alljoyn_interfacedescription description = alljoyn_busattachment_getinterface(AllJoynHelpers::GetInternalBusAttachment(m_busAttachment), "org.alljoyn.Bus.Samples.Secure.SecureInterface");
    if (nullptr == description)
    {
        return AllJoynStatus::Fail;
    }

    RETURN_IF_QSTATUS_ERROR(alljoyn_proxybusobject_addinterface(ProxyBusObject, description));
    RETURN_IF_QSTATUS_ERROR(alljoyn_busobject_addinterface(BusObject, description));


    SourceInterfaces[description] = m_weak;
    RETURN_IF_QSTATUS_ERROR(alljoyn_busattachment_registerbusobject(AllJoynHelpers::GetInternalBusAttachment(m_busAttachment), BusObject));
    m_signals->Initialize(BusObject, m_sessionId);

    alljoyn_sessionopts_destroy(sessionOpts);

    return AllJoynStatus::Ok;
}
