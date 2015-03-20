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

#pragma once

namespace org { namespace alljoyn { namespace Bus { namespace Samples { namespace Secure {

ref class SecureInterfaceConsumer;

public ref class SecureInterfaceCatResult sealed
{
public:
    property int32 Status
    {
        int32 get() { return m_status; }
    internal:
        void set(_In_ int32 value) { m_status = value; }
    }

    property Platform::String^ outStr
    {
        Platform::String^ get() { return m_outStr; }
    internal:
        void set(_In_ Platform::String^ value) { m_outStr = value; }
    }
    
    static SecureInterfaceCatResult^ CreateSuccessResult(_In_ Platform::String^ outStr)
    {
        auto result = ref new SecureInterfaceCatResult();
        result->Status = Windows::Devices::AllJoyn::AllJoynStatus::Ok;
        result->outStr = outStr;
        return result;
    }
    
    static SecureInterfaceCatResult^ CreateFailureResult(int status)
    {
        auto result = ref new SecureInterfaceCatResult();
        result->Status = status;
        return result;
    }

private:
    int32 m_status;
    Platform::String^ m_outStr;
};

public ref class SecureInterfaceJoinSessionResult sealed
{
public:
    property int32 Status
    {
        int32 get() { return m_status; }
    internal:
        void set(_In_ int32 value) { m_status = value; }
    }

    property SecureInterfaceConsumer^ Consumer
    {
        SecureInterfaceConsumer^ get() { return m_consumer; }
    internal:
        void set(_In_ SecureInterfaceConsumer^ value) { m_consumer = value; }
    };

private:
    int32 m_status;
    SecureInterfaceConsumer^ m_consumer;
};

} } } } } 