// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Security::Cryptography;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Creating a file",                                      "SDKTemplate.Scenario1" },
    { "Getting a file's parent folder",                       "SDKTemplate.Scenario2" },
    { "Writing and reading text in a file",                   "SDKTemplate.Scenario3" },
    { "Writing and reading bytes in a file",                  "SDKTemplate.Scenario4" },
    { "Writing and reading using a stream",                   "SDKTemplate.Scenario5" },
    { "Displaying file properties",                           "SDKTemplate.Scenario6" },
    { "Persisting access to a storage item for future use",   "SDKTemplate.Scenario7" },
    { "Copying a file",                                       "SDKTemplate.Scenario8" },
    { "Comparing two files to see if they are the same file", "SDKTemplate.Scenario9" },
    { "Deleting a file",                                      "SDKTemplate.Scenario10" },
    { "Attempting to get a file with no error on failure",    "SDKTemplate.Scenario11" },
};

void MainPage::ValidateFile()
{
    create_task(KnownFolders::GetFolderForUserAsync(nullptr /* current user */, KnownFolderId::PicturesLibrary)).then([this](StorageFolder^ picturesFolder)
    {
        return picturesFolder->TryGetItemAsync(Filename);
    }).then([this](IStorageItem^ item)
    {
        sampleFile = safe_cast<StorageFile^>(item);
        if (item == nullptr)
        {
            // If file doesn't exist, indicate users to use scenario 1
            NotifyUserFileNotExist();
        }
    });
}

void MainPage::NotifyUserFileNotExist()
{
    NotifyUser("The file '" + Filename + "' does not exist. Use scenario one to create this file.", NotifyType::ErrorMessage);
}

// I/O errors are reported as exceptions.
void MainPage::HandleIoException(Platform::COMException^ e, Platform::String^ description)
{
    if (e->HResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
    {
        NotifyUserFileNotExist();
    }
    else if (e->HResult == HRESULT_FROM_WIN32(ERROR_NO_UNICODE_TRANSLATION))
    {
        NotifyUser("File is not UTF-8 encoded.", NotifyType::ErrorMessage);
    }
    else
    {
        NotifyUser(description + ": " + e->Message, NotifyType::ErrorMessage);
    }
}

IBuffer^ MainPage::GetBufferFromString(String^ string)
{
    if (string->IsEmpty())
    {
        return ref new Buffer(0);
    }
    else
    {
        return CryptographicBuffer::ConvertStringToBinary(string, BinaryStringEncoding::Utf8);
    }
}

String^ MainPage::GetStringFromBuffer(IBuffer^ buffer)
{
    // Throws HRESULT_FROM_WIN32(ERROR_NO_UNICODE_TRANSLATION)
    // if the buffer is not properly encoded.
    return CryptographicBuffer::ConvertBinaryToString(BinaryStringEncoding::Utf8, buffer);
}
