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
#include "BackEndAudioHelpers.h"

// release and zero out a possible NULL pointer. note this will
// do the release on a temp copy to avoid reentrancy issues that can result from
// callbacks durring the release
template <class T> void SafeRelease(__deref_inout_opt T **ppT)
{
    T *pTTemp = *ppT;    // temp copy
    *ppT = nullptr;      // zero the input
    if (pTTemp)
    {
        pTTemp->Release();
    }
}

// Begin of audio helpers
size_t inline GetWaveFormatSize(const WAVEFORMATEX& format)
{
    return (sizeof WAVEFORMATEX) + (format.wFormatTag == WAVE_FORMAT_PCM ? 0 : format.cbSize);
}

void FillPcmFormat(WAVEFORMATEX& format, WORD wChannels, int nSampleRate, WORD wBits)
{
    format.wFormatTag        = WAVE_FORMAT_PCM;
    format.nChannels         = wChannels;
    format.nSamplesPerSec    = nSampleRate;
    format.wBitsPerSample    = wBits;
    format.nBlockAlign       = format.nChannels * (format.wBitsPerSample / 8);
    format.nAvgBytesPerSec   = format.nSamplesPerSec * format.nBlockAlign;
    format.cbSize            = 0;
}

size_t BytesFromDuration(int nDurationInMs, const WAVEFORMATEX& format)
{
    return size_t(nDurationInMs * FLOAT(format.nAvgBytesPerSec) / 1000);
}

size_t SamplesFromDuration(int nDurationInMs, const WAVEFORMATEX& format)
{
    return size_t(nDurationInMs * FLOAT(format.nSamplesPerSec) / 1000);
}


BOOL WaveFormatCompare(const WAVEFORMATEX& format1, const WAVEFORMATEX& format2)
{
    size_t cbSizeFormat1 = GetWaveFormatSize(format1);
    size_t cbSizeFormat2 = GetWaveFormatSize(format2);

    return (cbSizeFormat1 == cbSizeFormat2) && (memcmp(&format1, &format2, cbSizeFormat1) == 0);
}
// End of audio helpers
