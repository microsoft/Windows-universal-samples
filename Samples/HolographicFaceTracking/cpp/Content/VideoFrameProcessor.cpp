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
#include "VideoFrameProcessor.h"

using namespace HolographicFaceTracker;

using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation::Numerics;

using namespace Windows::Media::Capture;
using namespace Windows::Media::Capture::Frames;

using namespace concurrency;
using namespace Platform;

using namespace std::placeholders;

VideoFrameProcessor::VideoFrameProcessor(Platform::Agile<MediaCapture> mediaCapture, MediaFrameReader^ reader, MediaFrameSource^ source)
    : m_mediaCapture(std::move(mediaCapture))
    , m_mediaFrameReader(std::move(reader))
    , m_mediaFrameSource(std::move(source))
{
    // Listen for new frames, so we know when to update our m_latestFrame
    m_mediaFrameReader->FrameArrived +=
        ref new TypedEventHandler<MediaFrameReader^, MediaFrameArrivedEventArgs^>(
            std::bind(&VideoFrameProcessor::OnFrameArrived, this, _1, _2));
}

task<std::shared_ptr<VideoFrameProcessor>> VideoFrameProcessor::CreateAsync(void)
{
    return create_task(MediaFrameSourceGroup::FindAllAsync())
        .then([](IVectorView<MediaFrameSourceGroup^>^ groups)
    {
        MediaFrameSourceGroup^ selectedGroup = nullptr;
        MediaFrameSourceInfo^ selectedSourceInfo = nullptr;

        // Pick first color source.
        for (MediaFrameSourceGroup^ sourceGroup : groups)
        {
            for (MediaFrameSourceInfo^ sourceInfo : sourceGroup->SourceInfos)
            {
                if (sourceInfo->SourceKind == MediaFrameSourceKind::Color)
                {
                    selectedSourceInfo = sourceInfo;
                    break;
                }
            }

            if (selectedSourceInfo != nullptr)
            {
                selectedGroup = sourceGroup;
                break;
            }
        }

        // No valid camera was found. This will happen on the emulator.
        if (selectedGroup == nullptr || selectedSourceInfo == nullptr)
        {
            return task_from_result(std::shared_ptr<VideoFrameProcessor>(nullptr));
        }

        MediaCaptureInitializationSettings^ settings = ref new MediaCaptureInitializationSettings();
        settings->MemoryPreference = MediaCaptureMemoryPreference::Cpu; // Need SoftwareBitmaps for FaceAnalysis
        settings->StreamingCaptureMode = StreamingCaptureMode::Video;   // Only need to stream video
        settings->SourceGroup = selectedGroup;

        Platform::Agile<MediaCapture> mediaCapture(ref new MediaCapture());

        return create_task(mediaCapture->InitializeAsync(settings))
            .then([=]
        {
            MediaFrameSource^ selectedSource = mediaCapture->FrameSources->Lookup(selectedSourceInfo->Id);

            return create_task(mediaCapture->CreateFrameReaderAsync(selectedSource))
                .then([=](MediaFrameReader^ reader)
            {
                return create_task(reader->StartAsync())
                    .then([=](MediaFrameReaderStartStatus status)
                {
                    // Only create a VideoFrameProcessor if the reader successfully started
                    if (status == MediaFrameReaderStartStatus::Success)
                    {
                        return std::make_shared<VideoFrameProcessor>(mediaCapture, reader, selectedSource);
                    }
                    else
                    {
                        return std::shared_ptr<VideoFrameProcessor>(nullptr);
                    }
                });
            });
        });
    });
}

Windows::Media::Capture::Frames::MediaFrameReference^ VideoFrameProcessor::GetLatestFrame(void) const
{
    auto lock = std::shared_lock<std::shared_mutex>(m_propertiesLock);
    return m_latestFrame;
}

Windows::Media::Capture::Frames::VideoMediaFrameFormat^ VideoFrameProcessor::GetCurrentFormat(void) const
{
    return m_mediaFrameSource->CurrentFormat->VideoFormat;
}

void VideoFrameProcessor::OnFrameArrived(MediaFrameReader^ sender, MediaFrameArrivedEventArgs^ args)
{
    if (MediaFrameReference^ frame = sender->TryAcquireLatestFrame())
    {
        std::lock_guard<std::shared_mutex> lock(m_propertiesLock);
        m_latestFrame = frame;
    }
}
