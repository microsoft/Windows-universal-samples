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
#include "Scenario4_ConsumableProduct.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario4_ConsumableProduct sealed
    {
    public:
        Scenario4_ConsumableProduct();
        void BuyAndFulfillProduct1();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
    private:
        MainPage^ rootPage = MainPage::Current;

        void FulfillProduct1(Platform::String^ productId, Platform::Guid transactionId);

        void GrantFeatureLocally(Platform::Guid transactionId);
        bool IsLocallyFulfilled(Platform::Guid transactionId);

        int numberOfConsumablesPurchased = 0;
        Platform::Collections::Vector<Platform::Guid>^ consumedTransactionIds = ref new Platform::Collections::Vector<Platform::Guid>();
    };
}
