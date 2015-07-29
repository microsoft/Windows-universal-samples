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

#include "OcrFileImage.g.h"
#include "MainPage.xaml.h"
#include "WordOverlay.h"
#include <map>
#include <ppltasks.h>

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Media::Ocr;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Storage::Pickers;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Navigation;

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class OcrFileImage sealed
    {
    public:
        OcrFileImage();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
    private:

        MainPage^ rootPage;

        bool isAngleDetected;
        std::map<String^, Windows::Globalization::Language^> languageMap;

        // Bitmap holder of currently loaded image.
        Windows::Graphics::Imaging::SoftwareBitmap^ bitmap;

        // Recognized words overlay boxes.
        std::vector<WordOverlay^> wordBoxes;

        task<void> LoadImage(Windows::Storage::StorageFile^ file);
        void LoadSampleImage();
        void UpdateAvailableLanguages();
        void UpdateWordBoxTransform();
        void ClearResults();

        void LanguageList_SelectionChanged(Platform::Object^ sender, SelectionChangedEventArgs^ e);
        void PreviewImage_SizeChanged(Platform::Object^ sender, SizeChangedEventArgs^ e);
        void SampleButton_Tapped(Platform::Object^ sender, TappedRoutedEventArgs^ e);
        void LoadButton_Tapped(Platform::Object^ sender, TappedRoutedEventArgs^ e);
        void ExtractButton_Tapped(Platform::Object^ sender, TappedRoutedEventArgs^ e);
        void UserLanguageToggle_Toggled(Platform::Object^ sender, RoutedEventArgs^ e);
    };
}