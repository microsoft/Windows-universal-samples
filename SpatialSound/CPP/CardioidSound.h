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

//
// Sound with Cardioid radiation pattern.
//
class CardioidSound
{
public:
    virtual ~CardioidSound();
    HRESULT Initialize(_In_ LPCWSTR filename);
    HRESULT ConfigureApo(_In_ float scaling, _In_ float order);

    HRESULT Start();
    HRESULT Stop();
    HRESULT OnUpdate(_In_ float x, _In_ float, _In_ float z, _In_ float pitch, _In_ float yaw, _In_ float roll);
    HRESULT SetEnvironment(_In_ HrtfEnvironment environment);
    HrtfEnvironment GetEnvironment() { return _environment; }

private:
    HrtfOrientation OrientationFromAngles(float pitch, float yaw, float roll);

private:
    AudioFileReader                 _audioFile;
    ComPtr<IXAudio2>                _xaudio2;
    IXAudio2SourceVoice*            _sourceVoice = nullptr;
    ComPtr<IXAPOHrtfParameters>     _hrtfParams;
    HrtfEnvironment                 _environment = HrtfEnvironment::Outdoors;
};
