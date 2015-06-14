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
// Scenario3.xaml.h
// Declaration of the Scenario3 class
//

#pragma once

#include "pch.h"
#include "Scenario3.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    namespace Compression
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
        private:
            MainPage^ rootPage;

            void DoScenario(Windows::Storage::Compression::CompressAlgorithm Algorithm);

            void CompressAlgorithmComboBox_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void CompressFileButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void UpdateDescriptionVisibility(Windows::UI::Xaml::Controls::ComboBoxItem^ item, Windows::UI::Xaml::Visibility visibility);
            int CompressAlgorithmFromComboBoxItem(Windows::UI::Xaml::Controls::ComboBoxItem^ item);
        };
    }
}
