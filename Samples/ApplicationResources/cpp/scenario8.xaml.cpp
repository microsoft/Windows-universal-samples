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
// Scenario8.xaml.cpp
// Implementation of the Scenario8 class
//

#include "pch.h"
#include "Scenario8.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::ApplicationModel::Resources::Core;
using namespace Windows::Globalization;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario8::Scenario8()
{
    InitializeComponent();
    PopulateComboBox();
    UpdateCurrentAppLanguageMessage();
}

void Scenario8::ShowText()
{
    ResourceContext^ defaultContextForCurrentView = ResourceContext::GetForCurrentView();
    auto resourceStringMap = ResourceManager::Current->MainResourceMap->GetSubtree("Resources");
    Scenario8MessageTextBlock->Text = resourceStringMap->GetValue("string1", defaultContextForCurrentView)->ValueAsString;
}


void Scenario8::LanguageOverrideCombo_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
    auto combo = safe_cast<ComboBox^>(sender);
    auto item = safe_cast<ComboBoxItem^>(combo->SelectedValue);
    auto languageTag = safe_cast<String^>(item->Tag);

    // Ignore the divider ((tag = "-")
    if (languageTag == "-")
    {
        combo->SelectedIndex = lastSelectedIndex;
    }
    else
    {
        lastSelectedIndex = combo->SelectedIndex;

        // Set the persistent language override
        ApplicationLanguages::PrimaryLanguageOverride = languageTag;

        // update current app languages message
        UpdateCurrentAppLanguageMessage();
    }
}

void Scenario8::AddItemForLanguageTag(String^ languageTag)
{
    auto language = ref new Windows::Globalization::Language(languageTag);
    auto item = ref new ComboBoxItem();
    item->Content = language->DisplayName;
    item->Tag = languageTag;
    LanguageOverrideCombo->Items->Append(item);

    // Select this item if it is the primary language override.
    if (languageTag == ApplicationLanguages::PrimaryLanguageOverride)
    {
        LanguageOverrideCombo->SelectedItem = item;
    }
}


void Scenario8::PopulateComboBox()
{
    // First, show the default setting
    auto defaultItem = ref new ComboBoxItem();
    defaultItem->Content = "Use language preferences (recommended)";
    defaultItem->Tag = "";
    LanguageOverrideCombo->Items->Append(defaultItem);
    LanguageOverrideCombo->SelectedIndex = 0;

    // If there are app languages that the user speaks, show them next
    
    // Note: the first (non-override) language, if set as the primary language override
    // would give the same result as not having any primary language override. There's
    // still a difference, though: If the user changes their language preferences, the 
    // default setting (no override) would mean that the actual primary app language
    // could change. But if it's set as an override, then it will remain the primary
    // app language after the user changes their language preferences.

    for (auto languageTag : ApplicationLanguages::Languages)
    {
        AddItemForLanguageTag(languageTag);
    }

    // Now add a divider followed by all the app manifest languages (in case the user
    // wants to pick a language not currently in their profile)
    
    // NOTE: If an app is deployed using a bundle with resource packages, the following
    // addition to the list may not be useful: The set of languages returned by 
    // ApplicationLanguages.ManifestLanguages will consist of only those manifest 
    // languages in the main app package or in the resource packages that are installed 
    // and registered for the current user. Language resource packages get deployed for 
    // the user if the language is in the user's profile. Therefore, the only difference 
    // from the set returned by ApplicationLanguages.Languages above would depend on 
    // which languages are included in the main app package.

    auto dividerItem = ref new ComboBoxItem();
    dividerItem->Content = "——————";
    dividerItem->Tag = "-";
    LanguageOverrideCombo->Items->Append(dividerItem);

    // In a production app, this list should be sorted, but that's beyond the
    // focus of this sample.
    for (auto languageTag : ApplicationLanguages::ManifestLanguages)
    {
        AddItemForLanguageTag(languageTag);
    }    

    LanguageOverrideCombo->SelectionChanged += ref new SelectionChangedEventHandler(this, &Scenario8::LanguageOverrideCombo_SelectionChanged);

    lastSelectedIndex = LanguageOverrideCombo->SelectedIndex;
}

void Scenario8::UpdateCurrentAppLanguageMessage()
{
    Scenario8AppLanguagesTextBlock->Text = "Current app language(s): " + GetAppLanguagesAsFormattedString();
}

Platform::String^ Scenario8::GetAppLanguagesAsFormattedString()
{
    Platform::String^ appLanguages = "";

    for (auto languageTag : ApplicationLanguages::Languages)
    {
        if (appLanguages->Length() == 0)
        {
            appLanguages = languageTag;
        }
        else
        {
            appLanguages = appLanguages + ", " + languageTag;
        }
    }
    return appLanguages;
}
