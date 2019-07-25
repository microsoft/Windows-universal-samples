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

Scenario5_GetSingleDevice::Scenario5_GetSingleDevice()
{
    InitializeComponent();
}

void Scenario5_GetSingleDevice::OnNavigatedTo(NavigationEventArgs^ e)
{
    resultsListView->ItemsSource = resultCollection;
}

void Scenario5_GetSingleDevice::Page_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
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
            InformationKindTextBox->Text = deviceInformationKind.ToString();
            getButton->IsEnabled = true;
        }
    }, concurrency::task_continuation_context::use_current());
}

void Scenario5_GetSingleDevice::GetButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    resultCollection->Clear();

    String^ interfaceId = interfaceIdTextBox->Text;
    getButton->IsEnabled = false;

    rootPage->NotifyUser("CreateFromIdAsync operation started...", NotifyType::StatusMessage);

    // When you want to "save" a DeviceInformation to get it back again later,
    // use both the DeviceInformation.Kind and the DeviceInformation.Id.
    concurrency::create_task(DeviceInformation::CreateFromIdAsync(interfaceId, nullptr, deviceInformationKind)).then(
        [this](DeviceInformation^ deviceInfo)
    {
        rootPage->NotifyUser("CreateFromIdAsync operation completed.", NotifyType::StatusMessage);

        resultCollection->Append(ref new DeviceInformationDisplay(deviceInfo));
    }).then([this](concurrency::task<void> precedingTask)
    {
        try
        {
            precedingTask.get();
        }
        catch (COMException^ ex)
        {
            if (ex->HResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
            {
                rootPage->NotifyUser("Invalid Interface Id", NotifyType::ErrorMessage);
            }
            else
            {
                throw;
            }
        }
        getButton->IsEnabled = true;
        getButton->Focus(Windows::UI::Xaml::FocusState::Keyboard);
    });
}
