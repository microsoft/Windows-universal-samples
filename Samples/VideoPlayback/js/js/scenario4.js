(function () {
    "use strict";
    var Media = Windows.Media;
    var playbackItem;
    
    var mainVideo;
    var page = WinJS.UI.Pages.define("/html/scenario4.html", {
        ready: function (element, options) {
            document.getElementById("btn1").addEventListener("click", btn1Click, false);
            document.getElementById("btn2").addEventListener("click", btn2Click, false);
            document.getElementById("btn3").addEventListener("click", btn3Click, false);
            document.getElementById("btn4").addEventListener("click", btn4Click, false);
            document.getElementById("btn5").addEventListener("click", btn5Click, false);

            mainVideo = document.getElementById("mainVideo");
            var playlist = new Media.Playback.MediaPlaybackList();
            var source1 = Media.Core.MediaSource.createFromUri(new Windows.Foundation.Uri("https://mediaplatstorage1.blob.core.windows.net/windows-universal-samples-media/multivideo-with-captions.mkv"));

            playbackItem = new Media.Playback.MediaPlaybackItem(source1);
            playlist.items.append(playbackItem);

            
            mainVideo.src = URL.createObjectURL(playlist, { oneTimeOnly: true });


        }
    });

    function doSomething() {
        WinJS.log && WinJS.log("Error message here", "sample", "error");
    }

    function btn1Click() {
        playbackItem.videoTracks.selectedIndex = 0;
    }
    function btn2Click() {
        playbackItem.videoTracks.selectedIndex = 1;
    }
    function btn3Click() {
        playbackItem.videoTracks.selectedIndex = 2;
    }
    function btn4Click() {
        playbackItem.videoTracks.selectedIndex = 3;
    }
    function btn5Click() {
        playbackItem.videoTracks.selectedIndex = 4;
    }

})();




