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

#pragma once

#include "Scenario4.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario4 sealed
    {
    public:
        Scenario4();
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        void OnSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs e);
        void OnRecognizerChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnRecognizeAsync(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void OnClear(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        bool SetRecognizerByName(Platform::String^ recognizerName);
        void TextServiceManager_InputLanguageChanged(Windows::UI::Text::Core::CoreTextServicesManager^ sender, Platform::Object^ args);
        void SetDefaultRecognizerByCurrentInputMethodLanguageTag();
        void Scenario4::RecoButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        MainPage^ rootPage = MainPage::Current;

        Windows::UI::Input::Inking::InkRecognizerContainer^ inkRecognizerContainer;
        Windows::Foundation::Collections::IVectorView<Windows::UI::Input::Inking::InkRecognizer^>^ recoView;
        Windows::Globalization::Language^ previousInputLanguage;
        Windows::UI::Text::Core::CoreTextServicesManager^ textServiceManager;
        Windows::Foundation::EventRegistrationToken languageChangedToken{};
    };
}
