//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using System;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ReadBytes : Page
    {        
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private const int bytesPerRow = 16;
        private const int bytesPerSegment = 2;
        private const uint chunkSize = 4096;

        public ReadBytes()
        {
            this.InitializeComponent();
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
        /// This is the click handler for the 'Hex Dump' button.  Open the image file we want to
        /// perform a hex dump on.  Then open a sequential-access stream over the file and use
        /// ReadBytes() to extract the binary data.  Finally, convert each byte to hexadecimal, and
        /// display the formatted output in the HexDump textblock.
        /// </summary>
        /// <param name="sender">Contains information about the button that fired the event.</param>
        /// <param name="e">Contains state information and event data associated with a routed event.</param>
        private async void HexDump(object sender, RoutedEventArgs e)
        {
            try
            {
                // Retrieve the uri of the image and use that to load the file.
                Uri uri = new Uri("ms-appx:///assets/microsoft-sdk.png");
                var file = await Windows.Storage.StorageFile.GetFileFromApplicationUriAsync(uri);

                // Open a sequential-access stream over the image file.
                using (var inputStream = await file.OpenSequentialReadAsync())
                {
                    // Pass the input stream to the DataReader.
                    using (var dataReader = new Windows.Storage.Streams.DataReader(inputStream))
                    {
                        uint currChunk = 0;
                        uint numBytes;
                        ReadBytesOutput.Text = "";

                        // Create a byte array which can hold enough bytes to populate a row of the hex dump.
                        var bytes = new byte[bytesPerRow];

                        do
                        {
                            // Load the next chunk into the DataReader buffer.
                            numBytes = await dataReader.LoadAsync(chunkSize);

                            // Read and print one row at a time.
                            var numBytesRemaining = numBytes;
                            while (numBytesRemaining >= bytesPerRow)
                            {
                                // Use the DataReader and ReadBytes() to fill the byte array with one row worth of bytes.
                                dataReader.ReadBytes(bytes);

                                PrintRow(bytes, (numBytes - numBytesRemaining) + (currChunk * chunkSize));

                                numBytesRemaining -= bytesPerRow;
                            }

                            // If there are any bytes remaining to be read, allocate a new array that will hold
                            // the remaining bytes read from the DataReader and print the final row.
                            // Note: ReadBytes() fills the entire array so if the array being passed in is larger
                            // than what's remaining in the DataReader buffer, an exception will be thrown.
                            if (numBytesRemaining > 0)
                            {
                                bytes = new byte[numBytesRemaining];

                                // Use the DataReader and ReadBytes() to fill the byte array with the last row worth of bytes.
                                dataReader.ReadBytes(bytes);

                                PrintRow(bytes, (numBytes - numBytesRemaining) + (currChunk * chunkSize));
                            }

                            currChunk++;
                        // If the number of bytes read is anything but the chunk size, then we've just retrieved the last
                        // chunk of data from the stream.  Otherwise, keep loading data into the DataReader buffer.
                        } while (numBytes == chunkSize);
                    }
                }
            }
            catch (Exception ex)
            {
                ReadBytesOutput.Text = ex.Message;
            }
        }

        /// <summary>
        /// Format and print a row of the hex dump using the data retrieved by ReadBytes().
        /// </summary>
        /// <param name="bytes">An array that holds the data we want to format and output.</param>
        /// <param name="currByte">Value to be formatted into an address.</param>
        private void PrintRow(byte[] bytes, uint currByte)
        {
            var rowStr = "";

            // Format the address of byte i to have 8 hexadecimal digits and add the address
            // of the current byte to the output string.
            rowStr += currByte.ToString("x8");

            // Format the output:
            for (int i = 0; i < bytes.Length; i++)
            {
                // If finished with a segment, add a space.
                if (i % bytesPerSegment == 0)
                {
                    rowStr += " ";
                }

                // Convert the current byte value to hex and add it to the output string.
                rowStr += bytes[i].ToString("x2");
            }

            // Append the current row to the HexDump textblock.
            ReadBytesOutput.Text += rowStr + "\n";
        }
    }
}
