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

#include "Scenario2_Custom.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_Custom sealed
    {
    public:
        Scenario2_Custom();
    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        void GetCustomProfile();
        void PickFile(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void PickOutput(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnTargetFormatChanged(Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
        void SetPickFileButton(bool isEnabled);
        void StopPlayers();
        void PlayFile(Windows::Storage::StorageFile^ MediaFile);

        void TranscodeCustom(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void TranscodeCancel(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void TranscodeProgress(Windows::Foundation::IAsyncActionWithProgress<double>^ asyncInfo, double percent);
        void TranscodeError(Platform::String^ error);
        void TranscodeFailure(Windows::Media::Transcoding::TranscodeFailureReason reason);
        void SetCancelButton(bool isEnabled);
        void SetOutputFileButton(bool isEnabled);
        void SetTranscodeButton(bool isEnabled);
        void EnableButtons();
        void DisableButtons();

        // Media Controls
        void InputPlayButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void InputPauseButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void InputStopButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OutputPlayButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OutputPauseButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OutputStopButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);


        Platform::String^ _OutputFileName;
        Platform::String^ _OutputFileExtension = ".mp4";
        Platform::String^ _OutputType = "MP4";
        Windows::Media::MediaProperties::MediaEncodingProfile^ _Profile;
        Windows::Storage::StorageFile^ _InputFile;
        Windows::Storage::StorageFile^ _OutputFile;
        Windows::Media::Transcoding::MediaTranscoder^ _Transcoder;
        Concurrency::cancellation_token_source _CTS;

        boolean _UseMp4;

        SDKTemplate::MainPage^ rootPage;
    };
}
