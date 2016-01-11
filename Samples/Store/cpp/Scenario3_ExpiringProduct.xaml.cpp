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
#include "Scenario3_ExpiringProduct.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel::Store;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Navigation;

Scenario3_ExpiringProduct::Scenario3_ExpiringProduct()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario3_ExpiringProduct::OnNavigatedTo(NavigationEventArgs^ e)
{
    eventRegistrationToken = (CurrentAppSimulator::LicenseInformation->LicenseChanged += ref new LicenseChangedEventHandler(this, &Scenario3_ExpiringProduct::OnLicenseChanged));
    ConfigureSimulatorAsync("expiring-product.xml");
}

void Scenario3_ExpiringProduct::OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e)
{
    CurrentAppSimulator::LicenseInformation->LicenseChanged -= eventRegistrationToken;
}

void Scenario3_ExpiringProduct::OnLicenseChanged()
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
    {
        auto licenseInformation = CurrentAppSimulator::LicenseInformation;
        if (licenseInformation->IsActive)
        {
            if (licenseInformation->IsTrial)
            {
                rootPage->NotifyUser("You don't have full license to this app.", NotifyType::ErrorMessage);
            }
            else
            {
                auto productLicense1 = licenseInformation->ProductLicenses->Lookup("product1");
                if (productLicense1->IsActive)
                {
                    auto longdateTemplate = ref new Windows::Globalization::DateTimeFormatting::DateTimeFormatter("longdate");
                    Product1Message->Text = "Product 1 expires on " + longdateTemplate->Format(productLicense1->ExpirationDate) + ". Days remaining: " + DaysUntil(productLicense1->ExpirationDate);
                }
                else
                {
                    rootPage->NotifyUser("Product 1 is not available.", NotifyType::ErrorMessage);
                }
            }
        }
        else
        {
            rootPage->NotifyUser("You don't have active license.", NotifyType::ErrorMessage);
        }
    }));
}
