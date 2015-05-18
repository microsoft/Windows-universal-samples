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
#include "Scenario2_CloseDrawer.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
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
using namespace Windows::Devices::PointOfService;
using namespace Windows::UI::Core;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Scenario2_CloseDrawer::Scenario2_CloseDrawer() : rootPage(MainPage::Current)
{
    InitializeComponent();
}


/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.</param>
void Scenario2_CloseDrawer::OnNavigatedTo(NavigationEventArgs^ e)
{
    ResetScenarioState();
}


/// <summary>
/// Invoked before the page is unloaded and is no longer the current source of a Frame.
/// </summary>
/// <param name="e">Event data describing the navigation that was requested.</param>
void Scenario2_CloseDrawer::OnNavigatedFrom(NavigationEventArgs^ e)
{
    ResetScenarioState();
}


/// <summary>
/// Event handler for Initialize Drawer button.
/// Claims and enables the default cash drawer.
/// </summary>
/// <param name="sender">Button that was clicked.</param>
/// <param name="e">Event data associated with click event.</param>
void Scenario2_CloseDrawer::InitDrawerButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    create_task(CreateDefaultCashDrawerObject()).then([this](bool success)
    {
        if (!success)
        {
            rootPage->NotifyUser("Cash drawer not found. Please connect a cash drawer.", NotifyType::ErrorMessage);
            return;
        }

        create_task(ClaimCashDrawer()).then([this](bool success)
        {
            if (!success)
            {
                rootPage->NotifyUser("Failed to claim cash drawer.", NotifyType::ErrorMessage);
                return;
            }

            create_task(claimedDrawer->EnableAsync()).then([this](bool success)
            {
                if (!success)
                {
                    rootPage->NotifyUser("Failed to enable cash drawer.", NotifyType::ErrorMessage);
                    return;
                }

                statusUpdatedToken = drawer->StatusUpdated::add(ref new TypedEventHandler<CashDrawer^, CashDrawerStatusUpdatedEventArgs^>
                    (this, &Scenario2_CloseDrawer::drawer_StatusUpdated));

                UpdateStatusOutput(drawer->Status->StatusKind);
                rootPage->NotifyUser("Successfully enabled cash drawer. Device ID: " + claimedDrawer->DeviceId, NotifyType::StatusMessage);

                InitDrawerButton->IsEnabled = false;
                DrawerWaitButton->IsEnabled = true;
            });
        });
    });
}


/// <summary>
/// Set up an alarm and wait for the drawer to close.
/// </summary>
/// <param name="sender">Button that was clicked.</param>
/// <param name="e">Event data associated with click event.</param>
void Scenario2_CloseDrawer::WaitForDrawerCloseButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    if (claimedDrawer == nullptr)
    {
        rootPage->NotifyUser("Drawer must be initialized.", NotifyType::ErrorMessage);
        return;
    }
    

    CashDrawerCloseAlarm^ alarm = claimedDrawer->CloseAlarm;
    if (alarm == nullptr)
    {
        rootPage->NotifyUser("Failed to create drawer alarm.", NotifyType::ErrorMessage);
        return;
    }

    TimeSpan alarmTimeSpan;
    alarmTimeSpan.Duration = 300000000L;    // 30 second alarm timeout
    alarm->AlarmTimeout = alarmTimeSpan;

    alarmTimeSpan.Duration = 30000000L;     // 3 second delay
    alarm->BeepDelay = alarmTimeSpan;

    alarmTimeSpan.Duration = 10000000L;     // 1 second duration
    alarm->BeepDuration = alarmTimeSpan;

    alarm->BeepFrequency = 700;
    alarm->AlarmTimeoutExpired += ref new TypedEventHandler<CashDrawerCloseAlarm^, Platform::Object^>(this, &Scenario2_CloseDrawer::drawer_AlarmExpired);

    rootPage->NotifyUser("Waiting for drawer to close.", NotifyType::StatusMessage);

    create_task(alarm->StartAsync()).then([this](bool success)
    {
        if (success)
            rootPage->NotifyUser("Successfully waited for drawer close.", NotifyType::StatusMessage);
        else
            rootPage->NotifyUser("Failed to wait for drawer close.", NotifyType::ErrorMessage);
    });
}


/// <summary>
/// Creates the default cash drawer.
/// </summary>
/// <returns>True if the cash drawer was created, false otherwise.</returns>
task<bool> Scenario2_CloseDrawer::CreateDefaultCashDrawerObject()
{
    rootPage->NotifyUser("Creating cash drawer object.", NotifyType::StatusMessage);

    return create_task(CashDrawer::GetDefaultAsync()).then([this](CashDrawer^ defaultDrawer)
    {
        if (defaultDrawer == nullptr)
            return false;

        drawer = defaultDrawer;
        return true;
    });
}


/// <summary>
/// Attempt to claim the connected cash drawer.
/// </summary>
/// <returns>True if the operation succeeds, false otherwise.</returns>
task<bool> Scenario2_CloseDrawer::ClaimCashDrawer()
{
    return create_task(drawer->ClaimDrawerAsync()).then([this](ClaimedCashDrawer^ claimed)
    {
        if (claimed == nullptr)
            return false;

        claimedDrawer = claimed;
        return true;
    });
}


/// <summary>
/// Event callback for device status updates.
/// </summary>
/// <param name="drawer">CashDrawer object sending the status update event.</param>
/// <param name="e">Event data associated with the status update.</param>
void Scenario2_CloseDrawer::drawer_StatusUpdated(CashDrawer^ sender, CashDrawerStatusUpdatedEventArgs^ e)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, e]()
    {
        UpdateStatusOutput(e->Status->StatusKind);
        rootPage->NotifyUser("Status updated event: " + e->Status->StatusKind.ToString(), NotifyType::StatusMessage);
    }));
}


/// <summary>
/// Event callback for the alarm expiring.
/// </summary>
/// <param name="alarm">CashDrawerCloseAlarm that has expired.</param>
/// <param name="sender">Unused by AlarmTimeoutExpired events.</param>
void Scenario2_CloseDrawer::drawer_AlarmExpired(CashDrawerCloseAlarm^ alarm, Object^ e)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
    {
        rootPage->NotifyUser("Alarm expired.  Still waiting for drawer to be closed.", NotifyType::StatusMessage);
    }));
}


/// <summary>
/// Reset the scenario to its initial state.
/// </summary>
void Scenario2_CloseDrawer::ResetScenarioState()
{
    if (drawer != nullptr)
    {
        drawer->StatusUpdated::remove(statusUpdatedToken);

        delete drawer;
        drawer = nullptr;
    }

    if (claimedDrawer != nullptr)
    {
        delete claimedDrawer;
        claimedDrawer = nullptr;
    }

    InitDrawerButton->IsEnabled = true;
    DrawerWaitButton->IsEnabled = false;

    rootPage->NotifyUser("Click the init drawer button to begin.", NotifyType::StatusMessage);
}


/// <summary>
/// Update the cash drawer text block.
/// </summary>
/// <param name="status">Cash drawer status to be displayed.</param>
void Scenario2_CloseDrawer::UpdateStatusOutput(CashDrawerStatusKind status)
{
    DrawerStatusBlock->Text = status.ToString();
}

