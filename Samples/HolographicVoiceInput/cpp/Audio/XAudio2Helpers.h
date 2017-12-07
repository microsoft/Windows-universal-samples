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

// Sets up XAudio2 for HRTF processing
static HRESULT SetupXAudio2(_In_ const WAVEFORMATEX* format, _In_ IXAPO* xApo, _Outptr_ IXAudio2** xAudio2, _Outptr_ IXAudio2SourceVoice** sourceVoice)
{
    using namespace Microsoft::WRL;

    // Initialize XAudio2 for HRTF processing with "XAUDIO2_1024_QUANTUM" flag which specifies processing frame size of 1024 samples.
    ComPtr<IXAudio2> xAudio2Instance;
    auto hr = XAudio2Create(&xAudio2Instance, XAUDIO2_1024_QUANTUM);

    // HRTF APO expects mono audio data at 48kHz and produces stereo output at 48kHz
    // so we create a stereo mastering voice with specific rendering sample rate of 48kHz.
    // Mastering voice will be automatically destroyed when XAudio2 instance is destroyed.
    IXAudio2MasteringVoice* masteringVoice = nullptr;
    if (SUCCEEDED(hr))
    {
        hr = xAudio2Instance->CreateMasteringVoice(&masteringVoice, 2, 48000);
    }

    // Create a source voice to accept audio data in the specified format.
    IXAudio2SourceVoice* sourceVoiceInstance = nullptr;
    if (SUCCEEDED(hr))
    {
        hr = xAudio2Instance->CreateSourceVoice(&sourceVoiceInstance, format);
    }

    // Create a submix voice that will host the xAPO.
    // This submix voice will be detroyed when XAudio2 instance is destroyed.
    IXAudio2SubmixVoice* submixVoice = nullptr;

    if (SUCCEEDED(hr))
    {
        XAUDIO2_EFFECT_DESCRIPTOR fxDesc{};
        fxDesc.InitialState = TRUE;
        fxDesc.OutputChannels = 2;          // Stereo output
        fxDesc.pEffect = xApo;              // HRTF xAPO set as the effect.

        XAUDIO2_EFFECT_CHAIN fxChain{};
        fxChain.EffectCount = 1;
        fxChain.pEffectDescriptors = &fxDesc;

        XAUDIO2_VOICE_SENDS sends = {};
        XAUDIO2_SEND_DESCRIPTOR sendDesc = {};
        sendDesc.pOutputVoice = masteringVoice;
        sends.SendCount = 1;
        sends.pSends = &sendDesc;

        // HRTF APO expects mono 48kHz input, so we configure the submix voice for that format.
        hr = xAudio2Instance->CreateSubmixVoice(&submixVoice, 1, 48000, 0, 0, &sends, &fxChain);
    }

    // Route the source voice to the submix voice.
    // The complete graph pipeline looks like this -
    // Source Voice -> Submix Voice (HRTF xAPO) -> Mastering Voice
    if (SUCCEEDED(hr))
    {
        XAUDIO2_VOICE_SENDS sends = {};
        XAUDIO2_SEND_DESCRIPTOR sendDesc = {};
        sendDesc.pOutputVoice = submixVoice;
        sends.SendCount = 1;
        sends.pSends = &sendDesc;
        hr = sourceVoiceInstance->SetOutputVoices(&sends);
    }

    // Return the XAudio2 instance and the source voice.
    if (SUCCEEDED(hr))
    {
        *sourceVoice = sourceVoiceInstance;
        *xAudio2 = xAudio2Instance.Detach();
    }

    return hr;
}

