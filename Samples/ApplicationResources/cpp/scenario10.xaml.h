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
// Scenario10.xaml.h
// Declaration of the Scenario10 class
//

#pragma once

#include "pch.h"
#include "Scenario10.g.h"
#include "MainPage.xaml.h"
namespace SDKTemplate
{
/// <summary>
        /// An empty page that can be used on its own or navigated to within a Frame.
        /// </summary>
        [Windows::Foundation::Metadata::WebHostHidden]
        public ref class Scenario10 sealed
        {
        public:
            Scenario10();
    
        protected:
            virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        private:
            MainPage^ rootPage;
            void Scenario10Button_Show_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    
            void Scenario10_SearchMultipleResourceIds(Windows::ApplicationModel::Resources::Core::ResourceContext^ context, Platform::Collections::Vector<Platform::String^>^ resourceIds);
            void Scenario10_ShowCandidates(Platform::String^ resourceId, Windows::Foundation::Collections::IVectorView<Windows::ApplicationModel::Resources::Core::ResourceCandidate^>^ resourceCandidates);
        };

}
