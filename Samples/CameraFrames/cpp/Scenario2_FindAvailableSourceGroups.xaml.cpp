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

#include "pch.h"
#include "Scenario2_FindAvailableSourceGroups.xaml.h"
#include "FrameRenderer.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Media::Capture;
using namespace Windows::Media::Capture::Frames;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Navigation;

Scenario2_FindAvailableSourceGroups::Scenario2_FindAvailableSourceGroups()
{
    InitializeComponent();
    m_logger = ref new SimpleLogger(outputTextBlock);
    m_frameRenderer = ref new FrameRenderer(PreviewImage);
}

void Scenario2_FindAvailableSourceGroups::OnNavigatedTo(NavigationEventArgs^ e)
{
    /// SourceGroupCollection will setup device watcher to monitor
    /// SourceGroup devices enabled or disabled from the system.
    m_groupCollection = ref new SourceGroupCollection(Dispatcher);
    GroupComboBox->ItemsSource = m_groupCollection->FrameSourceGroups;
}

void Scenario2_FindAvailableSourceGroups::OnNavigatedFrom(NavigationEventArgs^ e)
{
    delete m_groupCollection;
    m_currentTask = m_currentTask.then([this]()
    {
        return StopReaderAsync();
    }).then([this]()
    {
        DisposeMediaCapture();
    }, task_continuation_context::get_current_winrt_context());
}

task<void> Scenario2_FindAvailableSourceGroups::UpdateButtonStateAsync()
{
    return create_task(Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
    {
        StartButton->IsEnabled = m_source != nullptr && !m_streaming;
        StopButton->IsEnabled = m_source != nullptr && m_streaming;
    })));
}

void Scenario2_FindAvailableSourceGroups::DisposeMediaCapture()
{
    FormatComboBox->ItemsSource = nullptr;
    SourceComboBox->ItemsSource = nullptr;

    m_source = nullptr;

    delete m_mediaCapture.Get();
    m_mediaCapture = nullptr;
}

void Scenario2_FindAvailableSourceGroups::GroupComboBox_SelectionChanged(Object^ sender, SelectionChangedEventArgs^ e)
{
    m_currentTask = m_currentTask.then([this]()
    {
        return StopReaderAsync();
    }).then([this]()
    {
        DisposeMediaCapture();

        auto group = dynamic_cast<FrameSourceGroupModel^>(GroupComboBox->SelectedItem);
        if (group != nullptr)
        {
            return TryInitializeCaptureAsync().then([this, group](bool initialized)
            {
                if (initialized) {
                    SourceComboBox->ItemsSource = group->SourceInfos;
                    SourceComboBox->SelectedIndex = 0;
                }
            }, task_continuation_context::get_current_winrt_context());
        }
        return task_from_result();
    }, task_continuation_context::get_current_winrt_context());
}

void Scenario2_FindAvailableSourceGroups::SourceComboBox_SelectionChanged(Object^ sender, SelectionChangedEventArgs^ e)
{
    m_currentTask = m_currentTask.then([this]()
    {
        return StopReaderAsync();
    }).then([this]()
    {
        if (SourceComboBox->SelectedItem != nullptr)
        {
            return StartReaderAsync().then([this]()
            {
                // Get the formats supported by the selected source into the FormatComboBox.
                auto formats = ref new Vector<FrameFormatModel^>();
                if (m_mediaCapture != nullptr && m_source != nullptr)
                {
                    for (MediaFrameFormat^ format : m_source->SupportedFormats)
                    {
                        formats->Append(ref new FrameFormatModel(format));
                    }
                }

                FormatComboBox->ItemsSource = formats;
            }, task_continuation_context::get_current_winrt_context());
        }
        return task_from_result();
    }, task_continuation_context::get_current_winrt_context());
}

void Scenario2_FindAvailableSourceGroups::FormatComboBox_SelectionChanged(Object^ sender, SelectionChangedEventArgs^ e)
{
    auto format = dynamic_cast<FrameFormatModel^>(FormatComboBox->SelectedItem);
    m_currentTask = m_currentTask.then([this, format]()
    {
        return ChangeMediaFormatAsync(format);
    });
}

task<void> Scenario2_FindAvailableSourceGroups::StartReaderAsync()
{
    return CreateReaderAsync().then([this]()
    {
        if (m_reader != nullptr && !m_streaming)
        {
            return create_task(m_reader->StartAsync())
                .then([this](MediaFrameReaderStartStatus result)
            {
                m_logger->Log("Start reader with result: " + result.ToString());

                if (result == MediaFrameReaderStartStatus::Success)
                {
                    m_streaming = true;
                    return UpdateButtonStateAsync();
                }
                return task_from_result();
            });
        }
        return task_from_result();
    });
}

task<void> Scenario2_FindAvailableSourceGroups::CreateReaderAsync()
{
    return TryInitializeCaptureAsync().then([this](bool initialized)
    {
        if (initialized) {
            UpdateFrameSource();
            if (m_source != nullptr)
            {
                return create_task(m_mediaCapture->CreateFrameReaderAsync(m_source))
                    .then([this](MediaFrameReader^ reader)
                {
                    m_reader = reader;
                    m_frameArrivedToken = reader->FrameArrived +=
                        ref new TypedEventHandler<MediaFrameReader^, MediaFrameArrivedEventArgs^>(
                            this, &Scenario2_FindAvailableSourceGroups::Reader_FrameArrived);
                    m_logger->Log("Reader created on source: " + m_source->Info->Id);
                });
            }
        }
        return task_from_result();
    }, task_continuation_context::get_current_winrt_context());
}

void Scenario2_FindAvailableSourceGroups::UpdateFrameSource()
{
    auto info = dynamic_cast<FrameSourceInfoModel^>(SourceComboBox->SelectedItem);
    if (m_mediaCapture != nullptr && info != nullptr && info->SourceGroup != nullptr)
    {
        auto groupModel = dynamic_cast<FrameSourceGroupModel^>(GroupComboBox->SelectedItem);
        if (groupModel == nullptr || groupModel->Id != info->SourceGroup->Id)
        {
            SourceComboBox->SelectedItem = nullptr;
            return;
        }
        if (m_source == nullptr || m_source->Info->Id != info->SourceInfo->Id)
        {
            if (m_mediaCapture->FrameSources->HasKey(info->SourceInfo->Id))
            {
                m_source = m_mediaCapture->FrameSources->Lookup(info->SourceInfo->Id);
            }
            else
            {
                m_source = nullptr;
            }
        }
    }
    else
    {
        m_source = nullptr;
    }
}

task<bool> Scenario2_FindAvailableSourceGroups::TryInitializeCaptureAsync()
{
    if (m_mediaCapture != nullptr)
    {
        return task_from_result(true);
    }

    auto groupModel = dynamic_cast<FrameSourceGroupModel^>(GroupComboBox->SelectedItem);
    if (groupModel == nullptr)
    {
        return task_from_result(false);
    }

    // Create a new media capture object.
    m_mediaCapture = ref new MediaCapture();

    auto settings = ref new MediaCaptureInitializationSettings();

    // Select the source we will be reading from.
    settings->SourceGroup = groupModel->SourceGroup;

    // This media capture has exclusive control of the source.
    settings->SharingMode = MediaCaptureSharingMode::ExclusiveControl;

    // Set to CPU to ensure frames always contain CPU SoftwareBitmap images,
    // instead of preferring GPU D3DSurface images.
    settings->MemoryPreference = MediaCaptureMemoryPreference::Cpu;

    // Capture only video. Audio device will not be initialized.
    settings->StreamingCaptureMode = StreamingCaptureMode::Video;

    // Initialize MediaCapture with the specified group.
    // This must occur on the UI thread because some device families
    // (such as Xbox) will prompt the user to grant consent for the
    // app to access cameras.
    // This can raise an exception if the source no longer exists,
    // or if the source could not be initialized.
    return create_task(m_mediaCapture->InitializeAsync(settings))
        .then([this, groupModel](task<void> initializeMediaCaptureTask)
    {
        try
        {
            // Get the result of the initialization. This call will throw if initialization failed
            // This pattern is docuemnted at https://msdn.microsoft.com/en-us/library/dd997692.aspx
            initializeMediaCaptureTask.get();
            m_logger->Log("Successfully initialized MediaCapture for " + groupModel->DisplayName);
            return true;
        }
        catch (Exception^ exception)
        {
            m_logger->Log("Failed to initialize media capture: " + exception->Message);
            DisposeMediaCapture();
            return false;
        }
    });
}

task<void> Scenario2_FindAvailableSourceGroups::ChangeMediaFormatAsync(FrameFormatModel^ format)
{
    if (m_source == nullptr)
    {
        m_logger->Log("Unable to set format when source is not set.");
        return task_from_result();
    }

    // Do nothing if no format was selected, or if the selected format is the same as the current one.
    if (format == nullptr || format->HasSameFormat(m_source->CurrentFormat))
    {
        return task_from_result();
    }

    return create_task(m_source->SetFormatAsync(format->Format)).then([this, format]()
    {
        m_logger->Log("Format set to " + format->DisplayName);
    });
}

task<void> Scenario2_FindAvailableSourceGroups::StopReaderAsync()
{
    task<void> currentTask = task_from_result();

    m_streaming = false;
    if (m_reader != nullptr)
    {
        // Stop streaming from reader.
        currentTask = currentTask.then([this]()
        {
            return m_reader->StopAsync();
        }).then([this]()
        {
            m_reader->FrameArrived -= m_frameArrivedToken;
            m_reader = nullptr;
            m_logger->Log("Reader stopped");
        });
    }

    return currentTask.then([this]()
    {
        return UpdateButtonStateAsync();
    });
}

void Scenario2_FindAvailableSourceGroups::StartButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    m_currentTask = m_currentTask.then([this]()
    {
        return StartReaderAsync();
    }, task_continuation_context::get_current_winrt_context());
}

void Scenario2_FindAvailableSourceGroups::StopButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    m_currentTask = m_currentTask.then([this]()
    {
        return StopReaderAsync();
    });
}

void Scenario2_FindAvailableSourceGroups::Reader_FrameArrived(MediaFrameReader^ reader, MediaFrameArrivedEventArgs^ args)
{
    // TryAcquireLatestFrame will return the latest frame that has not yet been acquired.
    // This can return null if there is no such frame, or if the reader is not in the
    // "Started" state. The latter can occur if a FrameArrived event was in flight
    // when the reader was stopped.
    if (MediaFrameReference^ frame = reader->TryAcquireLatestFrame())
    {
        m_frameRenderer->ProcessFrame(frame);
    }
}
