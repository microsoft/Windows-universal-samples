// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

// A simple reader/writer class that provides support for reading and writing
// files on disk. Provides synchronous and asynchronous methods.

class BasicReaderWriter
{
public:
    BasicReaderWriter();

    BasicReaderWriter(
        _In_ winrt::Windows::Storage::StorageFolder folder
        );

    std::vector<byte> ReadData(
        _In_ winrt::hstring const& filename
        );

    winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::IBuffer> ReadDataAsync(
        _In_ winrt::hstring const& filename
        );

    uint32_t WriteData(
        _In_ winrt::hstring const& filename,
        _In_ std::vector<byte> const& fileData
        );

    winrt::Windows::Foundation::IAsyncAction WriteDataAsync(
        _In_ winrt::hstring const& filename,
        _In_ std::vector<byte> const& fileData
        );

private:
    winrt::Windows::Storage::StorageFolder m_location{ nullptr };
};