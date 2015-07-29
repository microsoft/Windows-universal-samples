// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario5_GetSingleDevice.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Devices::Enumeration;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Scenario5::Scenario5() : 
    rootPage(MainPage::Current),
    deviceInformationKind(DeviceInformationKind::Unknown)
{
    InitializeComponent();
}

void Scenario5::OnNavigatedTo(NavigationEventArgs^ e)
{
    ResultCollection = ref new Vector<DeviceInformationDisplay^>();

    DataContext = this;
}

void Scenario5::InterfaceIdTextBox_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    concurrency::create_task(DeviceInformation::FindAllAsync(DeviceClass::AudioRender)).then(
        [this](DeviceInformationCollection^ deviceInfoCollection)
    {
        if (deviceInfoCollection->Size > 0)
        {
            // When you want to "save" a DeviceInformation to get it back again later,
            // use both the DeviceInformation.Kind and the DeviceInformation.Id.
            interfaceIdTextBox->Text = deviceInfoCollection->GetAt(0)->Id;
            deviceInformationKind = deviceInfoCollection->GetAt(0)->Kind;
        }
    }, concurrency::task_continuation_context::use_current());
}

void Scenario5::GetButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    String^ interfaceId = interfaceIdTextBox->Text;

    interfaceIdTextBox->IsEnabled = false;
    getButton->IsEnabled = false;
    ResultCollection->Clear();

    rootPage->NotifyUser("CreateFromIdAsync operation started...", NotifyType::StatusMessage);

    try
    {
        // When you want to "save" a DeviceInformation to get it back again later,
        // use both the DeviceInformation.Kind and the DeviceInformation.Id.
        concurrency::create_task(DeviceInformation::CreateFromIdAsync(interfaceId)).then(
            [this](DeviceInformation^ deviceInfo)
        {
            rootPage->NotifyUser("CreateFromIdAsync operation completed.", NotifyType::StatusMessage);

            ResultCollection->Append(ref new DeviceInformationDisplay(deviceInfo));

            getButton->IsEnabled = true;
            getButton->Focus(Windows::UI::Xaml::FocusState::Keyboard);
            interfaceIdTextBox->IsEnabled = true;

        }, concurrency::task_continuation_context::use_current());
    }
    catch (COMException^)
    {
        rootPage->NotifyUser("Invalid Interface Id", NotifyType::ErrorMessage);
    }
}
