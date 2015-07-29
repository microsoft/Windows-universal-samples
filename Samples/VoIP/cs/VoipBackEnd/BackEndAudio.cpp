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
#include "BackEndAudio.h"
#include "ApiLock.h"
#include "BackEndNativeBuffer.h"
#include "BackEndAudioHelpers.h"

using namespace VoipBackEnd;
using namespace Windows::System::Threading;
using namespace Windows::Media::Devices;

BackEndAudio::BackEndAudio() :
    m_pDefaultRenderDevice(NULL),
    m_pRenderClient(NULL),
    m_pClock(NULL),
    m_pVolume(NULL),
    m_nMaxFrameCount(0),
    m_pwfx(NULL),
    m_pDefaultCaptureDevice(NULL),
    m_pCaptureClient(NULL),
    m_sourceFrameSizeInBytes(0),
    hCaptureEvent(NULL),
    hShutdownEvent(NULL),
    m_CaptureThread(nullptr),
    transportController(nullptr),
    started(false)
{
    this->onTransportMessageReceivedHandler = ref new MessageReceivedEventHandler(this, &BackEndAudio::OnTransportMessageReceived);
}

void BackEndAudio::OnTransportMessageReceived(Windows::Storage::Streams::IBuffer^ stream, UINT64, UINT64)
{
    BYTE* pBuffer = NativeBuffer::GetBytesFromIBuffer(stream);
    int size = stream->Length;

    while (m_pRenderClient && size && pBuffer)
    {
        HRESULT hr = E_FAIL;
        unsigned int padding = 0;

        hr = m_pDefaultRenderDevice->GetCurrentPadding(&padding);
        if (SUCCEEDED(hr))
        {
            BYTE* pRenderBuffer = NULL;
            unsigned int incomingFrameCount = size / m_sourceFrameSizeInBytes;
            unsigned int framesToWrite = m_nMaxFrameCount - padding;
            
            if (framesToWrite > incomingFrameCount)
            {
                framesToWrite = incomingFrameCount;
            }

            if (framesToWrite)
            {
                hr = m_pRenderClient->GetBuffer(framesToWrite, &pRenderBuffer);

                if (SUCCEEDED(hr))
                {
                    unsigned int bytesToBeWritten = framesToWrite * m_sourceFrameSizeInBytes;

                    memcpy(pRenderBuffer, pBuffer, bytesToBeWritten);

                    // Release the buffer
                    m_pRenderClient->ReleaseBuffer(framesToWrite, 0);

                    pBuffer += bytesToBeWritten;
                    size -= bytesToBeWritten;
                }
            }
        }
    }
}

void BackEndAudio::Start()
{
    // Make sure only one API call is in progress at a time
    std::lock_guard<std::recursive_mutex> lock(g_apiLock);

    if (started)
        return;

    HRESULT hr = InitCapture();

    if (FAILED(hr))
    {
        Stop();
        throw ref new Platform::COMException(hr, L"Unable to start audio");
    }

    started = true;
}

void BackEndAudio::Stop()
{
    // Make sure only one API call is in progress at a time
    std::lock_guard<std::recursive_mutex> lock(g_apiLock);

    if (!started)
        return;

    // Shutdown the threads
    if (hShutdownEvent)
    {
        SetEvent(hShutdownEvent);
    }

    if (m_CaptureThread != nullptr)
    {
        transportController->Disconnect();
        m_CaptureThread->Cancel();
        m_CaptureThread->Close();
        m_CaptureThread = nullptr;
    }

    if (m_pDefaultRenderDevice)
    {
        m_pDefaultRenderDevice->Stop();
    }

    if (m_pDefaultCaptureDevice)
    {
        m_pDefaultCaptureDevice->Stop();
    }

    if (m_pVolume)
    {
        m_pVolume->Release();
        m_pVolume = NULL;
    }
    if (m_pClock)
    {
        m_pClock->Release();
        m_pClock = NULL;
    }

    if (m_pRenderClient)
    {
        m_pRenderClient->Release();
        m_pRenderClient = NULL;
    }

    if (m_pCaptureClient)
    {
        m_pCaptureClient->Release();
        m_pCaptureClient = NULL;
    }

    if (m_pDefaultRenderDevice)
    {
        m_pDefaultRenderDevice->Release();
        m_pDefaultRenderDevice = NULL;
    }

    if (m_pDefaultCaptureDevice)
    {
        m_pDefaultCaptureDevice->Release();
        m_pDefaultCaptureDevice = NULL;
    }

    if (m_pwfx)
    {
        CoTaskMemFree((LPVOID)m_pwfx);
        m_pwfx = NULL;
    }

    if (hCaptureEvent)
    {
        CloseHandle(hCaptureEvent);
        hCaptureEvent = NULL;
    }

    if (hShutdownEvent)
    {
        CloseHandle(hShutdownEvent);
        hShutdownEvent = NULL;
    }

    started = false;
}


void BackEndAudio::CaptureThread(Windows::Foundation::IAsyncAction^ operation)
{
    HRESULT hr = m_pDefaultCaptureDevice->Start();
    BYTE *pLocalBuffer = new BYTE[MAX_RAW_BUFFER_SIZE];
    HANDLE eventHandles[] = {
                             hCaptureEvent,        // WAIT_OBJECT0 
                             hShutdownEvent        // WAIT_OBJECT0 + 1
                            };

    if (SUCCEEDED(hr) && pLocalBuffer)
    {
        unsigned int uAccumulatedBytes = 0;
        while (SUCCEEDED(hr))
        {
            DWORD waitResult = WaitForMultipleObjectsEx(SIZEOF_ARRAY(eventHandles), eventHandles, FALSE, INFINITE, FALSE);
            if (WAIT_OBJECT_0 == waitResult)
            {
                BYTE* pbData = nullptr;
                UINT32 nFrames = 0;
                DWORD dwFlags = 0;
                if (SUCCEEDED(hr))
                {
                    hr = m_pCaptureClient->GetBuffer(&pbData, &nFrames, &dwFlags, nullptr, nullptr);
                    unsigned int incomingBufferSize = nFrames * m_sourceFrameSizeInBytes;

                    if (MAX_RAW_BUFFER_SIZE - uAccumulatedBytes < incomingBufferSize)
                    {
                        // Send what has been accumulated
                        if (transportController)
                        {
                            transportController->WriteAudio(pLocalBuffer, uAccumulatedBytes);
                        }

                        // Reset our counter
                        uAccumulatedBytes = 0;
                    }

                    memcpy(pLocalBuffer + uAccumulatedBytes, pbData, incomingBufferSize);
                    uAccumulatedBytes += incomingBufferSize;
                }

                if (SUCCEEDED(hr))
                {
                    hr = m_pCaptureClient->ReleaseBuffer(nFrames);
                }
            }
            else if (WAIT_OBJECT_0 + 1 == waitResult)
            {
                // We're being asked to shutdown
                break;
            }
            else
            {
                // Unknown return value
                DbgRaiseAssertionFailure();
            }
        }
    }
    delete[] pLocalBuffer;
}

HRESULT BackEndAudio::StartAudioThreads()
{
    hShutdownEvent = CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);
    if (!hShutdownEvent)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    
    m_CaptureThread = ThreadPool::RunAsync(ref new WorkItemHandler(this, &BackEndAudio::CaptureThread), WorkItemPriority::High, WorkItemOptions::TimeSliced);
    return S_OK;
}

BackEndAudio::~BackEndAudio()
{
    if (transportController)
    {
        transportController->AudioMessageReceived -= onTransportMessageReceivedHandlerToken;
        transportController = nullptr;
    }
}

STDMETHODIMP BackEndAudio::OnCaptureActivateCompleted()
{
    HRESULT hr = S_OK;

    if (m_pDefaultCaptureDevice == nullptr)
    {
        return E_FAIL;
    }

    hr = m_pDefaultCaptureDevice->GetMixFormat(&m_pwfx);

    // Set the category through SetClientProperties
    AudioClientProperties properties = {};
    if (SUCCEEDED(hr))
    {
        properties.cbSize = sizeof AudioClientProperties;
        properties.eCategory = AudioCategory_Communications;
        hr = m_pDefaultCaptureDevice->SetClientProperties(&properties);
    }

    if (SUCCEEDED(hr))
    {
        //0x88140000 has AUDCLNT_STREAMFLAGS_EVENTCALLBACK in it already
        WAVEFORMATEX temp;
        FillPcmFormat(temp, 2, 48000, 32);
        *m_pwfx = temp;
        m_sourceFrameSizeInBytes = (m_pwfx->wBitsPerSample / 8) * m_pwfx->nChannels;

        hr = m_pDefaultCaptureDevice->Initialize(AUDCLNT_SHAREMODE_SHARED, 0x88140000, 1000 * 10000, 0, m_pwfx, NULL);
    }

    if (SUCCEEDED(hr))
    {
        hCaptureEvent = CreateEventEx(NULL, NULL, 0, EVENT_ALL_ACCESS);
        if (NULL == hCaptureEvent)
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }
    }

    if (SUCCEEDED(hr))
    {
        hr = m_pDefaultCaptureDevice->SetEventHandle(hCaptureEvent);
    }

    if (SUCCEEDED(hr))
    {
        hr = m_pDefaultCaptureDevice->GetService(__uuidof(IAudioCaptureClient), (void**)&m_pCaptureClient);
    }

    if (SUCCEEDED(hr))
    {
        hr = InitRender();
    }

    if (FAILED(hr))
    {
        Stop();
    }
    return hr;
}

STDMETHODIMP BackEndAudio::OnRenderActivateCompleted()
{
    HRESULT hr = S_OK;

    if (m_pDefaultRenderDevice == nullptr)
    {
        return E_FAIL;
    }

    // Set the category through SetClientProperties
    AudioClientProperties properties = {};
    if (SUCCEEDED(hr))
    {
        properties.cbSize = sizeof AudioClientProperties;
        properties.eCategory = AudioCategory_Communications;
        hr = m_pDefaultRenderDevice->SetClientProperties(&properties);
    }

    WAVEFORMATEX* pwfx = nullptr;
    if (SUCCEEDED(hr))
    {
        hr = m_pDefaultRenderDevice->GetMixFormat(&pwfx);
    }

    WAVEFORMATEX format = {};
    if (SUCCEEDED(hr))
    {
        FillPcmFormat(format, pwfx->nChannels, pwfx->nSamplesPerSec, pwfx->wBitsPerSample);
        hr = m_pDefaultRenderDevice->Initialize(AUDCLNT_SHAREMODE_SHARED,
            0,
            2000 * 10000,  // Seconds in hns
            0, // periodicity
            &format,
            NULL);
    }

    if (SUCCEEDED(hr))
    {
        hr = m_pDefaultRenderDevice->GetService(__uuidof(IAudioRenderClient), (void**)&m_pRenderClient);
    }

    // Check for other supported GetService interfaces as well
    if (SUCCEEDED(hr))
    {
        hr = m_pDefaultRenderDevice->GetService(__uuidof(IAudioClock), (void**)&m_pClock);
    }

    if (SUCCEEDED(hr))
    {
        hr = m_pDefaultRenderDevice->GetService(__uuidof(ISimpleAudioVolume), (void**)&m_pVolume);
    }

    if (SUCCEEDED(hr))
    {
        hr = m_pDefaultRenderDevice->GetBufferSize(&m_nMaxFrameCount);
    }

    if (SUCCEEDED(hr))
    {
        hr = m_pDefaultRenderDevice->Start();
    }
    
    if (SUCCEEDED(hr))
    {
        hr = StartAudioThreads();
    }

    if (FAILED(hr))
    {
        Stop();
    }

    return hr;
}

HRESULT BackEndAudio::InitRender()
{
    HRESULT hr = E_FAIL;
    
    Platform::String^ rendererIdString = MediaDevice::GetDefaultAudioRenderId(AudioDeviceRole::Communications);
    
    if (nullptr == rendererIdString)
    {
        hr = E_FAIL;
    }

    m_spRenderActivationHelper = Make<RenderActivationHelper>();
    m_spRenderActivationHelper->backEndAudio = this;
    hr = m_spRenderActivationHelper->InitializeAudioDeviceAsync(rendererIdString);
   
    return hr;
}

HRESULT BackEndAudio::InitCapture()
{
    HRESULT hr = E_FAIL;


    Platform::String^ captureIdString = MediaDevice::GetDefaultAudioCaptureId(AudioDeviceRole::Communications);

    if (nullptr == captureIdString)
    {
        hr = E_FAIL;
    }

    m_spCaptureActivationHelper = Make<CaptureActivationHelper>();
    m_spCaptureActivationHelper->backEndAudio = this;
    hr = m_spCaptureActivationHelper->InitializeAudioDeviceAsync(captureIdString);

    
    return hr;
}

void BackEndAudio::SetTransport(BackEndTransport^ transport)
{
    transportController = transport;
    if (transportController != nullptr)
    {
        onTransportMessageReceivedHandlerToken = transportController->AudioMessageReceived += onTransportMessageReceivedHandler;
    }
}

STDMETHODIMP BackEndAudio::CaptureActivationHelper::InitializeAudioDeviceAsync(Platform::String^ deviceIdString)
{
    IActivateAudioInterfaceAsyncOperation *asyncOp;
    HRESULT hr = S_OK;

    // This call must be made on the main UI thread.  Async operation will call back to 
    // IActivateAudioInterfaceCompletionHandler::ActivateCompleted, which must be an agile interface implementation
    hr = ActivateAudioInterfaceAsync(deviceIdString->Data(), __uuidof(IAudioClient2), nullptr, this, &asyncOp);

    return hr;
}

STDMETHODIMP BackEndAudio::CaptureActivationHelper::ActivateCompleted(IActivateAudioInterfaceAsyncOperation * operation)
{

    HRESULT hr = S_OK;
    HRESULT hrActivateResult = S_OK;
    IUnknown *punkAudioInterface = nullptr;

    // Check for a successful activation result
    hr = operation->GetActivateResult(&hrActivateResult, &punkAudioInterface);
    if (SUCCEEDED(hr) && SUCCEEDED(hrActivateResult))
    {
        // Get the pointer for the Audio Client
        punkAudioInterface->QueryInterface(IID_PPV_ARGS(&backEndAudio->m_pDefaultCaptureDevice));
    }
    
    backEndAudio->OnCaptureActivateCompleted();

    return hr;
}

STDMETHODIMP BackEndAudio::RenderActivationHelper::InitializeAudioDeviceAsync(Platform::String ^ deviceIdString)
{
    IActivateAudioInterfaceAsyncOperation *asyncOp;
    HRESULT hr = S_OK;

    // This call must be made on the main UI thread.  Async operation will call back to 
    // IActivateAudioInterfaceCompletionHandler::ActivateCompleted, which must be an agile interface implementation
    hr = ActivateAudioInterfaceAsync(deviceIdString->Data(), __uuidof(IAudioClient2), nullptr, this, &asyncOp);

    return hr;
}

STDMETHODIMP BackEndAudio::RenderActivationHelper::ActivateCompleted(IActivateAudioInterfaceAsyncOperation * operation)
{
    HRESULT hr = S_OK;
    HRESULT hrActivateResult = S_OK;
    IUnknown *punkAudioInterface = nullptr;

    // Check for a successful activation result
    hr = operation->GetActivateResult(&hrActivateResult, &punkAudioInterface);
    if (SUCCEEDED(hr) && SUCCEEDED(hrActivateResult))
    {
        // Get the pointer for the Audio Client
        punkAudioInterface->QueryInterface(IID_PPV_ARGS(&backEndAudio->m_pDefaultRenderDevice));
    }

    backEndAudio->OnRenderActivateCompleted();

    return hr;
}
