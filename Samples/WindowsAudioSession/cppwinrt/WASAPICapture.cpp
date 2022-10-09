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
#include "WASAPICapture.h"

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Media::Devices;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Storage::Streams;

using namespace std::literals;

namespace winrt::SDKTemplate
{
    static constexpr uint32_t BITS_PER_BYTE = 8;
    static constexpr TimeSpan FLUSH_INTERVAL = 3s;
    static constexpr TimeSpan DURATION_TO_VISUALIZE = 20ms;
    static constexpr wchar_t AUDIO_FILE_NAME[] = L"WASAPIAudioCapture.wav";

    WASAPICapture::WASAPICapture()
    {
        // Set the capture event work queue to use the MMCSS queue
        m_SampleReadyCallback.SetQueueID(m_queueId.get());
    }

    //
    //  InitializeAudioDeviceAsync()
    //
    //  Activates the default audio capture on a asynchronous callback thread.  This needs
    //  to be called from the main UI thread.
    //
    void WASAPICapture::AsyncInitializeAudioDevice() noexcept try
    {
        com_ptr<IActivateAudioInterfaceAsyncOperation> asyncOp;

        // Get a string representing the Default Audio Capture Device
        hstring deviceIdString = MediaDevice::GetDefaultAudioCaptureId(AudioDeviceRole::Default);

        // This call must be made on the main UI thread.  Async operation will call back to 
        // IActivateAudioInterfaceCompletionHandler::ActivateCompleted, which must be an agile interface implementation
        check_hresult(ActivateAudioInterfaceAsync(deviceIdString.c_str(), __uuidof(IAudioClient3), nullptr, this, asyncOp.put()));
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
    HRESULT WASAPICapture::ActivateCompleted(IActivateAudioInterfaceAsyncOperation* operation) try
    {
        HRESULT status = S_OK;
        com_ptr<::IUnknown> punkAudioInterface;

        // Check for a successful activation result
        check_hresult(operation->GetActivateResult(&status, punkAudioInterface.put()));
        check_hresult(status);

        // Get the pointer for the Audio Client
        m_audioClient = punkAudioInterface.as<IAudioClient3>();

        check_hresult(m_audioClient->GetMixFormat(m_mixFormat.put()));

        // convert from Float to 16-bit PCM
        switch (m_mixFormat->wFormatTag)
        {
        case WAVE_FORMAT_PCM:
            // nothing to do
            break;

        case WAVE_FORMAT_IEEE_FLOAT:
            m_mixFormat->wFormatTag = WAVE_FORMAT_PCM;
            m_mixFormat->wBitsPerSample = 16;
            m_mixFormat->nBlockAlign = m_mixFormat->nChannels * m_mixFormat->wBitsPerSample / BITS_PER_BYTE;
            m_mixFormat->nAvgBytesPerSec = m_mixFormat->nSamplesPerSec * m_mixFormat->nBlockAlign;
            break;

        case WAVE_FORMAT_EXTENSIBLE:
        {
            WAVEFORMATEXTENSIBLE* pWaveFormatExtensible = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(m_mixFormat.get());
            if (pWaveFormatExtensible->SubFormat == KSDATAFORMAT_SUBTYPE_PCM)
            {
                // nothing to do
            }
            else if (pWaveFormatExtensible->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
            {
                pWaveFormatExtensible->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
                pWaveFormatExtensible->Format.wBitsPerSample = 16;
                pWaveFormatExtensible->Format.nBlockAlign =
                    pWaveFormatExtensible->Format.nChannels *
                    pWaveFormatExtensible->Format.wBitsPerSample /
                    BITS_PER_BYTE;
                pWaveFormatExtensible->Format.nAvgBytesPerSec =
                    pWaveFormatExtensible->Format.nSamplesPerSec *
                    pWaveFormatExtensible->Format.nBlockAlign;
                pWaveFormatExtensible->Samples.wValidBitsPerSample =
                    pWaveFormatExtensible->Format.wBitsPerSample;

                // leave the channel mask as-is
            }
            else
            {
                // we can only handle float or PCM
                throw hresult_error(HRESULT_FROM_WIN32(ERROR_NOT_FOUND));
            }
            break;
        }

        default:
            // we can only handle float or PCM
            throw hresult_error(HRESULT_FROM_WIN32(ERROR_NOT_FOUND));
            break;
        }

        // The wfx parameter below is optional (Its needed only for MATCH_FORMAT clients). Otherwise, wfx will be assumed 
        // to be the current engine format based on the processing mode for this stream
        check_hresult(m_audioClient->GetSharedModeEnginePeriod(m_mixFormat.get(), &m_defaultPeriodInFrames, &m_fundamentalPeriodInFrames, &m_minPeriodInFrames, &m_maxPeriodInFrames));

        // Initialize the AudioClient in Shared Mode with the user specified buffer
        if (!m_isLowLatency)
        {
            check_hresult(m_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
                AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                TimeSpan{ 20ms }.count(), // hnsBufferDuration
                0,
                m_mixFormat.get(),
                nullptr));
        }
        else
        {
            check_hresult(m_audioClient->InitializeSharedAudioStream(
                AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                m_minPeriodInFrames,
                m_mixFormat.get(),
                nullptr));
        }

        // Get the maximum size of the AudioClient Buffer
        check_hresult(m_audioClient->GetBufferSize(&m_bufferFrames));

        // Get the capture client
        m_audioCaptureClient.capture(m_audioClient, &IAudioClient::GetService);

        // Create Async callback for sample events
        check_hresult(MFCreateAsyncResult(nullptr, &m_SampleReadyCallback, nullptr, m_sampleReadyAsyncResult.put()));

        // Provides the event handle for the system to signal when an audio buffer is ready to be processed by the client
        check_hresult(m_audioClient->SetEventHandle(m_SampleReadyEvent.get()));

        // Create the visualization array
        InitializeScopeData();

        // Creates the WAV file asynchronously.  If successful, will transition to DeviceState::Initialized
        CreateWAVFile();

        // Need to return S_OK
        return S_OK;
    }
    catch (...)
    {
        SetState(DeviceState::Error, to_hresult());
        m_audioClient = nullptr;
        m_audioCaptureClient = nullptr;
        m_sampleReadyAsyncResult = nullptr;

        // Must return S_OK even on failure.
        return S_OK;
    }

    //
    //  CreateWAVFile()
    //
    //  Creates a WAV file in KnownFolders::MusicLibrary
    //
    fire_and_forget WASAPICapture::CreateWAVFile() try
    {
        auto lifetime = get_strong();

        // Create the WAV file, appending a number if file already exists
        StorageFile file = co_await KnownFolders::MusicLibrary().CreateFileAsync(AUDIO_FILE_NAME, CreationCollisionOption::GenerateUniqueName);

        // Then create a RandomAccessStream
        IRandomAccessStream stream = co_await file.OpenAsync(FileAccessMode::ReadWrite);

        // Get the OutputStream for the file
        m_contentStream = stream;
        m_outputStream = stream.GetOutputStreamAt(0);

        // Create the DataWriter
        m_dataWriter = DataWriter(m_outputStream);

        // Create the WAV header
        DWORD formatSize = sizeof(WAVEFORMATEX) + m_mixFormat->cbSize; // Size of fmt chunk
        DWORD header[] = {
            FCC('RIFF'),        // RIFF header
            0,                  // Total size of WAV (will be filled in later)
            FCC('WAVE'),        // WAVE FourCC
            FCC('fmt '),        // Start of 'fmt ' chunk
            formatSize          // Size of fmt chunk
        };

        DWORD data[] = { FCC('data'), 0 };  // Start of 'data' chunk (total size will be filled in later)

        // Write the header
        m_dataWriter.WriteBytes({ reinterpret_cast<uint8_t*>(header), sizeof(header) });
        m_dataWriter.WriteBytes({ reinterpret_cast<uint8_t*>(m_mixFormat.get()), formatSize });
        m_dataWriter.WriteBytes({ reinterpret_cast<uint8_t*>(data), sizeof(data) });

        m_headerSize = co_await m_dataWriter.StoreAsync();
        m_dataSize = 0;
        m_bytesSinceLastFlush = 0;

        // Wait for file data to be written to file
        co_await m_dataWriter.FlushAsync();

        SetState(DeviceState::Initialized);
    }
    catch (...)
    {
        SetState(DeviceState::Error, to_hresult());
    }

    //
    //  FixWAVHeader()
    //
    //  The size values were not known when we originally wrote the header, so now go through and fix the values
    //
    fire_and_forget WASAPICapture::FixWAVHeader() try
    {
        auto lifetime = get_strong();

        // Prepare a buffer to write DWORDs into the stream header.
        Buffer dwordBuffer(sizeof(DWORD));
        dwordBuffer.Length(sizeof(DWORD));
        auto dwordBufferPtr = reinterpret_cast<DWORD*>(dwordBuffer.data());

        IRandomAccessStream stream = m_contentStream.CloneStream();

        // Write the size of the 'data' chunk first
        stream.Seek(m_headerSize - sizeof(DWORD));
        *dwordBufferPtr = m_dataSize;
        co_await stream.WriteAsync(dwordBuffer);

        // Write the total file size, minus RIFF chunk and size
        stream.Seek(sizeof(DWORD)); // sizeof(DWORD) == sizeof(FOURCC)
        *dwordBufferPtr = m_dataSize + m_headerSize - 8; // 8 = RIFF chunk and size
        co_await stream.WriteAsync(dwordBuffer);

        co_await stream.FlushAsync();
        stream.Close();

        SetState(DeviceState::Stopped);
    }
    catch (...)
    {
        SetState(DeviceState::Error, to_hresult());
    }

    //
    //  InitializeScopeData()
    //
    //  Setup data structures for sample visualizations
    //
    void WASAPICapture::InitializeScopeData()
    {
        // Only Support 16 bit Audio for now
        if (m_mixFormat->wBitsPerSample != 16)
        {
            m_plotDataBuffer = nullptr;
            return;
        }

        m_plotDataPointsFilled = 0;
        m_plotDataMaxPoints = static_cast<uint32_t>(DURATION_TO_VISUALIZE * m_mixFormat->nSamplesPerSec / 1s);
        m_plotDataBuffer = Buffer(m_plotDataMaxPoints * sizeof(int16_t));
    }

    //
    //  AsyncStartCapture()
    //
    //  Starts asynchronous capture on a separate thread via MF Work Item
    //
    void WASAPICapture::AsyncStartCapture()
    {
        // We should be in the initialzied state if this is the first time through getting ready to capture.
        if (m_deviceState == DeviceState::Initialized)
        {
            SetState(DeviceState::Starting);
            MFPutWorkItem2(MFASYNC_CALLBACK_QUEUE_MULTITHREADED, 0, &m_StartCaptureCallback, nullptr);
        }
    }

    //
    //  OnStartCapture()
    //
    //  Callback method to start capture
    //
    HRESULT WASAPICapture::OnStartCapture(IMFAsyncResult*) try
    {
        // Start the capture
        check_hresult(m_audioClient->Start());
        check_hresult(MFPutWaitingWorkItem(m_SampleReadyEvent.get(), 0, m_sampleReadyAsyncResult.get(), &m_sampleReadyKey));
        SetState(DeviceState::Capturing);

        return S_OK;
    }
    catch (...)
    {
        SetState(DeviceState::Error, to_hresult());
        // Must return S_OK.
        return S_OK;
    }

    //
    //  AsyncStopCapture()
    //
    //  Stop capture asynchronously via MF Work Item
    //
    void WASAPICapture::AsyncStopCapture()
    {
        if ((m_deviceState == DeviceState::Capturing) ||
            (m_deviceState == DeviceState::Error))
        {
            SetState(DeviceState::Stopping);

            MFPutWorkItem2(MFASYNC_CALLBACK_QUEUE_MULTITHREADED, 0, &m_StopCaptureCallback, nullptr);
        }
    }

    //
    //  OnStopCapture()
    //
    //  Callback method to stop capture
    //
    HRESULT WASAPICapture::OnStopCapture(IMFAsyncResult*) try
    {
        // Stop capture by cancelling Work Item
        // Cancel the queued work item (if any)
        if (0 != m_sampleReadyKey)
        {
            MFCancelWorkItem(std::exchange(m_sampleReadyKey, 0));
        }

        m_sampleReadyAsyncResult = nullptr;
        if (m_audioClient)
        {
            m_audioClient->Stop();

            if (m_writing)
            {
                // A StoreAsync is already in progress. Let that one complete and finish the capture.
            }
            else
            {
                SetState(DeviceState::Flushing);
                AsyncStoreData();
            }
        }
        return S_OK;
    }
    catch (...) { return to_hresult(); }

    //
    //  AsyncFinishCapture()
    //
    //  Finalizes WAV file on a separate thread via MF Work Item
    //
    fire_and_forget WASAPICapture::AsyncStoreData()
    {
        auto lifetime = get_strong();

        co_await m_dataWriter.StoreAsync();
        m_writing = false;

        if (m_deviceState == DeviceState::Stopping)
        {
            // A Stop request is pending. Transition to Flushing to write out the WAV header.
            SetState(DeviceState::Flushing);
        }

        if (m_deviceState == DeviceState::Flushing)
        {
            check_hresult(MFPutWorkItem2(MFASYNC_CALLBACK_QUEUE_MULTITHREADED, 0, &m_FinishCaptureCallback, nullptr));
        }
    }

    //
    //  OnFinishCapture()
    //
    //  Because of the asynchronous nature of the MF Work Queues and the DataWriter, there could still be
    //  a sample processing.  So this will get called to finalize the WAV header.
    //
    HRESULT WASAPICapture::OnFinishCapture(IMFAsyncResult*)
    {
        // FixWAVHeader will set the DeviceState::Stopped when all async tasks are complete
        FixWAVHeader();
        return S_OK;
    }

    //
    //  OnSampleReady()
    //
    //  Callback method when ready to fill sample buffer
    //
    HRESULT WASAPICapture::OnSampleReady(IMFAsyncResult*) try
    {
        OnAudioSampleRequested();

        // Re-queue work item for next sample
        if (m_deviceState == DeviceState::Capturing)
        {
            check_hresult(MFPutWaitingWorkItem(m_SampleReadyEvent.get(), 0, m_sampleReadyAsyncResult.get(), &m_sampleReadyKey));
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
    void WASAPICapture::OnAudioSampleRequested()
    {
        auto guard = slim_lock_guard(m_lock);

        // If this flag is set, we have already queued up the async call to finialize the WAV header
        // So we don't want to grab or write any more data that would possibly give us an invalid size
        if ((m_deviceState == DeviceState::Stopping) ||
            (m_deviceState == DeviceState::Flushing))
        {
            return;
        }

        // A word on why we have a loop here:
        // Suppose it has been 10 milliseconds or so since the last time
        // this routine was invoked, and that we're capturing 48000 samples per second.
        //
        // The audio engine can be reasonably expected to have accumulated about that much
        // audio data - that is, about 480 samples.
        //
        // However, the audio engine is free to accumulate this in various ways:
        // a. as a single packet of 480 samples, OR
        // b. as a packet of 80 samples plus a packet of 400 samples, OR
        // c. as 48 packets of 10 samples each.
        //
        // In particular, there is no guarantee that this routine will be
        // run once for each packet.
        //
        // So every time this routine runs, we need to read ALL the packets
        // that are now available;
        //
        // We do this by calling IAudioCaptureClient::GetNextPacketSize
        // over and over again until it indicates there are no more packets remaining.
        uint32_t framesAvailable = 0;
        while (SUCCEEDED(m_audioCaptureClient->GetNextPacketSize(&framesAvailable)) && framesAvailable > 0)
        {
            DWORD bytesToCapture = framesAvailable * m_mixFormat->nBlockAlign;

            // WAV files have a 4GB (0xFFFFFFFF) size limit, so likely we have hit that limit when we
            // overflow here.  Time to stop the capture
            if ((m_dataSize + bytesToCapture) < m_dataSize)
            {
                AsyncStopCapture();
                return;
            }

            { // Scope for GetBuffer / ReleaseBuffer variables.
                uint8_t* data = nullptr;
                DWORD dwCaptureFlags;
                uint64_t devicePosition = 0;
                uint64_t qpcPosition = 0;

                // Get sample buffer
                check_hresult(m_audioCaptureClient->GetBuffer(&data, &framesAvailable, &dwCaptureFlags, &devicePosition, &qpcPosition));

                // Ensure that the buffer is released at scope exit, even if an exception occurs.
                auto release = scope_exit([&] { m_audioCaptureClient->ReleaseBuffer(framesAvailable); });

                if (dwCaptureFlags & AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY)
                {
                    // Pass down a discontinuity flag in case the app is interested and reset back to capturing
                    SetState(DeviceState::Discontinuity);
                    SetStateNoNotify(DeviceState::Capturing);
                }

                // Zero out sample if silence
                if (dwCaptureFlags & AUDCLNT_BUFFERFLAGS_SILENT)
                {
                    memset(data, 0, framesAvailable * m_mixFormat->nBlockAlign);
                }

                // Update plotter data
                array_view<uint8_t> dataBytes{ data, bytesToCapture };
                ProcessScopeData(dataBytes);

                // Write File and async store
                m_dataWriter.WriteBytes(dataBytes);

                // End of scope: Buffer is released back, and GetBuffer variables are no longer valid
            }

            // Increase the size of our 'data' chunk and bytes since last flush.  m_dataSize needs to be accurate but
            // m_bytesSinceLastFlush need only be an approximation.
            m_dataSize += bytesToCapture;
            m_bytesSinceLastFlush += bytesToCapture;

            if ((m_bytesSinceLastFlush > (m_mixFormat->nAvgBytesPerSec * FLUSH_INTERVAL / 1s)) && !m_writing)
            {
                // Set this flag when about to commit the async storage operation.  We don't want to 
                // accidently call stop and finalize the WAV header or run into a scenario where the 
                // store operation takes longer than FLUSH_INTERVAL as multiple concurrent calls 
                // to StoreAsync() can cause an exception
                m_writing = true;

                // Reset the counter now since we can process more samples during the async callback
                m_bytesSinceLastFlush = 0;

                // Save the data we just wrote to the output file.
                AsyncStoreData();
            }
        }
    }

    //
    //  ProcessScopeData()
    //
    //  Copies sample data to the buffer array and fires the event
    //
    void WASAPICapture::ProcessScopeData(array_view<uint8_t> rawBytes)
    {
        // There is no buffer if the data format is not one we support.
        // (For now, the only format we support is 16-bit audio.)
        if (m_plotDataBuffer == nullptr)
        {
            return;
        }

        uint32_t pointCount = rawBytes.size() / m_mixFormat->nChannels / (m_mixFormat->wBitsPerSample / BITS_PER_BYTE);

        auto plotData = reinterpret_cast<int16_t*>(m_plotDataBuffer.data());

        // Read the 16-bit samples from channel 0
        int16_t* pi16 = reinterpret_cast<int16_t*>(rawBytes.data());

        for (DWORD i = 0; m_plotDataPointsFilled < m_plotDataMaxPoints && i < pointCount; i++)
        {
            plotData[m_plotDataPointsFilled] = *pi16;
            pi16 += m_mixFormat->nChannels;

            m_plotDataPointsFilled++;
        }

        // Send off the event and get ready for the next set of samples
        if (m_plotDataPointsFilled == m_plotDataMaxPoints)
        {
            auto plotDataBuffer = std::exchange(m_plotDataBuffer, nullptr);
            m_plotDataPointsFilled = 0;

            plotDataBuffer.Length(m_plotDataMaxPoints * sizeof(int16_t));
            MFPutWorkItem2(MFASYNC_CALLBACK_QUEUE_MULTITHREADED, 0, &m_SendScopeDataCallback, winrt::get_unknown(plotDataBuffer));

            // Create a new buffer to hold the next set of points.
            m_plotDataBuffer = Buffer(sizeof(int16_t) * m_plotDataMaxPoints);
        }
    }

    //
    //  OnSendScopeData()
    //
    //  Callback method to delivery data for client visualizations
    //
    HRESULT WASAPICapture::OnSendScopeData(IMFAsyncResult* pResult) try
    {
        com_ptr<::IUnknown> points;
        check_hresult(pResult->GetState(points.put()));
        ReportPlotDataReady(points.as<IBuffer>());
        return S_OK;
    }
    catch (...)
    {
        return S_OK;
    }
}
