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

//
// Scenario1.xaml.h
// Declaration of the Scenario1 class
//

#pragma once

#include "pch.h"
#include "Scenario1_GetFeed.g.h"
#include "MainPage.xaml.h"

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Web::Syndication;
using namespace Platform;

namespace SDKTemplate
{
    namespace Syndication
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
            virtual void OnNavigatedTo(NavigationEventArgs^ e) override;
        private:
            MainPage^ rootPage;
            SyndicationFeed^ currentFeed;
            unsigned int currentItemIndex;
    
            void GetFeed_Click(Object^ sender, RoutedEventArgs^ e);
            void PreviousItem_Click(Object^ sender, RoutedEventArgs^ e);
            void NextItem_Click(Object^ sender, RoutedEventArgs^ e);
            void LinkField_Click(Object^ sender, RoutedEventArgs^ e);
            void DisplayFeed();
            void DisplayCurrentItem();
        };
    }
}
