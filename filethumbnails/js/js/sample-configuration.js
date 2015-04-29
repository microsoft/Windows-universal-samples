//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "File thumbnails JS sample";

    var errors = {
        noExif:             "No result (no EXIF thumbnail or cached thumbnail available for fast retrieval)",
        noAlbumArt:         "No result (no album art available for this song)",
        noIcon:             "No result (no icon available for this document type)",
        noThumbnail:        "No result (no thumbnail could be obtained from the selected file)",
        noImages:           "No result (no thumbnail could be obtained from the selected folder - make sure that the folder contains images)",
        emptyFilegroup:     "No result (unexpected error: retrieved file group was null)",
        filegroupLocation:  "File groups are only available for library locations, please select a folder from one of your libraries",
        fail:               "No result (unexpected error: no thumbnail could be retrieved)",
        cancel:             "No result (operation cancelled, no item selected)",
        invalidSize:        "Invalid size (specified size must be numerical and greater than zero)",
    };

    var scenarios = [
        { url: "/html/scenario1.html", title: "Display a thumbnail for a picture" },
        { url: "/html/scenario2.html", title: "Display album art for a song" },
        { url: "/html/scenario3.html", title: "Display an icon for a document" },
        { url: "/html/scenario4.html", title: "Display a thumbnail for a folder" },
        { url: "/html/scenario5.html", title: "Display a thumbnail for a file group" },
        { url: "/html/scenario6.html", title: "Display a scaled image" },
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),
        errors: errors,
    });
})();
