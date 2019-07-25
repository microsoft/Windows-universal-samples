// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario6_CustomFilterAddedProps.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Devices::Enumeration;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Scenario6_CustomFilterAddedProps::Scenario6_CustomFilterAddedProps()
{
    InitializeComponent();
    deviceWatcherHelper = ref new DeviceWatcherHelper(resultCollection, Dispatcher);
}

void Scenario6_CustomFilterAddedProps::OnNavigatedTo(NavigationEventArgs^ e)
{
    resultsListView->ItemsSource = resultCollection;
}

void Scenario6_CustomFilterAddedProps::OnNavigatedFrom(NavigationEventArgs^ e)
{
    deviceWatcherHelper->Reset();
}

void Scenario6_CustomFilterAddedProps::StartWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    StartWatcher();
}

void Scenario6_CustomFilterAddedProps::StopWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    StopWatcher();
}

void Scenario6_CustomFilterAddedProps::StartWatcher()
{
    aqsFilterTextBox->IsEnabled = false;
    startWatcherButton->IsEnabled = false;
    resultCollection->Clear();

    // Request some additional properties.  In this saample, these extra properties are just used in the ResultsListViewTemplate. 
    // Take a look there in the XAML. Also look at the coverter used by the XAML GeneralPropertyValueConverter.  In general you just use
    // DeviceInformation.Properties["<property name>"] to get a property. e.g. DeviceInformation.Properties["System.Devices.InterfaceClassGuid"].
    Vector<String^>^ requestedProperties = ref new Vector<String^>();
    requestedProperties->Append("System.Devices.InterfaceClassGuid");
    requestedProperties->Append("System.ItemNameDisplay");

    // Use AQS string filter from the text box
    DeviceWatcher^ deviceWatcher = DeviceInformation::CreateWatcher(
        aqsFilterTextBox->Text,
        requestedProperties);

    rootPage->NotifyUser("Starting Watcher...", NotifyType::StatusMessage);
    deviceWatcherHelper->StartWatcher(deviceWatcher);
    stopWatcherButton->IsEnabled = true;
    stopWatcherButton->Focus(::FocusState::Keyboard);
    aqsFilterTextBox->IsEnabled = true;
}

void Scenario6_CustomFilterAddedProps::StopWatcher()
{
    aqsFilterTextBox->IsEnabled = false;
    stopWatcherButton->IsEnabled = false;

    deviceWatcherHelper->StopWatcher();

    startWatcherButton->IsEnabled = true;
    startWatcherButton->Focus(::FocusState::Keyboard);
    aqsFilterTextBox->IsEnabled = true;
}

