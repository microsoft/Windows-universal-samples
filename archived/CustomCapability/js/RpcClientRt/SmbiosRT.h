#pragma once
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
WinRT wrapper component to access smbios exports
from JavaScript
*/
namespace RpcClientRt
{
    public ref class SmbiosResult sealed
    {
    public:
        property unsigned int ErrorCode
        {
            unsigned int get() { return m_errorCode; }
        }
        property Platform::Object^ Value
        {
            Platform::Object^ get() { return m_value; }
        }

    internal:
        SmbiosResult(unsigned int errorCode, Platform::Object^ value)
            : m_errorCode(errorCode), m_value(value)
        {
        }

    private:
        Platform::Object^ m_value;
        unsigned int m_errorCode;
    };

    public ref class Smbios sealed
    {
    public:
        static SmbiosResult^ GetManufacturerName();
        static SmbiosResult^ GetSecureBootEnabled();
    };
}
