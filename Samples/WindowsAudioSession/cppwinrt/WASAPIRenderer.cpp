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
#include "WASAPIRenderer.h"

using namespace winrt::Windows::Media::Devices;

namespace winrt::SDKTemplate
{
    //
    //  InitializeAudioDeviceAsync()
    //
    //  Activates the default audio renderer on a asynchronous callback thread.
    //
    void WASAPIRenderer::AsyncInitializeAudioDevice() noexcept try
    {
        // Get a string representing the Default Audio Device Renderer
        m_DeviceIdString = MediaDevice::GetDefaultAudioRenderId(AudioDeviceRole::Default);

        // This call can be made safely from a background thread because we are asking for the IAudioClient3
        // interface of an audio device. Async operation will call back to 
        // IActivateAudioInterfaceCompletionHandler::ActivateCompleted, which must be an agile interface implementation
        com_ptr<IActivateAudioInterfaceAsyncOperation> asyncOp;
        check_hresult(ActivateAudioInterfaceAsync(m_DeviceIdString.c_str(), __uuidof(IAudioClient3), nullptr, this, asyncOp.put()));
    }
    catch (...)
    {
        SetState(DeviceState::Error, to_hresult());
    }

    //
    //  ActivateCompleted()
    //
    //  Callback implementation of ActivateAudioInterfaceAsync function.  This will be called on MTA thread
    //  when results of the activation are available.
    //
    HRESULT WASAPIRenderer::ActivateCompleted(IActivateAudioInterfaceAsyncOperation* operation) noexcept try
    {
        if (m_deviceState != DeviceState::Uninitialized)
        {
            throw hresult_error(E_NOT_VALID_STATE);
        }

        // Check for a successful activation result
        HRESULT hrActivateResult = S_OK;
        com_ptr<::IUnknown> punkAudioInterface;
        check_hresult(operation->GetActivateResult(&hrActivateResult, punkAudioInterface.put()));
        check_hresult(hrActivateResult);

        // Remember that we have been activated, but don't raise the event yet.
        SetStateNoNotify(DeviceState::Activated);

        // Get the pointer for the Audio Client
        m_AudioClient = punkAudioInterface.as<IAudioClient3>();

        // Configure user defined properties
        check_hresult(ConfigureDeviceInternal());

        // Initialize the AudioClient in Shared Mode with the user specified buffer
        if (!m_IsLowLatency || m_IsHWOffload)
        {
            check_hresult(m_AudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
                AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                m_hnsBufferDuration,
                m_hnsBufferDuration,
                m_MixFormat.get(),
                nullptr));
        }
        else
        {
            check_hresult(m_AudioClient->InitializeSharedAudioStream(
                AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                m_MinPeriodInFrames,
                m_MixFormat.get(),
                nullptr));
        }

        // Get the maximum size of the AudioClient Buffer
        check_hresult(m_AudioClient->GetBufferSize(&m_BufferFrames));

        // Get the render client
        m_AudioRenderClient.capture(m_AudioClient, &IAudioClient::GetService);

        // Create Async callback for sample events
        check_hresult(MFCreateAsyncResult(nullptr, &m_SampleReadyCallback, nullptr, m_SampleReadyAsyncResult.put()));

        // Sets the event handle that the system signals when an audio buffer is ready to be processed by the client
        check_hresult(m_AudioClient->SetEventHandle(m_SampleReadyEvent.get()));

        // Everything succeeded
        SetState(DeviceState::Initialized, S_OK);

        return S_OK;
    }
    catch (...)
    {
        SetState(DeviceState::Error, to_hresult());
        m_AudioClient = nullptr;
        m_AudioRenderClient = nullptr;
        m_SampleReadyAsyncResult = nullptr;

        // Must return S_OK even on failure.
        return S_OK;
    }

    //
    //  GetBufferFramesPerPeriod()
    //
    //  Get the time in seconds between passes of the audio device
    //
    UINT32 WASAPIRenderer::GetBufferFramesPerPeriod() noexcept
    {
        REFERENCE_TIME defaultDevicePeriod = 0;
        REFERENCE_TIME minimumDevicePeriod = 0;

        if (m_IsHWOffload)
        {
            return m_BufferFrames;
        }

        if (m_IsLowLatency)
        {
            return m_MinPeriodInFrames;
        }

        // Get the audio device period
        HRESULT hr = m_AudioClient->GetDevicePeriod(&defaultDevicePeriod, &minimumDevicePeriod);
        if (FAILED(hr))
        {
            return 0;
        }

        double devicePeriodInSeconds = defaultDevicePeriod / (10000.0 * 1000.0);
        return static_cast<UINT32>(m_MixFormat->nSamplesPerSec * devicePeriodInSeconds + 0.5);
    }

    //
    //  ConfigureDeviceInternal()
    //
    //  Sets additional playback parameters and opts into hardware offload
    //
    HRESULT WASAPIRenderer::ConfigureDeviceInternal() noexcept try
    {
        if (m_deviceState != DeviceState::Activated)
        {
            return E_NOT_VALID_STATE;
        }

        // Opt into HW Offloading.  If the endpoint does not support offload it will return AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE
        AudioClientProperties audioProps = { 0 };
        audioProps.cbSize = sizeof(AudioClientProperties);
        audioProps.bIsOffload = m_IsHWOffload;
        audioProps.eCategory = AudioCategory_Media;

        if (m_IsRawAudio)
        {
            audioProps.Options = AUDCLNT_STREAMOPTIONS_RAW;
        }

        check_hresult(m_AudioClient->SetClientProperties(&audioProps));

        // If application already has a preferred source format available,
        // it can test whether the format is supported by the device:
        //
        // unique_cotaskmem_ptr<WAVEFORMATEX> applicationFormat = { ... };
        // if (S_OK == m_AudioClient->IsFormatSupported(applicationFormat.get()))
        // {
        //     m_MixFormat = std::move(applicationFormat);
        // }
        // else
        // {
        //     // device does not support the application format, so ask the device what format it prefers
        //     check_hresult(m_AudioClient->GetMixFormat(&m_MixFormat.put()));
        // }

        // This sample opens the device is shared mode so we need to find the supported WAVEFORMATEX mix format
        check_hresult(m_AudioClient->GetMixFormat(m_MixFormat.put()));

        if (!audioProps.bIsOffload)
        {
            // The wfx parameter below is optional (Its needed only for MATCH_FORMAT clients). Otherwise, wfx will be assumed
            // to be the current engine format based on the processing mode for this stream
            check_hresult(m_AudioClient->GetSharedModeEnginePeriod(m_MixFormat.get(), &m_DefaultPeriodInFrames, &m_FundamentalPeriodInFrames, &m_MinPeriodInFrames, &m_MaxPeriodInFrames));
        }

        // Verify the user defined value for hardware buffer
        ValidateBufferValue();

        return S_OK;
    }
    catch (...) { return to_hresult(); }

    //
    //  ValidateBufferValue()
    //
    //  Verifies the user specified buffer value for hardware offload
    //  Throws an exception on failure.
    //
    void WASAPIRenderer::ValidateBufferValue()
    {
        if (!m_IsHWOffload)
        {
            // If we aren't using HW Offload, set this to 0 to use the default value
            m_hnsBufferDuration = 0;
            return;
        }

        REFERENCE_TIME hnsMinBufferDuration;
        REFERENCE_TIME hnsMaxBufferDuration;

        check_hresult(m_AudioClient->GetBufferSizeLimits(m_MixFormat.get(), true, &hnsMinBufferDuration, &hnsMaxBufferDuration));
        if (m_hnsBufferDuration < hnsMinBufferDuration)
        {
            // using MINIMUM size instead
            m_hnsBufferDuration = hnsMinBufferDuration;
        }
        else if (m_hnsBufferDuration > hnsMaxBufferDuration)
        {
            // using MAXIMUM size instead
            m_hnsBufferDuration = hnsMaxBufferDuration;
        }
    }

    HRESULT WASAPIRenderer::SetVolumeOnSession(double volumePercent) noexcept try
    {
        // Convert [0.100] to [0..1] range and clamp
        m_channelVolume = (volumePercent < 0.0) ? 0.0f : (volumePercent > 100.0) ? 1.0f : (float)(volumePercent / 100.0);

        // Set the session volume on the endpoint if we have one.
        if (m_AudioClient)
        {
            SetAudioClientChannelVolume();
        }

        return S_OK;
    }
    catch (...) { return to_hresult(); }

    void WASAPIRenderer::SetAudioClientChannelVolume()
    {
        check_hresult(capture<ISimpleAudioVolume>(m_AudioClient, &IAudioClient3::GetService)->SetMasterVolume(m_channelVolume, nullptr));
    }


    //
    //  ConfigureSource()
    //
    //  Configures tone or file playback
    //
    void WASAPIRenderer::ConfigureSource()
    {
        UINT32 FramesPerPeriod = GetBufferFramesPerPeriod();

        switch (m_contentType)
        {
        case ContentType::Tone:
            // Generate the sine wave sample buffer
            m_ToneSource = std::make_unique<ToneSampleGenerator>();
            m_ToneSource->GenerateSampleBuffer(m_Frequency, FramesPerPeriod, m_MixFormat.get());
            break;

        case ContentType::File:
            m_MFSource = make_self<MFSampleGenerator>();
            // This will throw if the stream cannot be decoded.
            m_MFSource->Initialize(m_ContentStream, FramesPerPeriod, m_MixFormat.get());
            break;

        default:
            assert(false);
            std::terminate();
        }
    }

    //
    //  StartPlaybackAsync()
    //
    //  Starts asynchronous playback on a separate thread via MF Work Item
    //  Errors are reported via the DeviceStateChanged event.
    //
    void WASAPIRenderer::StartPlayback() noexcept try
    {
        switch (m_deviceState)
        {
        // We should be stopped if the user stopped playback, or we should be
        // initialized if this is the first time through getting ready to playback.
        case DeviceState::Stopped:
        case DeviceState::Initialized:
            // Setup either ToneGeneration or File Playback
            ConfigureSource();

            SetState(DeviceState::Starting, S_OK);
            check_hresult(MFPutWorkItem2(MFASYNC_CALLBACK_QUEUE_MULTITHREADED, 0, &m_StartPlaybackCallback, nullptr));
            break;

        case DeviceState::Paused:
            check_hresult(MFPutWorkItem2(MFASYNC_CALLBACK_QUEUE_MULTITHREADED, 0, &m_StartPlaybackCallback, nullptr));
            break;

        default:
            // Otherwise something else happened
            throw hresult_error(E_FAIL);
        }
    }
    catch (...)
    {
        hresult error = to_hresult();
        SetState(DeviceState::Error, error);
    }

    //
    //  OnStartPlayback()
    //
    //  Callback method to start playback
    //
    HRESULT WASAPIRenderer::OnStartPlayback(IMFAsyncResult*) noexcept try
    {
        // Pre-Roll the buffer with silence
        OnAudioSampleRequested(true);

        // For MF Source playback, need to start the source reader
        if (m_contentType == ContentType::File)
        {
            m_MFSource->StartSource();
        }

        // Set the initial volume.
        SetAudioClientChannelVolume();

        // Actually start the playback
        check_hresult(m_AudioClient->Start());

        SetState(DeviceState::Playing, S_OK);
        check_hresult(MFPutWaitingWorkItem(m_SampleReadyEvent.get(), 0, m_SampleReadyAsyncResult.get(), &m_SampleReadyKey));

        return S_OK;
    }
    catch (...)
    {
        SetState(DeviceState::Error, to_hresult());
        // Must return S_OK.
        return S_OK;
    }

    //
    //  StopPlaybackAsync()
    //
    //  Stop playback asynchronously via MF Work Item
    //
    HRESULT WASAPIRenderer::StopPlaybackAsync() noexcept
    {
        if ((m_deviceState != DeviceState::Playing) &&
            (m_deviceState != DeviceState::Paused) &&
            (m_deviceState != DeviceState::Error))
        {
            return E_NOT_VALID_STATE;
        }

        SetState(DeviceState::Stopping, S_OK);

        return MFPutWorkItem2(MFASYNC_CALLBACK_QUEUE_MULTITHREADED, 0, &m_StopPlaybackCallback, nullptr);
    }

    //
    //  OnStopPlayback()
    //
    //  Callback method to stop playback
    //
    HRESULT WASAPIRenderer::OnStopPlayback(IMFAsyncResult*)
    {
        // Stop playback by cancelling Work Item
        // Cancel the queued work item (if any)
        if (0 != m_SampleReadyKey)
        {
            MFCancelWorkItem(m_SampleReadyKey);
            m_SampleReadyKey = 0;
        }

        // Flush anything left in buffer with silence, best effort.
        try
        {
            OnAudioSampleRequested(true);
        }
        catch (...) { }

        m_AudioClient->Stop();
        m_SampleReadyAsyncResult = nullptr;

        switch (m_contentType)
        {
        case ContentType::Tone:
            // Flush remaining buffers
            m_ToneSource->Flush();
            break;

        case ContentType::File:
            // Stop Source and Flush remaining buffers
            m_MFSource->StopSource();
            m_MFSource->Shutdown();
            break;
        }

        SetState(DeviceState::Stopped);

        return S_OK;
    }

    //
    //  PausePlaybackAsync()
    //
    //  Pause playback asynchronously via MF Work Item
    //
    HRESULT WASAPIRenderer::PausePlaybackAsync()
    {
        if ((m_deviceState != DeviceState::Playing) &&
            (m_deviceState != DeviceState::Error))
        {
            return E_NOT_VALID_STATE;
        }

        // Change state to stop automatic queueing of samples
        // Don't notify clients.
        SetStateNoNotify(DeviceState::Pausing);

        return MFPutWorkItem2(MFASYNC_CALLBACK_QUEUE_MULTITHREADED, 0, &m_PausePlaybackCallback, nullptr);

    }

    //
    //  OnPausePlayback()
    //
    //  Callback method to pause playback
    //
    HRESULT WASAPIRenderer::OnPausePlayback(IMFAsyncResult*)
    {
        m_AudioClient->Stop();
        SetState(DeviceState::Paused, S_OK);

        return S_OK;
    }

    //
    //  OnSampleReady()
    //
    //  Callback method when ready to fill sample buffer
    //
    HRESULT WASAPIRenderer::OnSampleReady(IMFAsyncResult*) try
    {
        OnAudioSampleRequested(false);

        // Re-queue work item for next sample
        if (m_deviceState == DeviceState::Playing)
        {
            check_hresult(MFPutWaitingWorkItem(m_SampleReadyEvent.get(), 0, m_SampleReadyAsyncResult.get(), &m_SampleReadyKey));
        }

        return S_OK;
    }
    catch (...)
    {
        hresult error = to_hresult();
        SetState(DeviceState::Error, error);
        return error;
    }

    //
    //  OnAudioSampleRequested()
    //
    //  Called when audio device fires m_SampleReadyEvent
    //
    void WASAPIRenderer::OnAudioSampleRequested(bool IsSilence) try
    {

        auto guard = slim_lock_guard(m_lock);

        // Get padding in existing buffer
        UINT32 PaddingFrames = 0;
        check_hresult(m_AudioClient->GetCurrentPadding(&PaddingFrames));

        // GetCurrentPadding represents the number of queued frames
        // so we can subtract that from the overall number of frames we have
        uint32_t framesAvailable = m_BufferFrames - PaddingFrames;

        // Only continue if we have buffer to write data
        if (framesAvailable == 0)
        {
            return;
        }

        if (IsSilence)
        {
            // Fill the buffer with silence
            uint8_t* data;
            check_hresult(m_AudioRenderClient->GetBuffer(framesAvailable, &data));
            check_hresult(m_AudioRenderClient->ReleaseBuffer(framesAvailable, AUDCLNT_BUFFERFLAGS_SILENT));
            return;
        }

        // Even if we cancel a work item, this may still fire due to the async
        // nature of things.  There should be a queued work item already to handle
        // the process of stopping or stopped
        if (m_deviceState == DeviceState::Playing)
        {
            // Fill the buffer with a playback sample
            switch (m_contentType)
            {
            case ContentType::Tone:
                GetToneSample(framesAvailable);
                break;

            case ContentType::File:
                GetMFSample(framesAvailable);
                break;
            }
        }
    }
    catch (hresult_error const& error)
    {
        if (error.code() != AUDCLNT_E_RESOURCES_INVALIDATED)
        {
            throw;
        }

        // Attempt auto-recovery from loss of resources.
        SetStateNoNotify(DeviceState::Uninitialized);
        m_AudioClient = nullptr;
        m_AudioRenderClient = nullptr;
        m_SampleReadyAsyncResult = nullptr;

        AsyncInitializeAudioDevice();
    }

    //
    //  GetToneSample()
    //
    //  Fills buffer with a tone sample
    //
    void WASAPIRenderer::GetToneSample(uint32_t framesAvailable)
    {
        uint8_t* data;

        // Post-Roll Silence
        if (m_ToneSource->IsEOF())
        {
            check_hresult(m_AudioRenderClient->GetBuffer(framesAvailable, &data));
            // Ignore the return
            m_AudioRenderClient->ReleaseBuffer(framesAvailable, AUDCLNT_BUFFERFLAGS_SILENT);

            StopPlaybackAsync();
        }
        else if (m_ToneSource->GetBufferLength() <= (framesAvailable * m_MixFormat->nBlockAlign))
        {
            uint32_t actualFramesToRead = m_ToneSource->GetBufferLength() / m_MixFormat->nBlockAlign;
            uint32_t actualBytesToRead = actualFramesToRead * m_MixFormat->nBlockAlign;

            check_hresult(m_AudioRenderClient->GetBuffer(actualFramesToRead, &data));
            if (actualBytesToRead > 0)
            {
                m_ToneSource->FillSampleBuffer({ data, actualBytesToRead });
            }
            check_hresult(m_AudioRenderClient->ReleaseBuffer(actualFramesToRead, 0));
        }
    }

    //
    //  GetMFSample()
    //
    //  Fills buffer with a MF sample
    //
    void WASAPIRenderer::GetMFSample(uint32_t framesAvailable)
    {
        BYTE* data = nullptr;

        // Post-Roll Silence
        if (m_MFSource->IsEOF())
        {
            check_hresult(m_AudioRenderClient->GetBuffer(framesAvailable, &data));
            // Ignore the return
            m_AudioRenderClient->ReleaseBuffer(framesAvailable, AUDCLNT_BUFFERFLAGS_SILENT);

            StopPlaybackAsync();
        }
        else
        {
            uint32_t actualBytesRead = 0;
            uint32_t actualBytesToRead = framesAvailable * m_MixFormat->nBlockAlign;

            check_hresult(m_AudioRenderClient->GetBuffer(framesAvailable, &data));
            if (!m_MFSource->FillSampleBuffer({ data, actualBytesToRead }, &actualBytesRead))
            {
                // Hit EOS
                m_AudioRenderClient->ReleaseBuffer(framesAvailable, AUDCLNT_BUFFERFLAGS_SILENT);
                StopPlaybackAsync();
            }
            else
            {
                // This can happen if we are pre-rolling so just insert silence
                if (0 == actualBytesRead)
                {
                    check_hresult(m_AudioRenderClient->ReleaseBuffer(framesAvailable, AUDCLNT_BUFFERFLAGS_SILENT));
                }
                else
                {
                    check_hresult(m_AudioRenderClient->ReleaseBuffer(actualBytesRead / m_MixFormat->nBlockAlign, 0));
                }
            }
        }
    }
}
