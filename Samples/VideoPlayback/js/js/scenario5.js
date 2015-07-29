(function () {
    "use strict";
    var Media = Windows.Media;
    var playbackItem;

    var mainVideo;
    var page = WinJS.UI.Pages.define("/html/scenario5.html", {
        ready: function (element, options) {
            document.getElementById("btn1").addEventListener("click", btn1Click, false);
            document.getElementById("btn2").addEventListener("click", btn2Click, false);
            document.getElementById("btn3").addEventListener("click", btn3Click, false);


            mainVideo = document.getElementById("mainVideo");
            var playlist = new Media.Playback.MediaPlaybackList();
            var source1 = Media.Core.MediaSource.createFromUri(new Windows.Foundation.Uri("https://mediaplatstorage1.blob.core.windows.net/windows-universal-samples-media/elephantsdream-clip-h264_sd-aac_eng-aac_spa-aac_eng_commentary-srt_eng-srt_por-srt_swe.mkv"));

            playbackItem = new Media.Playback.MediaPlaybackItem(source1);
            playlist.items.append(playbackItem);


            mainVideo.src = URL.createObjectURL(playlist, { oneTimeOnly: true });


        }
    });

    function doSomething() {
        WinJS.log && WinJS.log("Error message here", "sample", "error");
    }

    function btn1Click() {
        playbackItem.audioTracks.selectedIndex = 0;
    }
    function btn2Click() {
        playbackItem.audioTracks.selectedIndex = 1;
    }
    function btn3Click() {
        playbackItem.audioTracks.selectedIndex = 2;
    }


})();
