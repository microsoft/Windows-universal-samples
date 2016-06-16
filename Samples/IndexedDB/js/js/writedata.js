//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/writedata.html", {
        ready: function (element, options) {
            document.getElementById("readData").addEventListener("click", startReadData, false);
            document.getElementById("writeChanges").addEventListener("click", startWriteData, false);
        }
    });

    // Create our variable to hold our queue of items to update once the Write changes button is clicked.
    var pendingWrites = [];

    function startReadData() {
        // Create the request to open the database, named BookDB, and if it doesn't exist, create it and immediately
        // upgrade to version 1.
        var dbRequest = window.indexedDB.open("BookDB", 1);

        // Add asynchronous callback functions
        dbRequest.onerror = function () { WinJS.log && WinJS.log("Error opening database.", "sample", "error"); };
        dbRequest.onblocked = function () { WinJS.log && WinJS.log("Database access blocked.", "sample", "error"); };
        dbRequest.onupgradeneeded = function (evt) { WinJS.log && WinJS.log("Database is wrong version.", "sample", "error"); if (SdkSample.db) { SdkSample.db.close(); } };
        dbRequest.onsuccess = function (evt) { dbSuccess(evt); };
    }

    function startWriteData() {
        // If the database has not been opened, log an error.
        if (SdkSample.db === null) {
            WinJS.log && WinJS.log("Data has not been read yet.", "sample", "error");
            return;
        }

        // If no changes have been made to what would be written, log an error.
        if (pendingWrites.length === 0) {
            WinJS.log && WinJS.log("No changes have been made.", "sample", "error");
            return;
        }

        // Create a transaction to write the changes to the database        
        var txn = SdkSample.db.transaction(["checkout"], "readwrite");

        // Set the event callbacks for the transaction
        txn.onerror = function (evt) { WinJS.log && WinJS.log("Error writing data.", "sample", "error"); };
        txn.onabort = function (evt) { WinJS.log && WinJS.log("Writing of data aborted.", "sample", "error"); };

        // The oncomplete event handler is called asynchronously once all writes have completed; when that's done, we reset our pending write queue.
        txn.oncomplete = function () {
            pendingWrites = [];
            WinJS.log && WinJS.log("Changes saved to database.", "sample", "status");
        };

        for (var bookId in pendingWrites) {
            var checkoutStore = txn.objectStore("checkout");
            if (pendingWrites[bookId] === 0) {
                // Record was unchecked, so delete the row from the database
                var deleteResult = checkoutStore.delete (bookId);
                deleteResult.onerror = function (evt) {
                    WinJS.log && WinJS.log("Error deleting data.", "sample", "error");
                };
            } else {
                var addResult = checkoutStore.add({ id: bookId, status: 1 });
                addResult.onerror = function (evt) { 
                    WinJS.log && WinJS.log("Error adding data.", "sample", "error");
                };
            }
        }
    }

    function dbSuccess(evt) {
        // If the database was previously loaded, close it. Closing the database keeps it from becoming blocked for later deletes.
        if (SdkSample.db) {
            SdkSample.db.close();
        }
        SdkSample.db = evt.target.result;
        if (SdkSample.db.objectStoreNames.length === 0) {
            WinJS.log && WinJS.log("Database schema does not exist. Complete the first two scenarios before continuing.", "sample", "error");
            SdkSample.db.close();
            SdkSample.db = null;
            window.indexedDB.deleteDatabase("BookDB", 1);
        } else {
            readData(evt);
        }
    }

    function readData(evt) {
        // Declare arrays to hold the data to be read
        var books = [];
        var authors = [];
        var checkouts = [];

        // Create a transaction with which to query the IndexedDB.
        var txn = SdkSample.db.transaction(["books", "authors", "checkout"], "readonly");

        // Set the event callbacks for the transaction.
        txn.onerror = function () { WinJS.log && WinJS.log("Error reading data.", "sample", "error"); };
        txn.onabort = function () { WinJS.log && WinJS.log("Reading of data aborted.", "sample", "error"); };

        // The oncomplete event handler is called asynchronously once reading is finished and the data arrays are fully populated. This
        // completion event will occur later than the cursor iterations defined below, because the transaction will not complete until
        // the cursors are finished.
        txn.oncomplete = function () {
            var outputContent = "";

            // If there are no books to display, there is no need to continue.
            var len = books.length;
            if (len === 0) {
                return;
            }

            // Set checkout status for each book. Because the checkout table containing status for each book is sparsely
            // populated, we need to set the checkout status to 0 if it is not found in the checkouts array.
            for (var i = 0; i < len; i++) {
                var bookId = books[i].id;
                if (checkouts[bookId]) {
                    books[i].checkout = checkouts[bookId];
                }
                else {
                    books[i].checkout = 0;
                }
            }

            // Construct an output table with one row per book.
            outputContent = "<table><tr><th>Book</th><th>Author</th><th>Checked Out?</th></tr>";
            for (i = 0; i < len; i++) {
                outputContent += "<tr><td>" + books[i].title + "</td><td>" + authors[books[i].authorId] +
                    "</td><td><input class='bookCheckout' id='" + books[i].id + "' type='checkbox' " +
                    (books[i].checkout === 1 ? "checked='checked'" : "") + "/></tr>";
            }

            // Display the content.
            var outputDiv = document.getElementById("dataOutput");
            outputDiv.innerHTML = outputContent;

            // Now attach click handlers to each checkbox.
            var bookCheckBoxes = document.querySelectorAll(".bookCheckout");
            for (var checkIndex = 0; checkIndex < bookCheckBoxes.length; checkIndex++) {
                bookCheckBoxes[checkIndex].onclick = toggleCheckout;
            }

        };

        // Create a cursor on the books object store. Because we want the results to be returned in title order, we use the title index
        // on the object store for the cursor to operate on. We could pass a keyRange parameter to the openCursor() call to filter the cursor
        // to specific titles.
        var bookCursorRequest = txn.objectStore("books").index("title").openCursor();

        // As each record is returned (asynchronously), the cursor calls the onsuccess event; we store that data in our books array
        bookCursorRequest.onsuccess = function (e) {
            var cursor = e.target.result;
            if (cursor) {
                books.push(cursor.value);
                cursor.continue();
            }
        };

        // The authors object store cursor is handled slightly differently. Here we load the entire authors table into an array because we know there
        // is only a small amount of data. With larger or filtered datasets, we could have parsed the authorId in the book cursor onsuccess handler 
        // above and initiated a nested cursor request that created a keyRange for the one authorId desired and passed that keyRange to the openCursor
        // call below. For clarity of this sample, we follow the simpler model.
        var authorCursorRequest = txn.objectStore("authors").openCursor();

        // Asynchronously retrieve and store the data in the authors array with a key of the author id. This makes it easy to retrieve a specific 
        // author by id in the oncomplete handler of the transaction.
        authorCursorRequest.onsuccess = function (e) {
            var cursor = e.target.result;
            if (cursor) {
                var data = cursor.value;
                authors[data.id] = data.name;
                cursor.continue();
            }
        };

        // Here we also create a cursor on the checkout object store to retrieve whether each book is checked out. If you run the scenarios in
        // sequence, this cursor will return no results (the cursor variable will be null below). If you run the write scenario and save new
        // checkouts, you can return to this page and see the cursor now returns the previously saved checkout data.
        var checkoutCursorRequest = txn.objectStore("checkout").openCursor();
        checkoutCursorRequest.onsuccess = function (e) {
            var cursor = e.target.result;
            if (cursor) {
                var data = cursor.value;
                checkouts[data.id] = data.status;
                cursor.continue();
            }
        };
    }

    function toggleCheckout() {
        var id = this.id.toString();
        var checkOutStatus = this.checked ? 1 : 0;

        // If the record was previously queued to write to the database, remove it; otherwise, add it. In this way, the pendingWrites
        // array only ever contains differences from what exists in the object store.
        var pendingRecord = pendingWrites[id];
        if (typeof (pendingRecord) === "undefined") {
            pendingWrites[id] = checkOutStatus;
        } else {
            delete pendingWrites[id];
        }
    }

})();
