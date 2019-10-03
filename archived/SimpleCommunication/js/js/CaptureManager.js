//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    function createError(errorName, message) {
        var e = new Error(message);
        e.name = errorName;
        e.number = -1;
        return e;
    }

    // Capture device represents a device used in one capture session between
    // calling CaptureManager.lockAsync and CaptureManager.unlockAsync.
    var CaptureDevice = WinJS.Class.define(function () {
        this._incomingConnectionHandler = this.onIncomingConnection.bind(this);
        this._captureFailedHandler = this.onCaptureFailed.bind(this);
    },
    {
        // Media capture object
        _mediaCapture: null,
        // Custom media sink
        _mediaSink: null,
        // Flag indicating if recording to custom sink has started
        _recordingStarted: false,

        // Event handlers
        _incomingConnectionHandler: null,
        _captureFailedHandler: null,

        /// <summary>
        /// Initialization of the device.
        /// </summary>
        _initializeAsync: function () {
            if (this._mediaCapture) {
                // Double initialization
                throw createError("SimpleCommunication.CameraAlreadyInitialized", "Camera is already initialized");
            }

            this._mediaCapture = new Windows.Media.Capture.MediaCapture();
            this._mediaCapture.addEventListener(CaptureDevice.failedEvent, this._captureFailedHandler);

            var that = this;
            return this._mediaCapture.initializeAsync().then(null, function (e) {
                that._doCleanup();
                throw e;
            });
        },

        /// <summary>
        /// Cleaning up the sink.
        /// </summary>
        _cleanupSink: function () {
            if (this._mediaSink) {
                this._mediaSink.removeEventListener(CaptureDevice.incomingConnectionEvent, this._incomingConnectionHandler, false);
                this._mediaSink.close();
                this._mediaSink = null;
                this._recordingStarted = false;
            }
        },

        /// <summary>
        /// Cleans up the resources.
        /// </summary>
        _doCleanup: function () {
            if (this._mediaCapture) {
                this._mediaCapture.removeEventListener(CaptureDevice.failedEvent, this._captureFailedHandler, false);
                this._mediaCapture = null;
            }

            this._cleanupSink();
        },

        /// <summary>
        /// Asynchronous method cleaning up resources and stopping recording if necessary.
        /// </summary>
        _cleanupAsync: function () {
            if (!this._mediaCapture && !this._mediaSink) {
                return WinJS.Promise.wrap();
            }
            var promise = null;
            if (this._recordingStarted) {
                promise = this._mediaCapture.stopRecordAsync();
            } else {
                promise = new WinJS.Promise.wrap();
            }

            var that = this;
            return promise.then(function () {
                that._doCleanup();
            }, function (e) {
                that._doCleanup();
            });
        },

        /// <summary>
        /// Creates url object from MediaCapture
        /// </summary>
        createBlob: function () {
            return window.URL.createObjectURL(this._mediaCapture, { oneTimeOnly: true });
        },

        /// <summary>
        /// Function allows to select camera settings.
        /// </summary>
        /// <param name="mediaStreamType" type="Windows.Media.Capture.MediaStreamType">
        /// Type of a the media stream.
        /// </param>
        /// <param name="mediaStreamType">
        /// Function which will be called to filter the correct settings.
        /// </param>
        selectPreferredCameraStreamSettingAsync: function (mediaStreamType, settingsFilterFunc) {
            var prefferedSettings = this._mediaCapture.videoDeviceController.
                getAvailableMediaStreamProperties(mediaStreamType).
                filter(settingsFilterFunc).sort(
                    function (prop1, prop2) {
                        return (prop2.width - prop1.width);
                    });

            if (prefferedSettings.length > 0 && prefferedSettings[0]) {
                return this._mediaCapture.videoDeviceController.setMediaStreamPropertiesAsync(
                    mediaStreamType,
                    prefferedSettings[0]
                );
            }

            return new WinJS.Promise.wrap();
        },

        /// <summary>
        /// Begins recording
        /// </summary>
        /// <param name="mediaEncodingProfile">
        /// Encoding profile which should be used for the recording session.
        /// </param>
        startRecordingAsync: function (mediaEncodingProfile) {
            var that = this;

            // We cannot start recording twice.
            if (this._mediaSink && this._recordingStarted) {
                return WinJS.Promise.wrapError(createError("SimpleCommunication.InvalidState", "Recording already started"));
            }

            // Release sink if there is one already.
            this._cleanupSink();

            // Create new sink
            this._mediaSink = new Microsoft.Samples.SimpleCommunication.StspMediaSinkProxy();
            this._mediaSink.addEventListener(CaptureDevice.incomingConnectionEvent, this._incomingConnectionHandler, false);

            return this._mediaSink.initializeAsync(mediaEncodingProfile.audio, mediaEncodingProfile.video).then(function (mfExtension) {
                return that._mediaCapture.startRecordToCustomSinkAsync(mediaEncodingProfile, mfExtension);
            }).then(function () {
                that._recordingStarted = true;
            }, function (e) {
                that._cleanupSink();
                throw e;
            });
        },

        /// <summary>
        /// Stops recording.
        /// </summary>
        stopRecordingAsync: function () {
            var that = this;

            if (this._recordingStarted) {
                return this._mediaCapture.stopRecordAsync().then(function () {
                    that._cleanupSink();
                }, function (e) {
                    that._cleanupSink();
                });
            }
            // If recording not started just do nothing
            return WinJS.Promise.wrap();
        },

        /// <summary>
        /// Capture has failed.
        /// </summary>
        onCaptureFailed: function (e) {
            // Forward the error to listeners.
            this.dispatchEvent(CaptureDevice.failedEvent, e);
        },

        /// <summary>
        /// There is an incoming connection.
        /// </summary>
        onIncomingConnection: function (e) {
            // Forward the event to listeners.
            this.dispatchEvent(CaptureDevice.incomingConnectionEvent, e);
        }
    },
    {
        failedEvent: { value: "failed", writable: false },
        incomingConnectionEvent: { value: "incomingconnectionevent", writable: false }
    });

    WinJS.Class.mix(CaptureDevice, WinJS.Utilities.eventMixin);

    // Manager which makes sure that only one capture device object is in use.
    var CaptureManager = WinJS.Class.define(
        null,
    {
        // Promise which is used to complete lockAsyn method.
        _lockPromise: null,
        // Device which is currently being used.
        _lockDev: null,
        // We are in middle of locking operation.
        _locking: false,

        /// <summary>
        /// Locks the device
        /// </summary>
        lockAsync: function () {
            var that = this;
            var scenarioControl = document.getElementById("scenarioControl").winControl;

            if (this._locking) {
                // If we are in locking phase try again in 0.5sec.
                return WinJS.Promise.timeout(500).then(function () {
                    return that.lockAsync();
                });
            } else if (scenarioControl) {
                scenarioControl.tapBehavior = WinJS.UI.TapBehavior.none;
            }
            if (this._lockDev) {
                // If there is a client already waiting for the lock to happen cancel it.
                if (this._lockPromise) {
                    this._lockPromise.promise.cancel();
                    this._lockPromise = null;
                }
                this._lockPromise = {
                    promise: null,
                    complete: null,
                    error: null
                };

                this._lockPromise.promise = new WinJS.Promise(function (complete, error) {
                    that._lockPromise.complete = complete;
                    that._lockPromise.error = error;
                });

                return this._lockPromise.promise;
            } else {
                var dev = new CaptureDevice();
                this._locking = true;
                return dev._initializeAsync().then(function () {
                    that._lockDev = dev;
                    that._locking = false;
                    if (scenarioControl) {
                        scenarioControl.tapBehavior = WinJS.UI.TapBehavior.directSelect;
                    }
                    return WinJS.Promise.wrap(dev);
                }, function (e) {
                    that._locking = false;
                    if (scenarioControl) {
                        scenarioControl.tapBehavior = WinJS.UI.TapBehavior.directSelect;
                    }
                    throw e;
                });
            }
        },

        /// <summary>
        /// Unlocks the device
        /// </summary>
        unlockAsync: function (unlockDev) {
            var that = this;
            var scenarioControl = document.getElementById("scenarioControl").winControl;
            if (this._lockDev && this._lockDev === unlockDev) {
                return this._lockDev._cleanupAsync().then(function () {
                    return WinJS.Promise.timeout(500);
                }).then(function () {
                    that._lockDev = null;
                    if (that._lockPromise) {
                        var dev = new CaptureDevice();
                        that._locking = true;
                        return dev._initializeAsync().then(function () {
                            that._lockDev = dev;
                            that._locking = false;
                            // Transfer lock to another client
                            var promise = that._lockPromise;
                            that._lockPromise = null;
                            if (scenarioControl) {
                                scenarioControl.tapBehavior = WinJS.UI.TapBehavior.directSelect;
                            }
                            promise.complete(dev);
                        }, function (e) {
                            that._locking = false;
                            if (scenarioControl) {
                                scenarioControl.tapBehavior = WinJS.UI.TapBehavior.directSelect;
                            }
                            promise.error(e);
                        });
                    } else {
                        return WinJS.Promise.wrap();
                    }
                });
            } else {
                return WinJS.Promise.wrap();
            }
        },

        /// <summary>
        /// Checks if there is a camera installed in the system.
        /// </summary>
        checkForRecordingDevicesAsync: function () {
            return new WinJS.Promise(function (complete, error) {
                try {
                    Windows.Devices.Enumeration.DeviceInformation.findAllAsync(Windows.Devices.Enumeration.DeviceClass.videoCapture).
                    then(function (devices) {
                        if (devices.length === 0) {
                            complete(false);
                        } else {
                            Windows.Devices.Enumeration.DeviceInformation.findAllAsync(Windows.Devices.Enumeration.DeviceClass.audioCapture).
                            then(function (audioDevices) {
                                complete(audioDevices.length > 0);
                            },
                            function (e) {
                                error(e);
                            });
                        }
                    }).then(null,
                    function (e) {
                        error(e);
                    });
                } catch (e) {
                    error(e);
                }
            });
        }
    });

    var _captureMgr;

    // Establish members of  'SimpleCommunication' namespace.
    WinJS.Namespace.define("SimpleCommunication", {
        captureMgr: {
            get: function () {
                if (!_captureMgr) {
                    _captureMgr = new CaptureManager();
                }

                return _captureMgr;
            }
        },

        mediaExtensionMgr: new Windows.Media.MediaExtensionManager(),

        createError: createError
    });

    SimpleCommunication.mediaExtensionMgr.registerSchemeHandler("Microsoft.Samples.SimpleCommunication.StspSchemeHandler",
        "stsp:");
})();
