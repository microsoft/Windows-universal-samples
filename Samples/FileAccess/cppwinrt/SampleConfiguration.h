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

namespace winrt::SDKTemplate
{
    struct SampleState
    {
        static hstring const& FileName() { return fileName; }

        static Windows::Storage::StorageFile const& SampleFile() { return sampleFile; }
        static void SampleFile(Windows::Storage::StorageFile value) { sampleFile = std::move(value); }

        static fire_and_forget ValidateFile();
        static void NotifyUserFileNotExist();
        static Windows::Storage::Streams::IBuffer GetBufferFromString(hstring const& string);
        static hstring GetStringFromBuffer(Windows::Storage::Streams::IBuffer const& buffer);

        static hstring const& MruToken() { return mruToken; }
        static void MruToken(hstring value) { mruToken = std::move(value); }

        static hstring const& FalToken() { return falToken; }
        static void FalToken(hstring value) { falToken = std::move(value); }

    private:
        static hstring fileName;
        static Windows::Storage::StorageFile sampleFile;
        static hstring mruToken;
        static hstring falToken;
    };
}
