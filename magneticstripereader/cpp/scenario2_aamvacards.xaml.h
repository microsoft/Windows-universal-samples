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

#include "Scenario2_AamvaCards.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// Demonstrate how to use read AAMVA cards with MagneticStripeReader
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_AamvaCards sealed
    {
    public:
        Scenario2_AamvaCards();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        MainPage^ rootPage;
        Windows::Devices::PointOfService::MagneticStripeReader^ _reader;
        Windows::Devices::PointOfService::ClaimedMagneticStripeReader^ _claimedReader;
        Windows::Foundation::EventRegistrationToken _aamvaCardDataReceivedToken;
        Windows::Foundation::EventRegistrationToken _releaseDeviceRequestedToken;

        void ScenarioStartReadButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ScenarioEndReadButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        Concurrency::task<void> CreateDefaultReaderObject();
        Concurrency::task<void> ClaimReader();
        void ResetTheScenarioState();

        void OnReleaseDeviceRequested(Platform::Object ^sender, Windows::Devices::PointOfService::ClaimedMagneticStripeReader ^args);

        void OnAamvaCardDataReceived(Windows::Devices::PointOfService::ClaimedMagneticStripeReader^sender, Windows::Devices::PointOfService::MagneticStripeReaderAamvaCardDataReceivedEventArgs^args);
    };
}
