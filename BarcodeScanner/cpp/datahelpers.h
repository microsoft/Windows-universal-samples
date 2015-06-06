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

#include <strsafe.h>


inline Platform::String^ GetDataString(Windows::Storage::Streams::IBuffer^ data)
{
    Platform::String^ result = nullptr;

    if (data == nullptr)
    {
        result = ref new Platform::String(L"No data");
    }
    else
    {
        // Just to show that we have the raw data, we'll print the value of the bytes.
        // Arbitrarily limit the number of bytes printed to 20 so the UI isn't overloaded.
        const unsigned int MAX_BYTES_TO_PRINT = 20;
        unsigned int bytesToPrint = (data->Length < MAX_BYTES_TO_PRINT) ? data->Length : MAX_BYTES_TO_PRINT;

        Windows::Storage::Streams::DataReader^ reader = Windows::Storage::Streams::DataReader::FromBuffer(data);

        // Print 3 characters per byte (2 for the value of the byte in hex, 1 for the space), plus room for the elipsis, plus a null terminator
        unsigned int bufferSize = (bytesToPrint * 3) + 3 + 1;

        WCHAR* resultBuffer = new WCHAR[bufferSize];
        ZeroMemory(resultBuffer, bufferSize * sizeof(WCHAR));

        for (unsigned int byteIndex = 0; byteIndex < bytesToPrint; ++byteIndex)
        {
            LPWSTR nextWriteLocation = resultBuffer + (byteIndex * 3);
            unsigned int remainingBufferSpace = bufferSize - (byteIndex * 3);

            if (FAILED(StringCchPrintf(nextWriteLocation, remainingBufferSpace, L"%02X ", reader->ReadByte())))
            {
                delete[] resultBuffer;
                return ref new Platform::String(L"Error");
            }
        }

        if (bytesToPrint < data->Length)
        {
            if (FAILED(StringCchCat(resultBuffer, bufferSize, L"...")))
            {
                delete[] resultBuffer;
                return ref new Platform::String(L"Error");
            }
        }

        result = ref new Platform::String(resultBuffer);
    }

    return result;
}

inline Platform::String^ GetDataLabelString(Windows::Storage::Streams::IBuffer^ data, unsigned int scanDataType)
{
    Platform::String^ result = nullptr;
    // Only certain data types contain encoded text.
    //   To keep this simple, we'll just decode a few of them.
    if (data == nullptr)
    {
        result = ref new Platform::String(L"No data");
    }
    else
    {
        if (scanDataType == Windows::Devices::PointOfService::BarcodeSymbologies::Upca ||
            scanDataType == Windows::Devices::PointOfService::BarcodeSymbologies::UpcaAdd2 ||
            scanDataType == Windows::Devices::PointOfService::BarcodeSymbologies::UpcaAdd5 ||
            scanDataType == Windows::Devices::PointOfService::BarcodeSymbologies::Upce ||
            scanDataType == Windows::Devices::PointOfService::BarcodeSymbologies::UpceAdd2 ||
            scanDataType == Windows::Devices::PointOfService::BarcodeSymbologies::UpceAdd5 ||
            scanDataType == Windows::Devices::PointOfService::BarcodeSymbologies::Ean8 ||
            scanDataType == Windows::Devices::PointOfService::BarcodeSymbologies::TfStd)
        {
            // The UPC, EAN8, and 2 of 5 families encode the digits 0..9
            // which are then sent to the app in a UTF8 string (like "01234")

            // This is not an exhaustive list of symbologies that can be converted to a string

            Windows::Storage::Streams::DataReader^ reader = Windows::Storage::Streams::DataReader::FromBuffer(data);
            result = reader->ReadString(data->Length);
        }
        else
        {
            // Some other symbologies (typically 2-D symbologies) contain binary data that
            //  should not be converted to text.
            result = ref new Platform::String(L"Decoded data unavailable. Raw label data: ");
            result += GetDataString(data);
        }
    }

    return result;
}
