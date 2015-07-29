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
// Scenario8.xaml.h
// Declaration of the Scenario8 class
//

#pragma once

#include "pch.h"
#include "Scenario8.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
  
        /// <summary>
        /// Class used in populating a combo box with language choices
        /// </summary>
        [Windows::UI::Xaml::Data::Bindable]
        public ref class ComboBoxValue sealed
        {
        public:
            ComboBoxValue()
            { 
            }

            ComboBoxValue(Platform::String^ displayName, Platform::String^ languageTag);

            property Platform::String^ DisplayName;
            property Platform::String^ LanguageTag;
        };


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
            void Scenario8Button_Show_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void Scenario8::LanguageOverrideCombo_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
            int lastSelectedIndex;

            Platform::Collections::Vector<ComboBoxValue^>^ comboBoxValues;
            void PopulateComboBox();
            void UpdateCurrentAppLanguageMessage();
            Platform::String^ GetAppLanguagesAsFormattedString();
            unsigned int FindPrimaryLanguageOverrideIndex(Platform::Collections::Vector<ComboBoxValue^>^ comboBoxValues);
        };


    
}
