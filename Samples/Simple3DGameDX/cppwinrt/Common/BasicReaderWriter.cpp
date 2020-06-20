// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "BasicReaderWriter.h"

using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::ApplicationModel;

BasicReaderWriter::BasicReaderWriter()
{
    m_location = Package::Current().InstalledLocation();
}

BasicReaderWriter::BasicReaderWriter(
    _In_ winrt::Windows::Storage::StorageFolder folder
    )
    : m_location(std::move(folder))
{
    winrt::hstring path = m_location.Path();
    if (path.size() == 0)
    {
        // Applications are not permitted to access certain
        // folders, such as the Documents folder, using this
        // code path. In such cases, the Path property for
        // the folder will be an empty string.
        winrt::throw_hresult(E_FAIL);
    }
}

std::vector<byte> BasicReaderWriter::ReadData(
    _In_ winrt::hstring const& filename
    )
{
    CREATEFILE2_EXTENDED_PARAMETERS extendedParams = { 0 };
    extendedParams.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
    extendedParams.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
    extendedParams.dwFileFlags = FILE_FLAG_SEQUENTIAL_SCAN;
    extendedParams.dwSecurityQosFlags = SECURITY_ANONYMOUS;
    extendedParams.lpSecurityAttributes = nullptr;
    extendedParams.hTemplateFile = nullptr;

    winrt::file_handle file{
        CreateFile2(
            filename.c_str(),
            GENERIC_READ,
            FILE_SHARE_READ,
            OPEN_EXISTING,
            &extendedParams
            )
    };
    winrt::check_bool(bool{ file });
    if (file.get() == INVALID_HANDLE_VALUE)
    {
        winrt::throw_hresult(E_FAIL);
    }

    FILE_STANDARD_INFO fileInfo = { 0 };
    if (!GetFileInformationByHandleEx(
        file.get(),
        FileStandardInfo,
        &fileInfo,
        sizeof(fileInfo)
        ))
    {
        winrt::throw_hresult(E_FAIL);
    }

    if (fileInfo.EndOfFile.HighPart != 0)
    {
        winrt::throw_hresult(E_OUTOFMEMORY);
    }

    std::vector<byte> fileData(fileInfo.EndOfFile.LowPart);

    if (!ReadFile(
        file.get(),
        fileData.data(),
        (DWORD)fileData.size(),
        nullptr,
        nullptr
        ))
    {
        winrt::throw_hresult(E_FAIL);
    }

    return fileData;
}

winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::IBuffer> BasicReaderWriter::ReadDataAsync(
    _In_ winrt::hstring const& filename
    )
{
    StorageFile file{ co_await m_location.GetFileAsync(filename) };
    co_return co_await FileIO::ReadBufferAsync(file);
}

uint32_t BasicReaderWriter::WriteData(
    _In_ winrt::hstring const& filename,
    _In_ std::vector<byte> const& fileData
    )
{
    CREATEFILE2_EXTENDED_PARAMETERS extendedParams = { 0 };
    extendedParams.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
    extendedParams.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
    extendedParams.dwFileFlags = FILE_FLAG_SEQUENTIAL_SCAN;
    extendedParams.dwSecurityQosFlags = SECURITY_ANONYMOUS;
    extendedParams.lpSecurityAttributes = nullptr;
    extendedParams.hTemplateFile = nullptr;

    winrt::file_handle file{
        CreateFile2(
            filename.c_str(),
            GENERIC_WRITE,
            0,
            CREATE_ALWAYS,
            &extendedParams
            )
    };
    winrt::check_bool(bool{ file });
    if (file.get() == INVALID_HANDLE_VALUE)
    {
        winrt::throw_hresult(E_FAIL);
    }

    DWORD numBytesWritten{ 0 };
    if (
        !WriteFile(
            file.get(),
            fileData.data(),
            (DWORD)fileData.size(),
            &numBytesWritten,
            nullptr
            ) ||
        numBytesWritten != fileData.size()
        )
    {
        winrt::throw_hresult(E_FAIL);
    }

    return numBytesWritten;
}

winrt::Windows::Foundation::IAsyncAction BasicReaderWriter::WriteDataAsync(
    _In_ winrt::hstring const& filename,
    _In_ std::vector<byte> const& fileData
    )
{
    StorageFile file{ co_await m_location.CreateFileAsync(filename, CreationCollisionOption::ReplaceExisting) };
    co_await FileIO::WriteBytesAsync(file, fileData);
}
