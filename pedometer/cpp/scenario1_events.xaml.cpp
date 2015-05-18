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
#include "Scenario1_Events.xaml.h"

const unsigned int HundredNanoSecondsToMilliseconds = 10000;

using namespace PedometerCPP;
using namespace SDKTemplate;


using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Devices::Sensors;
using namespace Concurrency;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::UI::Core;

Scenario1_Events::Scenario1_Events() : rootPage(MainPage::Current)
{
    InitializeComponent();
    
    // disable the 'Register' button until we have a pedometer
    rootPage->NotifyUser("Waiting for Default Pedometer (Pedometer::GetDefaultAsync)", NotifyType::StatusMessage);
    RegisterButton->IsEnabled = false;

    // get the default pedometer asynchronously
    task<Pedometer^> pedometerTask = create_task(Pedometer::GetDefaultAsync);
    try
    {
        pedometerTask.then([this](task<Pedometer^> task)
        {
            pedometer = task.get();
            if (nullptr != pedometer)
            {
                rootPage->NotifyUser("Pedometer available - Not registered for events", NotifyType::StatusMessage);
                RegisterButton->IsEnabled = true;
            }
            else
            {
                rootPage->NotifyUser("No pedometer available", NotifyType::ErrorMessage);
                RegisterButton->IsEnabled = false;
            }
        });
    }
    catch (AccessDeniedException^)
    {
        rootPage->NotifyUser("Access to the default pedometer is denied", NotifyType::ErrorMessage);
    }
    registeredForEvents = false;
}

/// <summary>
/// 'Register ReadingChanged' button click handler. Registers to the '
/// ReadingChanged' event of the default pedometer opened earlier.
/// </summary>
/// <param name="sender">unused</param>
/// <param name="e">unused</param>
void Scenario1_Events::Button_Click(Platform::Object^ /*sender*/, Windows::UI::Xaml::RoutedEventArgs^ /*e*/)
{
    UpdateEventRegistration(!registeredForEvents);
}


/// <summary>
/// Invoked when navigating away from this page - unregisters for 'ReadingChanged' event.
/// </summary>
/// <param name="e">unused</param>
void Scenario1_Events::OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ /*e*/)
{
    if (registeredForEvents)
    {
        UpdateEventRegistration(false);
    }
}


/// <summary>
/// Invoked when the underlying Pedometer sees a change in the step count for a step kind
/// </summary>
/// <param name="sender">unused</param>
/// <param name="args">Pedometer reading that is being notified</param>
void Scenario1_Events::OnReadingChanged(Windows::Devices::Sensors::Pedometer ^/*sender*/, Windows::Devices::Sensors::PedometerReadingChangedEventArgs ^args)
{
    // We need to dispatch to the UI thread to display the output
    Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler(
        [this, args]()
        {
            PedometerReading^ reading = args->Reading;
            int32 newCount = 0;
            long long duration = 0;

            // update step counts based on the step kind
            switch (reading->StepKind)
            {
            case PedometerStepKind::Unknown:
                if (reading->CumulativeSteps < unknownStepCount)
                {
                    unknownStepCount = 0;
                }
                newCount = reading->CumulativeSteps - unknownStepCount;
                unknownStepCount = reading->CumulativeSteps;
                ScenarioOutput_UnknownCount->Text = unknownStepCount.ToString();
                duration = reading->CumulativeStepsDuration.Duration / HundredNanoSecondsToMilliseconds;
                ScenarioOutput_UnknownDuration->Text = duration.ToString();
                break;
            case PedometerStepKind::Walking:
                if (reading->CumulativeSteps < walkingStepCount)
                {
                    walkingStepCount = 0;
                }
                newCount = reading->CumulativeSteps - walkingStepCount;
                walkingStepCount = reading->CumulativeSteps;
                ScenarioOutput_WalkingCount->Text = walkingStepCount.ToString();
                duration = reading->CumulativeStepsDuration.Duration / HundredNanoSecondsToMilliseconds;
                ScenarioOutput_WalkingDuration->Text = duration.ToString();
                break;
            case PedometerStepKind::Running:
                if (reading->CumulativeSteps < runningStepCount)
                {
                    runningStepCount = 0;
                }
                newCount = reading->CumulativeSteps - runningStepCount;
                runningStepCount = reading->CumulativeSteps;
                ScenarioOutput_RunningCount->Text = runningStepCount.ToString();
                duration = reading->CumulativeStepsDuration.Duration / HundredNanoSecondsToMilliseconds;
                ScenarioOutput_RunningDuration->Text = duration.ToString();
                break;
            default:
                break;
            }

            totalCumulativeSteps += newCount;
            ScenarioOutput_TotalStepCount->Text = totalCumulativeSteps.ToString();

            auto timestampFormatter = ref new DateTimeFormatter("day month year hour minute second");
            ScenarioOutput_Timestamp->Text = timestampFormatter->Format(reading->Timestamp);
        },
        CallbackContext::Any
        )
    );
}


/// <summary>
/// Helper function to register/un-register to the 'ReadingChanged' event of the default pedometer
/// </summary>
/// <param name="regsiterForEvents">tell if a registration or un-registration actions needs to be taken</param>
void Scenario1_Events::UpdateEventRegistration(bool regsiterForEvents)
{
    if (regsiterForEvents)
    {
        pedometer->ReportInterval = 20000;
        readingToken = (pedometer->ReadingChanged += ref new Windows::Foundation::TypedEventHandler<Windows::Devices::Sensors::Pedometer ^, Windows::Devices::Sensors::PedometerReadingChangedEventArgs ^>(this, &Scenario1_Events::OnReadingChanged));
        rootPage->NotifyUser("Registered for step count changes", NotifyType::StatusMessage);
        registeredForEvents = true;
        RegisterButton->Content = "Un-Register ReadingChanged";
    }
    else
    {
        pedometer->ReadingChanged -= readingToken;
        rootPage->NotifyUser("Pedometer available - Not registered for events", NotifyType::StatusMessage);
        registeredForEvents = false;
        RegisterButton->Content = "Register ReadingChanged";
    }
}