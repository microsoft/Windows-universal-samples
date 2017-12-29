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
#include "Scenario1_DisplayDepthColorIR.xaml.h"
#include "FrameRenderer.h"
#include <unordered_set>

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Media::Capture;
using namespace Windows::Media::Capture::Frames;
using namespace Windows::UI::Xaml::Media::Imaging;

Scenario1_DisplayDepthColorIR::Scenario1_DisplayDepthColorIR()
{
    InitializeComponent();

    m_logger = ref new SimpleLogger(outputTextBlock);

    m_frameRenderers[MediaFrameSourceKind::Color] = ref new FrameRenderer(colorPreviewImage);
    m_frameRenderers[MediaFrameSourceKind::Depth] = ref new FrameRenderer(depthPreviewImage);
    m_frameRenderers[MediaFrameSourceKind::Infrared] = ref new FrameRenderer(infraredPreviewImage);
}

void Scenario1_DisplayDepthColorIR::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
    PickNextMediaSourceAsync();
}

void Scenario1_DisplayDepthColorIR::OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
    CleanupMediaCaptureAsync();
}

void Scenario1_DisplayDepthColorIR::NextButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    PickNextMediaSourceAsync();
}

task<void> Scenario1_DisplayDepthColorIR::PickNextMediaSourceAsync()
{
    NextButton->IsEnabled = false;
    return PickNextMediaSourceWorkerAsync()
        .then([this]()
    {
        NextButton->IsEnabled = true;
    }, task_continuation_context::use_current());
}

task<void> Scenario1_DisplayDepthColorIR::PickNextMediaSourceWorkerAsync()
{
    return CleanupMediaCaptureAsync()
        .then([this]()
    {
        return create_task(MediaFrameSourceGroup::FindAllAsync());
    }, task_continuation_context::get_current_winrt_context())
        .then([this](IVectorView<MediaFrameSourceGroup^>^ allGroups)
    {
        if (allGroups->Size == 0)
        {
            m_logger->Log("No source groups found.");
            return task_from_result();
        }

        // Pick next group in the array after each time the Next button is clicked.
        m_selectedSourceGroupIndex = (m_selectedSourceGroupIndex + 1) % allGroups->Size;
        MediaFrameSourceGroup^ selectedGroup = allGroups->GetAt(m_selectedSourceGroupIndex);

        m_logger->Log("Found " + allGroups->Size.ToString() + " groups and " +
            "selecting index [" + m_selectedSourceGroupIndex.ToString() + "] : " +
            selectedGroup->DisplayName);

        // Initialize MediaCapture with selected group.
        return TryInitializeMediaCaptureAsync(selectedGroup)
            .then([this, selectedGroup](bool initialized)
        {
            if (!initialized)
            {
                return CleanupMediaCaptureAsync();
            }

            // Set up frame readers, register event handlers and start streaming.
            auto startedKinds = std::make_shared<std::unordered_set<MediaFrameSourceKind>>();
            task<void> createReadersTask = task_from_result();
            for (IKeyValuePair<String^, MediaFrameSource^>^ kvp : m_mediaCapture->FrameSources)
            {
                MediaFrameSource^ source = kvp->Value;
                createReadersTask = createReadersTask.then([this, startedKinds, source]()
                {
                    MediaFrameSourceKind kind = source->Info->SourceKind;

                    // Ignore this source if we already have a source of this kind.
                    if (startedKinds->find(kind) != startedKinds->end())
                    {
                        return task_from_result();
                    }

                    // Look for a format which the FrameRenderer can render.
                    String^ requestedSubtype = nullptr;
                    auto found = std::find_if(begin(source->SupportedFormats), end(source->SupportedFormats),
                        [&](MediaFrameFormat^ format)
                    {
                        requestedSubtype = FrameRenderer::GetSubtypeForFrameReader(kind, format);
                        return requestedSubtype != nullptr;
                    });
                    if (requestedSubtype == nullptr)
                    {
                        // No acceptable format was found. Ignore this source.
                        return task_from_result();
                    }

                    // Tell the source to use the format we can render.
                    return create_task(source->SetFormatAsync(*found))
                        .then([this, source, requestedSubtype]()
                    {
                        return create_task(m_mediaCapture->CreateFrameReaderAsync(source, requestedSubtype));
                    }, task_continuation_context::get_current_winrt_context())
                        .then([this, kind](MediaFrameReader^ frameReader)
                    {
                        EventRegistrationToken token = frameReader->FrameArrived +=
                            ref new TypedEventHandler<MediaFrameReader^, MediaFrameArrivedEventArgs^>(this, &Scenario1_DisplayDepthColorIR::FrameReader_FrameArrived);

                        // Keep track of created reader and event handler so it can be stopped later.
                        m_readers.push_back(std::make_pair(frameReader, token));

                        m_logger->Log(kind.ToString() + " reader created");

                        return create_task(frameReader->StartAsync());
                    }, task_continuation_context::get_current_winrt_context())
                        .then([this, kind, startedKinds](MediaFrameReaderStartStatus status)
                    {
                        if (status == MediaFrameReaderStartStatus::Success)
                        {
                            m_logger->Log("Started " + kind.ToString() + " reader.");
                            startedKinds->insert(kind);
                        }
                        else
                        {
                            m_logger->Log("Unable to start " + kind.ToString() + "  reader. Error: " + status.ToString());
                        }
                    }, task_continuation_context::get_current_winrt_context());
                }, task_continuation_context::get_current_winrt_context());
            }
            // Run the loop and see if any sources were used.
            return createReadersTask.then([this, startedKinds, selectedGroup]()
            {
                if (startedKinds->size() == 0)
                {
                    m_logger->Log("No eligible sources in " + selectedGroup->DisplayName + ".");
                }
            }, task_continuation_context::get_current_winrt_context());
        }, task_continuation_context::get_current_winrt_context());
    }, task_continuation_context::get_current_winrt_context());
}

task<bool> Scenario1_DisplayDepthColorIR::TryInitializeMediaCaptureAsync(MediaFrameSourceGroup^ group)
{
    if (m_mediaCapture != nullptr)
    {
        // Already initialized.
        return task_from_result(true);
    }

    // Initialize mediacapture with the source group.
    m_mediaCapture = ref new MediaCapture();

    auto settings = ref new MediaCaptureInitializationSettings();

    // Select the source we will be reading from.
    settings->SourceGroup = group;

    // This media capture can share streaming with other apps.
    settings->SharingMode = MediaCaptureSharingMode::SharedReadOnly;

    // Only stream video and don't initialize audio capture devices.
    settings->StreamingCaptureMode = StreamingCaptureMode::Video;

    // Set to CPU to ensure frames always contain CPU SoftwareBitmap images,
    // instead of preferring GPU D3DSurface images.
    settings->MemoryPreference = MediaCaptureMemoryPreference::Cpu;

    // Only stream video and don't initialize audio capture devices.
    settings->StreamingCaptureMode = StreamingCaptureMode::Video;

    // Initialize MediaCapture with the specified group.
    // This must occur on the UI thread because some device families
    // (such as Xbox) will prompt the user to grant consent for the
    // app to access cameras.
    // This can raise an exception if the source no longer exists,
    // or if the source could not be initialized.
    return create_task(m_mediaCapture->InitializeAsync(settings))
        .then([this](task<void> initializeMediaCaptureTask)
    {
        try
        {
            // Get the result of the initialization. This call will throw if initialization failed
            // This pattern is docuemnted at https://msdn.microsoft.com/en-us/library/dd997692.aspx
            initializeMediaCaptureTask.get();
            m_logger->Log("MediaCapture is successfully initialized in shared mode.");
            return true;
        }
        catch (Exception^ exception)
        {
            m_logger->Log("Failed to initialize media capture: " + exception->Message);
            return false;
        }
    });
}

/// Unregisters FrameArrived event handlers, stops and disposes frame readers
/// and disposes the MediaCapture object.
task<void> Scenario1_DisplayDepthColorIR::CleanupMediaCaptureAsync()
{
    task<void> cleanupTask = task_from_result();

    if (m_mediaCapture != nullptr)
    {
        for (auto& readerAndToken : m_readers)
        {
            MediaFrameReader^ reader = readerAndToken.first;
            EventRegistrationToken token = readerAndToken.second;

            reader->FrameArrived -= token;
            cleanupTask = cleanupTask && create_task(reader->StopAsync());
        }
        m_readers.clear();
        m_mediaCapture = nullptr;
    }
    return cleanupTask;
}

void Scenario1_DisplayDepthColorIR::FrameReader_FrameArrived(MediaFrameReader^ sender, MediaFrameArrivedEventArgs^ args)
{
    // TryAcquireLatestFrame will return the latest frame that has not yet been acquired.
    // This can return null if there is no such frame, or if the reader is not in the
    // "Started" state. The latter can occur if a FrameArrived event was in flight
    // when the reader was stopped.
    if (MediaFrameReference^ frame = sender->TryAcquireLatestFrame())
    {
        if (frame != nullptr)
        {
            m_frameRenderers[frame->SourceKind]->ProcessFrame(frame);
        }
    }
}
