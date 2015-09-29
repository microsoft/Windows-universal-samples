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

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Platform;
using namespace Windows::ApplicationModel::Resources::Core;
using namespace Windows::Globalization;


Scenario8::Scenario8()
{
    InitializeComponent();
    PopulateComboBox();
    UpdateCurrentAppLanguageMessage();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario8::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;
}


void Scenario8::Scenario8Button_Show_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    Button^ b = safe_cast<Button^>(sender);
    if (b != nullptr)
    {
        ResourceContext^ defaultContextForCurrentView = ResourceContext::GetForCurrentView();

        auto resourceStringMap = ResourceManager::Current->MainResourceMap->GetSubtree("Resources");
        Scenario8MessageTextBlock->Text = resourceStringMap->GetValue("string1", defaultContextForCurrentView)->ValueAsString;
    }
}


void Scenario8::LanguageOverrideCombo_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
    ComboBox^ combo = safe_cast<ComboBox^>(sender);

    // Don't accept the list item for the divider (tag = "-")
    if (combo->SelectedValue->ToString() == "-")
    {
        combo->SelectedIndex = lastSelectedIndex;
    }
    else
    {
        lastSelectedIndex = combo->SelectedIndex;

        // Set the persistent language override
        Windows::Globalization::ApplicationLanguages::PrimaryLanguageOverride = combo->SelectedValue->ToString();

        // update current app languages message
        UpdateCurrentAppLanguageMessage();
    }
}


void Scenario8::PopulateComboBox()
{
    comboBoxValues = ref new Platform::Collections::Vector<ComboBoxValue^>();

    //First, show the default setting
    auto comboBoxValue_default = ref new ComboBoxValue("Use my default language preferences", "");
    comboBoxValues->Append(comboBoxValue_default);


    // If there are app languages that the user speaks, show them next
    
    // Note: the first (non-override) language, if set as the primary language override
    // would give the same result as not having any primary language override. There's
    // still a difference, though: If the user changes their language preferences, the 
    // default setting (no override) would mean that the actual primary app language
    // could change. But if it's set as an override, then it will remain the primary
    // app language after the user changes their language preferences.

    for (unsigned int i = 0; i < ApplicationLanguages::Languages->Size; i++)
    {
        // ApplicationLanguages.Languages items are BCP-47 language tags. Need to use
        // Windows.Globalization.Language to obtain a display name.
        auto language = ref new Windows::Globalization::Language(ApplicationLanguages::Languages->GetAt(i));
        auto comboBoxValue = ref new ComboBoxValue(language->NativeName, language->LanguageTag);
        comboBoxValues->Append(comboBoxValue);
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

    auto comboBoxValue_divider = ref new ComboBoxValue("——————", "-");
    comboBoxValues->Append(comboBoxValue_divider);

    // In a production app, this list should be sorted, but that's beyond the
    // focus of this sample.
    for (auto lang : ApplicationLanguages::ManifestLanguages)
    {
        Windows::Globalization::Language^ langObject = ref new Windows::Globalization::Language(lang);
        auto comboBoxValue = ref new ComboBoxValue(langObject->NativeName, langObject->LanguageTag);
        comboBoxValues->Append(comboBoxValue);
    }
    

    // Got all the entries; complete initialization of combo box
    LanguageOverrideCombo->ItemsSource = comboBoxValues;
    LanguageOverrideCombo->SelectedIndex = FindPrimaryLanguageOverrideIndex(comboBoxValues);
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

    auto countLanguages = ApplicationLanguages::Languages->Size;
    for (unsigned int i = 0; i < countLanguages - 1; i++)
    {
        appLanguages = appLanguages + ApplicationLanguages::Languages->GetAt(i) + ", ";
    }
    appLanguages = appLanguages + ApplicationLanguages::Languages->GetAt(countLanguages - 1);

    return appLanguages;
}


unsigned int Scenario8::FindPrimaryLanguageOverrideIndex(Platform::Collections::Vector<ComboBoxValue^>^ comboBoxValues)
{
    // Go through the list and return the first matching index
    for (unsigned int i = 0; i < comboBoxValues->Size; i++)
    {
        ComboBoxValue^ item = comboBoxValues->GetAt(i);
        if (item->LanguageTag == ApplicationLanguages::PrimaryLanguageOverride)
        {
            return i;
        }
    }

    // There should always be a match in the list. The following catches a pathological
    // case: for some reason, the current PrimaryLanguageOverride setting doesn't match
    // anything from the set of the app languages + manifest languages (shouldn't happen).
    // As a last resort, will clear the primary language override and select the default
    // item in the combo box.
    ApplicationLanguages::PrimaryLanguageOverride == "";
    return 0;
}



ComboBoxValue::ComboBoxValue(Platform::String^ displayName, Platform::String^ languageTag)
{
    DisplayName = displayName;
    LanguageTag = languageTag;
}


