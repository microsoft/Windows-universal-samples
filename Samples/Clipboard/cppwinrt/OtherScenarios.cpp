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

#include "pch.h"
#include "SampleConfiguration.h"
#include "OtherScenarios.h"
#include "OtherScenarios.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::ApplicationModel::DataTransfer;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    bool OtherScenarios::registerContentChanged = false;

    OtherScenarios::OtherScenarios()
    {
        InitializeComponent();
    }

    void OtherScenarios::OnNavigatedTo(NavigationEventArgs const&)
    {
        RegisterClipboardContentChanged().IsChecked(registerContentChanged);
    }

    void OtherScenarios::ShowFormatButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        DisplayFormats();
    }

    void OtherScenarios::EmptyClipboardButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        try
        {
            Clipboard::Clear();
        }
        catch (hresult_error const& ex)
        {
            rootPage.NotifyUser(L"Error emptying clipboard: " + ex.message() + L". Try again", NotifyType::ErrorMessage);
        }
    }

    void OtherScenarios::RegisterClipboardContentChanged_Click(IInspectable const&, RoutedEventArgs const&)
    {
        bool checked = RegisterClipboardContentChanged().IsChecked().Value();
        if (registerContentChanged != checked)
        {
            registerContentChanged = checked;
            ClearOutput();

            // In this sample, we register for Clipboard update notifications on the MainPage so that we can
            // navigate between scenarios and still receive updates. The sample also registers for window activated 
            // notifications since the app needs to be in the foreground to access the clipboard. Once we receive
            // a clipboard update notification, we display the new content (if the app is in the foreground). If the
            // sample is not in the foreground, we defer displaying it until it is.
            SampleState::EnableClipboardContentChangedNotifications(registerContentChanged);
            if (registerContentChanged)
            {
                OutputText().Text(L"Successfully registered for clipboard update notification.");
            }
            else
            {
                OutputText().Text(L"Successfully un-registered for clipboard update notification.");
            }
        }
    }

    void OtherScenarios::ClearOutputButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        ClearOutput();
    }

    void OtherScenarios::ClearOutput()
    {
        rootPage.NotifyUser(L"", NotifyType::StatusMessage);
        OutputText().Text(L"");
    }

    void OtherScenarios::DisplayFormats()
    {
        rootPage.NotifyUser(L"", NotifyType::StatusMessage);
        OutputText().Text(SampleState::BuildClipboardFormatsOutputString());
    }
}

