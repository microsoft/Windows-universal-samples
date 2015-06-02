//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/createschema.html", {
        ready: function (element, options) {
            document.getElementById("createDatabase").addEventListener("click", createDB, false);
            document.getElementById("deleteDatabase").addEventListener("click", deleteDB, false);
        }
    });

    var newCreate = false;

    function createDB() {
        // Create the request to open the database, named BookDB. If it doesn't exist, create it and immediately
        // upgrade to version 1.
        var dbRequest = window.indexedDB.open("BookDB", 1);

        // Add asynchronous callback functions
        dbRequest.onerror = function () { WinJS.log && WinJS.log("Error creating database.", "sample", "error"); };
        dbRequest.onsuccess = function (evt) { dbSuccess(evt); };
        dbRequest.onupgradeneeded = function (evt) { dbVersionUpgrade(evt); };
        dbRequest.onblocked = function () { WinJS.log && WinJS.log("Database create blocked.", "sample", "error");  };

        // Reset the flag that indicates whether this is a new creation request. 
        // Assume that the database was previously created.
        newCreate = false;
    }

    function deleteDB() {

        // Close and clear the handle to the database, held in the parent SdkSample namespace.
        if (SdkSample.db) {
            SdkSample.db.close();
        }
        SdkSample.db = null;
        var dbRequest = window.indexedDB.deleteDatabase("BookDB");
        dbRequest.onerror = function () { WinJS.log && WinJS.log("Error deleting database.", "sample", "error"); };
        dbRequest.onsuccess = function () { WinJS.log && WinJS.log("Database deleted.", "sample", "status"); };
        dbRequest.onblocked = function () {
            WinJS.log && WinJS.log("Database delete blocked.", "sample", "error");
        };
    }

    // Whenever an IndexedDB is created, the version is set to "", but can be immediately upgraded by calling createDB. 
    function dbVersionUpgrade(evt) {

        // If the database was previously loaded, close it. 
        // Closing the database keeps it from becoming blocked for later delete operations.
        if (SdkSample.db) {
            SdkSample.db.close();
        }
        SdkSample.db = evt.target.result;

        // Get the version update transaction handle, since we want to create the schema as part of the same transaction.
        var txn = evt.target.transaction;

        // Create the books object store, with an index on the book title. Note that we set the returned object store to a variable
        // in order to make further calls (index creation) on that object store.
        var bookStore = SdkSample.db.createObjectStore("books", { keyPath: "id", autoIncrement: true });
        bookStore.createIndex("title", "title", { unique: false });

        // Create the authors object store.
        SdkSample.db.createObjectStore("authors", { keyPath: "id" });

        // Create the book checkout status object store. Note that new object stores can only be created/deleted as part of the version change.
        // transaction that we're adding to in this function.
        SdkSample.db.createObjectStore("checkout", { keyPath: "id" });

        // Once the creation of the object stores is finished (they are created asynchronously), log success.
        txn.oncomplete = function () { WinJS.log && WinJS.log("Database schema created.", "sample", "status"); };
        newCreate = true;
    }

    function dbSuccess(evt) {

        // Log whether the app tried to create the database when it already existed. 
        if (!newCreate) {
            // Close this additional database request
            var db = evt.target.result;
            db.close();

            WinJS.log && WinJS.log("Database schema already exists.", "sample", "error");
            return;
        }
    }

})();
