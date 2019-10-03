//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Background media playback";

    var scenarios = [
        { url: "/html/scenario1-bg-audio.html", title: "Background media playback" },
    ];

    let Uri = Windows.Foundation.Uri;
    let MediaSource = Windows.Media.Core.MediaSource;
    let MediaPlaybackItem = Windows.Media.Playback.MediaPlaybackItem;
    let MediaPlaybackList = Windows.Media.Playback.MediaPlaybackList;
    let MediaPlayer = Windows.Media.Playback.MediaPlayer;
    let Storage = Windows.Storage;

    class SongViewModel {
        constructor(src) {
            if (typeof src === 'string') {
                let source = MediaSource.createFromUri(new Uri(src));
                this._playbackItem = new MediaPlaybackItem(source);
            } else {
                this._playbackItem = src;
            }
        }

        get title() {
            return this._playbackItem.getDisplayProperties().musicProperties.title;
        }

        set title(val) {
            const displayProps = this._playbackItem.getDisplayProperties();
            displayProps.musicProperties.title = val;
            this._playbackItem.applyDisplayProperties(displayProps);
        }

        get albumArtUri() {
            return this._playbackItem.source.customProperties.albumArtUri;
        }

        set albumArtUri(val) {
            this._playbackItem.source.customProperties.insert('albumArtUri', val);
        }

        get mediaPlaybackItem() {
            return this._playbackItem;
        }
    }

    class PlaylistViewModel {
        constructor(src) {
            const uri = new Windows.Foundation.Uri(src);
            this._playbackListPromise =
                Storage.StorageFile.getFileFromApplicationUriAsync(uri)
                .then((storageFile) => Storage.FileIO.readTextAsync(storageFile))
                .then((jsonText) => {
                    const items = JSON.parse(jsonText).mediaList.items;
                    const playlist = new MediaPlaybackList();
                    playlist.autoRepeatEnabled = true;

                    items.forEach(function (item) {
                        var song = new SongViewModel(item.mediaUri);

                        if (item.albumArtUri) {
                            song.albumArtUri = item.albumArtUri;
                        }

                        if (item.title) {
                            song.title = item.title;
                        }

                        playlist.items.append(song.mediaPlaybackItem);
                    });

                    return playlist;
                });
        }

        get currentSong() {
            return this._playbackListPromise
                .then((playbackList) => {
                    return playbackList.currentItem ? new SongViewModel(playbackList.currentItem) : null;
                });
        }

        get songs() {
            return this._playbackListPromise
                .then((playbackList) => {
                    return playbackList.items.map((val) => new SongViewModel(val));
                });
        }

        get ready() {
            return this._playbackListPromise;
        }

        set songChangedEventHandler(fn) {
            this._playbackListPromise
                .then((playbackList) => playbackList.addEventListener('currentitemchanged', (e) => {
                    fn({
                        target: e.target,
                        newSong: e.newItem ? new SongViewModel(e.newItem) : null,
                        oldSong: e.oldItem ? new SongViewModel(e.oldItem) : null,
                    });
                }));
        }
    }

    class MediaPlayerViewModel {
        constructor(playlistViewModel) {
            this._mediaPlayerPromise =
                playlistViewModel.ready
                    .then((playbackList) => {
                        const mediaPlayer = new MediaPlayer();
                        mediaPlayer.autoPlay = false;
                        mediaPlayer.source = playbackList;
                        return mediaPlayer;
                    });
        }

        play() {
            this._mediaPlayerPromise.then((mediaPlayer) => mediaPlayer.play());
        }

        pause() {
            this._mediaPlayerPromise.then((mediaPlayer) => mediaPlayer.pause());
        }

        skipForward() {
            this._mediaPlayerPromise.then((mediaPlayer) => mediaPlayer.source.moveNext());
        }

        skipBackward() {
            this._mediaPlayerPromise.then((mediaPlayer) => mediaPlayer.source.movePrevious());
        }

        skipTo(val) {
            this._mediaPlayerPromise.then((mediaPlayer) => mediaPlayer.source.moveTo(val));
        }
    }

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),
        SongViewModel: SongViewModel,
        PlaylistViewModel: PlaylistViewModel,
        MediaPlayerViewModel: MediaPlayerViewModel
    });
})();