// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario3_BackgroundDeviceWatcher.xaml.h"

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

Scenario3::Scenario3() : 
    rootPage(MainPage::Current),
    backgroundTaskRegistration(nullptr)
{
    InitializeComponent();
}

void Scenario3::OnNavigatedTo(NavigationEventArgs^ e)
{
    selectorComboBox->ItemsSource = DeviceSelectorChoices::BackgroundDeviceWatcherSelectors;
    selectorComboBox->SelectedIndex = 0;

    DataContext = this;

    // Determine if the background task is already active
    std::for_each(begin(BackgroundTaskRegistration::AllTasks), end(BackgroundTaskRegistration::AllTasks), [&](IKeyValuePair<Guid, IBackgroundTaskRegistration^>^ task)
    {
        if (backgroundTaskName == task->Value->Name)
        {
            backgroundTaskRegistration = task->Value;
            startWatcherButton->IsEnabled = false;
            stopWatcherButton->IsEnabled = true;
        }
    });
}

void Scenario3::StartWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    StartWatcher();
}

void Scenario3::StopWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    StopWatcher();
}

void Scenario3::StartWatcher()
{
    Vector<DeviceWatcherEventKind>^ triggerEventKinds = ref new Vector<DeviceWatcherEventKind>({
        DeviceWatcherEventKind::Add, 
        DeviceWatcherEventKind::Remove, 
        DeviceWatcherEventKind::Update });
    DeviceWatcher^ deviceWatcher = nullptr;

    startWatcherButton->IsEnabled = false;

    // First get the device selector chosen by the UI.
    DeviceSelectorInfo^ deviceSelectorInfo = safe_cast<DeviceSelectorInfo^>(selectorComboBox->SelectedItem);

    if (nullptr == deviceSelectorInfo->Selector)
    {
        // If the a pre-canned device class selector was chosen, call the DeviceClass overload
        deviceWatcher = DeviceInformation::CreateWatcher(deviceSelectorInfo->DeviceClassSelector);
    }
    else if (deviceSelectorInfo->Kind == DeviceInformationKind::Unknown)
    {
        // Use AQS string selector from dynamic call to a device api's GetDeviceSelector call
        // Kind will be determined by the selector
        deviceWatcher = DeviceInformation::CreateWatcher(
            deviceSelectorInfo->Selector,
            nullptr // don't request additional properties for this sample
            );
    }
    else
    {
        // Kind is specified in the selector info
        deviceWatcher = DeviceInformation::CreateWatcher(
            deviceSelectorInfo->Selector,
            nullptr, // don't request additional properties for this sample
            deviceSelectorInfo->Kind);
    }
    
    // Get the background trigger for this watcher
    DeviceWatcherTrigger^ deviceWatcherTrigger = deviceWatcher->GetBackgroundTrigger(triggerEventKinds);

    rootPage->NotifyUser("Starting Watcher...", NotifyType::StatusMessage);

    // Register this trigger for our background task
    RegisterBackgroundTask(deviceWatcherTrigger);

    stopWatcherButton->IsEnabled = true;

    rootPage->NotifyUser("Watcher started...", NotifyType::StatusMessage);
}

void Scenario3::RegisterBackgroundTask(DeviceWatcherTrigger^ deviceWatcherTrigger)
{
    BackgroundTaskBuilder^ taskBuilder = nullptr;

    // First see if we already have this background task registered. If so, unregister
    // it before we register it with the new trigger.

    std::for_each(begin(BackgroundTaskRegistration::AllTasks), end(BackgroundTaskRegistration::AllTasks), [&](IKeyValuePair<Guid, IBackgroundTaskRegistration^>^ task)
    {
        if (backgroundTaskName == task->Value->Name)
        {
            UnregisterBackgroundTask(task->Value);
        }
    });

    taskBuilder = ref new BackgroundTaskBuilder();
    taskBuilder->Name = backgroundTaskName;
    taskBuilder->TaskEntryPoint = "BackgroundDeviceWatcherTaskCpp.BackgroundDeviceWatcher";
    taskBuilder->SetTrigger(deviceWatcherTrigger);

    backgroundTaskRegistration = taskBuilder->Register();
    backgroundTaskRegistration->Completed += ref new BackgroundTaskCompletedEventHandler(
        [this](BackgroundTaskRegistration^ sender, BackgroundTaskCompletedEventArgs^ args)
    {
        rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(
            [this]()
        {
            rootPage->NotifyUser("Background task completed", NotifyType::StatusMessage);
        }));
    });
}

void Scenario3::StopWatcher()
{
    stopWatcherButton->IsEnabled = false;
    if (nullptr != backgroundTaskRegistration)
    {
        UnregisterBackgroundTask(backgroundTaskRegistration);
    }
    startWatcherButton->IsEnabled = true;
}

void Scenario3::UnregisterBackgroundTask(IBackgroundTaskRegistration^ taskReg)
{
    taskReg->Unregister(true);
    ApplicationDataContainer^ settings = ApplicationData::Current->LocalSettings;
    settings->Values->Insert("eventCount", nullptr);
}
