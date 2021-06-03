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
#include "Scenario7_ForegroundVisits.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    namespace GeolocationCPP
    {
        /// <summary>
        /// An empty page that can be used on its own or navigated to within a Frame.
        /// </summary>
        [Windows::Foundation::Metadata::WebHostHidden]
        public ref class Scenario7 sealed
        {
        public:
            Scenario7();

        protected:
            virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
            virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

        private:
            MainPage^ rootPage;

            Windows::Devices::Geolocation::GeovisitMonitor^ _visitMonitor;

            void StartMonitoring(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void StopMonitoring(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void UpdateVisitData(Windows::Devices::Geolocation::Geovisit^ Visit);

            void OnVisitStateChanged(Windows::Devices::Geolocation::GeovisitMonitor^ sender, Windows::Devices::Geolocation::GeovisitStateChangedEventArgs^ args);
        };
    }
}