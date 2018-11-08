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
#include "pch.h"
#include "Scenario1_Configuration.xaml.h"

using namespace Concurrency;
using namespace Platform;
using namespace SDKTemplate;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Media::Ocr;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

Scenario1_Configuration::Scenario1_Configuration()
{
    InitializeComponent();

    IVectorView<Windows::Globalization::Language^>^ availableLanguages = OcrEngine::AvailableRecognizerLanguages;

    // Check if any OCR language is available on device.
    if (availableLanguages->Size > 0)
    {
        String^ languageTag = (String^)ApplicationData::Current->LocalSettings->Values->Lookup("decoderLanguage");
        for (Windows::Globalization::Language^ language : availableLanguages)
        {
            auto item = ref new ComboBoxItem();
            item->Content = language->DisplayName;
            item->Tag = language;
            LanguageList->Items->Append(item);
            if (languageTag == language->LanguageTag)
            {
                LanguageList->SelectedItem = item;
                break;
            }
        }

        // If not found (or user had never set a preference), then select the first one.
        if (LanguageList->SelectedIndex < 0)
        {
            LanguageList->SelectedIndex = 0;
        }
    }
}

/// <summary>
/// Occurs when selected language is changed in available languages combo box.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario1_Configuration::LanguageList_SelectionChanged(Object^ sender, SelectionChangedEventArgs^ e)
{
    auto selectedItem = safe_cast<ComboBoxItem^>(LanguageList->SelectedItem);
    if (selectedItem != nullptr)
    {
        auto selectedLanguage = safe_cast<Windows::Globalization::Language^>(selectedItem->Tag);
        m_rootPage->NotifyUser(
            "Selected OCR language is " + selectedLanguage->DisplayName + ".",
            NotifyType::StatusMessage);

        ApplicationData::Current->LocalSettings->Values->Insert("decoderLanguage", selectedLanguage->LanguageTag);
    }
}