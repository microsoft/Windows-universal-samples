//// Copyright (c) Microsoft Corporation. All rights reserved
(function () {
    "use strict";

    var videoChatModelType = WinJS.Class.define(function () {

        this._IpAddressTextBox = document.querySelector("#ipAddressText");
        this._callBtn = document.querySelector("#callBtn");
        this._endCallBtn = document.querySelector("#endCallBtn");
        this._remoteVideo = document.querySelector("#remoteVideo");
        this._mediaCaptureFailedEventHandler = this._onMediaCaptureFailed.bind(this);
        this._incomingConnectionEventHandler = this._onIncomingConnection.bind(this);
    }, {
        _device: null,
        _mediaSink: null,
        _IpAddressTextBox: null,
        _callBtn: null,
        _endCallBtn: null,
        _remoteVideo: null,
        _role: null,
        _isTerminator: false,
        _initialization: false,
        _active: false,
        _mediaCaptureFailedEventHandler: null,
        _incomingConnectionEventHandler: null,

        initialize: function () {
            var that = this;
            if (this._initialization && this._device) {
                return;
            }
            this._initialization = true;
            this._active = true;
            SimpleCommunication.captureMgr.checkForRecordingDevicesAsync().then(function (result) {
                if (result) {
                    return SimpleCommunication.captureMgr.lockAsync();
                } else {
                    return WinJS.Promise.wrapError(SimpleCommunication.createError(
                        "SimpleCommunication.NoCamera",
                        "A machine with a camera and a microphone is required to run this sample."
                    ));
                }
            }).then(function (dev) {
                that._device = dev;
                dev._lockedBy = "chat";
                that._device.addEventListener("failed", that._mediaCaptureFailedEventHandler, false);
                that._device.addEventListener("incomingconnectionevent", that._incomingConnectionEventHandler, false);
                return that._startRecordingToCustomSinkAsync();
            }).done(function () {
                that._callBtn.disabled = false;
                that._IpAddressTextBox.disabled = false;
                that._endCallBtn.disabled = true;

                that._callBtn.addEventListener("click", that._onCallBtnClicked.bind(that), false);
                that._endCallBtn.addEventListener("click", that._onEndCallBtnClicked.bind(that), false);
                that._remoteVideo.addEventListener("error", that._videoPlaybackErrorHandler.bind(that), false);

                // Both side start out as passive
                that._role = "passive";
                that._isTerminator = false;
                that._initialization = false;
                SdkSample.displayStatus("Tap 'Call' button to start call");
            }, function (e) {
                that._initialization = false;
                that.deactivate();
                SdkSample.formatError(e, "Initialization error: ");
            });
        },

        deactivate: function () {
            this._active = false;
            this.cleanUp().done();
        },

        cleanUp: function () {
            this._callBtn.removeEventListener("click", this._onCallBtnClicked, false);
            this._endCallBtn.removeEventListener("click", this._onEndCallBtnClicked, false);
            this._remoteVideo.removeEventListener("error", this._videoPlaybackErrorHandler, false);

            if (this._remoteVideo && !this._remoteVideo.paused) {
                this._remoteVideo.src = "";
            }

            if (this._device) {
                var that = this;
                this._device.removeEventListener("failed", that._mediaCaptureFailedEventHandler, false);
                this._device.removeEventListener("incomingconnectionevent", that._incomingConnectionEventHandler, false);

                var device = this._device;
                that._device = null;
                return SimpleCommunication.captureMgr.unlockAsync(device);
            }

            return WinJS.Promise.wrap();
        },

        _startRecordingToCustomSinkAsync: function () {
            var that = this,
                mediaEncodingProfile =
                Windows.Media.MediaProperties.MediaEncodingProfile.createMp4(Windows.Media.MediaProperties.VideoEncodingQuality.qvga);

            mediaEncodingProfile.video.frameRate.numerator = 15;
            mediaEncodingProfile.video.frameRate.denominator = 1;
            mediaEncodingProfile.container = null;

            return this._device.startRecordingAsync(mediaEncodingProfile);
        },

        _onCallBtnClicked: function () {
            var address = this._IpAddressTextBox.value;
            if (address) {
                this._role = "active";
                this._remoteVideo.src = "stsp://" + address;
                this._remoteVideo.play();
                this._IpAddressTextBox.disabled = true;
                this._callBtn.disabled = true;
                SdkSample.displayStatus("Initiating connection... Please wait!");
            }
        },

        _onEndCallBtnClicked: function () {
            if (!this._isTerminator) {
                this._isTerminator = true;
                this._endCall();
            }
        },

        _onIncomingConnection: function (e) {
            var args = e.detail;
            args.accept();
            this._endCallBtn.disabled = false;
            this._isTerminator = false;

            switch (this._role) {
                case "active":
                    SdkSample.displayStatus("Connected. Remote machine address: " + args.remoteUrl.replace("stsp://", ""));
                    break;
                case "passive":
                    var address = args.remoteUrl;
                    this._remoteVideo.src = address;

                    SdkSample.displayStatus("Connected. Remote machine address: " + address.replace("stsp://", ""));
                    this._IpAddressTextBox.disabled = true;
                    this._callBtn.disabled = true;
                    break;
                default:
                    break;
            }
        },

        _endCall: function () {
            var that = this;
            this.cleanUp().done(
            function () {
                if (that._active) {
                    that.initialize();
                }
            });
        },

        _onMediaCaptureFailed: function (e) {
            SdkSample.formatError(e.detail, "Capture error code: ");

            if (!this._isTerminator) {
                this._isTerminator = true;
                this._endCall();
            }
        },

        _videoPlaybackErrorHandler: function (e) {
            if (this._isTerminator) { return; }

            switch (e.target.error.code) {
                case e.target.error.MEDIA_ERR_ABORTED:
                case e.target.error.MEDIA_ERR_NETWORK:
                case e.target.error.MEDIA_ERR_DECODE:
                case e.target.error.MEDIA_ERR_SRC_NOT_SUPPORTED:
                    this._isTerminator = true;
                    this._endCall();
                    break;
                default:
                    break;
            }
        }
    });

    WinJS.Namespace.define("SimpleCommunication.Videochat", {
        VideoChatModel: videoChatModelType
    });

})();

(function () {
    "use strict";
    var model = null;

    var page = WinJS.UI.Pages.define("/html/scenario2_VideoChat.html", {
        ready: function () {
            if (!model) {
                model = new SimpleCommunication.Videochat.VideoChatModel();
            }
            model.initialize();
        },

        unload: function () {
            model.deactivate();
            model = null;
        }
    });
})();
