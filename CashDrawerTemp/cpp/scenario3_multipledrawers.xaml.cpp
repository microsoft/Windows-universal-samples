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
#include "Scenario3_MultipleDrawers.xaml.h"

using namespace SDKTemplate;

using namespace Windows::Devices::PointOfService;
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
using namespace Windows::UI::Core;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Scenario3_MultipleDrawers::Scenario3_MultipleDrawers() : rootPage(MainPage::Current)
{
    InitializeComponent();
}


/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.</param>
void Scenario3_MultipleDrawers::OnNavigatedTo(NavigationEventArgs^ e)
{
    ResetScenarioState();
}


/// <summary>
/// Invoked before the page is unloaded and is no longer the current source of a Frame.
/// </summary>
/// <param name="e">Event data describing the navigation that was requested.</param>
void Scenario3_MultipleDrawers::OnNavigatedFrom(NavigationEventArgs^ e)
{
    ResetScenarioState();
}


/// <summary>
/// Creates the default cash drawer.
/// </summary>
/// <param name="instance">Specifies the cash drawer instance that should be used.</param>
/// <returns>True if the cash drawer was created, false otherwise.</returns>
task<bool> Scenario3_MultipleDrawers::CreateDefaultCashDrawerObject(CashDrawerInstance instance)
{
    rootPage->NotifyUser("Creating cash drawer object.", NotifyType::StatusMessage);

    return create_task(CashDrawer::GetDefaultAsync()).then([this, instance](CashDrawer^ defaultDrawer)
    {
        if (defaultDrawer == nullptr)
        {
            rootPage->NotifyUser("Cash drawer not found. Please connect a cash drawer.", NotifyType::ErrorMessage);
            return false;
        }

        switch (instance)
        {
        case Instance1:
            cashDrawerInstance1 = defaultDrawer;
            break;
        case Instance2:
            cashDrawerInstance2 = defaultDrawer;
            break;
        default:
            break;
        }

        return true;
    });
}


/// <summary>
/// Attempt to claim the connected cash drawer.
/// </summary>
/// <param name="instance">Specifies the cash drawer instance that should be used.</param>
/// <returns>True if the cash drawer was successfully claimed, false otherwise.</returns>
task<bool> Scenario3_MultipleDrawers::ClaimCashDrawer(CashDrawerInstance instance)
{
    CashDrawer^ drawer;
    if (instance == Instance1)
        drawer = cashDrawerInstance1;
    else
        drawer = cashDrawerInstance2;

    return create_task(drawer->ClaimDrawerAsync()).then([this, instance](ClaimedCashDrawer^ claimed)
    {
        if (claimed == nullptr)
            return false;

        if (instance == Instance1)
            claimedCashDrawerInstance1 = claimed;
        else
            claimedCashDrawerInstance2 = claimed;

        return true;
    });
}


/// <summary>
/// Event callback for claim instance 1 button.
/// </summary>
/// <param name="sender">Button that was clicked.</param>
/// <param name="e">Event data associated with click event.</param>
void Scenario3_MultipleDrawers::ClaimButton1_Click(Object^ sender, RoutedEventArgs^ e)
{
    create_task(CreateDefaultCashDrawerObject(Instance1)).then([this](bool success)
    {
        if (success)
        {
            create_task(ClaimCashDrawer(Instance1)).then([this](bool success)
            {
                if (!success)
                {
                    rootPage->NotifyUser("Failed to claim instance 1.", NotifyType::ErrorMessage);
                    return;
                }

                releaseRequestedToken1 = claimedCashDrawerInstance1->ReleaseDeviceRequested::add(
                    ref new TypedEventHandler<ClaimedCashDrawer^, Object^>(this, &Scenario3_MultipleDrawers::claimedCashDrawerInstance1_ReleaseDeviceRequested));

                rootPage->NotifyUser("Successfully claimed instance 1.", NotifyType::StatusMessage);
                SetClaimedUI(Instance1);
            });
        }
    });
}


/// <summary>
/// Event callback for claim instance 2 button.
/// </summary>
/// <param name="sender">Button that was clicked.</param>
/// <param name="e">Event data associated with click event.</param>
void Scenario3_MultipleDrawers::ClaimButton2_Click(Object^ sender, RoutedEventArgs^ e)
{
    create_task(CreateDefaultCashDrawerObject(Instance2)).then([this](bool success)
    {
        if (success)
        {
            create_task(ClaimCashDrawer(Instance2)).then([this](bool success)
            {
                if (!success)
                {
                    rootPage->NotifyUser("Failed to claim instance 2.", NotifyType::ErrorMessage);
                    return;
                }

                releaseRequestedToken2 = claimedCashDrawerInstance2->ReleaseDeviceRequested::add(
                    ref new TypedEventHandler<ClaimedCashDrawer^, Object^>(this, &Scenario3_MultipleDrawers::claimedCashDrawerInstance2_ReleaseDeviceRequested));

                rootPage->NotifyUser("Successfully claimed instance 2.", NotifyType::StatusMessage);
                SetClaimedUI(Instance2);
            });
        }
    });
}


/// <summary>
/// Event callback for release instance 1 button.
/// </summary>
/// <param name="sender">Button that was clicked.</param>
/// <param name="e">Event data associated with click event.</param>
void Scenario3_MultipleDrawers::ReleaseButton1_Click(Object^ sender, RoutedEventArgs^ e)
{
    if (claimedCashDrawerInstance1 != nullptr)
    {
        claimedCashDrawerInstance1->ReleaseDeviceRequested::remove(releaseRequestedToken1);
        delete claimedCashDrawerInstance1;
        claimedCashDrawerInstance1 = nullptr;

        SetReleasedUI(Instance1);

        rootPage->NotifyUser("Claimed instance 1 was released.", NotifyType::StatusMessage);
    }
}


/// <summary>
/// Event callback for release instance 2 button.
/// </summary>
/// <param name="sender">Button that was clicked.</param>
/// <param name="e">Event data associated with click event.</param>
void Scenario3_MultipleDrawers::ReleaseButton2_Click(Object^ sender, RoutedEventArgs^ e)
{
    if (claimedCashDrawerInstance2 != nullptr)
    {
        claimedCashDrawerInstance2->ReleaseDeviceRequested::remove(releaseRequestedToken2);
        delete claimedCashDrawerInstance2;
        claimedCashDrawerInstance2 = nullptr;

        SetReleasedUI(Instance2);

        rootPage->NotifyUser("Claimed instance 2 was released.", NotifyType::StatusMessage);
    }
}


/// <summary>
/// Event callback for a release device request for instance 1.
/// </summary>
/// <param name="sender">The drawer receiving the release device request.</param>
/// <param name="e">Unused for ReleaseDeviceRequested events.</param>
void Scenario3_MultipleDrawers::claimedCashDrawerInstance1_ReleaseDeviceRequested(ClaimedCashDrawer^ sender, Object^ e)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
    {
        rootPage->NotifyUser("Release instance 1 requested.", NotifyType::StatusMessage);

        if (RetainCheckBox1->IsChecked->Value)
        {
            create_task(claimedCashDrawerInstance1->RetainDeviceAsync()).then([this](bool success)
            {
                if (!success)
                    rootPage->NotifyUser("Cash drawer instance 1 retain failed.", NotifyType::ErrorMessage);
            });
        }
        else
        {
            claimedCashDrawerInstance1->ReleaseDeviceRequested::remove(releaseRequestedToken1);
            delete claimedCashDrawerInstance1;
            claimedCashDrawerInstance1 = nullptr;

            SetReleasedUI(Instance1);
        }
    }));
}


/// <summary>
/// Event callback for a release device request for instance 2.
/// </summary>
/// <param name="sender">The drawer receiving the release device request.</param>
/// <param name="e">Unused for ReleaseDeviceRequested events.</param>
void Scenario3_MultipleDrawers::claimedCashDrawerInstance2_ReleaseDeviceRequested(ClaimedCashDrawer^ sender, Object^ e)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
    {
        rootPage->NotifyUser("Release instance 2 requested.", NotifyType::StatusMessage);

        if (RetainCheckBox2->IsChecked->Value)
        {
            create_task(claimedCashDrawerInstance2->RetainDeviceAsync()).then([this](bool success)
            {
                if(!success)
                    rootPage->NotifyUser("Cash drawer instance 2 retain failed.", NotifyType::ErrorMessage);
            });
        }
        else
        {
            claimedCashDrawerInstance2->ReleaseDeviceRequested::remove(releaseRequestedToken2);
            delete claimedCashDrawerInstance2;
            claimedCashDrawerInstance2 = nullptr;

            SetReleasedUI(Instance2);
        }
    }));
}


/// <summary>
/// Reset the scenario to its initial state.
/// </summary>
void Scenario3_MultipleDrawers::ResetScenarioState()
{
    cashDrawerInstance1 = nullptr;
    cashDrawerInstance2 = nullptr;

    if (claimedCashDrawerInstance1 != nullptr)
    {
        delete claimedCashDrawerInstance1;
        claimedCashDrawerInstance1 = nullptr;
    }

    if (claimedCashDrawerInstance2 != nullptr)
    {
        delete claimedCashDrawerInstance2;
        claimedCashDrawerInstance2 = nullptr;
    }

    ClaimButton1->IsEnabled = true;
    ClaimButton2->IsEnabled = true;
    ReleaseButton1->IsEnabled = false;
    ReleaseButton2->IsEnabled = false;
    RetainCheckBox1->IsChecked = true;
    RetainCheckBox2->IsChecked = true;

    rootPage->NotifyUser("Click a claim button to begin.", NotifyType::StatusMessage);
}


/// <summary>
/// Sets the UI elements to a state corresponding to the specified instace being claimed.
/// </summary>
/// <param name="instance">Corresponds to instance that has been claimed.</param>
void Scenario3_MultipleDrawers::SetClaimedUI(CashDrawerInstance instance)
{
    switch (instance)
    {
    case Instance1:
        ClaimButton1->IsEnabled = false;
        ClaimButton2->IsEnabled = true;
        ReleaseButton1->IsEnabled = true;
        ReleaseButton2->IsEnabled = false;
        break;

    case Instance2:
        ClaimButton1->IsEnabled = true;
        ClaimButton2->IsEnabled = false;
        ReleaseButton1->IsEnabled = false;
        ReleaseButton2->IsEnabled = true;
        break;

    default:
        break;
    }
}


/// <summary>
/// Sets the UI elements to a state corresponding to the specified instace being released.
/// </summary>
/// <param name="instance">Corresponds to instance that has been released.</param>
void Scenario3_MultipleDrawers::SetReleasedUI(CashDrawerInstance instance)
{
    switch (instance)
    {
    case Instance1:
        ClaimButton1->IsEnabled = true;
        ReleaseButton1->IsEnabled = false;
        break;

    case Instance2:
        ClaimButton2->IsEnabled = true;
        ReleaseButton2->IsEnabled = false;
        break;

    default:
        break;
    }
}

