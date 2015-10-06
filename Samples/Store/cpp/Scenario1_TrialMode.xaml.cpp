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
#include "Scenario1_TrialMode.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::ApplicationModel::Store;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Navigation;

Scenario1_TrialMode::Scenario1_TrialMode()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario1_TrialMode::OnNavigatedTo(NavigationEventArgs^ e)
{
    eventRegistrationToken = (CurrentAppSimulator::LicenseInformation->LicenseChanged += ref new LicenseChangedEventHandler(this, &Scenario1_TrialMode::TrialModeRefreshScenario));
    ConfigureSimulatorAsync("trial-mode.xml").then([]()
    {
        return create_task(CurrentAppSimulator::LoadListingInformationAsync());
    }).then([this](task<ListingInformation^> currentTask)
    {
        try
        {
            ListingInformation^ listing = currentTask.get();
            PurchasePrice->Text = listing->FormattedPrice;
        }
        catch (Platform::Exception^ exception)
        {
            rootPage->NotifyUser("LoadListingInformationAsync API call failed", NotifyType::ErrorMessage);
        }
    });
}

void Scenario1_TrialMode::OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e)
{
    CurrentAppSimulator::LicenseInformation->LicenseChanged -= eventRegistrationToken;
}

void Scenario1_TrialMode::TrialModeRefreshScenario()
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
    {
        auto licenseInformation = CurrentAppSimulator::LicenseInformation;
        if (licenseInformation->IsActive)
        {
            if (licenseInformation->IsTrial)
            {
                LicenseMode->Text = "Trial license";
            }
            else
            {
                LicenseMode->Text = "Full license";
            }
        }
        else
        {
            LicenseMode->Text = "Inactive license";
        }
    }));
}

void Scenario1_TrialMode::ShowTrialPeriodInformation()
{
    auto licenseInformation = CurrentAppSimulator::LicenseInformation;
    if (licenseInformation->IsActive)
    {
        if (licenseInformation->IsTrial)
        {
            rootPage->NotifyUser("You can use this app for " + DaysUntil(licenseInformation->ExpirationDate) + " more days before the trial period ends.", NotifyType::StatusMessage);
        }
        else
        {
            rootPage->NotifyUser("You have a full license. The trial time is not meaningful.", NotifyType::ErrorMessage);
        }
    }
    else
    {
        rootPage->NotifyUser("You don't have a license. The trial time can't be determined.", NotifyType::ErrorMessage);
    }
}

void Scenario1_TrialMode::PurchaseFullLicense()
{
    auto licenseInformation = CurrentAppSimulator::LicenseInformation;
    rootPage->NotifyUser("Buying the full license...", NotifyType::StatusMessage);
    if (licenseInformation->IsTrial)
    {
        create_task(CurrentAppSimulator::RequestAppPurchaseAsync(false)).then([this](task<Platform::String^> currentTask)
        {
            try
            {
                currentTask.get();
                auto licenseInformation = CurrentAppSimulator::LicenseInformation;
                if (licenseInformation->IsActive && !licenseInformation->IsTrial)
                {
                    rootPage->NotifyUser("You successfully upgraded your app to the fully-licensed version.", NotifyType::StatusMessage);
                }
                else
                {
                    rootPage->NotifyUser("You still have a trial license for this app.", NotifyType::ErrorMessage);
                }
            }
            catch (Platform::Exception^ exception)
            {
                rootPage->NotifyUser("The upgrade transaction failed. You still have a trial license for this app.", NotifyType::ErrorMessage);
            }
        });
    }
    else
    {
        rootPage->NotifyUser("You already bought this app and have a fully-licensed version.", NotifyType::ErrorMessage);
    }
}
