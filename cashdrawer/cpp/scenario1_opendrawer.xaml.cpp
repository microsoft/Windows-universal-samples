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
#include "Scenario1_OpenDrawer.xaml.h"

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

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Scenario1_OpenDrawer::Scenario1_OpenDrawer() : rootPage(MainPage::Current)
{
    InitializeComponent();
}


/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.</param>
void Scenario1_OpenDrawer::OnNavigatedTo(NavigationEventArgs^ e)
{
    ResetScenarioState();
}


/// <summary>
/// Invoked before the page is unloaded and is no longer the current source of a Frame.
/// </summary>
/// <param name="e">Event data describing the navigation that was requested.</param>
void Scenario1_OpenDrawer::OnNavigatedFrom(NavigationEventArgs^ e)
{
    ResetScenarioState();
}


/// <summary>
/// Event handler for Initialize Drawer button.
/// Claims and enables the default cash drawer.
/// </summary>
/// <param name="sender">Button that was clicked.</param>
/// <param name="e">Event data associated with click event.</param>
void Scenario1_OpenDrawer::InitDrawerButton_Click(Object^ sender, RoutedEventArgs^ e)
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

                rootPage->NotifyUser("Successfully enabled cash drawer. Device ID: " + claimedDrawer->DeviceId, NotifyType::StatusMessage);

                InitDrawerButton->IsEnabled = false;
                OpenDrawerButton->IsEnabled = true;
            });
        });
    });
}


/// <summary>
/// Open the cash drawer.
/// </summary>
/// <param name="sender">Button that was clicked.</param>
/// <param name="e">Event data associated with click event.</param>
void Scenario1_OpenDrawer::OpenDrawerButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    create_task(claimedDrawer->OpenDrawerAsync()).then([this](bool success)
    {
        if (success)
            rootPage->NotifyUser("Cash drawer open succeeded.", NotifyType::StatusMessage);
        else
            rootPage->NotifyUser("Cash drawer open failed.", NotifyType::ErrorMessage);
    });

}


/// <summary>
/// Creates the default cash drawer.
/// </summary>
/// <returns>True if the cash drawer was created, false otherwise.</returns>
task<bool> Scenario1_OpenDrawer::CreateDefaultCashDrawerObject()
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
task<bool> Scenario1_OpenDrawer::ClaimCashDrawer()
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
/// Reset the scenario to its initial state.
/// </summary>
void Scenario1_OpenDrawer::ResetScenarioState()
{
    if (drawer != nullptr)
    {
        delete drawer;
        drawer = nullptr;
    }

    if (claimedDrawer != nullptr)
    {
        delete claimedDrawer;
        claimedDrawer = nullptr;
    }

    InitDrawerButton->IsEnabled = true;
    OpenDrawerButton->IsEnabled = false;

    rootPage->NotifyUser("Click the init drawer button to begin.", NotifyType::StatusMessage);
}

