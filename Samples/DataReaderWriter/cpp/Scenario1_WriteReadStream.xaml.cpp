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
// WriteReadStream.xaml.cpp
// Implementation of the WriteReadStream class
//

#include "pch.h"
#include "Scenario1_WriteReadStream.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Array<String^>^ _inputElements = ref new Array<String^>
{
    "Hello", "World", "1 2 3 4 5", "Très bien!", "Goodbye"
};

WriteReadStream::WriteReadStream()
{
    InitializeComponent();

    // Populate the text block with the input elements.
    ElementsToWrite->Text = "";
    for (unsigned int i = 0; i < _inputElements->Length; i++)
    {
        ElementsToWrite->Text += _inputElements[i] + ";";
    }
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void WriteReadStream::OnNavigatedTo(NavigationEventArgs^ e)
{
    // Get a pointer to our main page.
    rootPage = MainPage::Current;
}

/// <summary>
/// This is the click handler for the 'Copy Strings' button.  Here we will parse the
/// strings contained in the ElementsToWrite text block, write them to a stream using
/// DataWriter, retrieve them using DataReader, and output the results in the
/// ElementsRead text block.
/// </summary>
/// <param name="sender">Contains information about the button that fired the event.</param>
/// <param name="e">Contains state information and event data associated with a routed event.</param>
void WriteReadStream::TransferData(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Initialize the in-memory stream where data will be stored.
    InMemoryRandomAccessStream^ stream = ref new InMemoryRandomAccessStream();

    // Create the DataWriter object backed by the in-memory stream.  When
    // dataWriter is deleted, it will also close the underlying stream.
    DataWriter^ dataWriter = ref new DataWriter(stream);
    dataWriter->UnicodeEncoding = UnicodeEncoding::Utf8;
    dataWriter->ByteOrder = ByteOrder::LittleEndian;

    // Create the data reader by using the input stream set at position 0 so that 
    // the stream will be read from the beginning regardless of where the position
    // the original stream ends up in after the store.
    DataReader^ dataReader = ref new DataReader(stream);
    // The encoding and byte order need to match the settings of the writer we previously used.
    dataReader->UnicodeEncoding = UnicodeEncoding::Utf8;
    dataReader->ByteOrder = ByteOrder::LittleEndian;

    // Write the input data to the output stream.  Serialize the elements by writing
    // each string separately, preceded by its length.
    for (unsigned int i = 0; i < _inputElements->Length; i++)
    {
        unsigned int inputElementSize = dataWriter->MeasureString(_inputElements[i]);
        dataWriter->WriteUInt32(inputElementSize);
        dataWriter->WriteString(_inputElements[i]);
    }

    // Send the contents of the writer to the backing stream.
    create_task(dataWriter->StoreAsync()).then([this, dataWriter](unsigned int bytesStored)
    {
        // For the in-memory stream implementation we are using, the FlushAsync() call is superfluous,
        // but other types of streams may require it.
        return dataWriter->FlushAsync();
    }).then([this, dataReader, stream, dataWriter](bool flushOp)
    {
        // In order to prolong the lifetime of the stream, detach it from the DataWriter so that it 
        // will not be closed when the dataWriter is destructed. Were we to fail to detach the 
        // stream, the dataWriter destructor would close the underlying stream, preventing 
        // its subsequent use by the DataReader below.
        dataWriter->DetachStream();

        // Put the stream back at its start for the DataReader::LoadAsync call.
        stream->Seek(0);

        // Once we have written the contents successfully we load the stream.
        return dataReader->LoadAsync((unsigned int)(stream->Size));
    }).then([this, dataReader](task<unsigned int> bytesLoaded)
    {
        try
        {
            // Check for possible exceptions that could have been thrown in the async call chain.
            bytesLoaded.get();

            String^ readFromStream = "";

            // Keep reading until we consume the complete stream.
            while (dataReader->UnconsumedBufferLength > 0)
            {
                // Note that the call to ReadString requires a length of "code units" to read. This
                // is the reason each string is preceded by its length when "on the wire".
                unsigned int bytesToRead = dataReader->ReadUInt32();
                readFromStream += dataReader->ReadString(bytesToRead) + "\n";
            }

            // Populate the ElementsRead text block with the items we read from the stream
            ElementsRead->Text = readFromStream;
        }
        catch (Exception^ e)
        {
            ElementsRead->Text = "Error: " + e->Message;
        }
    });
}
