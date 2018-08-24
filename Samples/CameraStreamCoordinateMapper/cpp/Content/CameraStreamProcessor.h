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

namespace CameraStreamCoordinateMapper
{
    ////////////////////////////////////////////////////////////////////////////////
    // CameraStreamProcessor
    //
    // This class is mostly a helper for accessing the MediaFrameReader class
    // and change the frame arrival events into a polling pattern
    // 
    // So instead of listening for an event "FrameArrived", you can just query
    // with "GetLatestFrame" and check the index to see if you have seen it yet

    class CameraStreamProcessor
    {
    public:
        struct FrameData
        {
            Windows::Media::Capture::Frames::MediaFrameReference^ frame;
            uint32_t index;
        };

        static Concurrency::task<std::shared_ptr<CameraStreamProcessor>> CreateAndStartAsync(
            Windows::Media::Capture::Frames::MediaFrameSourceKind sourceKind,
            Platform::String^ format,
            Windows::Media::Capture::MediaCaptureMemoryPreference memoryPreference = Windows::Media::Capture::MediaCaptureMemoryPreference::Cpu);

        CameraStreamProcessor(
            Windows::Media::Capture::Frames::MediaFrameSourceKind sourceKind,
            Platform::String^ format,
            Windows::Media::Capture::MediaCaptureMemoryPreference memoryPreference);

        ~CameraStreamProcessor();

        FrameData GetLatestFrame() const;

        concurrency::task<void> StartAsync();
        concurrency::task<void> StopAsync();

        bool IsStreaming() const;

        void OnFrameArrived(
            Windows::Media::Capture::Frames::MediaFrameReader^ sender,
            Windows::Media::Capture::Frames::MediaFrameArrivedEventArgs^ args);

    private:
        // Media Capture
        Windows::Media::Capture::Frames::MediaFrameSourceKind  m_sourceKind;
        Platform::String^                                      m_sourceFormat;
        Windows::Media::Capture::MediaCaptureMemoryPreference  m_memoryPreference;

        Platform::Agile<Windows::Media::Capture::MediaCapture> m_mediaCapture;
        Windows::Media::Capture::Frames::MediaFrameReader^     m_mediaFrameReader;

        mutable std::shared_mutex                              m_lock;

        std::atomic<bool>                                      m_isStreaming = false;

        FrameData                                              m_latestFrameData;

        Windows::Foundation::EventRegistrationToken            m_frameArrivedToken;

        uint32_t                                               m_frameCount = 0u;
    };
}
