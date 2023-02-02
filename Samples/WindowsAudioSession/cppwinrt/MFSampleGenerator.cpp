//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include "MFSampleGenerator.h"

using namespace winrt::Windows::Storage::Streams;

namespace winrt::SDKTemplate
{
#define PREROLL_DURATION_SEC 3     // Arbitrary value for seconds of data in preroll buffer

    //
    //  Shutdown()
    //
    void MFSampleGenerator::Shutdown()
    {
        if (m_isInitialized)
        {
            m_audioMediaType = nullptr;
            m_sourceReader = nullptr;

            m_sampleQueue.clear();

            m_isInitialized = false;
        }
    }

    //
    //  StartSource()
    //
    //  Start the media source to read samples
    //
    void MFSampleGenerator::StartSource()
    {
        if (!m_isInitialized)
        {
            throw hresult_illegal_state_change();
        }

        // We can be in EOS if this is a resume from pause
        if ((m_readerState == ReaderState::PreRoll) ||
            (m_readerState == ReaderState::Playing) ||
            (m_readerState == ReaderState::EndOfStream))
            return;

        // This will start the source reader and asynchronously call OnReadSample
        check_hresult(m_sourceReader->ReadSample((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, nullptr, nullptr, nullptr));
        m_readerState = ReaderState::PreRoll;
    }

    //
    //  StopSource()
    //
    //  Stops the media source from reading samples
    //
    void MFSampleGenerator::StopSource()
    {
        // We only really need this if Source is still reading samples, otherwise
        // we should be in an EndOfStream state and parsing existing samples in the queue
        m_readerState = ReaderState::Stopped;
    }

    //
    //  Initialize()
    //
    //  Configure the Source Reader
    //
    void MFSampleGenerator::Initialize(IRandomAccessStream const& stream, UINT32 framesPerPeriod, WAVEFORMATEX* wfx)
    {
        m_audioMediaType = nullptr;
        m_sourceReader = nullptr;

        com_ptr<IMFAttributes> attributes;
        check_hresult(MFCreateAttributes(attributes.put(), 3));

        // Specify Source Reader Attributes
        check_hresult(attributes->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, this));
        check_hresult(attributes->SetString(MF_READWRITE_MMCSS_CLASS_AUDIO, L"Audio"));
        check_hresult(attributes->SetUINT32(MF_READWRITE_MMCSS_PRIORITY_AUDIO, 0));

        // Create a stream from IRandomAccessStream
        com_ptr<IMFByteStream> byteStream;
        check_hresult(MFCreateMFByteStreamOnStreamEx(winrt::get_unknown(stream), byteStream.put()));

        // Create source reader
        com_ptr<IMFSourceReader> sourceReader;
        check_hresult(MFCreateSourceReaderFromByteStream(byteStream.get(), attributes.get(), sourceReader.put()));

        // Configure media types. If this succeeds, then everything passed and we commit our changes.
        m_audioMediaType = ConfigureStreams(sourceReader, wfx);
        m_sourceReader = std::move(sourceReader);

        m_bytesPerPeriod = framesPerPeriod * wfx->nBlockAlign;

        // For uncompressed formats, nAvgBytesPerSec should equal nSamplesPerSecond * nBlockAlign
        m_bytesPerSecond = wfx->nAvgBytesPerSec;

        m_isInitialized = true;
    }


    //
    //  ConfigureStreams()
    //
    //  Enables the first audio stream and configures the media type
    //
    com_ptr<IMFMediaType> MFSampleGenerator::ConfigureStreams(com_ptr<IMFSourceReader> const& reader, WAVEFORMATEX* wfx)
    {
        // Disable all streams
        check_hresult(reader->SetStreamSelection((DWORD)MF_SOURCE_READER_ALL_STREAMS, false));

        // Enable first audio stream
        check_hresult(reader->SetStreamSelection((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, true));

        com_ptr<IMFMediaType> partialMediaType = CreateAudioType(wfx);

        // Set type on source reader so necessary converters / decoders will be added
        check_hresult(reader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, partialMediaType.get()));

        // Verify the media type on stream
        com_ptr<IMFMediaType> uncompressedMediaType;
        check_hresult(reader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, uncompressedMediaType.put()));

        unique_cotaskmem_ptr<WAVEFORMATEX> createdWfx;
        UINT32 cbFormat = 0;
        check_hresult(MFCreateWaveFormatExFromMFMediaType(uncompressedMediaType.get(), createdWfx.put(), &cbFormat));

        return uncompressedMediaType;
    }

    //
    //  CreateAudioType()
    //
    //  Create an audio type based on the default mix format from the renderer
    //
    com_ptr<IMFMediaType> MFSampleGenerator::CreateAudioType(WAVEFORMATEX* wfx)
    {
        com_ptr<IMFMediaType> mediaType;

        // Create a partial media type for our mix format (PCM or IEEE Float)
        check_hresult(MFCreateMediaType(mediaType.put()));

        // Set media type attributes
        check_hresult(mediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio));

        switch (GetRenderSampleType(wfx))
        {
        case RenderSampleType::Pcm16Bit:
        case RenderSampleType::Pcm24In32Bit:
            check_hresult(mediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM));
            break;
        case RenderSampleType::Float:
            check_hresult(mediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_Float));
            break;
        default:
            throw hresult_error(E_UNEXPECTED);
        }

        check_hresult(mediaType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, wfx->nChannels));
        check_hresult(mediaType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, wfx->nSamplesPerSec));
        check_hresult(mediaType->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, wfx->nBlockAlign));
        check_hresult(mediaType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, wfx->nAvgBytesPerSec));
        check_hresult(mediaType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, wfx->wBitsPerSample));
        check_hresult(mediaType->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, true));

        return mediaType;
    }

    //
    //  OnEvent()
    //
    //  Implementation of IMFSourceReaderCallback::OnEvent()
    //
    HRESULT MFSampleGenerator::OnEvent([[maybe_unused]] DWORD streamIndex, [[maybe_unused]] IMFMediaEvent* event)
    {
        return S_OK;
    }

    //
    //  OnFlush()
    //
    //  Implementation of IMFSourceReaderCallback::OnFlush()
    //
    HRESULT MFSampleGenerator::OnFlush([[maybe_unused]] DWORD streamIndex)
    {
        return S_OK;
    }

    //
    //  OnReadSample()
    //
    //  Implementation of IMFSourceReaderCallback::OnReadSample().  When a sample is ready, add it to the sample queue.
    //
    HRESULT MFSampleGenerator::OnReadSample(HRESULT error, [[maybe_unused]] DWORD streamIndex,  DWORD streamFlags, [[maybe_unused]] LONGLONG timestamp, IMFSample* sample) try
    {
        if ((m_readerState != ReaderState::Playing) &&
            (m_readerState != ReaderState::PreRoll))
        {
            return S_OK;
        }

        // If we have a failure, change in stream format, or hit EOF, then we stop reading samples
        if ((FAILED(error)) ||
            (streamFlags & MF_SOURCE_READERF_ENDOFSTREAM) ||
            (streamFlags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED))
        {
            m_readerState = ReaderState::EndOfStream;
            return S_OK;
        }

        // If there were no samples read, then ignore the callback.
        if (!sample)
        {
            return S_OK;
        }

        // Add the data from the Media Sample to our buffer queue
        AddSamplesToQueue(sample);

        // Pre-roll PREROLL_DURATION seconds worth of data
        if (m_readerState == ReaderState::PreRoll)
        {
            if (IsPreRollFilled())
            {
                // Once Pre-roll is filled, audio endpoint will stop rendering silence and start
                // picking up data from the queue
                m_readerState = ReaderState::Playing;
            }
        }

        // Call ReadSample for next asynchronous sample event
        check_hresult(m_sourceReader->ReadSample((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, nullptr, nullptr, nullptr));

        return S_OK;
    }
    catch (...) { return to_hresult(); }

    //
    //  FillSampleBuffer()
    //
    //  Called by the audio endpoint to get data to render, and will set cbWritten to valid number of
    //  bytes in the buffer.  The buffers are zeroed out when initialized so the endpoint could feasibly
    //  treat the data as silence. If there are no more samples to render, will return false.
    //
    bool MFSampleGenerator::FillSampleBuffer(array_view<uint8_t> buffer, uint32_t* cbWritten)
    {
        *cbWritten = 0;

        if (!m_isInitialized)
        {
            throw hresult_error(E_UNEXPECTED);
        }

        // Not going to return any data here while in a pre-roll state.
        if (m_readerState == ReaderState::PreRoll)
        {
            return true;
        }

        if (m_sampleQueue.empty())
        {
            if (m_readerState == ReaderState::EndOfStream)
            {
                // We are EOS, should be set from OnReadSample() and the client should check for EOS before
                // calling FillSampleBuffer()
                return false;
            }
            else
            {
                // There shouldn't be a situation where we get here unless the reader has
                // somehow gotten into the wrong state
                throw hresult_error(E_FAIL);
            }
        }
        else
        {
            RenderBuffer& sampleBuffer = m_sampleQueue.front();

            if (sampleBuffer.BufferSize > buffer.size())
            {
                // We don't have enough data in the client audio buffer this pass
                // Do nothing and wait for the next sample period
            }
            else
            {
                CopyMemory(buffer.data(), sampleBuffer.Buffer.get(), sampleBuffer.BufferSize);

                *cbWritten = sampleBuffer.BufferSize;
                m_sampleQueue.pop_front();
            }
        }
        return true;
    }

    //
    //  AddSamplesToBuffer()
    //
    //  Data is added to the end of the sample queue
    //
    void MFSampleGenerator::AddSamplesToQueue(IMFSample* sample)
    {
        com_ptr<IMFMediaBuffer> mediaBuffer;

        // Since we are storing the raw byte data, convert this to a single buffer
        check_hresult(sample->ConvertToContiguousBuffer(mediaBuffer.put()));

        // Lock the sample
        BYTE* audioData = nullptr;
        DWORD cbAudioData = 0;
        check_hresult(mediaBuffer->Lock(&audioData, NULL, &cbAudioData));

        // Make sure we unlock the buffer.
        auto unlock = scope_exit([&] { mediaBuffer->Unlock(); });

        DWORD cbBytesCopied = 0;

        // First fill up any partial buffers from the previous sample
        if ((m_sampleQueueTail != m_sampleQueue.before_begin()) &&
            (m_sampleQueueTail->BytesFilled < m_sampleQueueTail->BufferSize))
        {
            RenderBuffer& PartialBuffer = *m_sampleQueueTail;

            DWORD cbBytesRemaining = PartialBuffer.BufferSize - PartialBuffer.BytesFilled;
            DWORD cbBytesToCopy = (std::min)(cbAudioData, cbBytesRemaining);

            CopyMemory(PartialBuffer.Buffer.get() + PartialBuffer.BytesFilled, audioData, cbBytesToCopy);

            PartialBuffer.BytesFilled += cbBytesToCopy;
            cbBytesCopied += cbBytesToCopy;
        }

        // Create buffers for the remainder of the samples
        while (cbBytesCopied < cbAudioData)
        {
            m_sampleQueueTail = m_sampleQueue.emplace_after(m_sampleQueueTail, m_bytesPerPeriod);
            RenderBuffer& SampleBuffer = *m_sampleQueueTail;

            DWORD cbBytesRemaining = cbAudioData - cbBytesCopied;
            DWORD cbBytesToCopy = (std::min)(static_cast<DWORD>(SampleBuffer.BufferSize), cbBytesRemaining);

            SampleBuffer.BytesFilled = 0;

            ZeroMemory(SampleBuffer.Buffer.get(), m_bytesPerPeriod);
            CopyMemory(SampleBuffer.Buffer.get(), audioData + cbBytesCopied, cbBytesToCopy);

            SampleBuffer.BytesFilled += cbBytesToCopy;
            cbBytesCopied += cbBytesToCopy;
        }
    }

    //
    //  IsPreRollFilled()
    //
    //  Loops through the queue quickly to see if our pre-roll buffer is filled
    //
    bool MFSampleGenerator::IsPreRollFilled()
    {
        DWORD totalBytes = 0;

        for (RenderBuffer& sampleBuffer : m_sampleQueue)
        {
            totalBytes += sampleBuffer.BytesFilled;

            if (totalBytes >= m_bytesPerSecond * PREROLL_DURATION_SEC)
            {
                return true;
            }
        }

        return false;
    }
}
