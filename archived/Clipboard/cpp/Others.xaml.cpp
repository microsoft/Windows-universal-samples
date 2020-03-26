//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

//
// Others.xaml.cpp
// Implementation of the OtherScenarios class
//

#include "pch.h"
#include "Others.xaml.h"
#include "SampleConfiguration.h"
#include "MainPage.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::DataTransfer;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

bool OtherScenarios::registerContentChanged = false;

OtherScenarios::OtherScenarios()
{
    InitializeComponent();
}

void OtherScenarios::OnNavigatedTo(NavigationEventArgs^ e)
{
    rootPage = MainPage::Current;
    RegisterClipboardContentChanged->IsChecked = registerContentChanged;
}

void OtherScenarios::ClearOutput()
{
    rootPage->NotifyUser("", NotifyType::StatusMessage);
    OutputText->Text = "";
}

void OtherScenarios::DisplayFormats()
{
    ClearOutput();
    OutputText->Text = rootPage->BuildClipboardFormatsOutputString();
}

#pragma region OtherScenarios handlers

void OtherScenarios::ShowFormatButton_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    DisplayFormats();
}

void OtherScenarios::EmptyClipboardButton_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    try
    {
        DataTransfer::Clipboard::Clear();
        OutputText->Text = "Clipboard has been emptied.";
    }
    catch (Exception^ ex)
    {
        // Emptying Clipboard can potentially fail - for example, if another application is holding Clipboard open
        rootPage->NotifyUserBackgroundThread("Error emptying the clipboard - " + ex->Message, NotifyType::ErrorMessage);
    }
}

void OtherScenarios::ClearOutputButton_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    ClearOutput();
}

void OtherScenarios::RegisterClipboardContentChanged_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    if (registerContentChanged != RegisterClipboardContentChanged->IsChecked->Value)
    {
        ClearOutput();
        registerContentChanged = RegisterClipboardContentChanged->IsChecked->Value;

        // In this sample, we register for Clipboard update notifications on the MainPage so that we can
        // navigate between scenarios and still receive updates. The sample also registers for window activated 
        // notifications since the app needs to be in the foreground to access the clipboard. Once we receive
        // a clipboard update notification, we display the new content (if the app is in the foreground). If the
        // sample is not in the foreground, we defer displaying it until it is.
        rootPage->EnableClipboardContentChangedNotifications(registerContentChanged);
        if (registerContentChanged)
        {
            OutputText->Text = "Successfully registered for clipboard update notification.";
        }
        else
        {
            OutputText->Text = "Successfully un-registered for clipboard update notification.";
        }
    }
}

#pragma endregion
