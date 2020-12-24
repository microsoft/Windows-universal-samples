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
#include <sstream>
#include <iomanip>
#include "DataHelpers.h"

namespace winrt
{
    using namespace Windows::Storage::Streams;
    using namespace Windows::Devices::PointOfService;
}

std::wstring GetDataString(winrt::IBuffer const& data)
{
    if (!data)
    {
        return L"No data";
    }

    // Just to show that we have the raw data, we'll print the value of the bytes.
    // Arbitrarily limit the number of bytes printed so the UI isn't overloaded.
    const unsigned int MAX_BYTES_TO_PRINT = 20;
    unsigned int byteCount = (std::min)(data.Length(), MAX_BYTES_TO_PRINT);

    std::wstringstream buffer;
    buffer << std::setfill(L'0') << std::hex;
    for (uint32_t value : winrt::array_view(data.data(), byteCount))
    {
        buffer << std::setw(2) << value  << L" ";
    }

    if (byteCount < data.Length())
    {
        buffer << L"...";
    }

    return buffer.str();
}

winrt::hstring GetDataLabelString(winrt::IBuffer const& data, uint32_t scanDataType)
{
    // Only certain data types contain encoded text.
    //   To keep this simple, we'll just decode a few of them.
    if (!data)
    {
        return L"No data";
    }

    if (scanDataType == winrt::BarcodeSymbologies::Upca() ||
        scanDataType == winrt::BarcodeSymbologies::UpcaAdd2() ||
        scanDataType == winrt::BarcodeSymbologies::UpcaAdd5() ||
        scanDataType == winrt::BarcodeSymbologies::Upce() ||
        scanDataType == winrt::BarcodeSymbologies::UpceAdd2() ||
        scanDataType == winrt::BarcodeSymbologies::UpceAdd5() ||
        scanDataType == winrt::BarcodeSymbologies::Ean8() ||
        scanDataType == winrt::BarcodeSymbologies::TfStd() ||
        scanDataType == winrt::BarcodeSymbologies::OcrA() ||
        scanDataType == winrt::BarcodeSymbologies::OcrB())
    {
        // The UPC, EAN8, and 2 of 5 families encode the digits 0..9
        // which are then sent to the app in a UTF8 string (like "01234")

        // This is not an exhaustive list of symbologies that can be converted to a string

        winrt::DataReader reader = winrt::DataReader::FromBuffer(data);
        return reader.ReadString(data.Length());
    }
    else
    {
        // Some other symbologies (typically 2-D symbologies) contain binary data that
        //  should not be converted to text.
        return winrt::hstring{ L"Decoded data unavailable. Raw label data: " + GetDataString(data) };
    }
}
