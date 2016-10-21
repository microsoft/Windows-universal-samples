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
using namespace Windows::Foundation;
using namespace Windows::Services::Store;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Navigation;

Scenario1_TrialMode::Scenario1_TrialMode()
{
    InitializeComponent();
}

void Scenario1_TrialMode::OnNavigatedTo(NavigationEventArgs^ e)
{
    storeContext = StoreContext::GetDefault();
    eventRegistrationToken = storeContext->OfflineLicensesChanged += ref new TypedEventHandler<StoreContext^, Object^>(this, &Scenario1_TrialMode::OfflineLicensesChanged);

    create_task(storeContext->GetStoreProductForCurrentAppAsync()).then([this](StoreProductResult^ result)
    {
        if (result->ExtendedError.Value == S_OK)
        {
            PurchasePrice->Text = result->Product->Price->FormattedPrice;
        }
    });

    GetLicenseState();
}

void Scenario1_TrialMode::OnNavigatingFrom(NavigatingCancelEventArgs^ e)
{
    storeContext->OfflineLicensesChanged -= eventRegistrationToken;
}

void Scenario1_TrialMode::OfflineLicensesChanged(StoreContext^ sender, Object^ args)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
    {
        GetLicenseState();
    }));
}

void Scenario1_TrialMode::GetLicenseState()
{
    create_task(storeContext->GetAppLicenseAsync()).then([this](StoreAppLicense^ license)
    {
        if (license->IsActive)
        {
            if (license->IsTrial)
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
    }, task_continuation_context::get_current_winrt_context());
}

void Scenario1_TrialMode::ShowTrialPeriodInformation()
{
    create_task(storeContext->GetAppLicenseAsync()).then([this](StoreAppLicense^ license)
    {
        if (license->IsActive)
        {
            if (license->IsTrial)
            {
                rootPage->NotifyUser("You can use this app for " + Utils::DaysUntil(license->ExpirationDate) + " more days before the trial period ends.", NotifyType::StatusMessage);
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
    }, task_continuation_context::get_current_winrt_context());
}

void Scenario1_TrialMode::PurchaseFullLicense()
{
    create_task(storeContext->GetStoreProductForCurrentAppAsync()).then([this](StoreProductResult^ productResult)
    {
        if (productResult->ExtendedError.Value != S_OK)
        {
            Utils::ReportExtendedError(productResult->ExtendedError);
            return;
        }

        rootPage->NotifyUser("Buying the full license...", NotifyType::StatusMessage);
        create_task(storeContext->GetAppLicenseAsync()).then([this, productResult](StoreAppLicense^ license)
        {
            if (license->IsTrial)
            {
                create_task(productResult->Product->RequestPurchaseAsync()).then([this](StorePurchaseResult^ result)
                {
                    switch (result->Status)
                    {
                    case StorePurchaseStatus::AlreadyPurchased:
                        rootPage->NotifyUser("You already bought this app and have a fully-licensed version.", NotifyType::ErrorMessage);
                        break;

                    case StorePurchaseStatus::Succeeded:
                        // License will refresh automatically using the StoreContext.OfflineLicensesChanged event
                        break;

                    case StorePurchaseStatus::NotPurchased:
                        rootPage->NotifyUser("Product was not purchased, it may have been canceled.", NotifyType::ErrorMessage);
                        break;

                    case StorePurchaseStatus::NetworkError:
                        rootPage->NotifyUser("Product was not purchased due to a Network Error.", NotifyType::ErrorMessage);
                        break;

                    case StorePurchaseStatus::ServerError:
                        rootPage->NotifyUser("Product was not purchased due to a Server Error.", NotifyType::ErrorMessage);
                        break;

                    default:
                        rootPage->NotifyUser("Product was not purchased due to an Unknown Error.", NotifyType::ErrorMessage);
                        break;
                    }
                });
            }
            else
            {
                rootPage->NotifyUser("You already bought this app and have a fully-licensed version.", NotifyType::ErrorMessage);
            }
        }, task_continuation_context::get_current_winrt_context());
    }, task_continuation_context::get_current_winrt_context());
}
