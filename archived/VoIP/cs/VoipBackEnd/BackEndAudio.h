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
#include "windows.h"

#define MAX_RAW_BUFFER_SIZE 1024*128

#include <synchapi.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <wrl\implements.h>

#include "BackEndTransport.h"

using namespace Microsoft::WRL;

namespace VoipBackEnd
{
    public ref class BackEndAudio sealed
    {
    public:
        // Constructor
        BackEndAudio();

        // Destructor
        virtual ~BackEndAudio();

        void SetTransport(BackEndTransport^ transport);
            
        void Start();
        void Stop();

    private:
        class CaptureActivationHelper:
            public RuntimeClass< RuntimeClassFlags< ClassicCom >, FtmBase, IActivateAudioInterfaceCompletionHandler >
            {
            public:
                BackEndAudio^ backEndAudio;

                STDMETHOD(InitializeAudioDeviceAsync)(Platform::String^ deviceIdString);

                // IActivateAudioInterfaceCompletionHandler
                STDMETHOD(ActivateCompleted)(IActivateAudioInterfaceAsyncOperation *operation);
            };
        class RenderActivationHelper :
            public RuntimeClass< RuntimeClassFlags< ClassicCom >, FtmBase, IActivateAudioInterfaceCompletionHandler >
        {
        public:
            BackEndAudio^ backEndAudio;

            STDMETHOD(InitializeAudioDeviceAsync)(Platform::String^ deviceIdString);

            // IActivateAudioInterfaceCompletionHandler
            STDMETHOD(ActivateCompleted)(IActivateAudioInterfaceAsyncOperation *operation);
        };

        STDMETHOD(OnCaptureActivateCompleted)();
        STDMETHOD(OnRenderActivateCompleted)();
        HRESULT InitRender();
        HRESULT InitCapture();
        HRESULT StartAudioThreads();
        void CaptureThread(Windows::Foundation::IAsyncAction^ operation);
        void OnTransportMessageReceived(Windows::Storage::Streams::IBuffer^ stream, UINT64, UINT64);
            
        BackEndTransport^ transportController;

        VoipBackEnd::MessageReceivedEventHandler^ onTransportMessageReceivedHandler;
        Windows::Foundation::EventRegistrationToken onTransportMessageReceivedHandlerToken;

        int m_sourceFrameSizeInBytes;

        WAVEFORMATEX* m_pwfx;

        // Devices
        IAudioClient2* m_pDefaultRenderDevice;
        IAudioClient2* m_pDefaultCaptureDevice;

        // Actual render and capture objects
        IAudioRenderClient* m_pRenderClient;
        IAudioCaptureClient* m_pCaptureClient;

        // Misc interfaces
        IAudioClock* m_pClock; 
        ISimpleAudioVolume* m_pVolume;

        // Audio buffer size
        UINT32 m_nMaxFrameCount;
        HANDLE hCaptureEvent;

        // Event for stopping audio capture/render
        HANDLE hShutdownEvent;

        Windows::Foundation::IAsyncAction^ m_CaptureThread;

        ComPtr<CaptureActivationHelper> m_spCaptureActivationHelper;
        ComPtr<RenderActivationHelper> m_spRenderActivationHelper;

        // Has audio started?
        bool started;
    };
}
