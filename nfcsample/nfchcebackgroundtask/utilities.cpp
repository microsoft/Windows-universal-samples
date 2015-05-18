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
#include <wrl.h>
#include <robuffer.h>
#include <ppltasks.h>
#include "Utilities.h"
#include <winsock2.h>

using namespace Windows::Storage;
using namespace Concurrency;
using namespace Windows::Storage::Streams;
using namespace Microsoft::WRL;

// Retrieves the raw buffer data from the provided IBuffer object.
// Warning: The lifetime of the returned buffer is controlled by
// the lifetime of the buffer object that's passed to this method.
// When the buffer has been released, the pointer becomes invalid
// and must not be used.
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


IBuffer^ IBufferFromPointer(_In_ LPBYTE pbData, _In_ DWORD cbData)
{
	auto byteArray = new Platform::ArrayReference<unsigned char>(pbData, cbData);
	return IBufferFromArray(reinterpret_cast<Platform::Array<unsigned char>^>(byteArray));
}

IBuffer^ IBufferFromArray(_In_ Platform::Array<unsigned char>^ data)
{
	DataWriter^ dataWriter = ref new DataWriter();
	dataWriter->WriteBytes(data);
	return dataWriter->DetachBuffer();
}

void ChkHR(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw ref new Platform::COMException(hr);
	}
}

Concurrency::task<Platform::Array<byte>^> ReadAndUnprotectFileAsync(Platform::String^ filename)
{
	return create_task(Windows::Storage::ApplicationData::Current->LocalFolder->TryGetItemAsync(filename)).then(
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
                        throw ref new Platform::Exception(E_FAIL);
                    }
                    auto size = static_cast<unsigned int>(stream->Size);
                    auto buffer = ref new Buffer(size);
                    return stream->ReadAsync(buffer, size, Streams::InputStreamOptions::None);
                }).then([](IBuffer^ buffer)
                {
                    // Now unprotect/decrypt it
                    auto provider = ref new Windows::Security::Cryptography::DataProtection::DataProtectionProvider();
                    return provider->UnprotectAsync(buffer);
                }).then([](IBuffer^ buffer) -> Platform::Array<byte>^
                {
                    auto data = ref new Platform::Array<byte>(buffer->Length);
                    DataReader::FromBuffer(buffer)->ReadBytes(data);
                    return data;
                });
            }
            else
            {
                // File doesn't exist, return nullptr
                return concurrency::task_from_result<Platform::Array<byte>^>(nullptr);
            }
    });
}

void AppendFile(Platform::String^ filename, Platform::String^ data)
{
    auto file = create_task(Windows::Storage::ApplicationData::Current->LocalFolder->CreateFileAsync(
        filename,
        CreationCollisionOption::OpenIfExists)).get();

    create_task(FileIO::AppendTextAsync(file, data)).wait();
}

HRESULT ParseApdu(_In_ BYTE* pbCommandApdu, _In_ DWORD cbCommandApdu, _Out_ USHORT* pusClaIns, _Out_ BYTE* pbP1, _Out_ BYTE* pbP2, _Out_ BYTE** ppbPayload, _Out_ DWORD* pcbPayload, _Out_ DWORD* pcbLE)
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

