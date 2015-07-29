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


_Use_decl_annotations_
HRESULT CustomDecaySound::Initialize(LPCWSTR filename)
{
    auto hr = _audioFile.Initialize(filename);

    ComPtr<IXAPO> xapo;
    if (SUCCEEDED(hr))
    {
        HrtfDistanceDecay customDecay;
        customDecay.type                = HrtfDistanceDecayType::CustomDecay;       // Custom decay behavior, we'll pass in the gain value on every frame.
        customDecay.maxGain             = 0;                                        // 0dB max gain
        customDecay.minGain             = -96.0f;                                   // -96dB min gain
        customDecay.unityGainDistance   = HRTF_DEFAULT_UNITY_GAIN_DISTANCE;         // Default unity gain distance
        customDecay.cutoffDistance      = HRTF_DEFAULT_CUTOFF_DISTANCE;             // Default cutoff distance

        // Setting the directivity to nullptr specifies omnidirectional sound.
        HrtfApoInit init;
        init.directivity = nullptr;
        init.distanceDecay = &customDecay;

        // CreateHrtfApo will fail with E_NOTIMPL on unsupported platforms.
        hr = CreateHrtfApo(&init, &xapo);
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

    // Submit audio data to the source voice
    if (SUCCEEDED(hr))
    {
        XAUDIO2_BUFFER buffer{};
        buffer.AudioBytes = static_cast<UINT32>(_audioFile.GetSize());
        buffer.pAudioData = _audioFile.GetData();
        buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
        hr = _sourceVoice->SubmitSourceBuffer(&buffer);
    }
    return hr;
}

CustomDecaySound::~CustomDecaySound()
{
    if (_sourceVoice)
    {
        _sourceVoice->DestroyVoice();
    }
}

HRESULT CustomDecaySound::Start()
{
    return _sourceVoice->Start();
}

HRESULT CustomDecaySound::Stop()
{
    return _sourceVoice->Stop();
}

_Use_decl_annotations_
HRESULT CustomDecaySound::SetEnvironment(HrtfEnvironment environment)
{
    return _hrtfParams->SetEnvironment(environment);
}

// Called on every world update tick
_Use_decl_annotations_
HRESULT CustomDecaySound::OnUpdate(float x, float y, float z)
{
    auto hr = S_OK;
    if (_hrtfParams)
    {
        auto position = HrtfPosition{ x, y, z };
        hr = _hrtfParams->SetSourcePosition(&position);
        if (SUCCEEDED(hr))
        {
            // Calculate the total distance between the listener and the source
            auto distance = sqrt(x*x + y*y + z*z);
            // If the source is closer than 6m apply gain based on the distance,
            // If the source is beyond 6m, attenuate by -12dB,
            // If the source is beyond 9m, attenuate by -96dB (silent).
            auto gain = distance <= 6.0f ? (0 - (2 * distance)) : (distance > 6.0f && distance <= 9.0f) ? -12.0f : -96.0f;
            hr = _hrtfParams->SetSourceGain(gain);
        }
    }
    return hr;
}
