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

//
// ReadBytes.xaml.cpp
// Implementation of the ReadBytes class
//

#include "pch.h"
#include "Scenario2_ReadBytes.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

ReadBytes::ReadBytes()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void ReadBytes::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;
}

/// <summary>
/// This is the click handler for the 'Hex Dump' button.  Open the image file we want to
/// perform a hex dump on.  Then open a sequential-access stream over the file and use
/// ReadBytes() to extract the binary data.  Finally, convert each byte to hexadecimal, and
/// display the formatted output in the HexDump textblock.
/// </summary>
/// <param name="sender">Contains information about the button that fired the event.</param>
/// <param name="e">Contains state information and event data associated with a routed event.</param>
void ReadBytes::HexDump(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Retrieve the uri of the image and use that to load the file.
    Uri^ uri = ref new Uri("ms-appx:///Assets/microsoft-sdk.png");
    create_task(StorageFile::GetFileFromApplicationUriAsync(uri)).then([] (StorageFile^ file)
    {
        // Open a sequential-access stream over the image file.
        return file->OpenSequentialReadAsync();
    }).then(
        // Since this is the last task in the chain, use task<IInputStream ^> instead of IInputStream ^
        // as the parameter here in order to handle any exceptions which might have been thrown in
        // the previous async calls.
        [=] (task<IInputStream ^> inputStream)
    {
        try
        {
            currChunk = 0;
            ReadBytesOutput->Text = "";

            // Use inputStream.get() to pass the input stream returned by OpenSequentialReadAsync() to the
            // DataReader, or to catch any exceptions that might have arisen in the chained async calls.
            DataReader^ dataReader = ref new DataReader(inputStream.get());

            // Read and process the data contained in the input stream.
            ReadLoop(dataReader);
        }
        catch (Exception^ e)
        {
            ReadBytesOutput->Text = e->Message;
        }
    });
}

/// <summary>
/// The read loop recursively loads a chunk of bytes from the stream into the DataReader
/// buffer until all elements are read.  After each load, ReadBytes() retrieves and
/// formats one row worth of bytes at a time.
/// </summary>
/// <param name="dataReader">The DataReader which will retrieve the data from the stream.</param>
void ReadBytes::ReadLoop(DataReader^ dataReader)
{
    create_task(dataReader->LoadAsync(chunkSize)).then(
        // Again, use task<unsigned int> instead of unsigned int as the parameter here in order to
        // handle any exceptions which might have been thrown in the LoadAsync() call.
        [=] (task<unsigned int> bytesLoaded) 
    {
        try
        {
            // Use bytesLoaded.get() to determine how many bytes we have left to print, or
            // to catch any exceptions that might have arisen in the LoadAsync() call.
            int numBytes = bytesLoaded.get();

            // Create a byte array which can hold enough bytes to populate a row of the hex dump.
            auto bytes = ref new Array<byte>(bytesPerRow);

            // Read and print one row at a time.
            int numBytesRemaining = numBytes;
            while (numBytesRemaining >= bytesPerRow)
            {
                // Use the DataReader and ReadBytes() to fill the byte array with one row worth of bytes.
                dataReader->ReadBytes(bytes);

                PrintRow(bytes, (numBytes - numBytesRemaining) + (currChunk * chunkSize));

                numBytesRemaining -= bytesPerRow;
            }

            // If there are any bytes remaining to be read, allocate a new array that will hold
            // the remaining bytes read from the DataReader and print the final row.
            // Note: ReadBytes() fills the entire array so if the array being passed in is larger
            // than what's remaining in the DataReader buffer, an exception will be thrown.
            if (numBytesRemaining > 0)
            {
                bytes = ref new Array<byte>(numBytesRemaining);

                // Use the DataReader and ReadBytes() to fill the byte array with the last row worth of bytes.
                 dataReader->ReadBytes(bytes);

                PrintRow(bytes, (numBytes - numBytesRemaining) + (currChunk * chunkSize));
            }

            currChunk++;

            // If the number of bytes read is anything but the chunk size, then we've just retrieved the last
            // chunk of data from the stream.  Otherwise, keep loading data into the DataReader buffer.
            if (numBytes == chunkSize)
            {
                ReadLoop(dataReader);
            }
        }
        catch (Exception^ e)
        {
            ReadBytesOutput->Text = e->Message;
        }
    });
}

/// <summary>
/// Format and print a row of the hex dump using the data retrieved by ReadBytes().
/// </summary>
/// <param name="bytes">An array that holds the data we want to format and output.</param>
/// <param name="currByte">The first byte of the current row to compute the value of the address.</param>
void ReadBytes::PrintRow(Array<byte>^ bytes, int currByte)
{
    String^ rowStr = "";

    // Add the address of the current byte to the output string.
    rowStr += GetAddress(currByte);

    // Format the output:
    for (unsigned int i = 0; i < bytes->Length; i++)
    {
        // If finished with a segment, add a space.
        if (i % bytesPerSegment == 0)
        {
            rowStr += " ";
        }

        // Convert the current byte value to hex and add it to the output string.
        rowStr += ToHex(bytes[i]);
    }

    // Append the current row to the HexDump textblock.
    ReadBytesOutput->Text += rowStr + "\n";
}

/// <summary>
/// Format the address of the current byte by converting currByte to hexadecimal and then
//  left-padding the string with zeroes so that the address always has 8 digits.
/// </summary>
/// <param name="currByte">Value to be formatted into an address.</param>
/// <returns>The formatted address represnted as a Platform::String ^.</returns>
String^ ReadBytes::GetAddress(int currByte) 
{
    // 8 digits for the address plus '\0'
    int const arraySize = 9;

    wchar_t address[arraySize] = { 0 };

    // Convert to hex.
    swprintf_s(address, arraySize, L"%08x", currByte);

    return ref new String(address);
}

/// <summary>
/// Convert the byte value to hexadecimal and return the result.
/// </summary>
/// <param name="value">Value to be converted to hexadecimal.</param>
/// <returns>The formatted hex value represnted as a Platform::String ^.</returns>
String^ ReadBytes::ToHex(byte value)
{
    // 2 digits for the value plus '\0'
    int const arraySize = 3;

    wchar_t hexValue[arraySize] = { 0 };

    // Convert to hex.
    swprintf_s(hexValue, arraySize, L"%02x", (int) value);

    return ref new String(hexValue);
}
