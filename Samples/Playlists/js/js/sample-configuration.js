//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Playlists";

    var scenarios = [
        { url: "/html/scenario1-create.html", title: "Create a playlist" },
        { url: "/html/scenario2-display.html", title: "Display a playlist" },
        { url: "/html/scenario3-add.html", title: "Add items to a playlist" },
        { url: "/html/scenario4-remove.html", title: "Remove an item from a playlist" },
        { url: "/html/scenario5-clear.html", title: "Clear a playlist" },
    ];

    var audioExtensions = [".wma", ".mp3", ".mp2", ".aac", ".adt", ".adts", ".m4a"];
    var playlistExtensions = [".m3u", ".wpl", ".zpl"];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),
        audioExtensions: audioExtensions,
        playlistExtensions: playlistExtensions
    });
})();
