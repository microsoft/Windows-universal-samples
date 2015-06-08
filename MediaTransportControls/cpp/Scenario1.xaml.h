//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

//
// Scenario1.xaml.h
// Declaration of the Scenario1 class
//

#pragma once
#include "Scenario1.g.h"

namespace SDKTemplate
{
    /// <summary>
    /// Page for scenario 1 of this sample.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1 sealed
    {
    public:
        Scenario1();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        void SetupSystemMediaTransportControls();

        void SelectFilesButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void MyMediaElement_MediaOpened(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void MyMediaElement_MediaEnded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void MyMediaElement_MediaFailed(Platform::Object^ sender, Windows::UI::Xaml::ExceptionRoutedEventArgs^ e);
        void MyMediaElement_CurrentStateChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void systemMediaControls_ButtonPressed(
            Windows::Media::SystemMediaTransportControls^ sender,
            Windows::Media::SystemMediaTransportControlsButtonPressedEventArgs^ args
            );            
        void SyncPlaybackStatusToMediaElementState();
        Windows::Media::MediaPlaybackType GetMediaTypeFromFileContentType(Windows::Storage::StorageFile^ file);
        Concurrency::task<void> UpdateSystemMediaControlsDisplayAsync(Windows::Storage::StorageFile^ mediaFile);

        void SetNewMediaItem(int playlistIndex);

    private:
        MainPage^ rootPage;

        bool isInitialized;

        /// <summary>
        /// Indicates whether this scenario page is still active. Changes value during navigation 
        /// to or away from the page.
        /// </summary>
        bool isThisPageActive;

        // same type as returned from Windows::Storage::Pickers::FileOpenPicker::PickMultipleFilesAsync()
        Windows::Foundation::Collections::IVectorView<Windows::Storage::StorageFile^>^ playlist;

        /// <summary>
        /// index to current media item in playlist
        /// </summary>
        int currentItemIndex;

        Windows::Media::SystemMediaTransportControls^ systemMediaControls;
        Windows::Foundation::EventRegistrationToken systemMediaControlsButtonPressedEventToken;
    };
}
