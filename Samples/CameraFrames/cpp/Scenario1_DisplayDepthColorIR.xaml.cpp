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

MediaFrameSourceInfo^ Scenario1_DisplayDepthColorIR::GetFirstSourceInfoOfKind(MediaFrameSourceGroup^ group, MediaFrameSourceKind kind)
{
    auto matchingInfo = std::find_if(begin(group->SourceInfos), end(group->SourceInfos),
        [kind](MediaFrameSourceInfo^ sourceInfo)
    {
        return sourceInfo->SourceKind == kind;
    });
    return (matchingInfo != end(group->SourceInfos)) ? *matchingInfo : nullptr;
}

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
    struct EligibleGroup
    {
        MediaFrameSourceGroup^ Group;
        std::array<MediaFrameSourceInfo^, 3> SourceInfos;
    };

    return CleanupMediaCaptureAsync()
        .then([this]()
    {
        return create_task(MediaFrameSourceGroup::FindAllAsync());
    }).then([this](IVectorView<MediaFrameSourceGroup^>^ allGroups)
    {
        // Identify the color, depth, and infrared sources of each group,
        // and keep only the groups that have at least one recognized source.
        std::vector<EligibleGroup> eligibleGroups;
        for (auto group : allGroups)
        {
            EligibleGroup eligibleGroup;
            eligibleGroup.Group = group;
            // For each source kind, find the source which offers that kind of media frame,
            // or null if there is no such source.
            eligibleGroup.SourceInfos[0] = GetFirstSourceInfoOfKind(group, MediaFrameSourceKind::Color);
            eligibleGroup.SourceInfos[1] = GetFirstSourceInfoOfKind(group, MediaFrameSourceKind::Depth);
            eligibleGroup.SourceInfos[2] = GetFirstSourceInfoOfKind(group, MediaFrameSourceKind::Infrared);
            // If any source was found of any kind we support, keep this group.
            if (std::any_of(eligibleGroup.SourceInfos.begin(), eligibleGroup.SourceInfos.end(),
                [](MediaFrameSourceInfo^ sourceInfo) { return sourceInfo != nullptr; }))
            {
                eligibleGroups.push_back(eligibleGroup);
            }
        }

        if (eligibleGroups.size() == 0)
        {
            m_logger->Log("No source group with color, depth or infrared found.");
            return task_from_result();
        }

        // Pick next group in the array after each time the Next button is clicked.
        m_selectedSourceGroupIndex = (m_selectedSourceGroupIndex + 1) % eligibleGroups.size();

        m_logger->Log("Found " + eligibleGroups.size().ToString() + " groups and " +
            "selecting index [" + m_selectedSourceGroupIndex.ToString() + "] : " +
            eligibleGroups[m_selectedSourceGroupIndex].Group->DisplayName);
        EligibleGroup selected = eligibleGroups[m_selectedSourceGroupIndex];

        // Initialize MediaCapture with selected group.
        return TryInitializeMediaCaptureAsync(selected.Group)
            .then([this, selected](bool initialized)
        {
            if (!initialized)
            {
                return CleanupMediaCaptureAsync();
            }

            // Set up frame readers, register event handlers and start streaming.
            task<void> createReadersTask = task_from_result();
            for (size_t i = 0; i < selected.SourceInfos.size(); i++)
            {
                MediaFrameSourceInfo^ info = selected.SourceInfos[i];
                if (info != nullptr)
                {
                    createReadersTask = createReadersTask && CreateReaderAsync(selected.Group, info);
                }
                else
                {
                    String^ frameKind = (i == 0 ? "Color" : i == 1 ? "Depth" : "Infrared");
                    m_logger->Log("No " + frameKind + " source in group " + selected.Group->DisplayName);
                }

            }
            return createReadersTask;
        });
    }, task_continuation_context::get_current_winrt_context());
}

task<void> Scenario1_DisplayDepthColorIR::CreateReaderAsync(MediaFrameSourceGroup^ group, MediaFrameSourceInfo^ info)
{
    // Access the initialized frame source by looking up the the Id of the source.
    // Verify that the Id is present, because it may have left the group while were were
    // busy deciding which group to use.
    if (!m_mediaCapture->FrameSources->HasKey(info->Id))
    {
        m_logger->Log("Unable to start " + info->SourceKind.ToString() + " reader: Frame source not found");
        return task_from_result();
    }

    return create_task(m_mediaCapture->CreateFrameReaderAsync(m_mediaCapture->FrameSources->Lookup(info->Id)))
        .then([this, info](MediaFrameReader^ frameReader)
    {
        EventRegistrationToken token = frameReader->FrameArrived +=
            ref new TypedEventHandler<MediaFrameReader^, MediaFrameArrivedEventArgs^>(this, &Scenario1_DisplayDepthColorIR::FrameReader_FrameArrived);

        m_logger->Log(info->SourceKind.ToString() + " reader created");

        // Keep track of created reader and event handler so it can be stopped later.
        m_readers.push_back(std::make_pair(frameReader, token));

        return create_task(frameReader->StartAsync());
    }).then([this, info](MediaFrameReaderStartStatus status)
    {
        if (status != MediaFrameReaderStartStatus::Success)
        {
            m_logger->Log("Unable to start " + info->SourceKind.ToString() + "  reader. Error: " + status.ToString());
        }
    });
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
