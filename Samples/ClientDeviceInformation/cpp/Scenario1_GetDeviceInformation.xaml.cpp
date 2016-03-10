// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario1_GetDeviceInformation.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Security::ExchangeActiveSyncProvisioning;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario1_GetDeviceInformation::Scenario1_GetDeviceInformation() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario1_GetDeviceInformation::Launch_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    DeviceID->Text = "";
    FriendlyName->Text = "";
    SystemManufacturer->Text = "";
    SystemProductName->Text = "";
    SystemSku->Text = "";
    OperatingSystem->Text = "";
    try
    {

        EasClientDeviceInformation^ CurrentDeviceInfor = ref new EasClientDeviceInformation();

        DeviceID->Text = CurrentDeviceInfor->Id.ToString();
        OperatingSystem->Text = CurrentDeviceInfor->OperatingSystem;
        FriendlyName->Text = CurrentDeviceInfor->FriendlyName;
        SystemManufacturer->Text = CurrentDeviceInfor->SystemManufacturer;
        SystemProductName->Text = CurrentDeviceInfor->SystemProductName;
        SystemSku->Text = CurrentDeviceInfor->SystemSku;

    }
    catch (Platform::Exception^ ex)
    {
        MainPage::Current->NotifyUser(ex->Message, NotifyType::ErrorMessage);
    }

}

void Scenario1_GetDeviceInformation::Reset_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    try
    {
        DeviceID->Text = "";
        FriendlyName->Text = "";
        SystemManufacturer->Text = "";
        SystemProductName->Text = "";
        SystemSku->Text = "";
        OperatingSystem->Text = "";
    }
    catch (Platform::Exception^ ex)
    {
        MainPage::Current->NotifyUser(ex->Message, NotifyType::ErrorMessage);
    }
}

