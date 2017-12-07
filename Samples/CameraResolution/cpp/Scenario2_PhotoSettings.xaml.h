//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once

#include "Scenario2_PhotoSettings.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_PhotoSettings sealed
    {
    public:
        Scenario2_PhotoSettings();
        virtual ~Scenario2_PhotoSettings();

    protected:
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        MainPage^ _rootPage;
        Platform::Agile<Windows::Media::Capture::MediaCapture^> _mediaCapture;
        Windows::Foundation::EventRegistrationToken _mediaCaptureFailedEventToken;

        bool _isPreviewing;

        // Folder in which the captures will be stored
        Windows::Storage::StorageFolder^ _captureFolder;

        Concurrency::task<void> InitializeCameraAsync();
        Concurrency::task<void> CleanupCameraAsync();

        void InitializeCameraButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void PhotoButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void PreviewSettings_Changed(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void PhotoSettings_Changed(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void CheckIfStreamsAreIdentical();
        void PopulateComboBox(Windows::Media::Capture::MediaStreamType streamType, Windows::UI::Xaml::Controls::ComboBox^ comboBox, bool showFrameRate = true);

        void MediaCapture_Failed(Windows::Media::Capture::MediaCapture^ sender, Windows::Media::Capture::MediaCaptureFailedEventArgs^ e);
    };
}
