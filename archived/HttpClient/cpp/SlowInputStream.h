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

#include "pch.h"

namespace SDKTemplate
{
    public ref class SlowInputStream sealed : public Windows::Storage::Streams::IInputStream
    {
    public:
        SlowInputStream(unsigned long long length);
        virtual ~SlowInputStream(void);
        virtual Windows::Foundation::IAsyncOperationWithProgress<
            Windows::Storage::Streams::IBuffer^,
            unsigned int>^ ReadAsync(
                Windows::Storage::Streams::IBuffer^ buffer,
                unsigned int count,
                Windows::Storage::Streams::InputStreamOptions options);

    private:
        unsigned long long length;
        unsigned long long position;
    };
}
