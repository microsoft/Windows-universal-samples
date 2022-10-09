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



namespace winrt::SDKTemplate
{
    struct MFSampleGenerator : implements<MFSampleGenerator, IMFSourceReaderCallback>
    {
        enum ReaderState
        {
            Stopped,
            PreRoll,
            Playing,
            EndOfStream
        };

        MFSampleGenerator() = default;
        ~MFSampleGenerator() = default;

        // IMFSourceReaderCallback
        STDMETHODIMP_(HRESULT) OnEvent(_In_ DWORD streamIndex, _In_ IMFMediaEvent* event);
        STDMETHODIMP_(HRESULT) OnFlush(_In_ DWORD streamIndex);
        STDMETHODIMP_(HRESULT) OnReadSample(HRESULT error, DWORD streamIndex, DWORD streamFlags, LONGLONG timestamp, IMFSample* sample);

        // MFSampleGenerator
        void StartSource();
        void StopSource();

        void Initialize(Windows::Storage::Streams::IRandomAccessStream const& stream, UINT32 FramesPerPeriod, WAVEFORMATEX* wfx);
        void Shutdown();
        bool FillSampleBuffer(array_view<uint8_t> buffer, uint32_t* cbWritten);

        bool IsEOF()
        {
            return m_sampleQueue.empty() && m_readerState == ReaderState::EndOfStream;
        }

    private:
        static com_ptr<IMFMediaType> ConfigureStreams(com_ptr<IMFSourceReader> const& reader, WAVEFORMATEX* wfx);
        static com_ptr<IMFMediaType> CreateAudioType(WAVEFORMATEX* wfx);
        void AddSamplesToQueue(IMFSample* MFSample);
        bool IsPreRollFilled();

    private:
        uint32_t m_bytesPerPeriod;
        uint32_t m_bytesPerSecond = 0;
        bool m_isInitialized = false;

        com_ptr<IMFSourceReader> m_sourceReader;
        com_ptr<IMFMediaType> m_audioMediaType;
        ReaderState m_readerState = ReaderState::Stopped;

        std::forward_list<RenderBuffer> m_sampleQueue;
        std::forward_list<RenderBuffer>::iterator m_sampleQueueTail = m_sampleQueue.before_begin();
    };
}
