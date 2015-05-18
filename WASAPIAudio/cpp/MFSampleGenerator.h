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
// MFSampleGenerator.h
//

#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mferror.h>
#include "MainPage.xaml.h"

using namespace Windows::Storage::Streams;

#define PREROLL_DURATION_SEC 3     // Arbitrary value for seconds of data in preroll buffer


#pragma once

namespace SDKSample
{
    namespace WASAPIAudio
    {
        class MFSampleGenerator :
            public IMFSourceReaderCallback
        {
        public:
            enum ReaderState
            {
                ReaderStateStopped,
                ReaderStatePreRoll,
                ReaderStatePlaying,
                ReaderStateEOS
            };

            MFSampleGenerator();

            // IUnknown
            STDMETHODIMP_ (ULONG) AddRef()
            {
                return InterlockedIncrement(&m_Ref);
            }

            STDMETHODIMP_ (ULONG) Release()
            {
                LONG ref = InterlockedDecrement(&m_Ref);

                if (ref == 0)
                {
                    delete this;
                }

                return ref;
            }

            STDMETHODIMP_ (HRESULT) QueryInterface(const IID &riid, void ** ppv)
            {
                HRESULT hr = E_NOINTERFACE;
                *ppv = NULL;

                if (riid == IID_IMFSourceReaderCallback)
                {
                    *ppv = static_cast<IMFSourceReaderCallback *>(this);
                    ((IUnknown*) *ppv)->AddRef();
                    hr = S_OK;
                }

                return hr;
            }

            // IMFSourceReaderCallback
            STDMETHODIMP_ (HRESULT) OnEvent(_In_ DWORD dwStreamIndex, _In_ IMFMediaEvent *pEvent);
            STDMETHODIMP_ (HRESULT) OnFlush(_In_ DWORD dwStreamIndex);
            STDMETHODIMP_ (HRESULT) OnReadSample(_In_ HRESULT hrStatus, _In_ DWORD dwStreamIndex, _In_ DWORD dwStreamFlags, _In_ LONGLONG llTimestamp, _In_opt_ IMFSample *pSample);

            // MFSampleGenerator
            HRESULT StartSource();
            void StopSource();

            HRESULT Initialize( IRandomAccessStream^ stream, UINT32 FramesPerPeriod, WAVEFORMATEX *wfx );
            void Shutdown();
            HRESULT FillSampleBuffer( UINT32 BytesToRead, BYTE *Data, UINT32 *cbWritten );
            void Flush();

            Platform::Boolean IsEOF()
            {
                if ( ( m_SampleQueue == nullptr ) &&
                     ( m_ReaderState == ReaderStateEOS ) )
                    return true;

                return false;
            }

        private:
            ~MFSampleGenerator();

            HRESULT ConfigureStreams();
            HRESULT CreateAudioType( IMFMediaType **MediaType );
            HRESULT AddSamplesToQueue( IMFSample *MFSample );
            Platform::Boolean IsPreRollFilled();

        private:
            volatile ULONG          m_Ref;
            IRandomAccessStream^    m_ContentStream;
            WAVEFORMATEX           *m_MixFormat;
            UINT32                  m_BytesPerPeriod;
            Platform::Boolean       m_IsInitialized;

            IMFSourceReader        *m_MFSourceReader;
            IMFMediaType           *m_AudioMT;
            ReaderState             m_ReaderState;

            RenderBuffer           *m_SampleQueue;
            RenderBuffer          **m_SampleQueueTail;
        };
    }
}

