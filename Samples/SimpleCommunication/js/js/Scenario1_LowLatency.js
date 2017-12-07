//// Copyright (c) Microsoft Corporation. All rights reserved
(function () {
    'use strict';

    var scenarioOneStates = {
        initializing: "initializing",
        waiting: "waiting",
        previewing: "previewing",
        streaming: "streaming",
        end: "end"
    },
    latencyModes = {
        highLatency: "highLatency",
        lowLatency: "lowLatency"
    };

    var LowLatencyModelType = WinJS.Class.define(function () {
        this._previewButtonElement = document.getElementById("previewButton");
        this._localClientElement = document.getElementById("clientButton");
        this._latencyModeToggleSwitch = document.getElementById("latencyModeToggle").winControl;

        this._previewVideoElement = document.getElementById("previewVideo");
        this._previewVideoElement.autoplay = false;
        this._previewVideoElement.msHorizontalMirror = true;

        this._playbackVideoElement = document.getElementById("playbackVideo");
        this._playbackVideoElement.autoplay = true;
        this._playbackVideoElement.msHorizontalMirror = true;

        this._realTimePlaybackVideoElement = document.getElementById("realTimePlaybackVideo");
        this._realTimePlaybackVideoElement.autoplay = true;
        this._realTimePlaybackVideoElement.msHorizontalMirror = true;

        this._previewButtonElement.addEventListener("click", this._previewButtonClicked.bind(this), false);
        this._localClientElement.addEventListener("click", this._localClientButtonClicked.bind(this), false);
        this._latencyModeToggleSwitch.addEventListener("change", this._latencyModeToggled.bind(this), false);


        this._localHostVideo = {
            highLatency: this._playbackVideoElement,
            lowLatency: this._realTimePlaybackVideoElement
        };

        SdkSample.displayStatus("Loading...Please wait.");

        this._latencyMode = latencyModes.highLatency;
        this._latencyModeToggleSwitch.checked = (this._latencyMode === latencyModes.lowLatency);
        this._mediaCaptureFailedEventHandler = this._onMediaCaptureFailed.bind(this);
        this._incomingConnectionEventHandler = this._onIncomingConnection.bind(this);
        this._visibilityChangeEventHandler = this._onVisibilityChange.bind(this);
    }, {
        _device: null,
        _currentState: "",
        _previousState: "",
        _latencyMode: "",
        _mediaSink: null,
        _previewButtonElement: null,
        _localClientElement: null,
        _latencyModeToggleSwitch: null,
        _previewVideoElement: null,
        _playbackVideoElement: null,
        _realTimePlaybackVideoElement: null,
        _streamFilteringCriteria: {
            aspectRatio: "1.333333333333333",
            horizontalResolution: 640,
            subType: "YUY2"
        },
        _mediaCaptureFailedEventHandler: null,
        _incomingConnectionEventHandler: null,
        _visibilityChangeEventHandler: null,

        _onMediaCaptureFailed: function (e) {
            /// <summary>Handler for the camera 'failed' event</summary>
            /// <param name='this' type="SimpleCommunication.LowLatencyModelType"/>
            SdkSample.formatError(e.detail, "Capture engine error code: ");
        },

        _onIncomingConnection: function (e) {
            e.detail.accept();
        },

        _onVisibilityChange: function (e) {
            if (document.visibilityState !== "visible") {
                this._cleanup();
            } else {
                this.initialize();
            }
        },

        initialize: function () {
            /// <summary>Initializes the scenario</summary>
            /// <param name='this' type="SimpleCommunication.LowLatencyModelType"/>
            if (this._currentState === scenarioOneStates.initializing || this._device) {
                // We are already in intializing state.
                return;
            }
            var that = this;

            document.addEventListener("visibilitychange", this._visibilityChangeEventHandler, false);

            this._currentState = scenarioOneStates.initializing;
            SimpleCommunication.captureMgr.lockAsync().then(function (device) {
                that._device = device;
                device._lockedBy = "lowlatency";
                return device.selectPreferredCameraStreamSettingAsync(
                    Windows.Media.Capture.MediaStreamType.videoPreview,
                    function (encodingProp, index) {
                        if (encodingProp.height && encodingProp.width) {
                            return (encodingProp.width === that._streamFilteringCriteria.horizontalResolution &&
                                encodingProp.subtype === that._streamFilteringCriteria.subType);
                        }
                        return false;
                    });
            }).done(function () {
                that.actionTransitionFunctions["initializing"]["previewSetupCompleted"].call(that);
            }, function (e) {
                SdkSample.formatError(e, "Initialization error: ");
                that._currentState = scenarioOneStates.end;
                if (that._device) {
                    SimpleCommunication.captureMgr.unlockAsync(that._device).done();
                    that._device = null;
                }
            });
        },

        _startRecordingToCustomSink: function () {
            var that = this,
                prefix = this._currentState,
                //// Use the MP4 preset to an obtain H.264 video encoding profile
                mediaEncodingProfile = Windows.Media.MediaProperties.MediaEncodingProfile.createMp4(Windows.Media.MediaProperties.VideoEncodingQuality.vga);

            mediaEncodingProfile.audio = null;
            mediaEncodingProfile.container = null;

            this._device.startRecordingAsync(mediaEncodingProfile).then(function () {
                that.actionTransitionFunctions[that._currentState]["recordingStarted"].call(that);
            }).done(null, function (e) {
                SdkSample.formatError(e, prefix +
                    ": Local streaming initialization error: "
                );
                that._currentState = scenarioOneStates.end;
            });
        },

        _cleanup: function () {
            /// <summary>Cleans up.</summary>
            /// <param name='this' type="SimpleCommunication.LowLatencyModelType"/>
            this._previousState = this._currentState;

            if (this._previewVideoElement) {
                this._previewVideoElement.removeAttribute("src");
            }

            if (this._device) {
                var v = this._localHostVideo[this._latencyMode];

                this._device.removeEventListener("failed", this._mediaCaptureFailedEventHandler, false);
                this._device.removeEventListener("incomingconnectionevent", this._incomingConnectionEventHandler, false);
                SimpleCommunication.captureMgr.unlockAsync(this._device).done(function () {
                    v.removeAttribute("src");
                });
                this._device = null;
            }

            this._currentState = scenarioOneStates.end;
            this._reset();
        },

        _previewButtonClicked: function (e) {
            SdkSample.displayStatus("");
            this.actionTransitionFunctions[this._currentState]["previewButtonClicked"].call(this);
            e.stopPropagation();
        },

        _localClientButtonClicked: function (e) {
            SdkSample.displayStatus("");
            this.actionTransitionFunctions["previewing"]["localClientButtonClicked"].call(this);
            e.stopPropagation();
        },

        _latencyModeToggled: function (e) {
            SdkSample.displayStatus("");
            this.actionTransitionFunctions["streaming"]["latencyModeToggled"].call(this);
            e.stopPropagation();
        },

        _previewStarted: function () {
            this._previewVideoElement.className = "item shown";
            this._previewButtonElement.innerText = "Stop Preview";
            this._localClientElement.disabled = false;
        },

        _previewStoped: function () {
            this._reset();
        },

        _updateLocalClientWindow: function () {
            this._latencyModeToggleSwitch.disabled = false;
            this._localClientElement.disabled = true;

            if (this._latencyMode === latencyModes.highLatency) {
                this._realTimePlaybackVideoElement.className = "item";
                this._playbackVideoElement.className = "item shown";
            } else {
                this._playbackVideoElement.className = "item";
                this._realTimePlaybackVideoElement.className = "item shown";
            }
        },

        _reset: function () {
            var sl = SimpleCommunication.LowLatency;

            SdkSample.clearLastStatus();
            this._previewVideoElement.className = "item shown";
            this._playbackVideoElement.className = "";
            this._realTimePlaybackVideoElement.className = "item";

            this._previewButtonElement.innerText = "Start Preview";
            this._localClientElement.disabled = true;
            this._latencyMode = latencyModes.highLatency;
            this._latencyModeToggleSwitch.checked = (this._latencyMode === latencyModes.lowLatency);
            this._latencyModeToggleSwitch.disabled = true;

            if (this._currentState === scenarioOneStates.end) {
                this._previewButtonElement.disabled = true;
            }
        },

        /// <summary>This is the FSM encapsulating the state table (states and actions) of the scenario.</summary>
        actionTransitionFunctions: {
            initializing: {
                previewSetupCompleted: function () {
                    this._device.addEventListener("failed", this._mediaCaptureFailedEventHandler);
                    this._device.addEventListener("incomingconnectionevent", this._incomingConnectionEventHandler);
                    this._previousState = this._currentState;
                    this._currentState = scenarioOneStates.waiting;
                    this._previewButtonElement.disabled = false;
                    SdkSample.displayStatus("Done. Tap or click 'Start Preview' button to start webcam preview");
                }
            },
            waiting: {
                previewButtonClicked: function () {
                    /// <summary>
                    /// Logic when preview button is clicked and the scenario is in "waiting" state.
                    /// </summary>
                    /// <param name="this" type="SimpleCommunication.LowLatency.ModelType"/>
                    /// <returns type="String">The next state</returns>
                    this._previewVideoElement.src = this._device.createBlob();
                    this._previewVideoElement.play();
                    this._previousState = this._currentState;
                    this._currentState = scenarioOneStates.previewing;
                    this._previewStarted();
                },
            },
            previewing: {
                previewButtonClicked: function () {
                    /// <summary>
                    /// The logic when the "stop" preview button is clicked while the scenario is in "previewing" state.
                    /// </summary>
                    /// <param name="this" type="SimpleCommunication.LowLatency.ModelType"/>
                    /// <returns type="String">The next state</returns>
                    this._previewVideoElement.removeAttribute("src");
                    this._previousState = this._currentState;
                    this._currentState = scenarioOneStates.waiting;
                    this._previewStoped();
                },
                localClientButtonClicked: function () {
                    this._startRecordingToCustomSink();
                },
                recordingStarted: function () {
                    this._localHostVideo[this._latencyMode].src = "stsp://localhost";

                    this._previousState = this._currentState;
                    this._currentState = scenarioOneStates.streaming;
                    this._updateLocalClientWindow();
                },
            },
            streaming: {
                previewButtonClicked: function () {
                    /// <summary>
                    /// The logic when the "stop" preview button is clicked while the scenario is in "streaming" state.
                    /// </summary>
                    /// <param name="this" type="SimpleCommunication.LowLatency.ModelType"/>
                    /// <returns type="String">The next state</returns>

                    //// stop localhost client video
                    var currLocalHostVideo = this._localHostVideo[this._latencyMode];
                    currLocalHostVideo.pause();

                    //// stop streaming to network and close network sink
                    var that = this;
                    this._device.stopRecordingAsync().done(function () {
                        currLocalHostVideo.removeAttribute("src");
                    }, function (e) {
                        SdkSample.formatError(e, "StopRecordAsync error: ");
                        that._currentState = scenarioOneStates.end;
                    });

                    this._latencyMode = latencyModes.highLatency;
                    this.actionTransitionFunctions["previewing"]["previewButtonClicked"].call(this);
                },
                latencyModeToggled: function () {
                    var newLatencyMode = (this._latencyModeToggleSwitch.checked) ?
                        latencyModes.lowLatency : latencyModes.highLatency;

                    if (newLatencyMode === this._latencyMode) {
                        return;
                    }
                    var currLocalHostVideo = this._localHostVideo[this._latencyMode];

                    currLocalHostVideo.pause();
                    this._latencyMode = newLatencyMode;

                    if (!this._device) {
                        return;
                    }
                    var that = this;
                    this._device.stopRecordingAsync().then(function () {
                        currLocalHostVideo.removeAttribute("src");
                        that._startRecordingToCustomSink();
                    }).done(null, function (e) {
                        SdkSample.formatError(e, "Latency toggle error: ");
                        that._currentState = scenarioOneStates.end;
                    });
                },
                recordingStarted: function () {
                    /// <summary>
                    /// Logic when the scenario is in "streaming" state and "recordingStarted"
                    /// event is triggered after toggling latency mode.
                    /// </summary>
                    /// <param name="this" type="SimpleCommunication.LowLatency.ModelType"/>
                    /// <returns type="String">The next state</returns>
                    this._localHostVideo[this._latencyMode].src = "stsp://localhost";
                    this._updateLocalClientWindow();
                },
            }
        }
    });

    // Establish members of  'SimpleCommunication' namespace.
    WinJS.Namespace.define("SimpleCommunication.LowLatency", {
        LowLatencyModel: LowLatencyModelType
    });

})();

(function () {
    "use strict";
    var model = null;

    var page = WinJS.UI.Pages.define("/html/scenario1_LowLatency.html", {
        ready: function (element, options) {
            if (!model) {
                model = new SimpleCommunication.LowLatency.LowLatencyModel();

                // Check for camera and init scenario if one is found
                SimpleCommunication.captureMgr.checkForRecordingDevicesAsync().done(function (result) {
                    if (result) {
                        model.initialize();
                    } else {
                        SdkSample.displayError("A machine with a camera and a microphone is required to run this sample.");
                    }
                }, function (e) {
                    SdkSample.formatError(e, "Initialization error: ");
                });
            } else {
                model.initialize();
            }
        },

        unload: function () {
            model._cleanup();
            model = null;
        }
    });
})();
