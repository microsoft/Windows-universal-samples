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
//-----------------------------------------------------------------------------
// <auto-generated> 
//   This code was generated by a tool. 
// 
//   Changes to this file may cause incorrect behavior and will be lost if  
//   the code is regenerated.
//
//   Tool: AllJoynCodeGenerator.exe
//
//   This tool is located in the Windows 10 SDK and the Windows 10 AllJoyn 
//   Visual Studio Extension in the Visual Studio Gallery.  
//
//   The generated code should be packaged in a Windows 10 C++/CX Runtime  
//   Component which can be consumed in any UWP-supported language using 
//   APIs that are available in Windows.Devices.AllJoyn.
//
//   Using AllJoynCodeGenerator - Invoke the following command with a valid 
//   Introspection XML file and a writable output directory:
//     AllJoynCodeGenerator -i <INPUT XML FILE> -o <OUTPUT DIRECTORY>
// </auto-generated>
//-----------------------------------------------------------------------------
#pragma once

namespace org { namespace alljoyn { namespace Onboarding {

ref class OnboardingConsumer;

public ref class OnboardingConfigureWifiResult sealed
{
public:
    property int32 Status
    {
        int32 get() { return m_status; }
    internal:
        void set(_In_ int32 value) { m_status = value; }
    }

    property int16 Status2
    {
        int16 get() { return m_interfaceMemberStatus2; }
    internal:
        void set(_In_ int16 value) { m_interfaceMemberStatus2 = value; }
    }
    
    static OnboardingConfigureWifiResult^ CreateSuccessResult(_In_ int16 interfaceMemberStatus2)
    {
        auto result = ref new OnboardingConfigureWifiResult();
        result->Status = Windows::Devices::AllJoyn::AllJoynStatus::Ok;
        result->Status2 = interfaceMemberStatus2;
        return result;
    }
    
    static OnboardingConfigureWifiResult^ CreateFailureResult(_In_ int32 status)
    {
        auto result = ref new OnboardingConfigureWifiResult();
        result->Status = status;
        return result;
    }

private:
    int32 m_status;
    int16 m_interfaceMemberStatus2;
};

public ref class OnboardingConnectResult sealed
{
public:
    property int32 Status
    {
        int32 get() { return m_status; }
    internal:
        void set(_In_ int32 value) { m_status = value; }
    }

    
    static OnboardingConnectResult^ CreateSuccessResult()
    {
        auto result = ref new OnboardingConnectResult();
        result->Status = Windows::Devices::AllJoyn::AllJoynStatus::Ok;
        return result;
    }
    
    static OnboardingConnectResult^ CreateFailureResult(_In_ int32 status)
    {
        auto result = ref new OnboardingConnectResult();
        result->Status = status;
        return result;
    }

private:
    int32 m_status;
};

public ref class OnboardingOffboardResult sealed
{
public:
    property int32 Status
    {
        int32 get() { return m_status; }
    internal:
        void set(_In_ int32 value) { m_status = value; }
    }

    
    static OnboardingOffboardResult^ CreateSuccessResult()
    {
        auto result = ref new OnboardingOffboardResult();
        result->Status = Windows::Devices::AllJoyn::AllJoynStatus::Ok;
        return result;
    }
    
    static OnboardingOffboardResult^ CreateFailureResult(_In_ int32 status)
    {
        auto result = ref new OnboardingOffboardResult();
        result->Status = status;
        return result;
    }

private:
    int32 m_status;
};

public ref class OnboardingGetScanInfoResult sealed
{
public:
    property int32 Status
    {
        int32 get() { return m_status; }
    internal:
        void set(_In_ int32 value) { m_status = value; }
    }

    property uint16 Age
    {
        uint16 get() { return m_interfaceMemberAge; }
    internal:
        void set(_In_ uint16 value) { m_interfaceMemberAge = value; }
    }
    property Windows::Foundation::Collections::IVector<OnboardingScanListItem^>^ ScanList
    {
        Windows::Foundation::Collections::IVector<OnboardingScanListItem^>^ get() { return m_interfaceMemberScanList; }
    internal:
        void set(_In_ Windows::Foundation::Collections::IVector<OnboardingScanListItem^>^ value) { m_interfaceMemberScanList = value; }
    }
    
    static OnboardingGetScanInfoResult^ CreateSuccessResult(_In_ uint16 interfaceMemberAge, _In_ Windows::Foundation::Collections::IVector<OnboardingScanListItem^>^ interfaceMemberScanList)
    {
        auto result = ref new OnboardingGetScanInfoResult();
        result->Status = Windows::Devices::AllJoyn::AllJoynStatus::Ok;
        result->Age = interfaceMemberAge;
        result->ScanList = interfaceMemberScanList;
        return result;
    }
    
    static OnboardingGetScanInfoResult^ CreateFailureResult(_In_ int32 status)
    {
        auto result = ref new OnboardingGetScanInfoResult();
        result->Status = status;
        return result;
    }

private:
    int32 m_status;
    uint16 m_interfaceMemberAge;
    Windows::Foundation::Collections::IVector<OnboardingScanListItem^>^ m_interfaceMemberScanList;
};

public ref class OnboardingJoinSessionResult sealed
{
public:
    property int32 Status
    {
        int32 get() { return m_status; }
    internal:
        void set(_In_ int32 value) { m_status = value; }
    }

    property OnboardingConsumer^ Consumer
    {
        OnboardingConsumer^ get() { return m_consumer; }
    internal:
        void set(_In_ OnboardingConsumer^ value) { m_consumer = value; }
    };

private:
    int32 m_status;
    OnboardingConsumer^ m_consumer;
};

public ref class OnboardingGetVersionResult sealed
{
public:
    property int32 Status
    {
        int32 get() { return m_status; }
    internal:
        void set(_In_ int32 value) { m_status = value; }
    }

    property uint16 Version
    {
        uint16 get() { return m_value; }
    internal:
        void set(_In_ uint16 value) { m_value = value; }
    }

    static OnboardingGetVersionResult^ CreateSuccessResult(_In_ uint16 value)
    {
        auto result = ref new OnboardingGetVersionResult();
        result->Status = Windows::Devices::AllJoyn::AllJoynStatus::Ok;
        result->Version = value;
        return result;
    }

    static OnboardingGetVersionResult^ CreateFailureResult(_In_ int32 status)
    {
        auto result = ref new OnboardingGetVersionResult();
        result->Status = status;
        return result;
    }

private:
    int32 m_status;
    uint16 m_value;
};

public ref class OnboardingGetStateResult sealed
{
public:
    property int32 Status
    {
        int32 get() { return m_status; }
    internal:
        void set(_In_ int32 value) { m_status = value; }
    }

    property int16 State
    {
        int16 get() { return m_value; }
    internal:
        void set(_In_ int16 value) { m_value = value; }
    }

    static OnboardingGetStateResult^ CreateSuccessResult(_In_ int16 value)
    {
        auto result = ref new OnboardingGetStateResult();
        result->Status = Windows::Devices::AllJoyn::AllJoynStatus::Ok;
        result->State = value;
        return result;
    }

    static OnboardingGetStateResult^ CreateFailureResult(_In_ int32 status)
    {
        auto result = ref new OnboardingGetStateResult();
        result->Status = status;
        return result;
    }

private:
    int32 m_status;
    int16 m_value;
};

public ref class OnboardingGetLastErrorResult sealed
{
public:
    property int32 Status
    {
        int32 get() { return m_status; }
    internal:
        void set(_In_ int32 value) { m_status = value; }
    }

    property OnboardingLastError^ LastError
    {
        OnboardingLastError^ get() { return m_value; }
    internal:
        void set(_In_ OnboardingLastError^ value) { m_value = value; }
    }

    static OnboardingGetLastErrorResult^ CreateSuccessResult(_In_ OnboardingLastError^ value)
    {
        auto result = ref new OnboardingGetLastErrorResult();
        result->Status = Windows::Devices::AllJoyn::AllJoynStatus::Ok;
        result->LastError = value;
        return result;
    }

    static OnboardingGetLastErrorResult^ CreateFailureResult(_In_ int32 status)
    {
        auto result = ref new OnboardingGetLastErrorResult();
        result->Status = status;
        return result;
    }

private:
    int32 m_status;
    OnboardingLastError^ m_value;
};

} } } 
