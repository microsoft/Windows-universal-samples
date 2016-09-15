#include "pch.h"
#include "MediaCaptureManager.h"
#include <windows.media.mediaproperties.h>
#include <mfmediacapture.h>
#include <collection.h>

using namespace HolographicMRCSample;
using namespace Windows::Media;
using namespace Windows::Media::Capture;

MediaCaptureManager::MediaCaptureManager() :
    m_currentState(Unknown),
    m_hologramEnabled(false),
    m_sysAudioEnabled(false),
    m_videoEffectAdded(false),
    m_photoEffectAdded(false),
    m_audioEffectAdded(false)
{
}

MediaCaptureManager::~MediaCaptureManager()
{
}

Concurrency::task<void> MediaCaptureManager::InitializeAsync(IMFDXGIDeviceManager *pDxgiDeviceManager)
{
    m_mediaCapture = ref new Windows::Media::Capture::MediaCapture;

    m_mediaCapture->Failed += ref new MediaCaptureFailedEventHandler([](MediaCapture^ sender, MediaCaptureFailedEventArgs ^args)
    {
        // Handle MediaCapture failure notification
        OutputDebugString(args->Message->Data());
    });

    auto initSetting = ref new Windows::Media::Capture::MediaCaptureInitializationSettings;
    initSetting->StreamingCaptureMode = Windows::Media::Capture::StreamingCaptureMode::AudioAndVideo;
    initSetting->MediaCategory = Windows::Media::Capture::MediaCategory::Media;

    if (pDxgiDeviceManager)
    {
        // Optionally, you can put your D3D device into MediaCapture.
        // But, in most case, this is not mandatory.
        m_spMFDXGIDeviceManager = pDxgiDeviceManager;
        Microsoft::WRL::ComPtr<IAdvancedMediaCaptureInitializationSettings> spAdvancedSettings;
        ((IUnknown*)initSetting)->QueryInterface(IID_PPV_ARGS(&spAdvancedSettings));
        spAdvancedSettings->SetDirectxDeviceManager(m_spMFDXGIDeviceManager.Get());
    }

    return Concurrency::create_task(m_mediaCapture->InitializeAsync(initSetting)).then([this]()
    {
        auto lock = m_lock.LockExclusive();
        m_currentState = Initialized;
    });
}

Concurrency::task<void> MediaCaptureManager::StartRecordingAsync()
{
    // Lock to check current state
    auto lock = m_lock.LockExclusive();

    if (m_currentState != Initialized)
    {
        OutputDebugString(L"Trying to start recording in invalid state.\n");
        throw ref new Platform::FailureException();
    }

    m_currentState = StartingRecord;

    return Concurrency::create_task(Windows::Storage::KnownFolders::GetFolderForUserAsync(nullptr, Windows::Storage::KnownFolderId::CameraRoll)).then([this](Windows::Storage::StorageFolder ^folder)
    {
        if (!folder)
        {
            OutputDebugString(L"Can't find camera roll folder for this user\n");
            throw ref new Platform::FailureException();
        }

        // Create storage file for video capture
        return Concurrency::create_task(folder->CreateFileAsync(L"MrcVideo.mp4", Windows::Storage::CreationCollisionOption::GenerateUniqueName)).then([this](Windows::Storage::StorageFile ^saveFile)
        {
            if (!saveFile)
            {
                throw ref new Platform::FailureException();
            }

            Concurrency::task<void> addEffectTask;
            auto lock = m_lock.LockShared();

            if (m_hologramEnabled)
            {
                // Add video effect on videoRecord stream
                auto mrcVideoEffectDefinition = ref new MrcEffectDefinitions::MrcVideoEffectDefinition;
                mrcVideoEffectDefinition->StreamType = MediaStreamType::VideoRecord;

                addEffectTask = Concurrency::create_task(m_mediaCapture->AddVideoEffectAsync(mrcVideoEffectDefinition, MediaStreamType::VideoRecord)).then([this](IMediaExtension ^videoExtension)
                {
                    auto lock = m_lock.LockExclusive();
                    OutputDebugString(L"VideoEffect Added\n");
                    m_videoEffectAdded = true;

                    if (m_sysAudioEnabled)
                    {
                        // Add audio effect on audio stream
                        auto mrcAudioEffectDefinition = ref new MrcEffectDefinitions::MrcAudioEffectDefinition;
                        mrcAudioEffectDefinition->MixerMode = MrcEffectDefinitions::AudioMixerMode::MicAndLoopback;

                        auto addAudioEffectTask = Concurrency::create_task(m_mediaCapture->AddAudioEffectAsync(mrcAudioEffectDefinition)).then([this](IMediaExtension^ audioExtension)
                        {
                            auto lock = m_lock.LockExclusive();
                            OutputDebugString(L"AudioEffect Added\n");
                            m_audioEffectAdded = true;
                        });
                    }
                });
            }
            else if (m_sysAudioEnabled)
            {
                // Add audio effect on audio stream
                auto mrcAudioEffectDefinition = ref new MrcEffectDefinitions::MrcAudioEffectDefinition;
                mrcAudioEffectDefinition->MixerMode = MrcEffectDefinitions::AudioMixerMode::MicAndLoopback;

                addEffectTask = Concurrency::create_task(m_mediaCapture->AddAudioEffectAsync(mrcAudioEffectDefinition)).then([this](IMediaExtension^ audioExtension)
                {
                    auto lock = m_lock.LockExclusive();
                    OutputDebugString(L"AudioEffect Added\n");
                    m_audioEffectAdded = true;
                });
            }

            if (m_hologramEnabled || m_sysAudioEnabled)
            {
                return addEffectTask.then([this, saveFile]()
                {
                    // When all effects are added, start video recording
                    auto encodingProperties = MediaProperties::MediaEncodingProfile::CreateMp4(MediaProperties::VideoEncodingQuality::Auto);

                    return Concurrency::create_task(m_mediaCapture->StartRecordToStorageFileAsync(encodingProperties, saveFile)).then([this]()
                    {
                        auto lock = m_lock.LockExclusive();
                        OutputDebugString(L"Recording Started\n");
                        m_currentState = Recording;
                    });
                });
            }
            else
            {
                // Start video recording without any effects
                auto encodingProperties = MediaProperties::MediaEncodingProfile::CreateMp4(MediaProperties::VideoEncodingQuality::Auto);

                return Concurrency::create_task(m_mediaCapture->StartRecordToStorageFileAsync(encodingProperties, saveFile)).then([this]()
                {
                    auto lock = m_lock.LockExclusive();
                    OutputDebugString(L"Recording Started\n");
                    m_currentState = Recording;
                });
            }
        });
    });
}

Concurrency::task<void> MediaCaptureManager::StopRecordingAsync()
{
    // Lock to check current state
    auto lock = m_lock.LockExclusive();

    if (m_currentState != Recording)
    {
        OutputDebugString(L"Trying to stop recording in invalid state.\n");
        throw ref new Platform::FailureException();
    }

    m_currentState = StoppingRecord;

    // Stop video recording
    return Concurrency::create_task(m_mediaCapture->StopRecordAsync()).then([this]()
    {
        auto lock = m_lock.LockExclusive();
        OutputDebugString(L"Video Recording Stopped\n");
        m_currentState = Initialized;

        if (m_videoEffectAdded)
        {
            // Clear effects in videoRecord stream
            Concurrency::create_task(m_mediaCapture->ClearEffectsAsync(Windows::Media::Capture::MediaStreamType::VideoRecord)).then([this]()
            {
                auto lock = m_lock.LockExclusive();
                OutputDebugString(L"VideoEffect removed\n");
                m_videoEffectAdded = false;

                if (m_audioEffectAdded)
                {
                    // Clear effects in audio stream
                    Concurrency::create_task(m_mediaCapture->ClearEffectsAsync(Windows::Media::Capture::MediaStreamType::Audio)).then([this]()
                    {
                        auto lock = m_lock.LockExclusive();
                        OutputDebugString(L"AudioEffect removed\n");
                        m_audioEffectAdded = false;
                    });
                }
            });
        }
        else  if (m_audioEffectAdded)
        {
            // Clear effects in audio stream
            Concurrency::create_task(m_mediaCapture->ClearEffectsAsync(Windows::Media::Capture::MediaStreamType::Audio)).then([this]()
            {
                auto lock = m_lock.LockExclusive();
                OutputDebugString(L"AudioEffect removed\n");
                m_audioEffectAdded = false;
            });
        }
    });
}

Concurrency::task<void> MediaCaptureManager::TakePhotoAsync()
{
    // Lock to check current state
    auto lock = m_lock.LockExclusive();

    if (m_currentState != Initialized)
    {
        OutputDebugString(L"Trying to take photo in invalid state.\n");
        throw ref new Platform::FailureException();
    }

    m_currentState = TakingPhoto;

    return Concurrency::create_task(Windows::Storage::KnownFolders::GetFolderForUserAsync(nullptr, Windows::Storage::KnownFolderId::CameraRoll)).then([this](Windows::Storage::StorageFolder ^folder)
    {
        if (!folder)
        {
            OutputDebugString(L"Can't find camera roll folder for this user\n");
            throw ref new Platform::FailureException();
        }

        // Create storage file for picture
        return Concurrency::create_task(folder->CreateFileAsync(L"MRCPhoto.jpg", Windows::Storage::CreationCollisionOption::GenerateUniqueName)).then([this](Windows::Storage::StorageFile ^saveFile)
        {
            if (!saveFile)
            {
                OutputDebugString(L"Can't create MRC Photo file\n");
                throw ref new Platform::FailureException();
            }

            auto lock = m_lock.LockShared();

            if (m_hologramEnabled)
            {
                // Add video effect on photo stream
                auto mrcVideoEffectDefinition = ref new MrcEffectDefinitions::MrcVideoEffectDefinition;
                mrcVideoEffectDefinition->StreamType = MediaStreamType::Photo;

                return Concurrency::create_task(m_mediaCapture->AddVideoEffectAsync(mrcVideoEffectDefinition, MediaStreamType::Photo)).then([this, saveFile](IMediaExtension^ videoExtension)
                {
                    OutputDebugString(L"VideoEffect Added\n");
                    m_photoEffectAdded = true;

                    // Taking picture
                    auto encodingProperties = MediaProperties::ImageEncodingProperties::CreateJpeg();

                    return Concurrency::create_task(m_mediaCapture->CapturePhotoToStorageFileAsync(encodingProperties, saveFile)).then([this]()
                    {
                        auto lock = m_lock.LockExclusive();
                        OutputDebugString(L"Photo taken\n");
                        m_currentState = Initialized;

                        return Concurrency::create_task(m_mediaCapture->ClearEffectsAsync(MediaStreamType::Photo)).then([this]()
                        {
                            auto lock = m_lock.LockExclusive();
                            OutputDebugString(L"PhotoEffect removed\n");
                            m_photoEffectAdded = false;
                        });
                    });
                });
            }
            else
            {
                // Taking picture without any effect
                auto encodingProperties = MediaProperties::ImageEncodingProperties::CreateJpeg();

                return Concurrency::create_task(m_mediaCapture->CapturePhotoToStorageFileAsync(encodingProperties, saveFile)).then([this]()
                {
                    auto lock = m_lock.LockExclusive();
                    OutputDebugString(L"Photo taken\n");
                    m_currentState = Initialized;
                });
            }
        });
    });
}

bool MediaCaptureManager::CanTakePhoto()
{
    auto lock = m_lock.LockShared();
    bool ret = false;

    if (m_currentState == Initialized)
    {
        OutputDebugString(L"Can Take Photo\n");
        ret = true;
    }
    else
    {
        OutputDebugString(L"Can NOT Take Photo\n");
        ret = false;
    }

    return ret;
}
