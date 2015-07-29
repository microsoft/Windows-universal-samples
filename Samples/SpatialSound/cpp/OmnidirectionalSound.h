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
// Sound with omnidirectional radiation pattern i.e. emits sound equally in all directions.
//
class OmnidirectionalSound
{
public:
    virtual ~OmnidirectionalSound();
    HRESULT Initialize(_In_ LPCWSTR filename);

    HRESULT Start();
    HRESULT Stop();
    HRESULT OnUpdate(_In_ float angularVelocity, _In_ float height, _In_ float radius);
    HRESULT SetEnvironment(_In_ HrtfEnvironment environment);
    HrtfEnvironment GetEnvironment() { return _environment; }

private:
    HrtfPosition ComputePositionInOrbit(_In_ float height, _In_ float radius, _In_ float angle);

private:
    AudioFileReader                 _audioFile;
    ComPtr<IXAudio2>                _xaudio2;
    IXAudio2SourceVoice*            _sourceVoice = nullptr;
    ComPtr<IXAPOHrtfParameters>     _hrtfParams;
    HrtfEnvironment                 _environment = HrtfEnvironment::Outdoors;
    ULONGLONG                       _lastTick = 0;
    float                           _angle = 0;
};
