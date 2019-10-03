//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    'use strict';

    const playlist = new SdkSample.PlaylistViewModel('ms-appx:///assets/playlist.json');
    const mediaPlayer = new SdkSample.MediaPlayerViewModel(playlist);

    let playlistElem;
    let albumartElem;
    let songtitleElem;

    let page = WinJS.UI.Pages.define("/html/scenario1-bg-audio.html", {
        ready: function (element, options) {
            playlistElem = document.getElementById('playlist');
            albumartElem = document.getElementById('album-art');
            songtitleElem = document.getElementById('song-title');

            // Create markup from playlist metadata
            buildPlaylistMarkup();

            // Wire up event listeners
            playlist.songChangedEventHandler = playlistItemChanged;
            document.getElementById('play-button').addEventListener('click', () => mediaPlayer.play());
            document.getElementById('pause-button').addEventListener('click', () => mediaPlayer.pause());
            document.getElementById('next-button').addEventListener('click', () => mediaPlayer.skipForward());
            document.getElementById('prev-button').addEventListener('click', () => mediaPlayer.skipBackward());
        }
    });

    function playlistItemChanged(e) {
        const { newSong } = e;

        if (newSong) {
            albumartElem.src = newSong.albumArtUri;
            songtitleElem.innerText = newSong.title;
        }

        buildPlaylistMarkup();
    }

    function buildPlaylistMarkup() {
        WinJS.Promise
            .join([playlist.songs, playlist.currentSong])
            .then((values) => {
                const [songs, currentSong] = values;

                // Clear inner HTML
                playlistElem.innerHTML = '';

                // Create and append song markup
                songs.forEach((song, i) => {
                    const albumElem = document.createElement('img');
                    albumElem.src = song.albumArtUri;

                    const titleElem = document.createElement('h4');
                    titleElem.innerText = song.title;

                    const songElem = document.createElement('div');
                    songElem.className = 'song';
                    if (currentSong && currentSong.mediaPlaybackItem === song.mediaPlaybackItem) {
                        songElem.classList.add('active');
                    }
                    songElem.addEventListener('click', (e) => {
                        mediaPlayer.skipTo(i);
                    });

                    songElem.appendChild(albumElem);
                    songElem.appendChild(titleElem);

                    playlistElem.appendChild(songElem);
                });
            });
    }
})();
