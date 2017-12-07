#pragma once

#include <ppltasks.h>
#include <mfapi.h>

namespace HolographicMRCSample
{
    class MediaCaptureManager
    {
    private:
        typedef enum
        {
            Unknown = 0,
            Initialized,
            StartingRecord,
            Recording,
            StoppingRecord,
            TakingPhoto
        } State;

    public:
        MediaCaptureManager();
        virtual ~MediaCaptureManager();

        Concurrency::task<void> InitializeAsync(IMFDXGIDeviceManager *pDxgiDeviceManager = nullptr);

        Concurrency::task<void> StartRecordingAsync();
        Concurrency::task<void> StopRecordingAsync();

        Concurrency::task<void> TakePhotoAsync();

        void SetHologramEnabled(const bool enabled)    { auto lock = m_lock.LockExclusive(); m_hologramEnabled = enabled; }
        void SetSystemAudioEnabled(const bool enabled) { auto lock = m_lock.LockExclusive(); m_sysAudioEnabled = enabled; }

        bool IsHologramEnabled()    { auto lock = m_lock.LockShared(); return m_hologramEnabled; }
        bool IsSystemAudioEnabled() { auto lock = m_lock.LockShared(); return m_sysAudioEnabled; }
        bool CanTakePhoto();

    private:
        Microsoft::WRL::Wrappers::SRWLock m_lock;
        Platform::Agile<Windows::Media::Capture::MediaCapture>  m_mediaCapture;

        State m_currentState;

        bool m_hologramEnabled;
        bool m_sysAudioEnabled;

        bool m_videoEffectAdded;
        bool m_photoEffectAdded;
        bool m_audioEffectAdded;

        Microsoft::WRL::ComPtr<IMFDXGIDeviceManager>    m_spMFDXGIDeviceManager;
    };
}
