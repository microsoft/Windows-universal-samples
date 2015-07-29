//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

//
// Scenario7.xaml.h
// Declaration of the Scenario7 class
//

#pragma once

#include "pch.h"
#include "Scenario7.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
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
            Windows::ApplicationModel::Resources::Core::ResourceContext^ defaultContextForCurrentView;
            void Scenario7Button_Show_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void ContextChanged(Windows::Foundation::Collections::IObservableMap<Platform::String^, Platform::String^>^ sender, Windows::Foundation::Collections::IMapChangedEventArgs<Platform::String^>^ eventArg);
            Windows::Foundation::EventRegistrationToken eventToken;
        };
    
}
