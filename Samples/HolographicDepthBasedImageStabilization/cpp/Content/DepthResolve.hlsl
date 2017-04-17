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

#define USE_FAST_GATHER

// These textures will contain normalized depth values in 16-bit unsigned integer format.
Texture2D<min16float> depthTexture : register(t0);
RWBuffer<min16float> outTexture : register(u0);

[numthreads(16, 2, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{ 
    uint outTextureIndex = DTid.y * 79u + DTid.x;

#ifdef USE_FAST_GATHER
    // Simply do a 16x16 reduction.
    int2 sampleStart = DTid.xy * 16;
    outTexture[outTextureIndex] = depthTexture.Load(int3(sampleStart, 0));
#else
    // Do an 8x8 reduction, and also do a 2x2 average.

    int2 sample00Start = DTid.xy * 16;
    min16float sample00 = depthTexture.Load(int3(sample00Start, 0));
    int2 sample01Start = sample00Start + int2(0, 8);
    min16float sample01 = depthTexture.Load(int3(sample01Start, 0));
    int2 sample10Start = sample00Start + int2(8, 0);
    min16float sample10 = depthTexture.Load(int3(sample10Start, 0));
    int2 sample11Start = sample00Start + int2(8, 8);
    min16float sample11 = depthTexture.Load(int3(sample11Start, 0));

    outTexture[outTextureIndex] = (sample00 + sample01 + sample10 + sample11) * min16float(0.25f);
#endif

    return;
}