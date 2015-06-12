//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

(function () {
    "use strict";

    function ConnectedDevice(displayName, wfdDevice, socketRW)
    {
        this.displayName = displayName;
        this.wfdDevice = wfdDevice;
        this.socketRW = socketRW;
    }

    function DiscoveredDevice(deviceInfo)
    {
        this.displayName = deviceInfo.name;
        this.deviceInfo = deviceInfo;
    }

    function SocketReaderWriter(socket, displayName)
    {
        this.dataReader = new Windows.Storage.Streams.DataReader(socket.inputStream);
        this.dataReader.unicodeEncoding = Windows.Storage.Streams.UnicodeEncoding.utf8
        this.dataReader.byteOrder = Windows.Storage.Streams.ByteOrder.littleEndian;

        this.dataWriter = new Windows.Storage.Streams.DataWriter(socket.outputStream);
        this.dataWriter.unicodeEncoding = Windows.Storage.Streams.UnicodeEncoding.utf8;
        this.dataWriter.byteOrder = Windows.Storage.Streams.ByteOrder.littleEndian;

        this.streamSocket = socket;
        this.displayName = displayName;
        this.currentMessage = null;
    }

    SocketReaderWriter.prototype.WriteMessage = function(message)
    {
            this.dataWriter.writeUInt32(this.dataWriter.measureString(message));
            this.dataWriter.writeString(message);
            this.dataWriter.storeAsync().done(function ()
            {
                WinJS.log && WinJS.log("Sent message: " + message, "sample", "status");
            },
            function (err)
            {
                WinJS.log && WinJS.log("WriteMessage threw exception: " + err.message, "sample", "error");
            });
    }

    SocketReaderWriter.prototype.ReadMessage = function () 
    {
        var self = this;
            this.dataReader.loadAsync(4).done(
                function (bytesRead)
                {
                    if (bytesRead > 0)
                    {
                        // Determine how long the string is.
                        var messageLength = self.dataReader.readInt32();
                        self.dataReader.loadAsync(messageLength).done(
                            function (bytesRead)
                            {
                                if (bytesRead > 0)
                                {
                                    // Decode the string.
                                    self.currentMessage = self.dataReader.readString(messageLength);
                                    WinJS.log && WinJS.log("Got message: " + self.currentMessage, "sample", "status");

                                    self.ReadMessage();
                                }
                            },
                            function (err)
                            {
                                WinJS.log && WinJS.log("ReadMessage threw exception: " + err, "sample", "error");
                            });
                    }
                },
                function (err)
                {
                    WinJS.log && WinJS.log("WriteMessage threw exception: " + err, "sample", "error");
                });
    }

    SocketReaderWriter.prototype.GetCurrentMessage = function() 
    {
        return this.currentMessage;
    }

    SocketReaderWriter.prototype.SetDisplayName = function(displayName) 
    {
        this.displayName = displayName;
    }

    SocketReaderWriter.prototype.Dispose = function()
    {
        this.dataReader.close();
        this.dataWriter.close();
        this.streamSocket.close();
    }

    WinJS.Namespace.define("WfdHelper",
    {
        ConnectedDevice: ConnectedDevice,
        SocketReaderWriter: SocketReaderWriter,
        DiscoveredDevice: DiscoveredDevice
    });

})();