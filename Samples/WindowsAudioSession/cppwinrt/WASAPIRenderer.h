//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once
#include "DeviceState.h"
#include "ToneSampleGenerator.h"
#include "MFSampleGenerator.h"

namespace winrt::SDKTemplate
{
    enum class ContentType
    {
        Tone,
        File
    };

    struct WASAPIRenderer;

    using DeviceStateChangedEventHandler = delegate<com_ptr<WASAPIRenderer>, com_ptr<DeviceStateChangedEventArgs>>;

    // Primary WASAPI Renderering Class
    struct WASAPIRenderer : winrt::implements<WASAPIRenderer, IActivateAudioInterfaceCompletionHandler, IDeviceStateSource>, implementation::DeviceStateSourceT<WASAPIRenderer>
    {
        void SetContentType(ContentType value) { m_contentType = value; }
        void SetFrequency(double value) { m_Frequency = value; }
        void SetContentStream(Windows::Storage::Streams::IRandomAccessStream const& value) { m_ContentStream = value; }
        void SetHWOffload(bool value) { m_IsHWOffload = value; }
        void SetBackgroundAudio(bool value) { m_IsBackground = value; }
        void SetRawAudio(bool value) { m_IsRawAudio = value; }
        void SetLowLatency(bool value) { m_IsLowLatency = value; }
        void SetBufferDuration(REFERENCE_TIME value) { m_hnsBufferDuration = value; }

        void AsyncInitializeAudioDevice() noexcept;
        void StartPlayback() noexcept;
        HRESULT StopPlaybackAsync() noexcept;
        HRESULT PausePlaybackAsync();

        HRESULT SetVolumeOnSession(double volume) noexcept;

        // IActivateAudioInterfaceCompletionHandler
        STDMETHOD(ActivateCompleted)(IActivateAudioInterfaceAsyncOperation* operation) noexcept;

    private:
        HRESULT OnStartPlayback(IMFAsyncResult* pResult) noexcept;
        HRESULT OnStopPlayback(IMFAsyncResult* pResult);
        HRESULT OnPausePlayback(IMFAsyncResult* pResult);
        HRESULT OnSampleReady(IMFAsyncResult* pResult);

        EmbeddedMFAsyncCallback<&WASAPIRenderer::OnStartPlayback> m_StartPlaybackCallback{ this };
        EmbeddedMFAsyncCallback<&WASAPIRenderer::OnStopPlayback> m_StopPlaybackCallback{ this };
        EmbeddedMFAsyncCallback<&WASAPIRenderer::OnPausePlayback> m_PausePlaybackCallback{ this };
        EmbeddedMFAsyncCallback<&WASAPIRenderer::OnSampleReady> m_SampleReadyCallback{ this };

        HRESULT ConfigureDeviceInternal() noexcept;
        void ValidateBufferValue();
        void OnAudioSampleRequested(bool IsSilence = false);
        void ConfigureSource();
        UINT32 GetBufferFramesPerPeriod() noexcept;
        void SetAudioClientChannelVolume();

        void GetToneSample(uint32_t framesAvailable);
        void GetMFSample(uint32_t framesAvailable);

    private:
        hstring m_DeviceIdString;
        uint32_t m_BufferFrames = 0;

        // Event for sample ready or user stop
        handle m_SampleReadyEvent{ check_pointer(CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS)) };

        MFWORKITEM_KEY m_SampleReadyKey;
        slim_mutex m_lock;

        unique_cotaskmem_ptr<WAVEFORMATEX> m_MixFormat;
        uint32_t m_DefaultPeriodInFrames;
        uint32_t m_FundamentalPeriodInFrames;
        uint32_t m_MaxPeriodInFrames;
        uint32_t m_MinPeriodInFrames;
        float m_channelVolume = 1.0f;

        com_ptr<IAudioClient3> m_AudioClient;
        com_ptr<IAudioRenderClient> m_AudioRenderClient;
        com_ptr<IMFAsyncResult> m_SampleReadyAsyncResult;

        ContentType m_contentType = ContentType::Tone;
        bool m_IsHWOffload = false;
        bool m_IsBackground = false;
        bool m_IsRawAudio = false;
        bool m_IsLowLatency = false;
        REFERENCE_TIME m_hnsBufferDuration = 0;
        double m_Frequency = 0.0;
        winrt::Windows::Storage::Streams::IRandomAccessStream m_ContentStream = nullptr;

        std::unique_ptr<ToneSampleGenerator> m_ToneSource;
        com_ptr<MFSampleGenerator> m_MFSource;
    };
}
