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

namespace winrt::SDKTemplate
{
    struct SampleState
    {
        static Windows::UI::Notifications::ToastNotification DisplayToast(hstring const& message);
        static bool IsClipboardContentChangedEnabled();
        static bool EnableClipboardContentChangedNotifications(bool enable);
        static hstring BuildClipboardFormatsOutputString();
    private:
        static event_token clipboardContentChangedToken;
        static event_token activatedToken;
        static bool isClipboardContentChangedEnabled;
        static bool needToPrintClipboardFormat;
        static bool isApplicationWindowActive;

        static void AddClipboardFormatsOutputString(std::wostringstream& output);
        static void DisplayChangedFormats();
        static void HandleClipboardChanged();
        static void OnWindowActivated(Windows::Foundation::IInspectable const& sender, Windows::UI::Core::WindowActivatedEventArgs const& e);
        static void OnClipboardChanged(Windows::Foundation::IInspectable const& sender, Windows::Foundation::IInspectable const& e);
    };
}
