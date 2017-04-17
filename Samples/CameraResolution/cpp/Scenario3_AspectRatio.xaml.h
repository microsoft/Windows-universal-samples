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

#include "Scenario3_AspectRatio.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario3_AspectRatio sealed
    {
    public:
        Scenario3_AspectRatio();
        virtual ~Scenario3_AspectRatio();
        
    protected:
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        MainPage^ _rootPage;
        Platform::Agile<Windows::Media::Capture::MediaCapture^> _mediaCapture;
        Windows::Foundation::EventRegistrationToken _mediaCaptureFailedEventToken;

        bool _isPreviewing;
        bool _isRecording;

        // Folder in which the captures will be stored
        Windows::Storage::StorageFolder^ _captureFolder;

        Concurrency::task<void> InitializeCameraAsync();
        Concurrency::task<void> CleanupCameraAsync();

        void InitializeCameraButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void VideoButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void PreviewSettings_Changed(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void VideoSettings_Changed(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void CheckIfStreamsAreIdentical();
        void PopulateComboBoxes();
        void MatchPreviewAspectRatio();

        void MediaCapture_Failed(Windows::Media::Capture::MediaCapture^ sender, Windows::Media::Capture::MediaCaptureFailedEventArgs^ e);
    };
}
