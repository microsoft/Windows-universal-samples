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

//
// pch.cpp
// Include the standard header and generate the precompiled header.
//

#include "pch.h"
#include "MainPage.xaml.h"

#include <robuffer.h>
#include <wrl\client.h>

using namespace Microsoft::WRL;
using namespace Windows::Storage::Streams;

byte* GetArrayFromBuffer(Windows::Storage::Streams::IBuffer^ buffer)
{
    ComPtr<IInspectable> base = reinterpret_cast<IInspectable*>(buffer);
    ComPtr<IBufferByteAccess> access;

    auto hr = base.As(&access);

    if (FAILED(hr))
    {
        throw Platform::Exception::CreateException(hr);
    }

    byte* data;

    hr = access->Buffer(&data);

    if (FAILED(hr))
    {
        throw Platform::Exception::CreateException(hr);
    }

    // The returned buffer is valid as long as the IBuffer passed in
    // remains valid.
    return data;
}

void NotifyStatusMessage(Platform::String^ message, SDKTemplate::NotifyType messageType)
{
    SDKTemplate::MainPage::Current->Dispatcher->RunAsync(
        Windows::UI::Core::CoreDispatcherPriority::Normal,
        ref new Windows::UI::Core::DispatchedHandler(
            [message, messageType]() {
        SDKTemplate::MainPage::Current->NotifyUser(message, messageType);
    }));
}
