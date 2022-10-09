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
#include "pch.h"

namespace winrt
{
    hstring to_hstring(Windows::System::UserType value);
    hstring to_hstring(Windows::System::UserAuthenticationStatus value);
}

namespace winrt::SDKTemplate::Helpers
{
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable>> GetUserViewModelsAsync();
}
