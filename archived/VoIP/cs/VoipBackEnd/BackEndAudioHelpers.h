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
#include "windows.h"
#include <audioclient.h>

template <class T> void SafeRelease(__deref_inout_opt T **ppT);

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) { SafeRelease(&x); }
#endif

size_t inline GetWaveFormatSize(const WAVEFORMATEX& format);

void FillPcmFormat(WAVEFORMATEX& format, WORD wChannels, int nSampleRate, WORD wBits);
size_t BytesFromDuration(int nDurationInMs, const WAVEFORMATEX& format);

size_t SamplesFromDuration(int nDurationInMs, const WAVEFORMATEX& format);

BOOL WaveFormatCompare(const WAVEFORMATEX& format1, const WAVEFORMATEX& format2);
