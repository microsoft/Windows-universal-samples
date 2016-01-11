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
#include "Scenario2_InAppPurchase.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel::Store;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Navigation;

Scenario2_InAppPurchase::Scenario2_InAppPurchase()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario2_InAppPurchase::OnNavigatedTo(NavigationEventArgs^ e)
{
    ConfigureSimulatorAsync("in-app-purchase.xml").then([]()
    {
        return create_task(CurrentAppSimulator::LoadListingInformationAsync());
    }).then([this](task<ListingInformation^> currentTask)
    {
        try
        {
            ListingInformation^ listing = currentTask.get();
            auto product1 = listing->ProductListings->Lookup("product1");
            Product1Name->Text = product1->Name;
            Product1Price->Text = product1->FormattedPrice;

            auto product2 = listing->ProductListings->Lookup("product2");
            Product2Name->Text = product2->Name;
            Product2Price->Text = product2->FormattedPrice;
        }
        catch (Platform::Exception^ exception)
        {
            rootPage->NotifyUser("LoadListingInformationAsync API call failed", NotifyType::ErrorMessage);
        }
    });
}

void Scenario2_InAppPurchase::TestProduct(Platform::String^ productId, Platform::String^ productName)
{
    auto licenseInformation = CurrentAppSimulator::LicenseInformation;
    auto productLicense = licenseInformation->ProductLicenses->Lookup(productId);
    if (productLicense->IsActive)
    {
        rootPage->NotifyUser("You can use " + productName + ".", NotifyType::StatusMessage);
    }
    else
    {
        rootPage->NotifyUser("You don't own " + productName + ".", NotifyType::ErrorMessage);
    }
}

void Scenario2_InAppPurchase::BuyProduct(Platform::String^ productId, Platform::String^ productName)
{
    auto licenseInformation = CurrentAppSimulator::LicenseInformation;
    if (!licenseInformation->ProductLicenses->Lookup(productId)->IsActive)
    {
        rootPage->NotifyUser("Buying " + productName + "...", NotifyType::StatusMessage);
        create_task(CurrentAppSimulator::RequestProductPurchaseAsync(productId)).then([this, productId, productName](task<PurchaseResults^> currentTask)
        {
            try
            {
                currentTask.get();
                auto licenseInformation = CurrentAppSimulator::LicenseInformation;
                if (licenseInformation->ProductLicenses->Lookup(productId)->IsActive)
                {
                    rootPage->NotifyUser("You bought " + productName + ".", NotifyType::StatusMessage);
                }
                else
                {
                    rootPage->NotifyUser(productName + " was not purchased.", NotifyType::StatusMessage);
                }
            }
            catch (Platform::Exception^ exception)
            {
                rootPage->NotifyUser("Unable to buy " + productName + ".", NotifyType::ErrorMessage);
            }
        });
    }
    else
    {
        rootPage->NotifyUser("You already own " + productName + ".", NotifyType::ErrorMessage);
    }
}
void Scenario2_InAppPurchase::TestProduct1()
{
    TestProduct("product1", Product1Name->Text);
}

void Scenario2_InAppPurchase::BuyProduct1()
{
    BuyProduct("product1", Product1Name->Text);
}

void Scenario2_InAppPurchase::TestProduct2()
{
    TestProduct("product2", Product2Name->Text);
}

void Scenario2_InAppPurchase::BuyProduct2()
{
    BuyProduct("product2", Product2Name->Text);
}
