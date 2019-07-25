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
#include <winrt/SDKTemplate.h>
#include "MainPage.h"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation::Collections;
using namespace Windows::Security::Cryptography;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace SDKTemplate;

hstring implementation::MainPage::FEATURE_NAME()
{
    return L"FileAccess C++/WinRT Sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
    {
        Scenario{ L"Creating a file", xaml_typename<SDKTemplate::Scenario1>() },
        Scenario{ L"Getting a file's parent folder", xaml_typename<SDKTemplate::Scenario2>() },
        Scenario{ L"Writing and reading text in a file", xaml_typename<SDKTemplate::Scenario3>() },
        Scenario{ L"Writing and reading bytes in a file", xaml_typename<SDKTemplate::Scenario4>() },
        Scenario{ L"Writing and reading using a stream", xaml_typename<SDKTemplate::Scenario5>() },
        Scenario{ L"Displaying file properties", xaml_typename<SDKTemplate::Scenario6>() },
        Scenario{ L"Persisting access to a storage item for future use", xaml_typename<SDKTemplate::Scenario7>() },
        Scenario{ L"Copying a file", xaml_typename<SDKTemplate::Scenario8>() },
        Scenario{L"Comparing two files to see if they are the same file", xaml_typename<SDKTemplate::Scenario9>() },
        Scenario{L"Deleting a file", xaml_typename<SDKTemplate::Scenario10>() },
        Scenario{L"Attempting to get a file with no error on failure", xaml_typename<SDKTemplate::Scenario11>() },
    });

hstring SampleState::fileName{ L"sample.dat" };
Windows::Storage::StorageFile SampleState::sampleFile{ nullptr };
hstring SampleState::mruToken;
hstring SampleState::falToken;

fire_and_forget SampleState::ValidateFile()
{
    StorageFolder picturesLibrary = co_await KnownFolders::GetFolderForUserAsync(nullptr /* current user */, KnownFolderId::PicturesLibrary);
    sampleFile = (co_await picturesLibrary.TryGetItemAsync(fileName)).try_as<StorageFile>();
    if (sampleFile == nullptr)
    {
        // If file doesn't exist, indicate users to use scenario 1
        NotifyUserFileNotExist();
    }
}

void SampleState::NotifyUserFileNotExist()
{
    MainPage::Current().NotifyUser(
        L"The file '" + fileName +  L"' does not exist. Use scenario one to create this file.",
        NotifyType::ErrorMessage);

}

IBuffer SampleState::GetBufferFromString(hstring const& string)
{
    if (string.empty())
    {
        return Buffer(0U);
    }
    else
    {
        return CryptographicBuffer::ConvertStringToBinary(string, BinaryStringEncoding::Utf8);
    }
}

hstring SampleState::GetStringFromBuffer(IBuffer const& buffer)
{
    // Throws HRESULT_FROM_WIN32(ERROR_NO_UNICODE_TRANSLATION)
    // if the buffer is not properly encoded.
    return CryptographicBuffer::ConvertBinaryToString(BinaryStringEncoding::Utf8, buffer);
}
