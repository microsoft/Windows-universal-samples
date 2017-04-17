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

#include "OmnidirectionalSound.h"
#include "XAudio2Helpers.h"

using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Media::SpeechSynthesis;

#define HRTF_2PI 6.283185307f

_Use_decl_annotations_
HRESULT OmnidirectionalSound::Initialize(LPCWSTR filename, UINT32 const& loopCount)
{
    auto hr = _audioFile.Initialize(filename);

    if (SUCCEEDED(hr))
    {
        _sourceType = SoundSourceType::File;
    }

    ComPtr<IXAPO> xapo;
    if (SUCCEEDED(hr))
    {
        // Passing in nullptr as the first arg for HrtfApoInit initializes the APO with defaults of
        // omnidirectional sound with natural distance decay behavior.
        // CreateHrtfApo will fail with E_NOTIMPL on unsupported platforms.
        hr = CreateHrtfApo(nullptr, &xapo);
    }

    if (SUCCEEDED(hr))
    {
        hr = xapo.As(&_hrtfParams);
    }

    // Set the default environment.
    if (SUCCEEDED(hr))
    {
        hr = _hrtfParams->SetEnvironment(_environment);
    }

    // Initialize an XAudio2 graph that hosts the HRTF xAPO.
    // The source voice is used to submit audio data and control playback.
    if (SUCCEEDED(hr))
    {
        hr = SetupXAudio2(_audioFile.GetFormat(), xapo.Get(), &_xaudio2, &_sourceVoice);
    }

    // Submit audio data to the source voice.
    if (SUCCEEDED(hr))
    {
        XAUDIO2_BUFFER buffer{};
        buffer.AudioBytes = static_cast<UINT32>(_audioFile.GetSize());
        buffer.pAudioData = _audioFile.GetData();
        buffer.LoopCount = loopCount;
        hr = _sourceVoice->SubmitSourceBuffer(&buffer);

        const WAVEFORMATEX* format = _audioFile.GetFormat();
        unsigned int streamSize = _audioFile.GetSize();

        // compute audio length, in seconds
        unsigned int numSamples =
            streamSize * 8 /                            // number of bits
            format->wBitsPerSample;                     // bits per sample

        float numSeconds =
            static_cast<float>(numSamples) /            // number of samples
            static_cast<float>(format->nSamplesPerSec); // samples per second

        _duration = numSeconds;
    }

    return (_initStatus = hr);
}

_Use_decl_annotations_
HRESULT OmnidirectionalSound::Initialize(SpeechSynthesisStream^ stream, UINT32 const& loopCount)
{
    auto hr = _audioStream.Initialize(stream);

    if (SUCCEEDED(hr))
    {
        _sourceType = SoundSourceType::Stream;
    }

    ComPtr<IXAPO> xapo;
    if (SUCCEEDED(hr))
    {
        // Passing in nullptr as the first arg for HrtfApoInit initializes the APO with defaults of
        // omnidirectional sound with natural distance decay behavior.
        // CreateHrtfApo will fail with E_NOTIMPL on unsupported platforms.
        hr = CreateHrtfApo(nullptr, &xapo);
    }

    if (SUCCEEDED(hr))
    {
        hr = xapo.As(&_hrtfParams);
    }

    // Set the default environment.
    if (SUCCEEDED(hr))
    {
        hr = _hrtfParams->SetEnvironment(_environment);
    }

    // Initialize an XAudio2 graph that hosts the HRTF xAPO.
    // The source voice is used to submit audio data and control playback.
    if (SUCCEEDED(hr))
    {
        hr = SetupXAudio2(_audioStream.GetFormat(), xapo.Get(), &_xaudio2, &_sourceVoice);
    }

    // Set the initial position.
    if (SUCCEEDED(hr))
    {
        auto hrtfPosition = HrtfPosition{ 0.f, 0.f, 0.f };
        hr = _hrtfParams->SetSourcePosition(&hrtfPosition);
    }

    // Submit audio data to the source voice.
    if (SUCCEEDED(hr))
    {
        XAUDIO2_BUFFER buffer{};
        buffer.AudioBytes = static_cast<UINT32>(_audioStream.GetSize());
        buffer.pAudioData = _audioStream.GetData();
        buffer.LoopCount = loopCount;
        hr = _sourceVoice->SubmitSourceBuffer(&buffer);

        const WAVEFORMATEX* format = _audioStream.GetFormat();
        unsigned int streamSize = _audioStream.GetSize();

        // compute audio length, in seconds
        unsigned int numSamples =
            streamSize * 8 /                            // number of bits
            format->wBitsPerSample;                     // bits per sample

        float numSeconds =
            static_cast<float>(numSamples) /            // number of samples
            static_cast<float>(format->nSamplesPerSec); // samples per second

        _duration = numSeconds;
    }

    return (_initStatus = hr);
}

OmnidirectionalSound::~OmnidirectionalSound()
{
    if (_sourceVoice)
    {
        _sourceVoice->DestroyVoice();
    }
}

HRESULT OmnidirectionalSound::Start()
{
    _lastTick = GetTickCount64();
    return _sourceVoice->Start();
}

HRESULT OmnidirectionalSound::Stop()
{
    return _sourceVoice->Stop();
}

_Use_decl_annotations_
HRESULT OmnidirectionalSound::SetEnvironment(HrtfEnvironment environment)
{
    // Environment can be changed at any time.
    return _hrtfParams->SetEnvironment(environment);
}

//
// This method is called on every dispatcher timer tick.
// Caller passes in the information necessary to compute the source position.
//
_Use_decl_annotations_
HRESULT OmnidirectionalSound::OnUpdate(float angularVelocity, float height, float radius)
{
    auto tick = GetTickCount64();
    auto elapsedTime = tick - _lastTick;
    _lastTick = tick;
    _angle += elapsedTime * angularVelocity;
    _angle = _angle > HRTF_2PI ? (_angle - HRTF_2PI) : _angle;
    auto position = ComputePositionInOrbit(height, radius, _angle);
    return _hrtfParams->SetSourcePosition(&position);
}


HRESULT OmnidirectionalSound::OnUpdate(_In_ float3 position)
{
    auto hrtfPosition = HrtfPosition{ position.x, position.y, position.z };
    return _hrtfParams->SetSourcePosition(&hrtfPosition);
}

_Use_decl_annotations_
HrtfPosition OmnidirectionalSound::ComputePositionInOrbit(float height, float radius, float angle)
{
    // Calculate the position of the source based on height relative to listerner's head, radius of orbit and angle relative to the listener.
    // APO uses right-handed coordinate system where -ve z-axis is forward and +ve z-axis is backward.
    // All coordinates use real-world units (meters).
    float x = radius * sin(angle);
    float z = -radius * cos(angle);
    return HrtfPosition{x, height ,z};
}

