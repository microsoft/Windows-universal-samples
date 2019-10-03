//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

/*************************************************/
/* Definitions for WinJS.Compressor              */
/*************************************************/
(function (global, /*@override*/WinJS) {
    function getUnderlyingStreamPromise(underlyingStream, that) {
        //
        // There is no way to check if object supports particular interface so we use duck typing
        // to do the work. Keep in mind though, that underlying implementation relies on actual
        // COM interfaces so if we have "false-positive duck", call will fail somewhere in
        // implementation instead of throwing proper Error object. This is mitigated by the fact
        // that client of this code will always know concrete type of object it created and chances
        // to unintentionally provide a false-positive interface are fairly low
        // 
        if (typeof(underlyingStream) === "string" || underlyingStream instanceof String) {
            // Filename is provided
            var tempDir = Windows.Storage.ApplicationData.current.temporaryFolder;
            return tempDir.createFileAsync(underlyingStream, Windows.Storage.CreationCollisionOption.replaceExisting).then(function (file) {
                that._storageFile = file;
                return file.openAsync(Windows.Storage.FileAccessMode.readWrite);
            }).then(function (randomAccessStream) {
                that._ownedStream = randomAccessStream;
                return that._ownedStream;
            });
        } else if (typeof (underlyingStream.writeAsync) === "function") {
            // It quacks like IOutputStream
            // NOTE: we don't own this stream - don't close it when we close ourselves
            return WinJS.Promise.as(underlyingStream);
        } else if (typeof(underlyingStream.openAsync) === "function") {
            that._storageFile = underlyingStream;
            // It quacks like IStorageFile
            return underlyingStream.openAsync(Windows.Storage.FileAccessMode.readWrite).then(function (randomAccessStream) {
                that._ownedStream = randomAccessStream;
                return that._ownedStream;
            });
        } else {
            throw new Error("Cannot create IOutputStream from underlyingStream object");
        }
    }

    var Compressor = WinJS.Class.define(
        //
        // Constructor
        //
        function (underlyingStream, compressAlgorithm) {
            this._underlyingStreamPromise = getUnderlyingStreamPromise(underlyingStream, this);
            this._compressAlgorithm = compressAlgorithm ? compressAlgorithm : Windows.Storage.Compression.CompressAlgorithm.xpress;
        },

        //
        // Public members
        //
        {
            compressAsync: function (data) {
                var that = this;

                if (!that._underlyingStreamPromise) {
                    return WinJS.Promise.wrapError(new Error("Object has already been closed"));
                }

                return new WinJS.Promise(function (complete, error) {
                    that._underlyingStreamPromise.then(function (underlyingStream) {
                        var compressor = new Windows.Storage.Compression.Compressor(underlyingStream, that._compressAlgorithm, /* use default block size */0);
                        var writer = new Windows.Storage.Streams.DataWriter(compressor);

                        function finalize() {
                            compressor.finishAsync().then(function () {
                                compressor.detachStream();
                                compressor.close();
                                underlyingStream.flushAsync().done(complete, error);
                            }, error);
                        }

                        if (typeof(data.readAsync) === "function") {
                            // It quacks like IInputStream
                            Windows.Storage.Stream.RandomAccessStream.copyAsync(data, compressor).done(finalize, error);
                        } else if (typeof(data.openAsync) === "function") {
                            // It quacks like IStorageFile
                            data.openAsync(Windows.Storage.FileAccessMode.read).done(function (randomAccessStream) {
                                return Windows.Storage.Streams.RandomAccessStream.copyAsync(randomAccessStream, compressor).then(function () {
                                    randomAccessStream.close();
                                    finalize();
                                }, error);
                            }, error);
                        } else if (typeof(data) === "string" || data instanceof String) {
                            writer.writeString(data);
                            writer.storeAsync().done(finalize, error);
                        } else if (typeof(data) === "object") {
                            writer.writeString(JSON.stringify(data));
                            writer.storeAsync().done(finalize, error);
                        } else {
                            error(new Error("Invalid input data"));
                        }
                    }, error);
                });
            },

            //
            // Note: fileName is only valid for Compressor objects, created over string (filename) and
            // IStorageFile object. It's also not guaranteed that fileName will be the same as the name
            // passed to constructor because of file renaming. For example, if you already have file
            // "filename.ext" and want to create another one with the same name it will be created as
            // "filename (2).ext"
            //
            fileName: {
                get: function () {
                    if (this._storageFile) {
                        return this._storageFile.name;
                    } else {
                        return null;
                    }
                }
            },

            close: function () {
                if (this._ownedStream) {
                    this._ownedStream.close();
                    this._ownedStream = null;
                }
                this._underlyingStreamPromise = null;
            }
        }
    );

    WinJS.Namespace.define("WinJS", {
        Compressor: Compressor
    });
})(this, WinJS);

/*************************************************/
/* Definitions for WinJS.Decompressor            */
/*************************************************/
(function (global, /*@override*/WinJS) {
    function getUnderlyingStreamPromise(underlyingStream, that) {
        //
        // See "duck typing" comment above
        //
        if (typeof(underlyingStream) === "string" || underlyingStream instanceof String) {
            // Filename is provided
            var tempDir = Windows.Storage.ApplicationData.current.temporaryFolder;
            return tempDir.getFileAsync(underlyingStream).then(function (file) {
                return file.openAsync(Windows.Storage.FileAccessMode.read);
            }).then(function (randomAccessStream) {
                that._ownedStream = randomAccessStream;
                return that._ownedStream;
            });
        } else if (typeof (underlyingStream.readAsync) === "function") {
            // It quacks like IInputStream
            // NOTE: we don't own this stream - don't close it when we close ourselves
            return WinJS.Promise.as(underlyingStream);
        } else if (typeof(underlyingStream.openAsync) === "function") {
            // It quacks like IStorageFile
            return underlyingStream.openAsync(Windows.Storage.FileAccessMode.read).then(function (randomAccessStream) {
                that._ownedStream = randomAccessStream;
                return that._ownedStream;
            });
        } else {
            throw new Error("Cannot create IInputStream from underlyingStream object");
        }
    }

    var Decompressor = WinJS.Class.define(
        //
        // Constructor
        //
        function (underlyingStream) {
            this._underlyingStreamPromise = getUnderlyingStreamPromise(underlyingStream, this);
        },

        //
        // Public members
        //
        {
            readStringAsync: function () {
                var that = this;

                if (!that._underlyingStreamPromise) {
                    return WinJS.Promise.wrapError(new Error("Object has already been closed"));
                }

                return new WinJS.Promise(function (complete, error) {
                    that._underlyingStreamPromise.then(function (underlyingStream) {
                        var decompressor = new Windows.Storage.Compression.Decompressor(underlyingStream);
                        var reader = new Windows.Storage.Streams.DataReader(decompressor);

                        var result = "";
                        function readStream(chunkLength) {
                            reader.loadAsync(chunkLength).done(function (bytesRead) {
                                if (bytesRead === 0) {
                                    decompressor.detachStream();
                                    decompressor.close();
                                    complete(result);
                                    return;
                                }

                                result += reader.readString(bytesRead);
                                readStream(chunkLength);
                            }, error);
                        }

                        readStream(16384);
                    }, error);
                });
            },

            readObjectAsync: function (reviver) {
                var that = this;
                return new WinJS.Promise(function (complete, error) {
                    that.readStringAsync().then(function (str) {
                        try {
                            complete(JSON.parse(str, reviver));
                        } catch (e) {
                            error(e);
                        }
                    }, error);
                });
            },

            copyAsync: function (destinationStream) {
                var that = this;

                if (!that._underlyingStreamPromise) {
                    return WinJS.Promise.wrapError(new Error("Object has already been closed"));
                }

                return new WinJS.Promise(function (complete, error) {
                    that._underlyingStreamPromise.then(function (underlyingStream) {
                        var decompressor = new Windows.Storage.Compression.Decompressor(underlyingStream);
                        var openedStream = null;

                        function finalize() {
                            decompressor.detachStream();
                            decompressor.close();
                            if (openedStream) {
                                openedStream.flushAsync().then(function () {
                                    openedStream.close();
                                    complete();
                                }, error);
                            } else {
                                complete();
                            }
                        }

                        if (typeof(destinationStream) === "string" || destinationStream instanceof String) {
                            // Filename is provided
                            var tempDir = Windows.Storage.ApplicationData.current.temporaryFolder;
                            tempDir.createFileAsync(destinationStream, Windows.Storage.CreationCollisionOption.replaceExisting).then(function (file) {
                                return file.openAsync(Windows.Storage.FileAccessMode.readWrite);
                            }).then(function (randomAccessStream) {
                                openedStream = randomAccessStream;
                                Windows.Storage.Streams.RandomAccessStream.copyAsync(decompressor, randomAccessStream).done(finalize, error);
                            }, error);
                        } else if (typeof(destinationStream.writeAsync) === "function") {
                            // It quacks like IOutputStream
                            Windows.Storage.Stream.RandomAccessStream.copyAsync(decompressor, destinationStream).done(finalize, error);
                        } else if (typeof(data.openAsync) === "function") {
                            // It quacks like IStorageFile
                            data.openAsync(Windows.Storage.FileAccessMode.readWrite).then(function (randomAccessStream) {
                                openedStream = randomAccessStream;
                                Windows.Storage.Streams.RandomAccessStream.copyAsync(decompressor, randomAccessStream).done(finalize, error);
                            }, error);
                        } else {
                            error(new Error("Invalid destination stream"));
                        }
                    }, error);
                });
            },

            close: function () {
                if (this._ownedStream) {
                    this._ownedStream.close();
                    this._ownedStream = null;
                }
                this._underlyingStreamPromise = null;
            }
        }
    );

    WinJS.Namespace.define("WinJS", {
        Decompressor: Decompressor
    });
})(this, WinJS);
