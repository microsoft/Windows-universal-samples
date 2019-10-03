//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario1_WriteReadStream.html", {
        ready: function (element, options) {
            var sourceElement = document.getElementById("ElementsToSend");
            sourceElement.innerHTML = "Hello;World;1 2 3 4 5;Très bien!;Goodbye";
            var sendButton = document.getElementById("SendButton");
            sendButton.addEventListener("click", transferData);
        }
    });

function transferData() {
        var sourceElement = document.getElementById("ElementsToSend");
        var destinationElement = document.getElementById("scenario1Output");

        // First a DataWriter object is created, backed by an in-memory stream where the data
        // will be stored.
        var writer = Windows.Storage.Streams.DataWriter(new Windows.Storage.Streams.InMemoryRandomAccessStream());
        writer.unicodeEncoding = Windows.Storage.Streams.UnicodeEncoding.utf8;
        writer.byteOrder = Windows.Storage.Streams.ByteOrder.littleEndian;

        // We separate the contents of the sourceElement div in multiple strings using ';' as
        // the separator. Each string will be written separately.
        var elements = sourceElement.innerHTML.split(";");
        elements.forEach(function (element) {
            var codeUnits = writer.measureString(element);
            writer.writeInt32(codeUnits);
            writer.writeString(element);
        });

        var reader;
        var stream;

        // The call to store async sends the actual contents of the writer to the backing stream
        writer.storeAsync().then(function () {
            // For the in-memory stream implementation we are using, the flushAsync call is superfluous,
            // but other types of streams may require it.
            return writer.flushAsync();
        }).then(function () {
            // We detach the stream to prolong its useful lifetime. Were we to fail to detach the stream,
            // the call to writer.close() would close the underlying stream, preventing its subsequent use
            // by the DataReader below. Most clients of DataWriter will have no reason to use the
            // underlying stream after writer.close() is called, and will therefore have no reason to call
            // writer.detachStream(). Note that once we detach the stream, we assume responsibility for
            // closing the stream subsequently; after the stream has been detached, a call to
            // writer.close() will have no effect on the stream.
            stream = writer.detachStream();
            // Make sure the stream is read from the beginning in the reader we are creating below.
            stream.seek(0);
            // Most DataWriter clients will not call writer.detachStream(), and furthermore will be
            // working with a file-backed or network-backed stream, rather than an in-memory-stream. In
            // such cases, it would be particularly important to call writer.close(). Doing so is always
            // a best practice.
            writer.close();

            reader = new Windows.Storage.Streams.DataReader(stream);
            // The encoding and byte order need to match the settings of the writer we previously used.
            reader.unicodeEncoding = Windows.Storage.Streams.UnicodeEncoding.utf8;
            reader.byteOrder = Windows.Storage.Streams.ByteOrder.littleEndian;

            // Once we have written the contents successfully we load the stream, this is also an
            // asynchronous operation
            return reader.loadAsync(stream.size);
        }).done(function () {
            var receivedStrings = "";
            // Keep reading until we consume the complete stream
            while (reader.unconsumedBufferLength > 0) {
                // Note that the call to readString requires a length of "code units" to read. This
                // is the reason each string is preceeded by its length when "on the wire".
                var codeUnitsToRead = reader.readInt32();
                receivedStrings += reader.readString(codeUnitsToRead) + "<br/>";
            }
            // Calling reader.close() closes the underlying stream. It would be particularly important
            // to call reader.close() if the underlying stream were file-backed or network-backed. Note
            // that this call to reader.close() satisfies our obligation to close the stream previously
            // detached from DataReader.
            reader.close();
            destinationElement.innerHTML = receivedStrings;
        });
    };
})();
