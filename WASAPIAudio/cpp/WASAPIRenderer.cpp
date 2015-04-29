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
// wasapirenderer.h
//

#include "pch.h"
#include "WASAPIRenderer.h"

using namespace Windows::System::Threading;
using namespace SDKSample::WASAPIAudio;

//
//  WASAPIRenderer()
//
WASAPIRenderer::WASAPIRenderer() :
    m_BufferFrames( 0 ),
    m_DeviceStateChanged( nullptr ),
    m_AudioClient( nullptr ),
    m_AudioRenderClient( nullptr ),
    m_SampleReadyAsyncResult( nullptr ),
    m_ToneSource( nullptr ),
    m_MFSource( nullptr )
{
    // Create events for sample ready or user stop
    m_SampleReadyEvent = CreateEventEx( nullptr, nullptr, 0, EVENT_ALL_ACCESS );
    if (nullptr == m_SampleReadyEvent)
    {
        ThrowIfFailed( HRESULT_FROM_WIN32( GetLastError() ) );
    }

    if (!InitializeCriticalSectionEx( &m_CritSec, 0, 0 ))
    {
        ThrowIfFailed( HRESULT_FROM_WIN32( GetLastError() ) );
    }

    m_DeviceStateChanged = ref new DeviceStateChangedEvent();
    if (nullptr == m_DeviceStateChanged)
    {
        ThrowIfFailed( E_OUTOFMEMORY );
    }
}

//
//  ~WASAPIRenderer()
//
WASAPIRenderer::~WASAPIRenderer()
{
    SAFE_RELEASE( m_AudioClient );
    SAFE_RELEASE( m_AudioRenderClient );
    SAFE_RELEASE( m_SampleReadyAsyncResult );
    SAFE_RELEASE( m_MFSource );
    SAFE_DELETE( m_ToneSource );
    
    if (INVALID_HANDLE_VALUE != m_SampleReadyEvent)
    {
        CloseHandle( m_SampleReadyEvent );
        m_SampleReadyEvent = INVALID_HANDLE_VALUE;
    }

    DeleteCriticalSection( &m_CritSec );

    m_DeviceStateChanged = nullptr;
}

//
//  InitializeAudioDeviceAsync()
//
//  Activates the default audio renderer on a asynchronous callback thread.  This needs
//  to be called from the main UI thread.
//
HRESULT WASAPIRenderer::InitializeAudioDeviceAsync()
{
    IActivateAudioInterfaceAsyncOperation *asyncOp;
    HRESULT hr = S_OK;

    // Get a string representing the Default Audio Device Renderer
    m_DeviceIdString = MediaDevice::GetDefaultAudioRenderId( Windows::Media::Devices::AudioDeviceRole::Default );

   // This call must be made on the main UI thread.  Async operation will call back to 
    // IActivateAudioInterfaceCompletionHandler::ActivateCompleted, which must be an agile interface implementation
    hr = ActivateAudioInterfaceAsync( m_DeviceIdString->Data(), __uuidof(IAudioClient3), nullptr, this, &asyncOp );
    if (FAILED( hr ))
    {
        m_DeviceStateChanged->SetState( DeviceState::DeviceStateInError, hr, true );
    }

    SAFE_RELEASE( asyncOp );

    return hr;
}

//
//  ActivateCompleted()
//
//  Callback implementation of ActivateAudioInterfaceAsync function.  This will be called on MTA thread
//  when results of the activation are available.
//
HRESULT WASAPIRenderer::ActivateCompleted( IActivateAudioInterfaceAsyncOperation *operation )
{
    HRESULT hr = S_OK;
    HRESULT hrActivateResult = S_OK;
    IUnknown *punkAudioInterface = nullptr;

    if (m_DeviceStateChanged->GetState() != DeviceState::DeviceStateUnInitialized)
    {
        hr = E_NOT_VALID_STATE;
        goto exit;
    }

    // Check for a successful activation result
    hr = operation->GetActivateResult( &hrActivateResult, &punkAudioInterface );
    if (SUCCEEDED( hr ) && SUCCEEDED( hrActivateResult ))
    {
        m_DeviceStateChanged->SetState( DeviceState::DeviceStateActivated, S_OK, false );

        // Get the pointer for the Audio Client
        punkAudioInterface->QueryInterface( IID_PPV_ARGS(&m_AudioClient) );
        if( nullptr == m_AudioClient )
        {
            hr = E_FAIL;
            goto exit;
        }

        // Configure user defined properties
        hr = ConfigureDeviceInternal();
        if (FAILED( hr ))
        {
            goto exit;
        }

        // Initialize the AudioClient in Shared Mode with the user specified buffer
        if (m_DeviceProps.IsLowLatency == false)
        {
            hr = m_AudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
                AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST,
                m_DeviceProps.hnsBufferDuration,
                m_DeviceProps.hnsBufferDuration,
                m_MixFormat,
                nullptr);
        }
        else
        {
            hr = m_AudioClient->InitializeSharedAudioStream(
                AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                m_MinPeriodInFrames,
                m_MixFormat,
                nullptr);
        }
        if (FAILED( hr ))
        {
            goto exit;
        }

        // Get the maximum size of the AudioClient Buffer
        hr = m_AudioClient->GetBufferSize( &m_BufferFrames );
        if (FAILED( hr ))
        {
            goto exit;
        }

        // Get the render client
        hr = m_AudioClient->GetService( __uuidof(IAudioRenderClient), (void**) &m_AudioRenderClient );
        if (FAILED( hr ))
        {
            goto exit;
        }

        // Create Async callback for sample events
        hr = MFCreateAsyncResult( nullptr, &m_xSampleReady, nullptr, &m_SampleReadyAsyncResult );
        if (FAILED( hr ))
        {
            goto exit;
        }

        // Sets the event handle that the system signals when an audio buffer is ready to be processed by the client
        hr = m_AudioClient->SetEventHandle( m_SampleReadyEvent );
        if (FAILED( hr ))
        {
            goto exit;
        }

        // Everything succeeded
        m_DeviceStateChanged->SetState( DeviceState::DeviceStateInitialized, S_OK, true );

    }

exit:
    SAFE_RELEASE( punkAudioInterface );

    if (FAILED( hr ))
    {
        m_DeviceStateChanged->SetState( DeviceState::DeviceStateInError, hr, true );
        SAFE_RELEASE( m_AudioClient );
        SAFE_RELEASE( m_AudioRenderClient );
        SAFE_RELEASE( m_SampleReadyAsyncResult );
    }
    
    // Need to return S_OK
    return S_OK;
}

//
//  SetProperties()
//
//  Sets various properties that the user defines in the scenario
//
HRESULT WASAPIRenderer::SetProperties( DEVICEPROPS props )
{
    m_DeviceProps = props;
    return S_OK;
}

//
//  GetBufferFramesPerPeriod()
//
//  Get the time in seconds between passes of the audio device
//
UINT32 WASAPIRenderer::GetBufferFramesPerPeriod()
{
    REFERENCE_TIME defaultDevicePeriod = 0;
    REFERENCE_TIME minimumDevicePeriod = 0;

    if (m_DeviceProps.IsHWOffload)
    {
        return m_BufferFrames;
    }

    // Get the audio device period
    HRESULT hr = m_AudioClient->GetDevicePeriod( &defaultDevicePeriod, &minimumDevicePeriod);
    if (FAILED( hr ))
    {
        return 0;
    }

    double devicePeriodInSeconds;
    
    if (m_DeviceProps.IsLowLatency)
    {
        devicePeriodInSeconds = minimumDevicePeriod / (10000.0*1000.0);
    }
    else
    {
        devicePeriodInSeconds = defaultDevicePeriod / (10000.0*1000.0);
    }
    

    return static_cast<UINT32>( m_MixFormat->nSamplesPerSec * devicePeriodInSeconds + 0.5 );
}

//
//  ConfigureDeviceInternal()
//
//  Sets additional playback parameters and opts into hardware offload
//
HRESULT WASAPIRenderer::ConfigureDeviceInternal()
{
    if (m_DeviceStateChanged->GetState() != DeviceState::DeviceStateActivated)
    {
        return E_NOT_VALID_STATE;
    }

    HRESULT hr = S_OK;

    // Opt into HW Offloading.  If the endpoint does not support offload it will return AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE
    AudioClientProperties audioProps = {0};
    audioProps.cbSize = sizeof( AudioClientProperties );
    audioProps.bIsOffload = m_DeviceProps.IsHWOffload;
    audioProps.eCategory = AudioCategory_Media;

    if (m_DeviceProps.IsRawChosen && m_DeviceProps.IsRawSupported)
    {
        audioProps.Options = AUDCLNT_STREAMOPTIONS_RAW;
    }

    hr = m_AudioClient->SetClientProperties( &audioProps );
    if (FAILED( hr ))
    {
        return hr;
    }

    // This sample opens the device is shared mode so we need to find the supported WAVEFORMATEX mix format
    hr = m_AudioClient->GetMixFormat( &m_MixFormat );
    if (FAILED( hr ))
    {
        return hr;
    }

   // The wfx parameter below is optional (Its needed only for MATCH_FORMAT clients). Otherwise, wfx will be assumed 
   // to be the current engine format based on the processing mode for this stream
   hr = m_AudioClient->GetSharedModeEnginePeriod(m_MixFormat, &m_DefaultPeriodInFrames, &m_FundamentalPeriodInFrames, &m_MinPeriodInFrames, &m_MaxPeriodInFrames);
   if (FAILED( hr ))
   {
      return hr;
   }

    // Verify the user defined value for hardware buffer
    hr = ValidateBufferValue();

    return hr;
}

//
//  ValidateBufferValue()
//
//  Verifies the user specified buffer value for hardware offload
//
HRESULT WASAPIRenderer::ValidateBufferValue()
{
    HRESULT hr = S_OK;

    if (!m_DeviceProps.IsHWOffload)
    {
        // If we aren't using HW Offload, set this to 0 to use the default value
        m_DeviceProps.hnsBufferDuration = 0;
        return hr;
    }

    REFERENCE_TIME hnsMinBufferDuration;
    REFERENCE_TIME hnsMaxBufferDuration;

    hr = m_AudioClient->GetBufferSizeLimits( m_MixFormat, true, &hnsMinBufferDuration, &hnsMaxBufferDuration );
    if (SUCCEEDED( hr ))
    {
        if (m_DeviceProps.hnsBufferDuration < hnsMinBufferDuration)
        {
            // using MINIMUM size instead
            m_DeviceProps.hnsBufferDuration = hnsMinBufferDuration;
        }
        else if (m_DeviceProps.hnsBufferDuration > hnsMaxBufferDuration)
        {
            // using MAXIMUM size instead
            m_DeviceProps.hnsBufferDuration = hnsMaxBufferDuration;
        }
    }

    return hr;
}

//
//  SetVolumeOnSession()
//
HRESULT WASAPIRenderer::SetVolumeOnSession( UINT32 volume )
{
    if (volume > 100)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;
    ISimpleAudioVolume *SessionAudioVolume = nullptr;
    float ChannelVolume = 0.0;

    hr = m_AudioClient->GetService( __uuidof(ISimpleAudioVolume), reinterpret_cast<void**>(&SessionAudioVolume) );
    if (FAILED( hr ))
    {
        goto exit;
    }

    ChannelVolume = volume / (float)100.0;

    // Set the session volume on the endpoint
    hr = SessionAudioVolume->SetMasterVolume( ChannelVolume, nullptr );

exit:
    SAFE_RELEASE( SessionAudioVolume );
    return hr;
}

//
//  ConfigureSource()
//
//  Configures tone or file playback
//
HRESULT WASAPIRenderer::ConfigureSource()
{
    HRESULT hr = S_OK;
    UINT32 FramesPerPeriod = GetBufferFramesPerPeriod();

    if (m_DeviceProps.IsTonePlayback)
    {
        // Generate the sine wave sample buffer
        m_ToneSource = new ToneSampleGenerator();
        if (m_ToneSource)
        {
            hr = m_ToneSource->GenerateSampleBuffer( m_DeviceProps.Frequency, FramesPerPeriod, m_MixFormat );
        }
        else
        {
            hr = E_OUTOFMEMORY;
        }
    }
    else
    {
        m_MFSource = new MFSampleGenerator();
        if (m_MFSource)
        {
            hr = m_MFSource->Initialize( m_DeviceProps.ContentStream, FramesPerPeriod, m_MixFormat );
        }
        else
        {
            hr = E_OUTOFMEMORY;
        }
    }

    return hr;
}

//
//  StartPlaybackAsync()
//
//  Starts asynchronous playback on a separate thread via MF Work Item
//
HRESULT WASAPIRenderer::StartPlaybackAsync()
{
    HRESULT hr = S_OK;

    // We should be stopped if the user stopped playback, or we should be
    // initialzied if this is the first time through getting ready to playback.
    if ( (m_DeviceStateChanged->GetState() == DeviceState::DeviceStateStopped) ||
         (m_DeviceStateChanged->GetState() == DeviceState::DeviceStateInitialized) )
    {
        // Setup either ToneGeneration or File Playback
        hr = ConfigureSource();
        if (FAILED( hr ))
        {
            m_DeviceStateChanged->SetState( DeviceState::DeviceStateInError, hr, true );
            return hr;
        }

        m_DeviceStateChanged->SetState( DeviceState::DeviceStateStarting, S_OK, true );
        return MFPutWorkItem2( MFASYNC_CALLBACK_QUEUE_MULTITHREADED, 0, &m_xStartPlayback, nullptr );
    }
    else if (m_DeviceStateChanged->GetState() == DeviceState::DeviceStatePaused)
    {
        return MFPutWorkItem2( MFASYNC_CALLBACK_QUEUE_MULTITHREADED, 0, &m_xStartPlayback, nullptr );
    }

    // Otherwise something else happened
    return E_FAIL;
}

//
//  OnStartPlayback()
//
//  Callback method to start playback
//
HRESULT WASAPIRenderer::OnStartPlayback( IMFAsyncResult* pResult )
{
    HRESULT hr = S_OK;

    // Pre-Roll the buffer with silence
    hr = OnAudioSampleRequested( true );
    if (FAILED( hr ))
    {
        goto exit;
    }

    // For MF Source playback, need to start the source reader
    if (!m_DeviceProps.IsTonePlayback)
    {
        hr = m_MFSource->StartSource();
        if (FAILED( hr ))
        {
            goto exit;
        }
    }

    // Actually start the playback
    hr = m_AudioClient->Start();
    if (SUCCEEDED( hr ))
    {
        m_DeviceStateChanged->SetState( DeviceState::DeviceStatePlaying, S_OK, true );
        hr = MFPutWaitingWorkItem( m_SampleReadyEvent, 0, m_SampleReadyAsyncResult, &m_SampleReadyKey );
    }

exit:
    if (FAILED( hr ))
    {
        m_DeviceStateChanged->SetState( DeviceState::DeviceStateInError, hr, true );
    }

    return S_OK;
}

//
//  StopPlaybackAsync()
//
//  Stop playback asynchronously via MF Work Item
//
HRESULT WASAPIRenderer::StopPlaybackAsync()
{
    if ( (m_DeviceStateChanged->GetState() != DeviceState::DeviceStatePlaying) &&
         (m_DeviceStateChanged->GetState() != DeviceState::DeviceStatePaused) &&
         (m_DeviceStateChanged->GetState() != DeviceState::DeviceStateInError) )
    {
        return E_NOT_VALID_STATE;
    }

    m_DeviceStateChanged->SetState( DeviceState::DeviceStateStopping, S_OK, true );

    return MFPutWorkItem2( MFASYNC_CALLBACK_QUEUE_MULTITHREADED, 0, &m_xStopPlayback, nullptr );
}

//
//  OnStopPlayback()
//
//  Callback method to stop playback
//
HRESULT WASAPIRenderer::OnStopPlayback( IMFAsyncResult* pResult )
{
    // Stop playback by cancelling Work Item
    // Cancel the queued work item (if any)
    if (0 != m_SampleReadyKey)
    {
        MFCancelWorkItem( m_SampleReadyKey );
        m_SampleReadyKey = 0;
    }

    // Flush anything left in buffer with silence
    OnAudioSampleRequested( true );

    m_AudioClient->Stop();
    SAFE_RELEASE( m_SampleReadyAsyncResult );

    if (m_DeviceProps.IsTonePlayback)
    {
        // Flush remaining buffers
        m_ToneSource->Flush();
    }
    else
    {
        // Stop Source and Flush remaining buffers
        m_MFSource->StopSource();
        m_MFSource->Shutdown();
    }

    m_DeviceStateChanged->SetState( DeviceState::DeviceStateStopped, S_OK, true );
    return S_OK;
}

//
//  PausePlaybackAsync()
//
//  Pause playback asynchronously via MF Work Item
//
HRESULT WASAPIRenderer::PausePlaybackAsync()
{
    if ( (m_DeviceStateChanged->GetState() !=  DeviceState::DeviceStatePlaying) &&
         (m_DeviceStateChanged->GetState() != DeviceState::DeviceStateInError) )
    {
        return E_NOT_VALID_STATE;
    }

    // Change state to stop automatic queueing of samples
    m_DeviceStateChanged->SetState( DeviceState::DeviceStatePausing, S_OK, false );
    return MFPutWorkItem2( MFASYNC_CALLBACK_QUEUE_MULTITHREADED, 0, &m_xPausePlayback, nullptr );

}

//
//  OnPausePlayback()
//
//  Callback method to pause playback
//
HRESULT WASAPIRenderer::OnPausePlayback( IMFAsyncResult* pResult )
{
    m_AudioClient->Stop();
    m_DeviceStateChanged->SetState( DeviceState::DeviceStatePaused, S_OK, true );
    return S_OK;
}

//
//  OnSampleReady()
//
//  Callback method when ready to fill sample buffer
//
HRESULT WASAPIRenderer::OnSampleReady( IMFAsyncResult* pResult )
{
    HRESULT hr = S_OK;

    hr = OnAudioSampleRequested( false );

    if (SUCCEEDED( hr ))
    {
        // Re-queue work item for next sample
        if (m_DeviceStateChanged->GetState() ==  DeviceState::DeviceStatePlaying)
        {
            hr = MFPutWaitingWorkItem( m_SampleReadyEvent, 0, m_SampleReadyAsyncResult, &m_SampleReadyKey );
        }
    }
    else
    {
        m_DeviceStateChanged->SetState( DeviceState::DeviceStateInError, hr, true );
    }

    return hr;
}

//
//  OnAudioSampleRequested()
//
//  Called when audio device fires m_SampleReadyEvent
//
HRESULT WASAPIRenderer::OnAudioSampleRequested( Platform::Boolean IsSilence )
{
    HRESULT hr = S_OK;
    UINT32 PaddingFrames = 0;
    UINT32 FramesAvailable = 0;

    EnterCriticalSection( &m_CritSec );

    // Get padding in existing buffer
    hr = m_AudioClient->GetCurrentPadding( &PaddingFrames );
    if (FAILED( hr ))
    {
        goto exit;
    }

    // Audio frames available in buffer
    if (m_DeviceProps.IsHWOffload)
    {
        // In HW mode, GetCurrentPadding returns the number of available frames in the 
        // buffer, so we can just use that directly
        FramesAvailable = PaddingFrames;
    }
    else
    {
        // In non-HW shared mode, GetCurrentPadding represents the number of queued frames
        // so we can subtract that from the overall number of frames we have
        FramesAvailable = m_BufferFrames - PaddingFrames;
    }

    // Only continue if we have buffer to write data
    if (FramesAvailable > 0)
    {
        if (IsSilence)
        {
            BYTE *Data;

            // Fill the buffer with silence
            hr = m_AudioRenderClient->GetBuffer( FramesAvailable, &Data );
            if (FAILED( hr ))
            {
                goto exit;
            }

            hr = m_AudioRenderClient->ReleaseBuffer( FramesAvailable, AUDCLNT_BUFFERFLAGS_SILENT );
            goto exit;
        }

        // Even if we cancel a work item, this may still fire due to the async
        // nature of things.  There should be a queued work item already to handle
        // the process of stopping or stopped
        if (m_DeviceStateChanged->GetState() == DeviceState::DeviceStatePlaying)
        {
            // Fill the buffer with a playback sample
            if (m_DeviceProps.IsTonePlayback)
            {
                hr = GetToneSample( FramesAvailable );
            }
            else
            {
                hr = GetMFSample( FramesAvailable );
            }
        }
    }

exit:
    LeaveCriticalSection( &m_CritSec );

    if (AUDCLNT_E_RESOURCES_INVALIDATED == hr)
    {
        m_DeviceStateChanged->SetState( DeviceState::DeviceStateUnInitialized, hr, false );
        SAFE_RELEASE( m_AudioClient );
        SAFE_RELEASE( m_AudioRenderClient );
        SAFE_RELEASE( m_SampleReadyAsyncResult );

        hr = InitializeAudioDeviceAsync();
    }

    return hr;
}

//
//  GetToneSample()
//
//  Fills buffer with a tone sample
//
HRESULT WASAPIRenderer::GetToneSample( UINT32 FramesAvailable )
{
    HRESULT hr = S_OK;
    BYTE *Data;

    // Post-Roll Silence
    if (m_ToneSource->IsEOF())
    {
        hr = m_AudioRenderClient->GetBuffer( FramesAvailable, &Data );
        if (SUCCEEDED( hr ))
        {
            // Ignore the return
            hr = m_AudioRenderClient->ReleaseBuffer( FramesAvailable, AUDCLNT_BUFFERFLAGS_SILENT );
        }

        StopPlaybackAsync();
    }
    else if (m_ToneSource->GetBufferLength() <= ( FramesAvailable * m_MixFormat->nBlockAlign ))
    {
        UINT32 ActualFramesToRead = m_ToneSource->GetBufferLength() / m_MixFormat->nBlockAlign;
        UINT32 ActualBytesToRead = ActualFramesToRead * m_MixFormat->nBlockAlign;

        hr = m_AudioRenderClient->GetBuffer( ActualFramesToRead, &Data );
        if (SUCCEEDED( hr ))
        {
            hr = m_ToneSource->FillSampleBuffer( ActualBytesToRead, Data );
            if (SUCCEEDED( hr ))
            {
                hr = m_AudioRenderClient->ReleaseBuffer( ActualFramesToRead, 0 );
            }
        }
    }

    return hr;
}

//
//  GetMFSample()
//
//  Fills buffer with a MF sample
//
HRESULT WASAPIRenderer::GetMFSample( UINT32 FramesAvailable )
{
    HRESULT hr = S_OK;
    BYTE *Data = nullptr;

    // Post-Roll Silence
    if (m_MFSource->IsEOF())
    {
        hr = m_AudioRenderClient->GetBuffer( FramesAvailable, &Data );
        if (SUCCEEDED( hr ))
        {
            // Ignore the return
            hr = m_AudioRenderClient->ReleaseBuffer( FramesAvailable, AUDCLNT_BUFFERFLAGS_SILENT );
        }

        StopPlaybackAsync();
    }
    else
    {
        UINT32 ActualBytesRead = 0;
        UINT32 ActualBytesToRead = FramesAvailable * m_MixFormat->nBlockAlign;

        hr = m_AudioRenderClient->GetBuffer( FramesAvailable, &Data );
        if (SUCCEEDED( hr ))
        {
            hr = m_MFSource->FillSampleBuffer( ActualBytesToRead, Data, &ActualBytesRead );
            if (hr == S_FALSE)
            {
                // Hit EOS
                hr = m_AudioRenderClient->ReleaseBuffer( FramesAvailable, AUDCLNT_BUFFERFLAGS_SILENT );
                StopPlaybackAsync();
            }
            else if (SUCCEEDED( hr ))
            {
                // This can happen if we are pre-rolling so just insert silence
                if (0 == ActualBytesRead)
                {
                    hr = m_AudioRenderClient->ReleaseBuffer( FramesAvailable, AUDCLNT_BUFFERFLAGS_SILENT );
                }
                else
                {
                    hr = m_AudioRenderClient->ReleaseBuffer( ( ActualBytesRead / m_MixFormat->nBlockAlign ), 0 );
                }
            }
        }
    }

    return hr;
}

