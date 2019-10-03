//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Audio Category";

    var scenarios = [
        { url: "/html/movie.html", title: "Movie" },
        { url: "/html/media.html", title: "Media" },
        { url: "/html/gamechat.html", title: "GameChat" },
        { url: "/html/speech.html", title: "Speech" },
        { url: "/html/communications.html", title: "Communications" },
        { url: "/html/alerts.html", title: "Alerts" },
        { url: "/html/soundeffects.html", title: "SoundEffects" },
        { url: "/html/gameeffects.html", title: "GameEffects" },
        { url: "/html/gamemedia.html", title: "GameMedia" },
        { url: "/html/other.html", title: "Other" }

    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();