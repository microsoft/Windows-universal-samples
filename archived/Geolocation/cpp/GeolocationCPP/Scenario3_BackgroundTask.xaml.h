﻿//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

//
// Scenario3_BackgroundTask.xaml.h
// Declaration of the Scenario3 class
//

#pragma once

#include "pch.h"
#include "Scenario3_BackgroundTask.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    namespace GeolocationCPP
    {
        /// <summary>
        /// An empty page that can be used on its own or navigated to within a Frame.
        /// </summary>
        [Windows::Foundation::Metadata::WebHostHidden]
        public ref class Scenario3 sealed
        {
        public:
            Scenario3();

        protected:
            virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
            virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

        private:
            MainPage^ rootPage;
            Platform::String^ sampleBackgroundTaskName;
            Platform::String^ sampleBackgroundTaskEntryPoint;
            Windows::ApplicationModel::Background::BackgroundTaskRegistration^ geolocTask;
            Windows::Foundation::EventRegistrationToken taskCompletedToken;

            void RegisterBackgroundTask(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void UnregisterBackgroundTask(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void RequestLocationAccess();
            void OnCompleted(Windows::ApplicationModel::Background::BackgroundTaskRegistration^ sender, Windows::ApplicationModel::Background::BackgroundTaskCompletedEventArgs^ e);
            void UpdateButtonStates(bool registered);
        };
    }
}