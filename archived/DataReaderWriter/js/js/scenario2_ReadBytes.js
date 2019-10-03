//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario2_ReadBytes.html", {
        ready: function (element, options) {
            document.getElementById("hexDumpButton").addEventListener("click", hexDump);
        }
    });

    var bytesPerRow = 16;
    var bytesPerSegment = 2;
    var chunkSize = 4096;
    var currChunk;
    var outputStr;

    /// <summary>
    /// This is the click handler for the 'Hex Dump' button.  Open the image file we want to
    /// perform a hex dump on.  Then open a sequential-access stream over the file and use
    /// ReadBytes() to extract the binary data.  Finally, convert each byte to hexadecimal, and
    /// display the formatted output in the HexDump textblock.
    /// </summary>
    function hexDump() {
        var dataReader;

        // Retrieve the uri of the image and use that to load the file.
        var uri = new Windows.Foundation.Uri("ms-appx:///images/microsoft-sdk.png");
        Windows.Storage.StorageFile.getFileFromApplicationUriAsync(uri).then(function (file) {
            // Open a sequential-access stream over the image file.
            return file.openSequentialReadAsync();
        }).done(function (inputStream) {
            // Pass the input stream that was just opened to the DataReader.
            dataReader = new Windows.Storage.Streams.DataReader(inputStream);

            currChunk = 0;
            outputStr = "";

            // Read and process the data contained in the input stream.
            readLoop(dataReader);
        }, function (err) {
            // Catch any errors that might have arisen in the chained async calls.
            document.getElementById("readBytesOutput").innerHTML = err.message;
        });
    }

    /// <summary>
    /// The read loop recursively loads a chunk of bytes from the stream into the DataReader
    /// buffer until all elements are read.  After each load, readBytes() retrieves and
    /// formats one row worth of bytes at a time.
    /// </summary>
    /// <param name="dataReader">The DataReader which will retrieve the data from the stream.</param>
    function readLoop(dataReader) {
        dataReader.loadAsync(chunkSize).done(function (numBytes) {
            // Create a byte array which can hold enough bytes to populate a row of the hex dump.
            var bytes = new Uint8Array(bytesPerRow);

            // Read and print one row at a time.
            var numBytesRemaining = numBytes;
            while (numBytesRemaining >= bytesPerRow) {
                // Use the DataReader and readBytes() to fill the byte array with one row worth of bytes.
                dataReader.readBytes(bytes);

                // Format the current row and append it to the output string.
                outputStr += formatRow(bytes, (numBytes - numBytesRemaining) + (currChunk * chunkSize));

                numBytesRemaining -= bytesPerRow;
            }

            // If there are any bytes remaining to be read, allocate a new array that will hold
            // the remaining bytes read from the DataReader and print the final row.
            // Note: readBytes() fills the entire array so if the array being passed in is larger
            // than what's remaining in the DataReader buffer, an exception will be thrown.
            if (numBytesRemaining > 0) {
                bytes = new Uint8Array(numBytesRemaining);

                // Use the DataReader and readBytes() to fill the byte array with the last row worth of bytes.
                dataReader.readBytes(bytes);

                // Format the current row and append it to the output string.
                outputStr += formatRow(bytes, (numBytes - numBytesRemaining) + (currChunk * chunkSize));
            }

            currChunk++;

            // If the DataReader didn't load an entire chunk worth of bytes, then output the hex dump since
            // we've consumed the entire file.  Otherwise, load the next chunk.
            if (numBytes < chunkSize) {
                document.getElementById("readBytesOutput").innerHTML = outputStr;
                
                // Since we're now done reading all of the chunks, close the dataReader to ensure
                // deterministic cleanup of this resource.
                dataReader.close();
            } else {
                // Create another promise to asynchronously execute the next readLoop iteration.
                // Because loadAsync is permitted to complete synchronously, we avoid using a simple
                // nested call to readLoop.  Such a nested call could lead to deep recursion, and, if enough
                // loadAsync calls complete synchronously, eventual stack overflow.
                WinJS.Promise.timeout(0).done(function () { return readLoop(dataReader); });
            }
        }, function (err) {
            // Catch any errors that might have arisen in the loadAsync() call.
            document.getElementById("readBytesOutput").innerHTML = err.message;
            
            dataReader.close();
        });
    }

    /// <summary>
    /// Format a row of the hex dump using the data retrieved by readBytes() and return the result.
    /// </summary>
    /// <param name="bytes">An array that holds the data we want to format and output.</param>
    /// <param name="currByte">Value to be formatted into an address.</param>
    /// <returns>The formatted row represented as a string.</returns>
    function formatRow(bytes, currByte) {
        var rowStr = "";

        // Format the address of byte i to have 8 hexadecimal digits and add the address
        // of the current byte to the output string.
        rowStr += ("0000000" + currByte.toString(16)).substr(-8);

        // Format the output:
        for (var i = 0; i < bytes.length; i++) {
            // If finished with a segment, add a space.
            if (i % bytesPerSegment === 0) {
                rowStr += " ";
            }

            // Convert the current byte value to hex and add it to the output string.
            rowStr += ("0" + bytes[i].toString(16)).substr(-2);
        }

        return rowStr + "<br />";
    }
})();
