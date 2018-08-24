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
#include "CameraStreamProcessor.h"

using namespace CameraStreamCoordinateMapper;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation::Numerics;

using namespace Windows::Media::Capture;
using namespace Windows::Media::Capture::Frames;

using namespace concurrency;
using namespace Platform;

using namespace std::placeholders;

task<std::shared_ptr<CameraStreamProcessor>> CameraStreamProcessor::CreateAndStartAsync(
    MediaFrameSourceKind sourceKind, 
    Platform::String^ sourceFormat, 
    MediaCaptureMemoryPreference memoryPreference)
{
    auto streamProcessor = std::make_shared<CameraStreamProcessor>(sourceKind, sourceFormat, memoryPreference);

    co_await streamProcessor->StartAsync();

    return streamProcessor;
}

CameraStreamProcessor::CameraStreamProcessor(
    MediaFrameSourceKind sourceKind,
    Platform::String^ format,
    MediaCaptureMemoryPreference memoryPreference) :
    m_sourceKind(sourceKind),
    m_sourceFormat(std::move(format)),
    m_memoryPreference(memoryPreference)
{}

CameraStreamProcessor::~CameraStreamProcessor()
{
    std::lock_guard<std::shared_mutex> lock(m_lock);
    m_mediaFrameReader->FrameArrived -= m_frameArrivedToken;
}

CameraStreamProcessor::FrameData CameraStreamProcessor::GetLatestFrame(void) const
{
    auto lock = std::shared_lock<std::shared_mutex>(m_lock);
    return m_latestFrameData;
}

task<void> CameraStreamProcessor::StartAsync()
{
    IVectorView<MediaFrameSourceGroup^>^ groups = co_await MediaFrameSourceGroup::FindAllAsync();

    MediaFrameSourceGroup^ selectedGroup;
    MediaFrameSourceInfo^ selectedSourceInfo;

    // Pick the first group and first matching source from that group
    for (MediaFrameSourceGroup^ group : groups)
    {
        for (MediaFrameSourceInfo^ sourceInfo : group->SourceInfos)
        {
            if (sourceInfo->SourceKind == m_sourceKind)
            {
                selectedSourceInfo = sourceInfo;
                break;
            }
        }

        if (selectedSourceInfo != nullptr)
        {
            selectedGroup = group;
            break;
        }
    }

    if (selectedGroup == nullptr || selectedSourceInfo == nullptr)
    {
        OutputDebugString(L"Warning: No source found that matched the requested parameters\n");
        co_return;
    }

    MediaCaptureInitializationSettings^ settings = ref new MediaCaptureInitializationSettings();
    settings->MemoryPreference = m_memoryPreference;
    settings->StreamingCaptureMode = StreamingCaptureMode::Video;
    settings->SourceGroup = selectedGroup;
    settings->SharingMode = MediaCaptureSharingMode::SharedReadOnly;

    Platform::Agile<MediaCapture> mediaCapture(ref new MediaCapture());

    co_await mediaCapture->InitializeAsync(settings);

    MediaFrameSource^ selectedSource = mediaCapture->FrameSources->Lookup(selectedSourceInfo->Id);

    MediaFrameReader^ reader = co_await mediaCapture->CreateFrameReaderAsync(selectedSource, m_sourceFormat);

    MediaFrameReaderStartStatus status = co_await reader->StartAsync();

    if (status == MediaFrameReaderStartStatus::Success)
    {
        std::lock_guard<std::shared_mutex> lock(m_lock);

        m_mediaCapture = std::move(mediaCapture);
        m_mediaFrameReader = std::move(reader);

        m_frameArrivedToken = m_mediaFrameReader->FrameArrived +=
            ref new TypedEventHandler<MediaFrameReader^, MediaFrameArrivedEventArgs^>(
                std::bind(&CameraStreamProcessor::OnFrameArrived, this, _1, _2));

        m_isStreaming = true;
    }
    else
    {
        OutputDebugString(L"Warning: Reader failed to start\n");
    }
}

task<void> CameraStreamProcessor::StopAsync()
{
    m_isStreaming = false;

    std::lock_guard<std::shared_mutex> lock(m_lock);

    m_mediaFrameReader->FrameArrived -= m_frameArrivedToken;
    co_await m_mediaFrameReader->StopAsync();
    
    m_mediaFrameReader = nullptr;
    m_mediaCapture = nullptr;
    m_latestFrameData = {};
    m_frameArrivedToken = {};
    m_frameCount = 0u;
}

bool CameraStreamCoordinateMapper::CameraStreamProcessor::IsStreaming() const
{
    return m_isStreaming;
}

void CameraStreamProcessor::OnFrameArrived(MediaFrameReader^ sender, MediaFrameArrivedEventArgs^ args)
{
    if (MediaFrameReference^ frame = sender->TryAcquireLatestFrame())
    {
        std::lock_guard<std::shared_mutex> lock(m_lock);

        m_latestFrameData.frame = frame;
        m_latestFrameData.index = m_frameCount++;
    }
}
