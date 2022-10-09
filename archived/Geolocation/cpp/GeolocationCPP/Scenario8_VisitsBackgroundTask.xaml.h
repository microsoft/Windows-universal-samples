// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************


#pragma once

#include "pch.h"
#include "Scenario8_VisitsBackgroundTask.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    namespace GeolocationCPP
    {
        /// <summary>
        /// An empty page that can be used on its own or navigated to within a Frame.
        /// </summary>
        [Windows::Foundation::Metadata::WebHostHidden]
        public ref class Scenario8 sealed
        {
        public:
            Scenario8();

        protected:
            virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

        private:
            MainPage^ rootPage;
            Platform::String^ _sampleBackgroundTaskName;
            Platform::String^ _sampleBackgroundTaskEntryPoint;
            Windows::ApplicationModel::Background::BackgroundTaskRegistration^ _visitTask;
            Windows::Foundation::EventRegistrationToken _taskCompletedToken;

            Platform::Collections::Vector<Platform::String^>^ _visitBackgroundEvents;

            void RegisterBackgroundTask(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void UnregisterBackgroundTask(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void RequestLocationAccess();
            void OnCompleted(Windows::ApplicationModel::Background::BackgroundTaskRegistration^ sender, Windows::ApplicationModel::Background::BackgroundTaskCompletedEventArgs^ e);
            void UpdateButtonStates(bool registered);
            void FillEventListBoxWithExistingEvents();
            void ClearExistingEvents();
        };
    }
}