//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

//
// MFSampleGenerator.cpp
//

#include "pch.h"
#include "MFSampleGenerator.h"

using namespace SDKSample::WASAPIAudio;

//
//  MFSampleGenerator()
//
MFSampleGenerator::MFSampleGenerator() :
    m_Ref( 1 ),
    m_IsInitialized( false ),
    m_BytesPerPeriod( 0 ),
    m_ReaderState( ReaderStateStopped ),
    m_MFSourceReader( nullptr ),
    m_AudioMT( nullptr ),
    m_SampleQueue( nullptr ),
    m_SampleQueueTail( &m_SampleQueue )
{
}

//
//  ~MFSampleGenerator()
//
MFSampleGenerator::~MFSampleGenerator()
{
    Shutdown();
}

//
//  Flush()
//
void MFSampleGenerator::Flush()
{
    while( m_SampleQueue != nullptr )
    {
        RenderBuffer *SampleBuffer = m_SampleQueue;
        m_SampleQueue = SampleBuffer->Next;
        SAFE_DELETE( SampleBuffer );
    }
}

//
//  Shutdown()
//
void MFSampleGenerator::Shutdown()
{
    if (m_IsInitialized)
    {
        SAFE_RELEASE( m_AudioMT );
        SAFE_RELEASE( m_MFSourceReader );

        Flush();

        m_IsInitialized = false;
    }
}

//
//  StartSource()
//
//  Start the media source to read samples
//
HRESULT MFSampleGenerator::StartSource()
{
    if (!m_IsInitialized)
        return E_UNEXPECTED;

    // We can be in EOS if this is a resume from pause
    if ( (m_ReaderState == ReaderStatePreRoll) ||
         (m_ReaderState == ReaderStatePlaying) ||
         (m_ReaderState == ReaderStateEOS) )
        return S_FALSE;

    HRESULT hr = S_OK;

    // This will start the source reader and asynchronously call OnReadSample
    hr = m_MFSourceReader->ReadSample( MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, nullptr, nullptr, nullptr );
    if (SUCCEEDED( hr ))
    {
        m_ReaderState = ReaderStatePreRoll;
    }

    return hr;
}

//
//  StopSource()
//
//  Stops the media source from reading samples
//
void MFSampleGenerator::StopSource()
{
    // We only really need this if Source is still reading samples, otherwise
    // we should be in an EOS state and parsing existing samples in the queue
    m_ReaderState = ReaderStateStopped;
}

//
//  Initialize()
//
//  Configure the Source Reader
//
HRESULT MFSampleGenerator::Initialize( IRandomAccessStream^ stream, UINT32 FramesPerPeriod, WAVEFORMATEX *wfx )
{
    HRESULT hr = S_OK;

    m_ContentStream = stream;
    m_MixFormat = wfx;
    IMFByteStream *ByteStream = nullptr;
    IMFAttributes *Attributes = nullptr;

    SAFE_RELEASE( m_AudioMT );
    SAFE_RELEASE( m_MFSourceReader );

    hr = MFCreateAttributes( &Attributes, 3 );
    if (FAILED( hr ))
    {
        goto exit;
    }

    // Specify Source Reader Attributes
    Attributes->SetUnknown( MF_SOURCE_READER_ASYNC_CALLBACK, static_cast<IMFSourceReaderCallback *>(this) );
    if (FAILED( hr ))
    {
        goto exit;
    }

    Attributes->SetString( MF_READWRITE_MMCSS_CLASS_AUDIO, L"Audio" );
    if (FAILED( hr ))
    {
        goto exit;
    }

    Attributes->SetUINT32( MF_READWRITE_MMCSS_PRIORITY_AUDIO, 0 );
    if (FAILED( hr ))
    {
        goto exit;
    }

    // Create a stream from IRandomAccessStream
    hr = MFCreateMFByteStreamOnStreamEx( reinterpret_cast<IUnknown*>(m_ContentStream), &ByteStream );
    if ( FAILED( hr ) )
    {
        goto exit;
    }

    // Create source reader
    hr = MFCreateSourceReaderFromByteStream( ByteStream, Attributes, &m_MFSourceReader );
    if ( FAILED( hr ) )
    {
        goto exit;
    }

    // Configures media types
    hr = ConfigureStreams();
    if ( FAILED( hr ) )
    {
        goto exit;
    }

    m_BytesPerPeriod = FramesPerPeriod * m_MixFormat->nBlockAlign;
    m_IsInitialized = true;

exit:
    if ( FAILED( hr ) )
    {
        SAFE_RELEASE( m_AudioMT );
        SAFE_RELEASE( m_MFSourceReader );
    }

    SAFE_RELEASE( Attributes );
    SAFE_RELEASE( ByteStream );

    return hr;
}

//
//  ConfigureStreams()
//
//  Enables the first audio stream and configures the media type
//
HRESULT MFSampleGenerator::ConfigureStreams()
{
    HRESULT hr = S_OK;
    IMFMediaType *UncompressedMT = nullptr;
    IMFMediaType *PartialMT = nullptr;

    // Disable all streams
    hr = m_MFSourceReader->SetStreamSelection( MF_SOURCE_READER_ALL_STREAMS, false );
    if ( FAILED( hr ) )
    {
        goto exit;
    }

    // Enable first audio stream
    hr = m_MFSourceReader->SetStreamSelection( MF_SOURCE_READER_FIRST_AUDIO_STREAM, true );
    if ( FAILED( hr ) )
    {
        goto exit;
    }

    hr = CreateAudioType( &PartialMT );
    if ( FAILED( hr ) )
    {
        goto exit;
    }

    // Set type on source reader so necessary converters / decoders will be added
    hr = m_MFSourceReader->SetCurrentMediaType( MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, PartialMT );
    if ( FAILED( hr ) )
    {
        goto exit;
    }

    // Verify the media type on stream
    hr = m_MFSourceReader->GetCurrentMediaType( MF_SOURCE_READER_FIRST_AUDIO_STREAM, &UncompressedMT );
    if ( FAILED( hr ) )
    {
        goto exit;
    }

    WAVEFORMATEX *pwfx = NULL;
    UINT32 cbFormat = 0;
    MFCreateWaveFormatExFromMFMediaType( UncompressedMT, &pwfx, &cbFormat );

    CoTaskMemFree( pwfx );

    m_AudioMT = UncompressedMT;
    m_AudioMT->AddRef();

exit:
    SAFE_RELEASE( UncompressedMT );
    SAFE_RELEASE( PartialMT );

    return hr;
}

//
//  CreateAudioType()
//
//  Create an audio type based on the default mix format from the renderer
//
HRESULT MFSampleGenerator::CreateAudioType( IMFMediaType **MediaType )
{
    HRESULT hr = S_OK;
    IMFMediaType *MT = nullptr;

    // Create a partial media type for our mix format (PCM or IEEE Float)
    hr = MFCreateMediaType( &MT );
    if (FAILED( hr ))
    {
        goto exit;
    }

    // Set media type attributes
    hr = MT->SetGUID( MF_MT_MAJOR_TYPE, MFMediaType_Audio );
    if ( FAILED( hr ) )
    {
        goto exit;
    }

    if (RenderSampleType::SampleType16BitPCM == CalculateMixFormatType( m_MixFormat ))
    {
        hr = MT->SetGUID( MF_MT_SUBTYPE, MFAudioFormat_PCM );
    }
    else
    {
        hr = MT->SetGUID( MF_MT_SUBTYPE, MFAudioFormat_Float );
    }

    if (FAILED( hr ))
    {
        goto exit;
    }

    hr = MT->SetUINT32( MF_MT_AUDIO_NUM_CHANNELS, m_MixFormat->nChannels );
    if (FAILED( hr ))
    {
        goto exit;
    }

    hr = MT->SetUINT32( MF_MT_AUDIO_SAMPLES_PER_SECOND, m_MixFormat->nSamplesPerSec );
    if (FAILED( hr ))
    {
        goto exit;
    }

    hr = MT->SetUINT32( MF_MT_AUDIO_BLOCK_ALIGNMENT, m_MixFormat->nBlockAlign );
    if (FAILED( hr ))
    {
        goto exit;
    }

    hr = MT->SetUINT32( MF_MT_AUDIO_AVG_BYTES_PER_SECOND, m_MixFormat->nAvgBytesPerSec );
    if (FAILED( hr ))
    {
        goto exit;
    }

    hr = MT->SetUINT32( MF_MT_AUDIO_BITS_PER_SAMPLE, m_MixFormat->wBitsPerSample );
    if (FAILED( hr ))
    {
        goto exit;
    }

    hr = MT->SetUINT32( MF_MT_ALL_SAMPLES_INDEPENDENT, true );
    if (FAILED( hr ))
    {
        goto exit;
    }

    *MediaType = MT;
    (*MediaType)->AddRef();

exit:
    SAFE_RELEASE( MT );
    return hr;
}

//
//  OnEvent()
//
//  Implementation of IMFSourceReaderCallback::OnEvent()
//
HRESULT MFSampleGenerator::OnEvent(_In_ DWORD dwStreamIndex, _In_ IMFMediaEvent *pEvent)
{
    return S_OK;
}

//
//  OnFlush()
//
//  Implementation of IMFSourceReaderCallback::OnFlush()
//
HRESULT MFSampleGenerator::OnFlush(_In_ DWORD dwStreamIndex)
{
    return S_OK;
}

//
//  OnReadSample()
//
//  Implementation of IMFSourceReaderCallback::OnReadSample().  When a sample is ready, add it to the sample queue.
//
HRESULT MFSampleGenerator::OnReadSample(_In_ HRESULT hrStatus, _In_ DWORD dwStreamIndex, _In_ DWORD dwStreamFlags, _In_ LONGLONG llTimestamp, _In_opt_ IMFSample *pSample)
{
    HRESULT hr = S_OK;

    if ( (m_ReaderState != ReaderStatePlaying) &&
         (m_ReaderState != ReaderStatePreRoll) )
        return S_OK;

    // If we have a failure, change in stream format, or hit EOF, then we stop reading samples
    if ( (FAILED( hrStatus )) ||
         (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM) ||
         (dwStreamFlags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED) )
    {
        m_ReaderState = ReaderStateEOS;
        return S_OK;
    }

    // Add the data from the Media Sample to our buffer queue
    hr = AddSamplesToQueue( pSample );
    if (SUCCEEDED( hr ))
    {
        // Pre-roll PREROLL_DURATION seconds worth of data
        if (m_ReaderState == ReaderStatePreRoll)
        {
            if (IsPreRollFilled())
            {
                // Once Pre-roll is filled, audio endpoint will stop rendering silence and start
                // picking up data from the queue
                m_ReaderState = ReaderStatePlaying;
            }
        }

        // Call ReadSample for next asynchronous sample event
        hr = m_MFSourceReader->ReadSample( MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, nullptr, nullptr, nullptr );
    }

    return S_OK;
}

//
//  FillSampleBuffer()
//
//  Called by the audio endpoint to get data to render, and will set cbWritten to valid number of
//  bytes in the buffer.  The buffers are zeroed out when initialized so the endpoint could feasibly
//  treat the data as silence. If there are no more samples to render, will return S_FALSE.
//
HRESULT MFSampleGenerator::FillSampleBuffer( UINT32 BytesToRead, BYTE *Data, UINT32 *cbWritten )
{
    HRESULT hr = S_OK;
    *cbWritten = 0;

    if (nullptr == Data)
    {
        return E_POINTER;
    }

    if (!m_IsInitialized)
    {
        return E_UNEXPECTED;
    }

    // Not going to return any data here while in a pre-roll state.
    if (m_ReaderState == ReaderStatePreRoll)
        return S_OK;

    RenderBuffer *SampleBuffer = m_SampleQueue;
    if (nullptr == SampleBuffer)
    {
        if (m_ReaderState == ReaderStateEOS)
        {
            // We are EOS, should be set from OnReadSample() and the client should check for EOS before
            // calling FillSampleBuffer()
            hr = S_FALSE;
        }
        else
        {
            // There shouldn't be a situation where we get here unless the reader has
            // somehow gotten into the wrong state
            hr = E_FAIL;
        }
    }
    else
    {
        if (SampleBuffer->BufferSize > BytesToRead)
        {
            // We don't have enough data in the client audio buffer this pass
            // Do nothing and wait for the next sample period
            hr = S_OK;
        }
        else
        {
            CopyMemory( Data, SampleBuffer->Buffer, SampleBuffer->BufferSize );

            *cbWritten = SampleBuffer->BufferSize;
            m_SampleQueue = m_SampleQueue->Next;
            hr = S_OK;
        }
    }

    return hr;
}

//
//  AddSamplesToBuffer()
//
//  Data is added to the end of the sample queue
//
HRESULT MFSampleGenerator::AddSamplesToQueue( IMFSample *MFSample )
{
    if (nullptr == MFSample)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;
    IMFMediaBuffer *MediaBuffer = nullptr;
    BYTE *AudioData = nullptr;
    DWORD cbAudioData = 0;

    // Since we are storing the raw byte data, convert this to a single buffer
    hr = MFSample->ConvertToContiguousBuffer( &MediaBuffer );
    if (FAILED( hr ))
    {
        goto exit;
    }

    // Lock the sample
    hr = MediaBuffer->Lock( &AudioData, NULL, &cbAudioData );
    if ( (FAILED( hr )) || (cbAudioData == 0) )
    {
        goto exit;
    }

    DWORD cbBytesCopied = 0;

    // First fill up any partial buffers from the previous sample
    if ( (*m_SampleQueueTail != nullptr) &&
         ((*m_SampleQueueTail)->BytesFilled < m_BytesPerPeriod) )
    {
        RenderBuffer *PartialBuffer = *m_SampleQueueTail;

        DWORD cbBytesRemaining = m_BytesPerPeriod - PartialBuffer->BytesFilled;
        DWORD cbBytesToCopy = min( cbAudioData, cbBytesRemaining );

        CopyMemory( PartialBuffer->Buffer + PartialBuffer->BytesFilled, AudioData, cbBytesToCopy );

        PartialBuffer->BytesFilled += cbBytesToCopy;
        cbBytesCopied += cbBytesToCopy;

        // Move the tail pointer only if we have filled the sample
        if (PartialBuffer->BytesFilled == m_BytesPerPeriod)
        {
            m_SampleQueueTail = &PartialBuffer->Next;
        }
    }

    // Create buffers for the remainder of the samples
    while( cbBytesCopied < cbAudioData )
    {
        RenderBuffer *SampleBuffer = new (std::nothrow) RenderBuffer();
        if (nullptr == SampleBuffer)
        {
            hr = E_OUTOFMEMORY;
            goto exit;
        }

        DWORD cbBytesRemaining = cbAudioData - cbBytesCopied;
        DWORD cbBufferLengthInBytes = min( m_BytesPerPeriod, cbBytesRemaining );

        SampleBuffer->Buffer = new (std::nothrow) BYTE[ m_BytesPerPeriod ];
        if (nullptr == SampleBuffer->Buffer)
        {
            SAFE_DELETE( SampleBuffer );
            hr = E_OUTOFMEMORY;
            goto exit;
        }

        SampleBuffer->BufferSize = m_BytesPerPeriod;
        SampleBuffer->BytesFilled = 0;

        ZeroMemory( SampleBuffer->Buffer, m_BytesPerPeriod );
        CopyMemory( SampleBuffer->Buffer, AudioData + cbBytesCopied, cbBufferLengthInBytes );

        SampleBuffer->BytesFilled += cbBufferLengthInBytes;
        cbBytesCopied += cbBufferLengthInBytes;

        *m_SampleQueueTail = SampleBuffer;

        // Move the tail pointer only if we have filled the sample otherwise will get filled the
        // next time we add sample data to the queue
        if (SampleBuffer->BytesFilled == m_BytesPerPeriod)
        {
            m_SampleQueueTail = &SampleBuffer->Next;
        }
    }

exit:
    // Unlock the buffer
    hr = MediaBuffer->Unlock();
    AudioData = nullptr;

    SAFE_RELEASE( MediaBuffer );
    return hr;
}

//
//  IsPreRollFilled()
//
//  Loops through the queue quickly to see if our pre-roll buffer is filled
//
Platform::Boolean MFSampleGenerator::IsPreRollFilled()
{
    DWORD TotalBufferSize = 0;

    RenderBuffer *SampleBuffer = m_SampleQueue;
    while( SampleBuffer != nullptr )
    {
        TotalBufferSize += SampleBuffer->BufferSize;

        // For uncompressed formats, nAvgBytesPerSec should equal nSamplesPerSecond * nBlockAlign
        if (TotalBufferSize >= (m_MixFormat->nAvgBytesPerSec * PREROLL_DURATION_SEC))
        {
            return true;
        }

        SampleBuffer = SampleBuffer->Next;
    }

    return false;
}
