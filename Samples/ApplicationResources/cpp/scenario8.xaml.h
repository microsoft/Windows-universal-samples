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
        [Windows::Foundation::Metadata::WebHostHidden]
        public ref class Scenario8 sealed
        {
        public:
            Scenario8();
            void ShowText();

        private:
            void Scenario8::LanguageOverrideCombo_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
            int lastSelectedIndex;

            void AddItemForLanguageTag(Platform::String^ languageTag);
            void PopulateComboBox();
            void UpdateCurrentAppLanguageMessage();
            Platform::String^ GetAppLanguagesAsFormattedString();
        };


    
}
