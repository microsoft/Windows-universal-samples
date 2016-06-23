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

#include "Scenario1_PreviewSettings.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_PreviewSettings sealed
    {
    public:
        Scenario1_PreviewSettings();
        virtual ~Scenario1_PreviewSettings();

    protected:
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        MainPage^ _rootPage;
        Platform::Agile<Windows::Media::Capture::MediaCapture^> _mediaCapture;
        Windows::Foundation::EventRegistrationToken _mediaCaptureFailedEventToken;

        bool _isPreviewing;

        Concurrency::task<void> InitializeCameraAsync();
        Concurrency::task<void> CleanupCameraAsync();

        void InitializeCameraButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ComboBoxSettings_Changed(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void PopulateSettingsComboBox();

        void MediaCapture_Failed(Windows::Media::Capture::MediaCapture^ sender, Windows::Media::Capture::MediaCaptureFailedEventArgs^ e);
    };
}
