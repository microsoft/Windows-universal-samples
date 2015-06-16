//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

//
// ToneSampleGenerator.h
//

#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <limits.h>
#include "MainPage.xaml.h"

template<typename T> T Convert(double Value);

namespace SDKSample
{
 namespace WASAPIAudio
 {
  class ToneSampleGenerator
  {
   public:
   ToneSampleGenerator();
   ~ToneSampleGenerator();
    
   Platform::Boolean IsEOF(){ return (m_SampleQueue == nullptr);  };
   UINT32 GetBufferLength(){ return (m_SampleQueue !=  nullptr ? m_SampleQueue->BufferSize : 0 ); };
   void Flush();

   HRESULT GenerateSampleBuffer(DWORD Frequency, UINT32 FramesPerPeriod, WAVEFORMATEX *wfx);
   HRESULT FillSampleBuffer(UINT32 BytesToRead, BYTE *Data);

   private:
   template <typename T>
   void GenerateSineSamples(BYTE *Buffer, size_t BufferLength, DWORD Frequency, WORD ChannelCount, DWORD SamplesPerSecond, double Amplitude, double *InitialTheta);

   private:
   RenderBuffer *m_SampleQueue;
   RenderBuffer **m_SampleQueueTail;
  };
 }
}

