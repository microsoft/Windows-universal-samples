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

#include "pch.h"
#include <windowsnumerics.h>
#include "Scenario1_CorrelateStreams.xaml.h"
#include "FrameRenderer.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Media::Devices::Core;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Media::Capture;
using namespace Windows::Media::Capture::Frames;
using namespace Windows::Perception::Spatial;
using namespace Windows::UI::Xaml::Media::Imaging;

using namespace WindowsPreview::Media::Capture::Frames;

// Used to determine whether a source has a Perception major type.
static String^ PerceptionMediaType = L"Perception";

// Returns the values from a std::map as a std::vector.
template<typename K, typename T>
static inline std::vector<T> values(std::map<K, T> const& inputMap)
{
    std::vector<T> outputVector(inputMap.size());
    std::transform(inputMap.begin(), inputMap.end(), outputVector.begin(), [](auto const& pair)
    {
        return pair.second;
    });
    return outputVector;
}

Scenario1_CorrelateStreams::Scenario1_CorrelateStreams() : rootPage(MainPage::Current)
{
    InitializeComponent();
    
    m_logger = ref new SimpleLogger(outputTextBlock);

    m_correlatedFrameRenderer = std::make_unique<FrameRenderer>(previewImage);
}

void Scenario1_CorrelateStreams::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
    // Start streaming from the first available source group.
    PickNextMediaSourceAsync();
}

void Scenario1_CorrelateStreams::OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
    CleanupMediaCaptureAsync();
}

void Scenario1_CorrelateStreams::NextButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    PickNextMediaSourceAsync();
}

void Scenario1_CorrelateStreams::ToggleDepth_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    m_frameSources[MediaFrameSourceKind::Depth].enabled = !m_frameSources[MediaFrameSourceKind::Depth].enabled;
    UpdateUI();
}

void Scenario1_CorrelateStreams::ToggleSkeletons_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    m_frameSources[MediaFrameSourceKind::Custom].enabled = !m_frameSources[MediaFrameSourceKind::Custom].enabled;
    UpdateUI();
}

void Scenario1_CorrelateStreams::UpdateUI()
{
    ToggleDepth->IsEnabled = m_frameSources[MediaFrameSourceKind::Depth].sourceInfo != nullptr;
    ToggleSkeletons->IsEnabled = m_frameSources[MediaFrameSourceKind::Custom].sourceInfo != nullptr;

    DepthMessage->Text = m_frameSources[MediaFrameSourceKind::Depth].sourceInfo != nullptr
        ? "Depth overlay " + (m_frameSources[MediaFrameSourceKind::Depth].enabled ? "enabled" : "disabled")
        : "Depth source not found";
    SkeletalMessage->Text = m_frameSources[MediaFrameSourceKind::Depth].sourceInfo != nullptr
        ? "Skeletal overlay " + (m_frameSources[MediaFrameSourceKind::Custom].enabled ? "enabled" : "disabled")
        : "Skeletal tracking source not found";
}

task<void> Scenario1_CorrelateStreams::PickNextMediaSourceAsync()
{
    NextButton->IsEnabled = false;
    return PickNextMediaSourceWorkerAsync()
        .then([this]()
    {
        NextButton->IsEnabled = true;
        UpdateUI();
    }, task_continuation_context::use_current());
}

task<void> Scenario1_CorrelateStreams::PickNextMediaSourceWorkerAsync()
{
    return CleanupMediaCaptureAsync().then([this]()
    {
        return create_task(MediaFrameSourceGroup::FindAllAsync());
    }).then([this](IVectorView<MediaFrameSourceGroup^>^ allGroups)
    {
        std::vector<MediaFrameSourceGroup^> eligableGroups;
        for (auto const& group : allGroups)
        {
            auto sourceInfos = group->SourceInfos;

            // Keep this group if it at least supports color, as the other sources must be correlated with the color source.
            if (group != nullptr && std::any_of(begin(sourceInfos), end(sourceInfos),
                [](MediaFrameSourceInfo^ sourceInfo) { return sourceInfo != nullptr && sourceInfo->SourceKind == MediaFrameSourceKind::Color; }))
            {
                eligableGroups.push_back(group);
            }
        }

        if (eligableGroups.size() == 0)
        {
            m_logger->Log("No valid source groups found");
            return task_from_result();
        }

        // Pick next group in the array after each time the Next button is clicked.
        m_selectedSourceGroupIndex = (m_selectedSourceGroupIndex + 1) % eligableGroups.size();

        m_logger->Log("Found " + eligableGroups.size().ToString() + " groups and " +
            "selecting index [" + m_selectedSourceGroupIndex.ToString() + "] : " +
            eligableGroups[m_selectedSourceGroupIndex]->DisplayName);

        MediaFrameSourceGroup^ selectedGroup = eligableGroups[m_selectedSourceGroupIndex];

        // Initialize MediaCapture with selected group.
        return TryInitializeMediaCaptureAsync(selectedGroup)
            .then([this, selectedGroup](bool initialized)
        {
            if (!initialized)
            {
                return CleanupMediaCaptureAsync();
            }

            // Try to find color, depth and skeletal sources on this source group.
            auto sourceInfos = selectedGroup->SourceInfos;

            auto colorSourceInfo = std::find_if(begin(sourceInfos), end(sourceInfos), [](MediaFrameSourceInfo^ sourceInfo)
            {
                return sourceInfo->SourceKind == MediaFrameSourceKind::Color;
            });

            auto depthSourceInfo = std::find_if(begin(sourceInfos), end(sourceInfos), [](MediaFrameSourceInfo^ sourceInfo)
            {
                return sourceInfo->SourceKind == MediaFrameSourceKind::Depth;
            });

            // In order to determine whether or not the source info is a skeletal source, its major type must
            // be "Perception" and its subtype must be a Guid matching MFPoseTrackingPreview::MFPerceptionFormat_PoseTracking.
            // We muse use the initialized media capture object to look up the format and determine the major type and subtype
            // of the source.
            auto skeletalSourceInfo = std::find_if(begin(sourceInfos), end(sourceInfos), [this](MediaFrameSourceInfo^ sourceInfo)
            {
                if (m_mediaCapture->FrameSources->HasKey(sourceInfo->Id))
                {
                    MediaFrameFormat^ format = m_mediaCapture->FrameSources->Lookup(sourceInfo->Id)->CurrentFormat;
                    GUID subTypeGuid;
                    if (SUCCEEDED(IIDFromString(format->Subtype->Data(), &subTypeGuid)))
                    {
                        return sourceInfo->SourceKind == MediaFrameSourceKind::Custom
                            && format->MajorType == PerceptionMediaType
                            && Guid(subTypeGuid).Equals(PoseTrackingFrame::PoseTrackingSubtype);
                    }
                }

                return false;
            });

            // Reset our frame sources data
            m_frameSources[MediaFrameSourceKind::Color] = FrameSourceState();
            m_frameSources[MediaFrameSourceKind::Depth] = FrameSourceState();
            m_frameSources[MediaFrameSourceKind::Custom] = FrameSourceState();

            // Store the source info object if a source group was found.
            m_frameSources[MediaFrameSourceKind::Color].sourceInfo = colorSourceInfo != end(sourceInfos) ? *colorSourceInfo : nullptr;
            m_frameSources[MediaFrameSourceKind::Depth].sourceInfo = depthSourceInfo != end(sourceInfos) ? *depthSourceInfo : nullptr;
            m_frameSources[MediaFrameSourceKind::Custom].sourceInfo = skeletalSourceInfo != end(sourceInfos) ? *skeletalSourceInfo : nullptr;

            // Enable color always.
            m_frameSources[MediaFrameSourceKind::Color].enabled = true;

            // Enable depth if depth is available.
            m_frameSources[MediaFrameSourceKind::Depth].enabled = m_frameSources[MediaFrameSourceKind::Depth].sourceInfo != nullptr;

            // Create readers for found sources.
            std::vector<task<void>> createReadersTasks;

            if (m_frameSources[MediaFrameSourceKind::Color].sourceInfo)
            {
                createReadersTasks.push_back(CreateReaderAsync(m_frameSources[MediaFrameSourceKind::Color].sourceInfo));
            }

            if (m_frameSources[MediaFrameSourceKind::Depth].sourceInfo)
            {
                createReadersTasks.push_back(CreateReaderAsync(m_frameSources[MediaFrameSourceKind::Depth].sourceInfo));
            }

            if (m_frameSources[MediaFrameSourceKind::Custom].sourceInfo)
            {
                createReadersTasks.push_back(CreateReaderAsync(m_frameSources[MediaFrameSourceKind::Custom].sourceInfo));
            }

            // The when_all method will execute all tasks in parallel, and call the continuation when all tasks have completed.
            // This async method can be called even if no readers are present. In that case the continuation will be called immediately.
            return when_all(begin(createReadersTasks), end(createReadersTasks));
        });
    }, task_continuation_context::get_current_winrt_context());
}

task<void> Scenario1_CorrelateStreams::CreateReaderAsync(MediaFrameSourceInfo^ info)
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
        m_frameSources[info->SourceKind].frameArrivedEventToken = frameReader->FrameArrived +=
            ref new TypedEventHandler<MediaFrameReader^, MediaFrameArrivedEventArgs^>(this, &Scenario1_CorrelateStreams::FrameReader_FrameArrived);

        m_logger->Log(info->SourceKind.ToString() + " reader created");

        // Keep track of created reader and event handler so it can be stopped later.
        m_frameSources[info->SourceKind].reader = frameReader;
        return create_task(frameReader->StartAsync());
    }).then([this, info](MediaFrameReaderStartStatus status)
    {
        if (status != MediaFrameReaderStartStatus::Success)
        {
            m_logger->Log("Unable to start " + info->SourceKind.ToString() + "  reader. Error: " + status.ToString());
        }
    });
}

task<bool> Scenario1_CorrelateStreams::TryInitializeMediaCaptureAsync(MediaFrameSourceGroup^ group)
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
            // This pattern is documented at https://msdn.microsoft.com/en-us/library/dd997692.aspx
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

task<void> Scenario1_CorrelateStreams::CleanupMediaCaptureAsync()
{
    task<void> cleanupTask = task_from_result();

    if (m_mediaCapture != nullptr)
    {
        for (FrameSourceState frameSourceState : values(m_frameSources))
        {
            if (frameSourceState.reader)
            {
                frameSourceState.reader->FrameArrived -= frameSourceState.frameArrivedEventToken;
                cleanupTask = cleanupTask && create_task(frameSourceState.reader->StopAsync());
            }

            frameSourceState.enabled = false;
            frameSourceState.sourceInfo = nullptr;
            frameSourceState.reader = nullptr;
            frameSourceState.latestFrame = nullptr;
        }

        m_mediaCapture = nullptr;
    }
    return cleanupTask;
}

void Scenario1_CorrelateStreams::FrameReader_FrameArrived(MediaFrameReader^ sender, MediaFrameArrivedEventArgs^ args)
{
    // TryAcquireLatestFrame will return the latest frame that has not yet been acquired.
    // This can return null if there is no such frame, or if the reader is not in the
    // "Started" state. The latter can occur if a FrameArrived event was in flight
    // when the reader was stopped.
    if (MediaFrameReference^ candidateFrame = sender->TryAcquireLatestFrame())
    {
        // Since multiple sources will be receiving frames, we must synchronize access to m_frameSources.
        auto lock = m_frameLock.LockExclusive();

        // Buffer frame for later usage.
        m_frameSources[candidateFrame->SourceKind].latestFrame = candidateFrame;

        auto frameSourceObjects = values(m_frameSources);
        bool allFramesBuffered = std::none_of(frameSourceObjects.begin(), frameSourceObjects.end(),
            [](FrameSourceState const& frameSourceState)
        {
            return frameSourceState.enabled && frameSourceState.latestFrame == nullptr;
        });

        // If we have frames from currently enabled sources, render to UI.
        if (allFramesBuffered)
        {
            bool colorEnabled = m_frameSources[MediaFrameSourceKind::Color].enabled;
            bool depthEnabled = m_frameSources[MediaFrameSourceKind::Depth].enabled;
            bool skeletalEnabled = m_frameSources[MediaFrameSourceKind::Custom].enabled;

            MediaFrameReference^ colorFrame = m_frameSources[MediaFrameSourceKind::Color].latestFrame;
            MediaFrameReference^ depthFrame = m_frameSources[MediaFrameSourceKind::Depth].latestFrame;
            MediaFrameReference^ skeletalFrame = m_frameSources[MediaFrameSourceKind::Custom].latestFrame;

            // If depth and color enabled, correlate and output
            if (colorEnabled && depthEnabled)
            {
                m_correlatedFrameRenderer->ProcessDepthAndColorFrames(colorFrame, depthFrame);
            }
            // Render only color if enabled and available
            else if (colorEnabled)
            {
                m_correlatedFrameRenderer->ProcessColorFrame(colorFrame);
            }

            // Clear the skeleton shapes if we are not rendering a skeletal frame.
            Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal,
                ref new Windows::UI::Core::DispatchedHandler([this]()
            {
                skeletalFrameRenderer->Clear();
            }));

            // Render skeletal data if enabled and color and depth frames are available.
            // The skeletal rendering requires the color and depth coordinate systems and the 
            // color frame's camera intrinsics to map the resulting skeleton shapes into color image space.
            if (skeletalEnabled && colorFrame != nullptr && depthFrame != nullptr)
            {
                if (PoseTrackingFrameCreationResult^ result = PoseTrackingFrame::Create(skeletalFrame))
                {
                    if (result->Status == PoseTrackingFrameCreationStatus::Success)
                    {
                        // We must map the coordinates produced by the skeletal tracker from depth space to color space.
                        CameraIntrinsics^ colorIntrinsics = colorFrame->VideoMediaFrame->CameraIntrinsics;
                        SpatialCoordinateSystem^ colorCoordinateSystem = colorFrame->CoordinateSystem;
                        SpatialCoordinateSystem^ depthCoordinateSystem = depthFrame->CoordinateSystem;

                        Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal,
                            ref new Windows::UI::Core::DispatchedHandler([this, result, colorIntrinsics, colorCoordinateSystem, depthCoordinateSystem]()
                        {
                            // We must also scale up the coordinates to the size of the color image canves in order
                            // to map the bones onto people in the frame correctly.
                            float widthScale = static_cast<float>(previewImage->ActualWidth) / static_cast<float>(colorIntrinsics->ImageWidth);
                            float heightScale = static_cast<float>(previewImage->ActualHeight) / static_cast<float>(colorIntrinsics->ImageHeight);

                            if (IBox<float4x4>^ boxedDepthToColorTransform = depthCoordinateSystem->TryGetTransformTo(colorCoordinateSystem))
                            {
                                float4x4 depthToColorTransform = boxedDepthToColorTransform->Value;

                                skeletalFrameRenderer->Render(result->Frame, ref new CoordinateTransformationMethod(
                                    [colorIntrinsics, depthToColorTransform, widthScale, heightScale](float3 point)
                                {
                                    // Transform the point from depth to color space.
                                    Point transformedPoint = colorIntrinsics->ProjectOntoFrame(transform(point, depthToColorTransform));

                                    // Scale up the coordinates to match the color image.
                                    transformedPoint.X *= widthScale;
                                    transformedPoint.Y *= heightScale;

                                    // Return the result to the SkeletalFrameRenderer.
                                    return transformedPoint;
                                }));
                            }
                        }));
                    }
                    else
                    {
                        m_logger->Log("Failed to convert pose tracking frame: " + result->Status.ToString());
                    }
                }
            }

            // clear buffered frames if used
            if (colorEnabled)
            {
                m_frameSources[MediaFrameSourceKind::Color].latestFrame = nullptr;
            }
            if (depthEnabled)
            {
                m_frameSources[MediaFrameSourceKind::Depth].latestFrame = nullptr;
            }
            if (skeletalEnabled)
            {
                m_frameSources[MediaFrameSourceKind::Custom].latestFrame = nullptr;
            }
        }
    }
    else
    {
        m_logger->Log("Unable to acquire frame");
    }
}