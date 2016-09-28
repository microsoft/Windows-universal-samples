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

#include <streambuf>
#include <robuffer.h>
#include <wrl.h>

using namespace Microsoft::WRL;
using namespace Windows::Storage::Streams;

namespace WindowsHolographicCodeSamples
{
    template <typename t = byte>
    t* GetDataFromIBuffer(Windows::Storage::Streams::IBuffer^ container)
    {
        if (container == nullptr)
        {
            return nullptr;
        }

        unsigned int bufferLength = container->Length;

        if (!(bufferLength > 0))
        {
            return nullptr;
        }

        HRESULT hr = S_OK;

        ComPtr<IUnknown> pUnknown = reinterpret_cast<IUnknown*>(container);
        ComPtr<IBufferByteAccess> spByteAccess;
        hr = pUnknown.As(&spByteAccess);
        if (FAILED(hr))
        {
            return nullptr;
        }

        byte* pRawData = nullptr;
        hr = spByteAccess->Buffer(&pRawData);
        if (FAILED(hr))
        {
            return nullptr;
        }

        return reinterpret_cast<t*>(pRawData);
    }
}
