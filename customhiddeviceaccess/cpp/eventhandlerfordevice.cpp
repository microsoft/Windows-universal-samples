//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#include "pch.h"
#include "EventHandlerForDevice.h"
#include "MainPage.xaml.h"
#include "App.xaml.h"

using namespace SDKTemplate;
using namespace Platform;
using namespace Concurrency;
using namespace Windows::ApplicationModel;
using namespace Windows::Devices;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::HumanInterfaceDevice;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace CustomHidDeviceAccess;

EventHandlerForDevice^ EventHandlerForDevice::eventHandlerForDevice = nullptr;

SRWLOCK EventHandlerForDevice::srwSingletonCreationLock = SRWLOCK_INIT;

/// <summary>
/// Enforces the singleton pattern so that there is only one object handling app events
/// as it relates to the UsbDevice because this sample app only supports communicating with one device at a time. 
///
/// An instance of EventHandlerForDevice is globally available because the device needs to persist across scenario pages.
///
/// If there is no instance of EventHandlerForDevice created before this property is called,
/// an EventHandlerForDevice will be created.
/// </summary>
EventHandlerForDevice^ EventHandlerForDevice::Current::get(void)
{
    // Acquiring Shared lock is much quicker than an exclusive lock, so avoid exclusive lock unless it's necessary
    AcquireSRWLockShared(&srwSingletonCreationLock);

    if (eventHandlerForDevice == nullptr)
    {
        // Multiple threads could have entered here, so acquire exclusive lock to make sure only one instance is created.
        ReleaseSRWLockShared(&srwSingletonCreationLock);

        AcquireSRWLockExclusive(&srwSingletonCreationLock);

        if (eventHandlerForDevice == nullptr)
        {
            CreateNewEventHandlerForDevice();
        }

        ReleaseSRWLockExclusive(&srwSingletonCreationLock);
    }
    else
    {
        ReleaseSRWLockShared(&srwSingletonCreationLock);
    }

    return eventHandlerForDevice;
}

/// <summary>
/// Creates a new instance of EventHandlerForDevice, enables auto reconnect, and uses it as the Current instance.
/// </summary>
void EventHandlerForDevice::CreateNewEventHandlerForDevice(void)
{
    eventHandlerForDevice = ref new EventHandlerForDevice();
}

/// <summary>
/// This method opens the device using the WinRT Hid API. After the device is opened, we will save the device
/// so that it can be used across scenarios.
///
/// It is important that the FromIdAsync call is made on the UI thread because the consent prompt can only be displayed
/// on the UI thread.
/// 
/// This method is used to reopen the device after the device reconnects to the computer and when the app resumes.
/// </summary>
/// <param name="deviceInfo">Device information of the device to be opened</param>
/// <param name="deviceSelector">The AQS used to find this device</param>
/// <returns>True if the device was successfully opened, false if the device could not be opened for well known reasons.
/// An exception may be thrown if the device could not be opened for extraordinary reasons.</returns>
IAsyncOperation<bool>^ EventHandlerForDevice::OpenDeviceAsync(Enumeration::DeviceInformation^ deviceInfo, String^ deviceSelectorString)
{
    // This sample uses FileAccessMode.ReadWrite to open the device because we do not want other apps opening our device and 
    // changing the state of our device. FileAccessMode.Read can be used instead.
    return create_async([this, deviceInfo, deviceSelectorString]()
    {
        return create_task(HidDevice::FromIdAsync(deviceInfo->Id, FileAccessMode::ReadWrite))
            .then([this, deviceInfo, deviceSelectorString](task<HidDevice^> deviceTask)
            {
                bool successfullyOpenedDevice = false;
                NotifyType notificationStatus;
                String^ notificationMessage = nullptr;

                // This may throw an exception or return null if we could not open the device
                device = deviceTask.get();

                // Device could have been blocked by user or the device has already been opened by another app.
                if (device != nullptr)
                {
                    successfullyOpenedDevice = true;

                    deviceInformation = deviceInfo;
                    this->deviceSelector = deviceSelector;

                    notificationStatus = NotifyType::StatusMessage;
                    notificationMessage = "Device " + deviceInformation->Id + " opened";

                    if (appSuspendEventToken.Value == 0 || appResumeEventToken.Value == 0)
                    {
                        RegisterForAppEvents();
                    }

                    // User can block the device after it has been opened in the Settings charm. We can detect this by registering for the 
                    // DeviceAccessInformation->AccessChanged event
                    if (deviceAccessInformation == nullptr)
                    {
                        RegisterForDeviceAccessStatusChange();
                    }

                    // Create and register device watcher events for the device to be opened unless we're reopening the device
                    if (deviceWatcher == nullptr)
                    {
                        deviceWatcher = Enumeration::DeviceInformation::CreateWatcher(this->deviceSelector);

                        RegisterForDeviceWatcherEvents();
                    }

                    if (!watcherStarted)
                    {
                        // Start the device watcher after we made sure that the device is opened.
                        StartDeviceWatcher();
                    }
                }
                else
                {
                    successfullyOpenedDevice = false;

                    notificationStatus = NotifyType::ErrorMessage;

                    auto deviceAccessStatus = Enumeration::DeviceAccessInformation::CreateFromId(deviceInfo->Id)->CurrentStatus;

                    if (deviceAccessStatus == DeviceAccessStatus::DeniedByUser)
                    {
                        notificationMessage = "Access to the device was blocked by the user : " + deviceInfo->Id;
                    }
                    else if (deviceAccessStatus == DeviceAccessStatus::DeniedBySystem)
                    {
                        // This status is most likely caused by app permissions (did not declare the device in the app's package.appxmanifest)
                        // This status does not cover the case where the device is already opened by another app.
                        notificationMessage = "Access to the device was blocked by the system : " + deviceInfo->Id;
                    }
                    else
                    {
                        // Most likely the device is opened by another app, but cannot be sure
                        notificationMessage = "Unknown error, possibly opened by another app : " + deviceInfo->Id;
                    }
                }

                MainPage::Current->NotifyUser(notificationMessage, notificationStatus);

                // Notify registered callback handle whether or not the device has been opened
                if (deviceConnectedCallback != nullptr)
                {
                    auto deviceConnectedEventArgs = ref new OnDeviceConnectedEventArgs(successfullyOpenedDevice, deviceInfo);

                    deviceConnectedCallback(this, deviceConnectedEventArgs);
                }

                return successfullyOpenedDevice;
            });
    });
}

/// <summary>
/// Closes the device, stops the device watcher, stops listening for app events, and resets object state to before a device
/// was ever connected.
/// </summary>
void EventHandlerForDevice::CloseDevice(void)
{
    if (IsDeviceConnected)
    {
        CloseCurrentlyConnectedDevice();
    }

    if (deviceWatcher != nullptr)
    {
        if (watcherStarted)
        {
            StopDeviceWatcher();

            UnregisterFromDeviceWatcherEvents();
        }

        deviceWatcher = nullptr;
    }

    if (deviceAccessInformation != nullptr)
    {
        UnregisterFromDeviceAccessStatusChange();

        deviceAccessInformation = nullptr;
    }

    if (appSuspendEventToken.Value != 0 || appResumeEventToken.Value != 0)
    {
        UnregisterFromAppEvents();
    }

    deviceInformation = nullptr;
    deviceSelector = nullptr;

    deviceConnectedCallback = nullptr;
    deviceCloseCallback = nullptr;

    isEnabledAutoReconnect = true;
}

EventHandlerForDevice::EventHandlerForDevice(void) :
    watcherStarted(false),
    watcherSuspended(false),
    isEnabledAutoReconnect(true)
{
}

EventHandlerForDevice::~EventHandlerForDevice()
{
    if (IsDeviceConnected)
    {
        CloseDevice();
    }
}

/// <summary>
/// This method demonstrates how to close the device properly using the WinRT Hid API.
///
/// When the HidDevice is closing, it will cancel all IO operations that are still pending (not complete).
/// The close will not wait for any IO completion callbacks to be called, so the close call may complete before any of
/// the IO completion callbacks are called.
/// The pending IO operations will still call their respective completion callbacks with either a task 
/// cancelled error or the operation completed.
/// </summary>
void EventHandlerForDevice::CloseCurrentlyConnectedDevice(void)
{
    if (device != nullptr)
    {
        if (deviceCloseCallback != nullptr)
        {
            deviceCloseCallback(this, deviceInformation);
        }

        // This closes the handle to the device
        delete device;

        device = nullptr;

        // Save the deviceInformation->Id in case deviceInformation is set to null when closing the
        // device
        String^ deviceId = deviceInformation->Id;

        MainPage::Current->Dispatcher->RunAsync(
            CoreDispatcherPriority::Normal,
            ref new DispatchedHandler([deviceId]()
        {
            MainPage::Current->NotifyUser(deviceId + " is closed", NotifyType::StatusMessage);
        }));
    }
}

/// <summary>
/// Register for app suspension/resume events. See the comments
/// for the event handlers for more information on what is being done to the device.
///
/// We will also register for when the app exists so that we may close the device handle.
/// </summary>
void EventHandlerForDevice::RegisterForAppEvents()
{
    // This event is raised when the app is exited and when the app is suspended
    appSuspendEventToken = App::Current->Suspending += ref new SuspendingEventHandler(this, &EventHandlerForDevice::OnAppSuspension);

    appResumeEventToken = App::Current->Resuming += ref new EventHandler<Object^>(this, &EventHandlerForDevice::OnAppResume);
}

void EventHandlerForDevice::UnregisterFromAppEvents()
{
    App::Current->Suspending -= appSuspendEventToken;
    appSuspendEventToken.Value = 0;

    App::Current->Resuming -= appResumeEventToken;
    appResumeEventToken.Value = 0;
}

/// <summary>
/// Register for Added and Removed events
/// Note that, when disconnecting the device, the device may be closed by the system before the OnDeviceRemoved callback is invoked.
/// </summary>
void EventHandlerForDevice::RegisterForDeviceWatcherEvents()
{
    deviceAddedEventToken = deviceWatcher->Added += ref new TypedEventHandler<DeviceWatcher^, Enumeration::DeviceInformation^>(
        this, &EventHandlerForDevice::OnDeviceAdded);

    deviceRemovedEventToken = deviceWatcher->Removed += ref new TypedEventHandler<DeviceWatcher^, DeviceInformationUpdate^>(
        this, &EventHandlerForDevice::OnDeviceRemoved);
}

void EventHandlerForDevice::UnregisterFromDeviceWatcherEvents()
{
    deviceWatcher->Added -= deviceAddedEventToken;
    deviceAddedEventToken.Value = 0;

    deviceWatcher->Removed -= deviceRemovedEventToken;
    deviceRemovedEventToken.Value = 0;
}

/// <summary>
/// Listen for any changed in device access permission. The user can block access to the device while the device is in use.
/// If the user blocks access to the device while the device is opened, the device's handle will be closed automatically by
/// the system; it is still a good idea to close the device explicitly so that resources are cleaned up.
/// 
/// Note that by the time the AccessChanged event is raised, the device handle may already be closed by the system.
/// </summary>
void EventHandlerForDevice::RegisterForDeviceAccessStatusChange()
{
    deviceAccessInformation = Enumeration::DeviceAccessInformation::CreateFromId(deviceInformation->Id);

    deviceAccessChangedEventToken = deviceAccessInformation->AccessChanged
        += ref new TypedEventHandler<Enumeration::DeviceAccessInformation^, DeviceAccessChangedEventArgs^>(this, &EventHandlerForDevice::OnDeviceAccessChanged);
}

void EventHandlerForDevice::UnregisterFromDeviceAccessStatusChange()
{
    deviceAccessInformation->AccessChanged -= deviceAccessChangedEventToken;
    deviceAccessChangedEventToken.Value = 0;
}

void EventHandlerForDevice::StartDeviceWatcher(void)
{
    watcherStarted = true;

    if ((deviceWatcher->Status != DeviceWatcherStatus::Started)
        && (deviceWatcher->Status != DeviceWatcherStatus::EnumerationCompleted))
    {
        deviceWatcher->Start();
    }
}

void EventHandlerForDevice::StopDeviceWatcher(void)
{
    if ((deviceWatcher->Status == DeviceWatcherStatus::Started)
        || (deviceWatcher->Status == DeviceWatcherStatus::EnumerationCompleted))
    {
        deviceWatcher->Stop();
    }

    watcherStarted = false;
}

/// <summary>
/// If a HidDevice object has been instantiated (a handle to the device is opened), we must close it before the app 
/// goes into suspension because the API automatically closes it for us if we don't. When resuming, the API will
/// not reopen the device automatically, so we need to explicitly open the device in the app (Scenario1_DeviceConnect).
///
/// Since we have to reopen the device ourselves when the app resumes, it is good practice to explicitly call the close
/// in the app as well (For every open there is a close).
/// 
/// We must stop the DeviceWatcher because it will continue to raise events even if
/// the app is in suspension, which is not desired (drains battery). We resume the device watcher once the app resumes again.
/// </summary>
/// <param name="sender"></param>
/// <param name="eventArgs"></param>
void EventHandlerForDevice::OnAppSuspension(Object^ /* sender */, SuspendingEventArgs^ /* eventArgs */)
{
    if (watcherStarted)
    {
        watcherSuspended = true;
        StopDeviceWatcher();
    }
    else
    {
        watcherSuspended = false;
    }

    CloseCurrentlyConnectedDevice();
}

/// <summary>
/// When resume into the application, we should reopen a handle to the Usb device again. This will automatically
/// happen when we start the device watcher again; the device will be re-enumerated and we will attempt to reopen it
/// if IsEnabledAutoReconnect property is enabled.
/// 
/// See OnAppSuspension for why we are starting the device watcher again
/// </summary>
/// <param name="sender"></param>
/// <param name="args"></param>
void EventHandlerForDevice::OnAppResume(Object^ /* sender */, Object^ /* args */) 
{
    if (watcherSuspended)
    {
        watcherSuspended = false;
        StartDeviceWatcher();
    }
}

/// <summary>
/// Close the device that is opened so that all pending operations are canceled properly.
/// </summary>
/// <param name="sender"></param>
/// <param name="deviceInformationUpdate"></param>
void EventHandlerForDevice::OnDeviceRemoved(DeviceWatcher^ /* sender */, DeviceInformationUpdate^ deviceInformationUpdate)
{
    if (IsDeviceConnected && (deviceInformationUpdate->Id == deviceInformation->Id))
    {
        // The main reasons to close the device explicitly is to clean up resources, to properly handle errors,
        // and stop talking to the disconnected device.
        CloseCurrentlyConnectedDevice();
    }
}

/// <summary>
/// Open the device that the user wanted to open if it hasn't been opened yet and auto reconnect is enabled.
/// </summary>
/// <param name="sender"></param>
/// <param name="deviceInfo"></param>
void EventHandlerForDevice::OnDeviceAdded(DeviceWatcher^ /* sender */, Enumeration::DeviceInformation^ deviceInfo)
{
    if ((deviceInformation != nullptr) && (deviceInfo->Id == deviceInformation->Id) && !IsDeviceConnected && isEnabledAutoReconnect)
    {
        MainPage::Current->Dispatcher->RunAsync(
            CoreDispatcherPriority::Normal,
            ref new DispatchedHandler([this]()
        {
            // If we failed to reconnect to the device, don't try to connect anymore
            create_task(OpenDeviceAsync(deviceInformation, deviceSelector)).then([this](task<bool> openDeviceSuccess)
            {
                isEnabledAutoReconnect = openDeviceSuccess.get();
            });

            // Any app specific device intialization should be done here because we don't know the state of the device when it is re-enumerated.
        }));
    }
}

/// <summary>
/// Close the device if the device access was denied by anyone (system or the user) and reopen it if permissions are allowed again
/// </summary>
/// <param name="sender"></param>
/// <param name="eventArgs"></param>
void EventHandlerForDevice::OnDeviceAccessChanged(Enumeration::DeviceAccessInformation^ sender, DeviceAccessChangedEventArgs^ eventArgs)
{
    if ((eventArgs->Status == DeviceAccessStatus::DeniedBySystem)
        || (eventArgs->Status == DeviceAccessStatus::DeniedByUser))
    {
        CloseCurrentlyConnectedDevice();
    }
    else if ((eventArgs->Status == DeviceAccessStatus::Allowed) && (deviceInformation != nullptr) && isEnabledAutoReconnect)
    {
        MainPage::Current->Dispatcher->RunAsync(
            CoreDispatcherPriority::Normal,
            ref new DispatchedHandler([this]()
        {
            // If we failed to reconnect to the device, don't try to connect anymore
            create_task(OpenDeviceAsync(deviceInformation, deviceSelector)).then([this](task<bool> openDeviceSuccess)
            {
                isEnabledAutoReconnect = openDeviceSuccess.get();
            });

            // Any app specific device intialization should be done here because we don't know the state of the device when it is re-enumerated.
        }));
    }
}
