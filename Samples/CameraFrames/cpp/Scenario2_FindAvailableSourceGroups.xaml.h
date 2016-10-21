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

#include "Scenario2_FindAvailableSourceGroups.g.h"
#include "MainPage.xaml.h"
#include "SimpleLogger.h"
#include "FrameRenderer.h"
#include "FrameSourceViewModels.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_FindAvailableSourceGroups sealed
    {
    public:
        Scenario2_FindAvailableSourceGroups();

    protected:
        // Protected UI overrides.
        /// <summary>
        /// Called when user navigates to this Scenario.
        /// Sets the GroupCombobox item source.
        /// </summary>
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

        /// <summary>
        /// Called when user navigates away from this Scenario.
        /// Stops streaming and disposes of all objects.
        /// </summary>
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        // Private UI methods.
        /// <summary>
        /// Stops reading from the previous selection and starts reading frames from the newly selected source group.
        /// </summary>
        void GroupComboBox_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
        
        /// <summary>
        /// Called when a source is selected.
        /// Sets the FormatCombobox item source based on the selected source.
        /// </summary>
        void SourceComboBox_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);

        /// <summary>
        /// Called when a format is selected.
        /// Sets the format of the currently selected source.
        /// </summary>
        void FormatComboBox_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);

        /// <summary>
        /// Called when use clicks the Stop button.
        /// </summary>
        void StopButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        /// <summary>
        /// Called when use clicks the Start button.
        /// </summary>
        void StartButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

    private:
        // Private methods.
        /// <summary>
        /// Stops reading from the frame reader, disposes of the reader and updates the button state.
        /// </summary>
        concurrency::task<void> StopReaderAsync();

        /// <summary>
        /// Closes the MediaCapture object and clears the items from the Format and Source ComboBoxes.
        /// Must be called from the UI thread.
        /// </summary>
        void DisposeMediaCapture();

        /// <summary>
        /// Starts reading frames from the current reader.
        /// Must be called from the UI thread.
        /// </summary>
        concurrency::task<void> StartReaderAsync();

        /// <summary>
        /// Creates a reader from the current frame source and registers to handle its frame events.
        /// Must be called from the UI thread.
        /// </summary>
        concurrency::task<void> CreateReaderAsync();

        /// <summary>
        // Updates the current frame source to the one corresponding to the user's selection.
        /// </summary>
        void UpdateFrameSource();

        /// <summary>
        /// Initialize the media capture object.
        /// Must be called from the UI thread.
        /// </summary>
        concurrency::task<bool> TryInitializeCaptureAsync();

        /// <summary>
        /// Changes the format of the currently selected source.
        /// </summary>
        concurrency::task<void> ChangeMediaFormatAsync(FrameFormatModel^ format);

        /// <summary>
        /// Updates the state of the start and stop buttons, based on the current App Model.
        /// </summary>
        concurrency::task<void> UpdateButtonStateAsync();

        /// <summary>
        /// Handles the frame arrived event by converting the frame to a dislayable
        /// format and rendering it to the screen.
        /// </summary>
        void Reader_FrameArrived(Windows::Media::Capture::Frames::MediaFrameReader^ reader, Windows::Media::Capture::Frames::MediaFrameArrivedEventArgs^ args);

    private:
        // Private data.
        concurrency::task<void> m_currentTask = concurrency::task_from_result();

        // Whether or not we are currently streaming.
        bool m_streaming = false;

        Platform::Agile<Windows::Media::Capture::MediaCapture^> m_mediaCapture;
        
        Windows::Media::Capture::Frames::MediaFrameSource^ m_source;
        Windows::Media::Capture::Frames::MediaFrameReader^ m_reader;
        FrameRenderer^ m_frameRenderer;

        SourceGroupCollection^ m_groupCollection;
        SimpleLogger^ m_logger;

        Windows::Foundation::EventRegistrationToken m_frameArrivedToken;
    };
} // SDKTemplate