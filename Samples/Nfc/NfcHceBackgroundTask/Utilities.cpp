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

#include <pch.h>
#include "Utilities.h"

using namespace Concurrency;
using namespace Microsoft::WRL;
using namespace Platform;
using namespace Windows::Globalization;
using namespace Windows::Security::Cryptography::DataProtection;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;

String^ GetCurrentTimeString()
{
    auto calendar = ref new Calendar();
    calendar->SetToNow();

    int milliseconds = calendar->Nanosecond / 1000000;
    String^ strMilliseconds = milliseconds.ToString();

    // Pad zeros
    if (strMilliseconds->Length() == 1)
    {
        strMilliseconds = "00" + strMilliseconds;
    }
    else if (strMilliseconds->Length() == 2)
    {
        strMilliseconds = "0" + strMilliseconds;
    }

    return
        calendar->HourAsPaddedString(2) + ":" +
        calendar->MinuteAsPaddedString(2) + ":" +
        calendar->SecondAsPaddedString(2) + "." +
        strMilliseconds;
}

// Retrieves the raw buffer data from the provided IBuffer object.
// Warning: The lifetime of the returned buffer is controlled by
// the lifetime of the buffer object that's passed to this method.
// When the buffer has been released, the pointer becomes invalid
// and must not be used.
_Use_decl_annotations_
LPBYTE PointerFromIBuffer(IBuffer^ buffer, DWORD* pcbLength)
{
    if (pcbLength != nullptr)
    {
        *pcbLength = buffer->Length;
    }

    // Query the IBufferByteAccess interface.
    ComPtr<IBufferByteAccess> bufferByteAccess;
    ChkHR(reinterpret_cast<IInspectable*>(buffer)->QueryInterface(IID_PPV_ARGS(&bufferByteAccess)));

    // Retrieve the buffer data.
    LPBYTE pbData = nullptr;
    ChkHR(bufferByteAccess->Buffer(&pbData));
    return pbData;
}

_Use_decl_annotations_
IBuffer^ IBufferFromPointer(LPBYTE pbData, DWORD cbData)
{
    auto byteArray = new ArrayReference<unsigned char>(pbData, cbData);
    return IBufferFromArray(reinterpret_cast<Array<unsigned char>^>(byteArray));
}

_Use_decl_annotations_
IBuffer^ IBufferFromArray(Array<unsigned char>^ data)
{
    DataWriter^ dataWriter = ref new DataWriter();
    dataWriter->WriteBytes(data);
    return dataWriter->DetachBuffer();
}

_Use_decl_annotations_
std::wstring ByteArrayToString(Array<byte>^ byteArray)
{
    std::wstring str = L"";
    for (auto b : byteArray)
    {
        wchar_t strByte[3];
        swprintf_s(strByte, L"%02X", b);
        str += strByte;
    }

    return str;
}

void ChkHR(HRESULT hr)
{
    if (FAILED(hr))
    {
        throw ref new COMException(hr);
    }
}

task<Array<byte>^> ReadAndUnprotectFileAsync(String^ filename)
{
    return create_task(ApplicationData::Current->LocalFolder->TryGetItemAsync(filename)).then(
        [](IStorageItem^ item)
        {
            if (item != nullptr
                && item->IsOfType(StorageItemTypes::File))
            {
                // File exists, read it
                return create_task(dynamic_cast<StorageFile^>(item)->OpenReadAsync()).then([](IRandomAccessStreamWithContentType^ stream)
                {
                    if (stream->Size > MAXDWORD32)
                    {
                        throw ref new Exception(E_FAIL);
                    }

                    auto size = static_cast<unsigned int>(stream->Size);
                    auto buffer = ref new Buffer(size);
                    return stream->ReadAsync(buffer, size, InputStreamOptions::None);
                }).then([](IBuffer^ buffer)
                {
                    // Now unprotect/decrypt it
                    auto provider = ref new DataProtectionProvider();
                    return provider->UnprotectAsync(buffer);
                }).then([](IBuffer^ buffer) -> Array<byte>^
                {
                    auto data = ref new Array<byte>(buffer->Length);
                    DataReader::FromBuffer(buffer)->ReadBytes(data);
                    return data;
                });
            }
            else
            {
                // File doesn't exist, return nullptr
                return task_from_result<Array<byte>^>(nullptr);
            }
    });
}

task<IBuffer^> ReadFileToBufferAsync(String^ filename)
{
    return create_task(ApplicationData::Current->LocalFolder->TryGetItemAsync(filename)).then(
        [](IStorageItem^ item)
    {
        if (item != nullptr
            && item->IsOfType(StorageItemTypes::File))
        {
            // File exists, read it
            return create_task(dynamic_cast<StorageFile^>(item)->OpenReadAsync()).then([](IRandomAccessStreamWithContentType^ stream)
            {
                if (stream->Size > MAXDWORD32)
                {
                    throw ref new Exception(E_FAIL);
                }

                auto size = static_cast<unsigned int>(stream->Size);
                auto buffer = ref new Buffer(size);
                return stream->ReadAsync(buffer, size, InputStreamOptions::None);
            });
        }
        else
        {
            // File doesn't exist, return nullptr
            return task_from_result<IBuffer^>(nullptr);
        }
    });
}

void AppendFile(String^ filename, String^ data)
{
    auto file = create_task(ApplicationData::Current->LocalFolder->CreateFileAsync(
        filename,
        CreationCollisionOption::OpenIfExists)).get();

    create_task(FileIO::AppendTextAsync(file, data)).wait();
}

_Use_decl_annotations_
HRESULT ParseApdu(
    BYTE* pbCommandApdu,
    DWORD cbCommandApdu,
    USHORT* pusClaIns,
    BYTE* pbP1,
    BYTE* pbP2,
    BYTE** ppbPayload,
    DWORD* pcbPayload,
    DWORD* pcbLE)
{
    if (cbCommandApdu < 4)
    {
        // Invalid data, APDU should be minimum 4 bytes
        return E_FAIL;
    }

    // APDU Format is:
    // 0   1   2  3  4
    // CLA INS P1 P2 <NC> <cmd> <LE>

    // Grab the CLA, INS, P1 and P2
    memcpy(pusClaIns, pbCommandApdu, sizeof(USHORT));
    *pusClaIns = ntohs(*pusClaIns);
    *pbP1 = pbCommandApdu[2];
    *pbP2 = pbCommandApdu[3];
    *ppbPayload = nullptr;
    *pcbPayload = 0;
    *pcbLE = 0;
    if (cbCommandApdu == 5)
    {
        // Non-extended APDU with an LE byte
        *pcbLE = pbCommandApdu[4];
        if (*pcbLE == 0)
        {
            *pcbLE = 256;
        }
    }
    else if (cbCommandApdu > 5)
    {
        if (pbCommandApdu[4] != 0)
        {
            // Non-extended APDU with command data
            *pcbPayload = pbCommandApdu[4];

            if ((*pcbPayload + 5) == cbCommandApdu)
            {
                // No LE
            }
            else if ((*pcbPayload + 6) == cbCommandApdu)
            {
                // LE byte at the end
                *pcbLE = pbCommandApdu[*pcbPayload + 5];
                if (*pcbLE == 0)
                {
                    *pcbLE = 256;
                }
            }
            else
            {
                // Invalid length
                return E_FAIL;
            }
            *ppbPayload = pbCommandApdu + 5;
        }
        else
        {
            // Extended APDU

            if (cbCommandApdu == 7)
            {
                // Extended APDU with no command payload and only LE
                memcpy(pcbLE, pbCommandApdu + 5, sizeof(USHORT));
                *pcbLE = ntohs((u_short)*pcbLE);
                if (*pcbLE == 0)
                {
                    *pcbLE = 65536;
                }
            }
            else if (cbCommandApdu >= 8)
            {
                // We have an extended command payload
                memcpy(pcbPayload, pbCommandApdu + 5, sizeof(USHORT));
                *pcbPayload = ntohs((u_short)*pcbPayload);

                if (*pcbPayload + 9 == cbCommandApdu)
                {
                    // There is an extended LE at the end
                    memcpy(pcbLE, pbCommandApdu + 7 + *pcbPayload, sizeof(USHORT));
                    *pcbLE = ntohs((u_short)*pcbLE);
                    if (*pcbLE == 0)
                    {
                        *pcbLE = 65536;
                    }
                }
                else if (*pcbPayload + 7 == cbCommandApdu)
                {
                    // No LE
                }
                else
                {
                    return E_FAIL;
                }

                *ppbPayload = pbCommandApdu + 7;
            }
            else
            {
                return E_FAIL;
            }
        }
    }

    return S_OK;
}
