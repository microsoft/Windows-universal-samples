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
// Scenario2_Polling.xaml.cpp
// Implementation of the Scenario2_Polling class
//

#include "pch.h"
#include "Scenario2_Polling.xaml.h"

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
const GUID GUID_CustomSensorDevice_VendorDefinedTypeID = { 0x4025a865, 0x638c, 0x43aa, 0xa6, 0x88, 0x98, 0x58, 0x9, 0x61, 0xee, 0xae };

// A property key is defined by vendors for each datafield property a custom sensor driver exposes. Property keys are defined
// per custom sensor driver and is unique to each custom sensor type.
// 
// The following property key represents the CO2 level as defined in the custom sensor CO2 emulation driver sample available in the WDK.
// In this example only one key is defined, but other drivers may define more than one key by rev'ing up the property key index.
#define CO2_LEVEL_KEY "{74879888-a3cc-45c6-9ea9-058838256433} 1"

Scenario2_Polling::Scenario2_Polling() : m_RootPage(MainPage::Current)
{
    String^ customSensorSelector;
    Guid customSensorGuid(GUID_CustomSensorDevice_VendorDefinedTypeID);

    InitializeComponent();

    customSensorSelector = CustomSensor::GetDeviceSelector(customSensorGuid);
    m_Watcher = DeviceInformation::CreateWatcher(customSensorSelector);

    m_Watcher->Added += ref new TypedEventHandler<DeviceWatcher^, DeviceInformation^>(this, &Scenario2_Polling::OnCustomSensorAdded);

    m_Watcher->Start();

}

/// <summary>
/// Invoked when the device watcher finds a matching custom sensor device 
/// </summary>
/// <param name="watcher">device watcher</param>
/// <param name="customSensorDevice">device information for the custom sensor that was found</param>
void Scenario2_Polling::OnCustomSensorAdded(DeviceWatcher^ watcher, DeviceInformation^ customSensorDevice)
{
    try
    {
        IAsyncOperation<CustomSensor^>^ getCustomSensorRequest = CustomSensor::FromIdAsync(customSensorDevice->Id);

        auto getCustomSensorTask = create_task(getCustomSensorRequest);

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
void Scenario2_Polling::OnNavigatedTo(NavigationEventArgs^ e)
{
}

/// <summary>
/// Invoked when this page is no longer displayed.
/// </summary>
/// <param name="e"></param>
void Scenario2_Polling::OnNavigatedFrom(NavigationEventArgs^ e)
{
}

/// <summary>
/// Invoked when a user clicks on the GetCO2LevelButton
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario2_Polling::GetCO2Level(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    static int test = 0;
    if (m_CustomSensor != nullptr)
    {
        CustomSensorReading^ reading = m_CustomSensor->GetCurrentReading();
        if (reading != nullptr)
        {
            if (reading->Properties->HasKey(CO2_LEVEL_KEY))
            {
                ScenarioOutputCO2Level->Text = reading->Properties->Lookup(CO2_LEVEL_KEY)->ToString();
            }
        }
    }
    else
    {
        m_RootPage->NotifyUser("No custom sensor found", NotifyType::ErrorMessage);
    }
}
