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
#include "Scenario8_VisitsBackgroundTask.xaml.h"

using namespace SDKTemplate;
using namespace SDKTemplate::GeolocationCPP;

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::ApplicationModel::Background;
using namespace Windows::Data::Json;
using namespace Windows::Devices::Geolocation;
using namespace Windows::Storage;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;


Scenario8::Scenario8() :
    rootPage(MainPage::Current),
    _sampleBackgroundTaskName("SampleVisitBackgroundTask"),
    _sampleBackgroundTaskEntryPoint("BackgroundTask.VisitBackgroundTask")
{
    InitializeComponent();

    // using data binding to the root page collection of Visit Items associated with events
    _visitBackgroundEvents = ref new Vector<String^>();
    VisitBackgroundEventsListBox->DataContext = _visitBackgroundEvents;
    FillEventListBoxWithExistingEvents();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario8::OnNavigatedTo(NavigationEventArgs^ e)
{
    // Look for an already registered background task
    _visitTask = nullptr;

    auto iter = BackgroundTaskRegistration::AllTasks->First();
    while (iter->HasCurrent)
    {
        auto task = iter->Current;
        if (task->Value->Name == _sampleBackgroundTaskName)
        {
            _visitTask = safe_cast<BackgroundTaskRegistration^>(task->Value);
            break;
        }
        iter->MoveNext();
    }

    if (_visitTask != nullptr)
    {
        FillEventListBoxWithExistingEvents();

        // Register for background task completion notifications
        _taskCompletedToken = _visitTask->Completed += ref new BackgroundTaskCompletedEventHandler(this, &Scenario8::OnCompleted);

        // Check the background access status of the application and display the appropriate status message
        switch (BackgroundExecutionManager::GetAccessStatus())
        {
        case BackgroundAccessStatus::AlwaysAllowed:
        case BackgroundAccessStatus::AllowedSubjectToSystemPolicy:
            rootPage->NotifyUser("Background task is already registered. Waiting for next update...", NotifyType::StatusMessage);
            break;

        default:
            rootPage->NotifyUser("Not able to run in background.", NotifyType::ErrorMessage);
            break;
        }

        UpdateButtonStates(/*registered:*/ true);
    }
    else
    {
        UpdateButtonStates(/*registered:*/ false);
    }
}

void Scenario8::RegisterBackgroundTask(Object^ sender, RoutedEventArgs^ e)
{
    try
    {
        // Get permission for a background task from the user. If the user has already answered once,
        // this does nothing and the user must manually update their preference via PC Settings.
        create_task(BackgroundExecutionManager::RequestAccessAsync()).then([this](BackgroundAccessStatus backgroundAccessStatus)
        {
            // Create a new background task builder
            BackgroundTaskBuilder^ _visitTaskBuilder = ref new BackgroundTaskBuilder();

            _visitTaskBuilder->Name = _sampleBackgroundTaskName;
            _visitTaskBuilder->TaskEntryPoint = _sampleBackgroundTaskEntryPoint;

            // Create a new visit trigger
            auto trigger = ref new GeovisitTrigger();

            // Set the desired monitoring scope.
            // For higher granularity, choose venue/building level changes.
            // For lower granularity more or less in the range of zipcode level changes, choose city.
            // Choosing Venue here as an example.
            trigger->MonitoringScope = VisitMonitoringScope::Venue;

            // Associate the trigger with the background task builder
            _visitTaskBuilder->SetTrigger(trigger);

            // If it is important that there is user presence and/or
            // internet connection when OnCompleted is called
            // the following could be called before calling Register()
            // SystemCondition^ condition = ref new SystemCondition(SystemConditionType::UserPresent | SystemConditionType::InternetAvailable);
            // _visitTaskBuilder->AddCondition(condition);

            // Register the background task
            _visitTask = _visitTaskBuilder->Register();

            // Register for background task completion notifications
            _taskCompletedToken = _visitTask->Completed += ref new BackgroundTaskCompletedEventHandler(this, &Scenario8::OnCompleted);

            UpdateButtonStates(/*registered:*/ true);

            // Check the background access status of the application and display the appropriate status message
            switch (backgroundAccessStatus)
            {
            case BackgroundAccessStatus::AlwaysAllowed:
            case BackgroundAccessStatus::AllowedSubjectToSystemPolicy:
                rootPage->NotifyUser("Background task registered.", NotifyType::StatusMessage);

                // Need to request access to location
                // This must be done with background task registration
                // because the background task cannot display UI
                RequestLocationAccess();
                break;

            default:
                rootPage->NotifyUser("Background tasks may be disabled for this app", NotifyType::ErrorMessage);
                break;

            }
        });
    }
    catch (Exception^ ex)
    {
        rootPage->NotifyUser(ex->ToString(), NotifyType::ErrorMessage);
        UpdateButtonStates(/*registered:*/ false);
    }
}

void Scenario8::UnregisterBackgroundTask(Object^ sender, RoutedEventArgs^ e)
{
    // Unregister the background task
    if (_visitTask != nullptr)
    {
        _visitTask->Completed -= _taskCompletedToken;
        _visitTask->Unregister(true);
        _visitTask = nullptr;
    }

    rootPage->NotifyUser("Visit background task unregistered", NotifyType::StatusMessage);

    ClearExistingEvents();

    UpdateButtonStates(/*registered:*/ false);
}

void Scenario8::OnCompleted(BackgroundTaskRegistration^ sender, BackgroundTaskCompletedEventArgs^ args)
{
    // Update the UI with progress reported by the background task
    // We need to dispatch to the UI thread to display the output
    Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler(
            [this, args]()
    {
        try
        {
            // Throw an exception if the background task had an unrecoverable error
            args->CheckResult();

            // add background events to listbox
            FillEventListBoxWithExistingEvents();

        }
        catch (Exception^ ex)
        {
            // The background task had an error
            rootPage->NotifyUser(ex->Message, NotifyType::ErrorMessage);
        }
    }));
}

/// <summary>
/// Populate events list box with entries from BackgroundVisitEventCollection.
/// </summary>
void Scenario8::FillEventListBoxWithExistingEvents()
{
    auto settings = ApplicationData::Current->LocalSettings->Values;
    if (settings->HasKey("BackgroundVisitEventCollection"))
    {
        String^ visitEvent = safe_cast<String^>(settings->Lookup("BackgroundVisitEventCollection"));

        if (0 != visitEvent->Length())
        {
            // remove all entries and repopulate
            _visitBackgroundEvents->Clear();
            for (auto& element : JsonValue::Parse(visitEvent)->GetArray())
            {
                _visitBackgroundEvents->Append(element->ToString());
            }
        }
    }
}

/// <summary>
/// Clear events from BackgroundVisitEventCollection.
/// </summary>
void Scenario8::ClearExistingEvents()
{
    auto settings = ApplicationData::Current->LocalSettings->Values;
    settings->Insert("BackgroundVisitEventCollection", "");

    _visitBackgroundEvents->Clear();
}


/// <summary>
/// Update the enable state of the register/unregister buttons.
/// </summary>
void Scenario8::UpdateButtonStates(bool registered)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, registered]()
    {
        RegisterBackgroundTaskButton->IsEnabled = !registered;
        UnregisterBackgroundTaskButton->IsEnabled = registered;
    }));
}

void Scenario8::RequestLocationAccess()
{
    create_task(Windows::Devices::Geolocation::Geolocator::RequestAccessAsync()).then([this](GeolocationAccessStatus accessStatus)
    {
        switch (accessStatus)
        {
        case GeolocationAccessStatus::Allowed:
            break;
        case GeolocationAccessStatus::Denied:
            rootPage->NotifyUser("Access to location is denied.", NotifyType::ErrorMessage);
            break;
        default:
        case GeolocationAccessStatus::Unspecified:
            rootPage->NotifyUser("Unspecified error!", NotifyType::ErrorMessage);
            break;
        }
    });
}
