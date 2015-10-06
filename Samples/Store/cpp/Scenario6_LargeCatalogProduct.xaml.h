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

#pragma once

#include "pch.h"
#include "Scenario6_LargeCatalogProduct.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario6_LargeCatalogProduct sealed
    {
    public:
        Scenario6_LargeCatalogProduct();
        void ProductNameChanged();
        void BuyAndFulfillProduct();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
    private:
        MainPage^ rootPage = MainPage::Current;

        void FulfillProduct(Platform::String^ productId, Windows::ApplicationModel::Store::PurchaseResults^ purchaseResults);

        void GrantFeatureLocally(Platform::Guid transactionId);
        bool IsLocallyFulfilled(Platform::Guid transactionId);
        Platform::String^ BuildProductNameForDisplay();
        Platform::String^ BuildOfferStringForDisplay(Platform::String^ offerId);

        Platform::Collections::Vector<Platform::Guid>^ consumedTransactionIds = ref new Platform::Collections::Vector<Platform::Guid>();
        Platform::String^ product1ListingName;
    };
}
