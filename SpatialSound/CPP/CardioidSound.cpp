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
HRESULT CardioidSound::Initialize(LPCWSTR filename)
{
    auto hr = _audioFile.Initialize(filename);
    if (SUCCEEDED(hr))
    {
        // Initialize with "Scaling" fully directional and "Order" with broad radiation pattern.
        // As the order goes higher, the cardioid directivity region becomes narrower.
        // Any direct path signal outside of the directivity region will be attenuated based on the scaling factor.
        // For example, if scaling is set to 1 (fully directional) the direct path signal outside of the directivity
        // region will be fully attenuated and only the reflections from the environment will be audible.
        hr = ConfigureApo(1.0f, 4.0f);
    }
    return hr;
}

CardioidSound::~CardioidSound()
{
    if (_sourceVoice)
    {
        _sourceVoice->DestroyVoice();
    }
}

_Use_decl_annotations_
HRESULT CardioidSound::ConfigureApo(float scaling, float order)
{
    // Directivity is specified at xAPO instance initialization and cannot be changed per frame.
    // To change directivity, we'll need to stop audio processing and reinitialize another APO instance with the new directivity.
    if (_xaudio2)
    {
        _xaudio2->StopEngine();
        _xaudio2.Reset();
    }

    if (_hrtfParams)
    {
        _hrtfParams.Reset();
    }

    // Cardioid directivity configuration
    HrtfDirectivityCardioid cardioid;
    cardioid.directivity.type = HrtfDirectivityType::Cardioid;
    cardioid.directivity.scaling = scaling;
    cardioid.order = order;

    // APO intialization
    HrtfApoInit apoInit;
    apoInit.directivity = &cardioid.directivity;
    apoInit.distanceDecay = nullptr;                // This specifies natural distance decay behavior (simulates real world)

    // CreateHrtfApo will fail with E_NOTIMPL on unsupported platforms.
    ComPtr<IXAPO> xapo;
    auto hr = CreateHrtfApo(&apoInit, &xapo);

    if (SUCCEEDED(hr))
    {
        hr = xapo.As(&_hrtfParams);
    }

    // Set the initial environment.
    // Environment settings configure the "distance cues" used to compute the early and late reverberations.
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

HRESULT CardioidSound::Start()
{
    return _sourceVoice->Start();
}

HRESULT CardioidSound::Stop()
{
    return _sourceVoice->Stop();
}

_Use_decl_annotations_
HRESULT CardioidSound::SetEnvironment(HrtfEnvironment environment)
{
    // Environment can be changed at any time.
    return _hrtfParams->SetEnvironment(environment);
}

//
// This method is called on every dispatcher timer tick.
// Source position is provided in right-handed coordinate system and
// source orientation as Yaw, Pitch, Roll in radians. This information
// is relative to the listener's head (listener is always at {0, 0, 0}).
//
_Use_decl_annotations_
HRESULT CardioidSound::OnUpdate(float x, float y, float z, float pitch, float yaw, float roll)
{
    auto hr = S_OK;
    if (_hrtfParams)
    {
        auto position = HrtfPosition{ x, y, z };
        hr = _hrtfParams->SetSourcePosition(&position);
        if (SUCCEEDED(hr))
        {
            auto sourceOrientation = OrientationFromAngles(pitch, yaw, roll);
            hr = _hrtfParams->SetSourceOrientation(&sourceOrientation);
        }
    }
    return hr;
}

// Helper to translate from pitch, yaw, roll to the rotation matrix.
_Use_decl_annotations_
HrtfOrientation CardioidSound::OrientationFromAngles(float pitch, float yaw, float roll)
{
    // Negate all angles for right handed coordinate system.
    DirectX::XMFLOAT3 angles{ -pitch, -yaw, -roll };
    DirectX::XMVECTOR vector = DirectX::XMLoadFloat3(&angles);
    DirectX::XMMATRIX rm = DirectX::XMMatrixRotationRollPitchYawFromVector(vector);
    DirectX::XMFLOAT3X3 rm33{};
    DirectX::XMStoreFloat3x3(&rm33, rm);
    return HrtfOrientation{ rm33._11, rm33._12, rm33._13, rm33._21, rm33._22, rm33._23, rm33._31, rm33._32, rm33._33 };
}

