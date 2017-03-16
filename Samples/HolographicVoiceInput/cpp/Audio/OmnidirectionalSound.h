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

#include "AudioFileReader.h"
#include "AudioStreamReader.h"

//
// Sound with omnidirectional radiation pattern i.e. emits sound equally in all directions.
//
class OmnidirectionalSound
{
public:
    virtual ~OmnidirectionalSound();
    HRESULT Initialize(_In_ LPCWSTR filename, UINT32 const& loopCount);
    HRESULT Initialize(Windows::Media::SpeechSynthesis::SpeechSynthesisStream^ stream, UINT32 const& loopCount);

    HRESULT Start();
    HRESULT Stop();
    HRESULT GetInitializationStatus() { return _initStatus; }
    HRESULT OnUpdate(_In_ float angularVelocity, _In_ float height, _In_ float radius);
    HRESULT OnUpdate(_In_ Windows::Foundation::Numerics::float3 transform);
    HRESULT SetEnvironment(_In_ HrtfEnvironment environment);
    HrtfEnvironment GetEnvironment() { return _environment; }
    float GetDuration() { return _duration; };

private:
    HrtfPosition ComputePositionInOrbit(_In_ float height, _In_ float radius, _In_ float angle);

private:

    enum SoundSourceType {
        File,
        Stream,
    };

    HRESULT                                     _initStatus;
    SoundSourceType                             _sourceType;
    AudioFileReader                             _audioFile;
    AudioStreamReader                           _audioStream;
    Microsoft::WRL::ComPtr<IXAudio2>            _xaudio2;
    IXAudio2SourceVoice*                        _sourceVoice = nullptr;
    Microsoft::WRL::ComPtr<IXAPOHrtfParameters> _hrtfParams;
    HrtfEnvironment                             _environment = HrtfEnvironment::Outdoors;
    ULONGLONG                                   _lastTick = 0;
    float                                       _angle = 0;
    float                                       _duration = 0.f;
};
