//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

//
// Scenario1_DataEvents.xaml.cpp
// Implementation of the Scenario1_DataEvents class
//

#include "pch.h"
#include "Scenario1_DataEvents.xaml.h"

using namespace CustomSensors;
using namespace SDKTemplate;

using namespace concurrency;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::Sensors;
using namespace Windows::Devices::Sensors::Custom;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Platform;

// The following ID is defined by vendors and is unique to a custom sensor type. Each custom sensor driver should define one unique ID.
// 
// The ID below is defined in the custom sensor driver sample available in the SDK. It identifies the custom sensor CO2 emulation sample driver.
const GUID GUID_CustomSensorDevice_VendorDefinedSubTypeID = { 0x4025a865, 0x638c, 0x43aa, 0xa6, 0x88, 0x98, 0x58, 0x9, 0x61, 0xee, 0xae };

// A property key is defined by vendors for each datafield property a custom sensor driver exposes. Property keys are defined
// per custom sensor driver and is unique to each custom sensor type.
// 
// The following property key represents the CO2 level as defined in the custom sensor CO2 emulation driver sample available in the WDK.
// In this example only one key is defined, but other drivers may define more than one key by rev'ing up the property key index.
#define CO2_LEVEL_KEY "{74879888-a3cc-45c6-9ea9-058838256433} 1"

Scenario1_DataEvents::Scenario1_DataEvents() : m_RootPage(MainPage::Current), m_DesiredReportInterval(0)
{
    Guid customSensorGuid(GUID_CustomSensorDevice_VendorDefinedSubTypeID);

    InitializeComponent();

    String^ customSensorSelector = CustomSensor::GetDeviceSelector(customSensorGuid);
    m_Watcher = DeviceInformation::CreateWatcher(customSensorSelector);

    m_Watcher->Added += ref new TypedEventHandler<DeviceWatcher^, DeviceInformation^>(this, &Scenario1_DataEvents::OnCustomSensorAdded);

    m_Watcher->Start();

}

/// <summary>
/// Invoked when the device watcher finds a matching custom sensor device 
/// </summary>
/// <param name="watcher">device watcher</param>
/// <param name="customSensorDevice">device information for the custom sensor that was found</param>
void Scenario1_DataEvents::OnCustomSensorAdded(DeviceWatcher^ watcher, DeviceInformation^ customSensorDevice)
{
    try
    {
        auto getCustomSensorTask = create_task(CustomSensor::FromIdAsync(customSensorDevice->Id));

        getCustomSensorTask.then([this](CustomSensor^ customSensor)
        {
            if (customSensor != nullptr)
            {
                // When multiple custom sensors exist on the system, OnCustomSensorAdded 
                // may be called concurrently using multiple threadpool threads
                // Lock the section to protect this->customSensor from concurrent accesses
                auto lock = m_CritsecCustomSensor.Lock();

                // Just pick the first matching custom sensor, ignore the others
                if (nullptr == this->m_CustomSensor)
                {
                    this->m_CustomSensor = customSensor;
                    // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
                    // This value will be used later to activate the sensor.
                    // In the case below, we defined a 200ms report interval as being suitable for the purpose of this app.
                    uint32 minReportInterval = customSensor->MinimumReportInterval;
                    m_DesiredReportInterval = minReportInterval > 200 ? minReportInterval : 200;
                }
            }
            else
            {
                Dispatcher->RunAsync(
                    CoreDispatcherPriority::Normal,
                    ref new DispatchedHandler(
                        [this]()
                        {
                            m_RootPage->NotifyUser("No custom sensor found", NotifyType::ErrorMessage);
                        },
                        CallbackContext::Any
                    )
                );
            }
        });
    }
    catch (Platform::Exception^ e)
    {
        Dispatcher->RunAsync(
            CoreDispatcherPriority::Normal,
            ref new DispatchedHandler(
                [this, e]()
                {
                    // May happen when the user denies access to the sensor
                    m_RootPage->NotifyUser(e->ToString(), NotifyType::ErrorMessage);
                },
                CallbackContext::Any
            )
        );
    }
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario1_DataEvents::OnNavigatedTo(NavigationEventArgs^ e)
{
    ScenarioEnableButton->IsEnabled = true;
    ScenarioDisableButton->IsEnabled = false;
}

/// <summary>
/// Invoked when this page is no longer displayed.
/// </summary>
/// <param name="e"></param>
void Scenario1_DataEvents::OnNavigatedFrom(NavigationEventArgs^ e)
{
    // If the navigation is external to the app do not clean up.
    // This can occur on Phone when suspending the app.
    if (e->NavigationMode == NavigationMode::Forward && e->Uri == nullptr)
    {
        return;
    }

    if (ScenarioDisableButton->IsEnabled)
    {
        Window::Current->VisibilityChanged -= m_VisibilityToken;
        m_CustomSensor->ReadingChanged -= m_ReadingToken;
    }
}

/// <summary>
/// This is the event handler for VisibilityChanged events. You would register for these notifications
/// if handling sensor data when the app is not visible could cause unintended actions in the app.
/// </summary>
/// <param name="sender"></param>
/// <param name="e">
/// Event data that can be examined for the current visibility state.
/// </param>
void Scenario1_DataEvents::VisibilityChanged(Object^ sender, VisibilityChangedEventArgs^ e)
{
    // The app should watch for VisibilityChanged events to disable and re-enable sensor input as appropriate
    if (ScenarioDisableButton->IsEnabled)
    {
        if (e->Visible)
        {
            // Re-enable sensor input (no need to restore the desired reportInterval... it is restored for us upon app resume)
            m_ReadingToken = m_CustomSensor->ReadingChanged += ref new TypedEventHandler<CustomSensor^, CustomSensorReadingChangedEventArgs^>(this, &Scenario1_DataEvents::ReadingChanged);
        }
        else
        {
            // Disable sensor input (no need to restore the default reportInterval... resources will be released upon app suspension)
            m_CustomSensor->ReadingChanged -= m_ReadingToken;
        }
    }
}

void Scenario1_DataEvents::ReadingChanged(CustomSensor^ sender, CustomSensorReadingChangedEventArgs^ e)
{
    // We need to dispatch to the UI thread to display the output
    Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler(
            [this, e]()
            {
                CustomSensorReading^ reading = e->Reading;
                IMapView<String^, Object^>^ readingProps = reading->Properties;

                if (readingProps->HasKey(CO2_LEVEL_KEY))
                {
                    ScenarioOutput_CO2Level->Text = readingProps->Lookup(CO2_LEVEL_KEY)->ToString();
                }
            },
            CallbackContext::Any
            )
        );
}

void Scenario1_DataEvents::ScenarioEnable(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (m_CustomSensor != nullptr)
    {
        // Establish the report interval
        m_CustomSensor->ReportInterval = m_DesiredReportInterval;

        m_VisibilityToken = Window::Current->VisibilityChanged += ref new WindowVisibilityChangedEventHandler(this, &Scenario1_DataEvents::VisibilityChanged);
        m_ReadingToken = m_CustomSensor->ReadingChanged += ref new TypedEventHandler<CustomSensor^, CustomSensorReadingChangedEventArgs^>(this, &Scenario1_DataEvents::ReadingChanged);

        ScenarioEnableButton->IsEnabled = false;
        ScenarioDisableButton->IsEnabled = true;
    }
    else
    {
        m_RootPage->NotifyUser("No custom sensor found", NotifyType::ErrorMessage);
    }
}

void Scenario1_DataEvents::ScenarioDisable(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    Window::Current->VisibilityChanged -= m_VisibilityToken;
    m_CustomSensor->ReadingChanged::remove(m_ReadingToken);

    // Restore the default report interval to release resources while the sensor is not in use
    m_CustomSensor->ReportInterval = 0;

    ScenarioEnableButton->IsEnabled = true;
    ScenarioDisableButton->IsEnabled = false;
}
