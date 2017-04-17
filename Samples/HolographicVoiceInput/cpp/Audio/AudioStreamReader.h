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

#include <vector>

//
// Helper class uses MediaFoundation to read audio files
//
class AudioStreamReader
{
public:
    virtual ~AudioStreamReader() {}
    HRESULT Initialize(_In_ Windows::Media::SpeechSynthesis::SpeechSynthesisStream^ speechSynthesisStream);

    const WAVEFORMATEX* GetFormat() const
    {
        return &_format;
    }

    size_t GetSize() const
    {
        return _audioData.size();
    }

    const BYTE* GetData() const
    {
        return _audioData.data();
    }

private:
    WAVEFORMATEX        _format;
    std::vector<BYTE>   _audioData;
};