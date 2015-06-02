//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/populatedata.html", {
        ready: function (element, options) {
            document.getElementById("loadData").addEventListener("click", startLoadData, false);
        }
    });

    function startLoadData() {
        // Create the request to open the database, named BookDB, and if it doesn't exist, create it and immediately
        // upgrade to version 1.

        var dbRequest = window.indexedDB.open("BookDB", 1);

        // Add asynchronous callback functions
        dbRequest.onerror = function () { WinJS.log && WinJS.log("Error opening database.", "sample", "error"); };
        dbRequest.onsuccess = function (evt) { dbSuccess(evt); };
        dbRequest.onupgradeneeded = function (evt) { WinJS.log && WinJS.log("Database wrong version.", "sample", "error"); if (SdkSample.db) { SdkSample.db.close(); } };
        dbRequest.onblocked = function () { WinJS.log && WinJS.log("Database access blocked.", "sample", "error"); };
    }

    function dbSuccess(evt) {
        // If the database was previously loaded, close it... this keeps the database from becoming blocked for later deletes
        if (SdkSample.db) {
            SdkSample.db.close();
        }
        SdkSample.db = evt.target.result;
        if (SdkSample.db.objectStoreNames.length === 0) {
            WinJS.log && WinJS.log("Database schema does not exist. Complete the first scenario before continuing.", "sample", "error");
            SdkSample.db.close();
            SdkSample.db = null;
            window.indexedDB.deleteDatabase("BookDB", 1);
        } else {
            loadData(evt);
        }
    }

    function loadData(evt) {
        if (evt.type === "success") {
            var books = [];
            var authors = [];
            var foldername = "data";
            var filename = "data.xml";
            var results = "";

            // Clear previous results.
            var output = document.getElementById("progressOutput");
            output.innerHTML = "";

            // Open folder.
            Windows.ApplicationModel.Package.current.installedLocation.getFolderAsync(foldername).done(function (folder) {
                // Open file
                folder.getFileAsync(filename).done(function (file) {

                    // Prepare load settings.
                    var loadSettings = new Windows.Data.Xml.Dom.XmlLoadSettings;
                    loadSettings.prohibitDtd = false;
                    loadSettings.resolveExternals = false;

                    // Load XML - Important: We do this before opening the transaction to talk to the database, so the transaction won't expire.
                    Windows.Data.Xml.Dom.XmlDocument.loadFromFileAsync(file, loadSettings).done(function (xmlDoc) {
                        // Load the books
                        var nodes = xmlDoc.selectNodes('//book');
                        var len = nodes.length;
                        for (var i = 0; i < len; i++) {
                            var book = {
                                id: nodes[i].attributes[0].nodeValue,
                                title: nodes[i].attributes[1].nodeValue,
                                authorid: nodes[i].attributes[2].nodeValue
                            };
                            books.push(book);
                        }

                        // Load the authors.
                        nodes = xmlDoc.selectNodes('//author');
                        len = nodes.length;
                        for (i = 0; i < len; i++) {
                            var author = {
                                id: nodes[i].attributes[0].nodeValue,
                                name: nodes[i].attributes[1].nodeValue
                            };
                            authors.push(author);
                        }

                        // Open our database tables.
                        var txn = SdkSample.db.transaction(["books", "authors"], "readwrite");
                        txn.oncomplete = function () {
                            WinJS.log && WinJS.log("Database populated.", "sample", "status");
                        };
                        txn.onerror = function () {
                            WinJS.log && WinJS.log("Unable to populate database or database already populated.", "sample", "error");
                        };
                        txn.onabort = function () {
                            WinJS.log && WinJS.log("Unable to populate database or database already populated.", "sample", "error");
                        };

                        var booksStore = txn.objectStore("books");
                        var authorsStore = txn.objectStore("authors");

                        // Write books to IndexedDB table.
                        len = books.length;
                        for (i = 0; i < len; i++) {
                            var addResult = booksStore.add(books[i]);
                            addResult.book = books[i].title;
                            addResult.onsuccess = function () {
                                output.innerHTML += "Added book: " + this.book + ".<br />";
                            };
                            addResult.onerror = function () {
                                WinJS && WinJS.log("Failed to add book: " + this.book + ".", "sample", "error");
                            };
                        }

                        // Write authors to IndexedDB table.
                        len = authors.length;
                        for (i = 0; i < len; i++) {
                            addResult = authorsStore.add(authors[i]);
                            addResult.author = authors[i].name;
                            addResult.onsuccess = function () {
                                output.innerHTML += "Added author: " + this.author + ".<br />";
                            };
                            addResult.onerror = function () {
                                WinJS && WinJS.log("Failed to add author: " + this.author + ".", "sample", "error");
                            };
                        }
                    });
                });
            });
        }
    }
})();
