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

#include <Windows.h>
#include <wrl\implements.h>
#include <mfapi.h>
#include <AudioClient.h>
#include <mmdeviceapi.h>
#include "MainPage.xaml.h"
#include "DeviceState.h"
#include "ToneSampleGenerator.h"
#include "MFSampleGenerator.h"

using namespace Microsoft::WRL;
using namespace Windows::Media::Devices;
using namespace Windows::Storage::Streams;


#pragma once

namespace SDKSample
{
    namespace WASAPIAudio
    {
      public enum class ContentType
      {
         ContentTypeTone,
         ContentTypeFile
      };


        // User Configurable Arguments for Scenario
        struct DEVICEPROPS
        {
            Platform::Boolean       IsHWOffload;
            Platform::Boolean       IsTonePlayback;
            Platform::Boolean       IsBackground;
            Platform::Boolean       IsRawSupported;
            Platform::Boolean       IsRawChosen;
            Platform::Boolean       IsLowLatency;
            REFERENCE_TIME          hnsBufferDuration;
            DWORD                   Frequency;
            IRandomAccessStream^    ContentStream;
        };

        // Primary WASAPI Renderering Class
        class WASAPIRenderer :
            public RuntimeClass< RuntimeClassFlags< ClassicCom >, FtmBase, IActivateAudioInterfaceCompletionHandler > 
        {
        public:
            WASAPIRenderer();

            HRESULT SetProperties( DEVICEPROPS props );
            HRESULT InitializeAudioDeviceAsync();
            HRESULT StartPlaybackAsync();
            HRESULT StopPlaybackAsync();
            HRESULT PausePlaybackAsync();

            HRESULT SetVolumeOnSession( UINT32 volume );
            DeviceStateChangedEvent^ GetDeviceStateEvent() { return m_DeviceStateChanged; };

            METHODASYNCCALLBACK( WASAPIRenderer, StartPlayback, OnStartPlayback );
            METHODASYNCCALLBACK( WASAPIRenderer, StopPlayback, OnStopPlayback );
            METHODASYNCCALLBACK( WASAPIRenderer, PausePlayback, OnPausePlayback );
            METHODASYNCCALLBACK( WASAPIRenderer, SampleReady, OnSampleReady );

            // IActivateAudioInterfaceCompletionHandler
            STDMETHOD(ActivateCompleted)( IActivateAudioInterfaceAsyncOperation *operation );

        private:
            ~WASAPIRenderer();

            HRESULT OnStartPlayback( IMFAsyncResult* pResult );
            HRESULT OnStopPlayback( IMFAsyncResult* pResult );
            HRESULT OnPausePlayback( IMFAsyncResult* pResult );
            HRESULT OnSampleReady( IMFAsyncResult* pResult );

            HRESULT ConfigureDeviceInternal();
            HRESULT ValidateBufferValue();
            HRESULT OnAudioSampleRequested( Platform::Boolean IsSilence = false );
            HRESULT ConfigureSource();
            UINT32 GetBufferFramesPerPeriod();

            HRESULT GetToneSample( UINT32 FramesAvailable );
            HRESULT GetMFSample( UINT32 FramesAvailable );

        private:
            Platform::String^   m_DeviceIdString;
            UINT32              m_BufferFrames;
            HANDLE              m_SampleReadyEvent;
            MFWORKITEM_KEY      m_SampleReadyKey;
            CRITICAL_SECTION    m_CritSec;

            WAVEFORMATEX           *m_MixFormat;
            UINT32                  m_DefaultPeriodInFrames;
            UINT32                  m_FundamentalPeriodInFrames;
            UINT32                  m_MaxPeriodInFrames;
            UINT32                  m_MinPeriodInFrames;

            IAudioClient3          *m_AudioClient;
            IAudioRenderClient     *m_AudioRenderClient;
            IMFAsyncResult         *m_SampleReadyAsyncResult;

            DeviceStateChangedEvent^       m_DeviceStateChanged;
            DEVICEPROPS                    m_DeviceProps;

            ToneSampleGenerator    *m_ToneSource;
            MFSampleGenerator      *m_MFSource;
        };
    }
}

