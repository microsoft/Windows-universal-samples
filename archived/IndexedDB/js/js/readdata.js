//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/readdata.html", {
        ready: function (element, options) {
            document.getElementById("readData").addEventListener("click", startReadData, false);
        }
    });

    function startReadData() {
        // Create the request to open the database, named BookDB, and if it doesn't exist, create it and immediately
        // upgrade to version 1.
        var dbRequest = window.indexedDB.open("BookDB", 1);

        // Add asynchronous callback functions.
        dbRequest.onerror = function () { WinJS.log && WinJS.log("Error opening database.", "sample", "error"); };
        dbRequest.onblocked = function () { WinJS.log && WinJS.log("Database access blocked.", "sample", "error"); };
        dbRequest.onupgradeneeded = function (evt) { WinJS.log && WinJS.log("Database is wrong version.", "sample", "error"); if (SdkSample.db) { SdkSample.db.close(); } };
        dbRequest.onsuccess = function (evt) { dbSuccess(evt); };
    }

    function dbSuccess(evt) {
        // If the database was previously loaded, close it. Closing the database keeps it from becoming blocked for later deletes operations.
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
        // Declare arrays to hold the data to be read.
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
                outputContent += "<tr><td>" + books[i].title + "</td><td>" + authors[books[i].authorId] + "</td><td><input type='checkbox' " + (books[i].checkout === 1 ? "disabled='disabled' checked='checked'" : "disabled='disabled'") + "/></tr>";
            }

            // Display the content.
            var outputDiv = document.getElementById("dataOutput");
            outputDiv.innerHTML = outputContent;
        };

        // Create a cursor on the books object store. Because we want the results to be returned in title order, we use the title index
        // on the object store for the cursor to operate on. We could pass a keyRange parameter to the openCursor call to filter the cursor
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
})();
