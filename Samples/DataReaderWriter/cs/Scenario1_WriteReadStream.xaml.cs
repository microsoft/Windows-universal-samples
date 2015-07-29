//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using SDKTemplate;
using System;
using System.Diagnostics;
using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class WriteReadStream : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        string[] inputElements = new string[] {
            "Hello", "World", "1 2 3 4 5", "Très bien!", "Goodbye"
        };

        public WriteReadStream()
        {
            this.InitializeComponent();
 
            // Populate the text block with the input elements.
            ElementsToWrite.Text = String.Join(";", inputElements);
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
        }

        /// <summary>
        /// This is the click handler for the 'Copy Strings' button.  Here we will parse the
        /// strings contained in the ElementsToWrite text block, write them to a stream using
        /// DataWriter, retrieve them using DataReader, and output the results in the
        /// ElementsRead text block.
        /// </summary>
        /// <param name="sender">Contains information about the button that fired the event.</param>
        /// <param name="e">Contains state information and event data associated with a routed event.</param>
        private async void TransferData(object sender, RoutedEventArgs e)
        {
            // Initialize the in-memory stream where data will be stored.
            using (var stream = new Windows.Storage.Streams.InMemoryRandomAccessStream())
            {
                // Create the data writer object backed by the in-memory stream.
                using (var dataWriter = new Windows.Storage.Streams.DataWriter(stream))
                {
                    dataWriter.UnicodeEncoding = Windows.Storage.Streams.UnicodeEncoding.Utf8;
                    dataWriter.ByteOrder = Windows.Storage.Streams.ByteOrder.LittleEndian;

                    // Write each element separately.
                    foreach (string inputElement in inputElements)
                    {
                        uint inputElementSize = dataWriter.MeasureString(inputElement);
                        dataWriter.WriteUInt32(inputElementSize);
                        dataWriter.WriteString(inputElement);
                    }

                    // Send the contents of the writer to the backing stream.
                    await dataWriter.StoreAsync();

                    // For the in-memory stream implementation we are using, the flushAsync call is superfluous,
                    // but other types of streams may require it.
                    await dataWriter.FlushAsync();

                    // In order to prolong the lifetime of the stream, detach it from the DataWriter so that it 
                    // will not be closed when Dispose() is called on dataWriter. Were we to fail to detach the 
                    // stream, the call to dataWriter.Dispose() would close the underlying stream, preventing 
                    // its subsequent use by the DataReader below.
                    dataWriter.DetachStream();
                }

                // Create the input stream at position 0 so that the stream can be read from the beginning.
                stream.Seek(0);
                using (var dataReader = new Windows.Storage.Streams.DataReader(stream))
                {
                    // The encoding and byte order need to match the settings of the writer we previously used.
                    dataReader.UnicodeEncoding = Windows.Storage.Streams.UnicodeEncoding.Utf8;
                    dataReader.ByteOrder = Windows.Storage.Streams.ByteOrder.LittleEndian;

                    // Once we have written the contents successfully we load the stream.
                    await dataReader.LoadAsync((uint)stream.Size);

                    var receivedStrings = "";

                    // Keep reading until we consume the complete stream.
                    while (dataReader.UnconsumedBufferLength > 0)
                    {
                        // Note that the call to readString requires a length of "code units" to read. This
                        // is the reason each string is preceded by its length when "on the wire".
                        uint bytesToRead = dataReader.ReadUInt32();
                        receivedStrings += dataReader.ReadString(bytesToRead) + "\n";
                    }

                    // Populate the ElementsRead text block with the items we read from the stream.
                    ElementsRead.Text = receivedStrings;
                }
            }
        }
    }
}
