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
// Scenario1.xaml.h
// Declaration of the Scenario1 class
//

#pragma once

#include "pch.h"
#include "Scenario1_Identity.g.h"
#include "MainPage.xaml.h"


    namespace SDKTemplate
    {
        /// <summary>
        /// An empty page that can be used on its own or navigated to within a Frame.
        /// </summary>
        [Windows::Foundation::Metadata::WebHostHidden]
        public ref class Scenario1 sealed
        {
        public:
            Scenario1();
    
        protected:
            virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        private:
            MainPage^ rootPage;
    
            Platform::String^ VersionString(Windows::ApplicationModel::PackageVersion version);
            Platform::String^ ArchitectureString(Windows::System::ProcessorArchitecture architecture);
    
            void GetPackage_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        };

}
