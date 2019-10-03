//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Blob Builder Sample";

    WinJS.Namespace.define("BlobSample", {
        asyncError: function (error) {
            WinJS.log && WinJS.log("Async failure", "sample", "error");
        }
    });

    var scenarios = [
        { url: "/html/FileToBlobUrl.html", title: "FilePicker -> URL -> Image, Video, or Audio" },
        { url: "/html/MultiVsSingleUseBlobUrl.html", title: "File Picker -> URL -> Multi use VS Single use Blob Urls" },
        { url: "/html/StorageFileReadText.html", title: "File Picker -> StorageFile -> text of file" },
        { url: "/html/FileReadText.html", title: "input type='file' -> File -> text of file" },
        { url: "/html/CanvasBlob-Image.html", title: "Canvas -> Blob -> Image" },
        { url: "/html/XhrBlobSaveToDisk.html", title: "XHR Blob Download -> Save to Disk" },
        { url: "/html/BlobBuilder.html", title: "BlobBuilder" },
        { url: "/html/PostMessageBlob.html", title: "Postmessage Blob to Web Context" },
        { url: "/html/ThumbnailBlob-Image.html", title: "StorageFile Thumbnail -> Blob -> Image tag" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();