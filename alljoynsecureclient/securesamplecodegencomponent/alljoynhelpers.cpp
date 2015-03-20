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

using namespace Windows::Devices::AllJoyn;
using namespace Microsoft::WRL;
using namespace Platform;

alljoyn_busattachment AllJoynHelpers::GetInternalBusAttachment(_In_ AllJoynBusAttachment^ busAttachment)
{
    ComPtr<IUnknown> iUnknown = reinterpret_cast<IUnknown*>(busAttachment);
    ComPtr<IWindowsDevicesAllJoynBusAttachmentInterop> interop;
    iUnknown.As(&interop);
    UINT64 win32handle;
    interop->get_Win32Handle(&win32handle);
    return reinterpret_cast<alljoyn_busattachment>(win32handle);
}

String^ AllJoynHelpers::MultibyteToPlatformString(_In_ PCSTR str)
{
    size_t length = strlen(str);

    UINT32 requiredBytes = MultiByteToWideChar(
        CP_UTF8,
        0,
        str,
        (int)length,
        nullptr,
        0);

    if (requiredBytes == 0)
    {
        return ref new String();
    }

    std::vector<wchar_t> buffer(requiredBytes + 1);

    UINT32 convertedBytes = MultiByteToWideChar(
        CP_UTF8,
        0,
        str,
        (int)length,
        (LPWSTR)&buffer[0],
        requiredBytes);

    buffer[convertedBytes] = '\0';
    return ref new String(&buffer[0]);
}

std::vector<char> AllJoynHelpers::PlatformToMultibyteString(_In_ String^ str)
{
    UINT32 requiredBytes = WideCharToMultiByte(
        CP_UTF8,
        WC_ERR_INVALID_CHARS,
        str->Data(),
        str->Length(),
        nullptr,
        0,
        nullptr,
        nullptr);

    if (requiredBytes == 0)
    {
        return std::vector<char>();
    }

    std::vector<char> buffer(requiredBytes + 1);

    UINT32 convertedBytes = WideCharToMultiByte(
        CP_UTF8,
        WC_ERR_INVALID_CHARS,
        str->Data(),
        str->Length(),
        (LPSTR)&buffer[0],
        requiredBytes,
        nullptr,
        nullptr);

    buffer[convertedBytes] = '\0';
    return buffer;
}

QStatus AllJoynHelpers::CreateInterfaces(_Inout_ AllJoynBusAttachment^ busAttachment, _In_ PCSTR introspectionXml)
{
    return alljoyn_busattachment_createinterfacesfromxml(AllJoynHelpers::GetInternalBusAttachment(busAttachment), introspectionXml);
}

Platform::String^ AllJoynHelpers::GetObjectPath(_In_ alljoyn_aboutobjectdescription objectDescription, _In_ PCSTR interfaceName)
{
    PCSTR path;
    if (alljoyn_aboutobjectdescription_getinterfacepaths(objectDescription, interfaceName, &path, 1) == 0)
    {
        return nullptr;
    }

    return AllJoynHelpers::MultibyteToPlatformString(path);
}