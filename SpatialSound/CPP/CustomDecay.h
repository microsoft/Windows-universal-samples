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
// Sound that demonstrates custom distance decay behavior where the 
// gain applied on the source can be controlled on every frame.
//
class CustomDecaySound
{
public:
    virtual ~CustomDecaySound();
    HRESULT Initialize(_In_ LPCWSTR filename);

    HRESULT Start();
    HRESULT Stop();
    HRESULT OnUpdate(_In_ float x, _In_ float, _In_ float z);
    HRESULT SetEnvironment(_In_ HrtfEnvironment environment);
    HrtfEnvironment GetEnvironment() { return _environment; }

private:
    AudioFileReader                 _audioFile;
    ComPtr<IXAudio2>                _xaudio2;
    IXAudio2SourceVoice*            _sourceVoice = nullptr;
    ComPtr<IXAPOHrtfParameters>     _hrtfParams;
    HrtfEnvironment                 _environment = HrtfEnvironment::Outdoors;
};
