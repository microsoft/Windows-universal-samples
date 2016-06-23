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
#include "Scenario5_AdvancedConsumableProduct.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario5_AdvancedConsumableProduct sealed
    {
    public:
        Scenario5_AdvancedConsumableProduct();
        void ShowUnfulfilledConsumables();
        void BuyProduct1();
        void FulfillProduct1();
        void BuyProduct2();
        void FulfillProduct2();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
    private:
        MainPage^ rootPage = MainPage::Current;

        struct ProductPurchaseInfo
        {
            Platform::Guid tempTransactionId = Platform::Guid();
            int numPurchases = 0;
            int numFulfillments = 0;

            Platform::String^ productId;
            Windows::UI::Xaml::Documents::Run^ nameRun;
            Windows::UI::Xaml::Documents::Run^ priceRun;
            Windows::UI::Xaml::Documents::Run^ purchasesRun;
            Windows::UI::Xaml::Documents::Run^ fulfilledRun;

            Platform::Collections::Vector<Platform::Guid>^ consumedTransactionIds = ref new Platform::Collections::Vector<Platform::Guid>();
        };
        ProductPurchaseInfo purchaseInfos[2];

        void BuyProduct(ProductPurchaseInfo& info);
        void FulfillProduct(ProductPurchaseInfo& info);
        void GrantFeatureLocally(ProductPurchaseInfo& info, Platform::Guid transactionId);
        bool IsLocallyFulfilled(ProductPurchaseInfo& info, Platform::Guid transactionId);

        void UpdateStatistics();
    };
}
