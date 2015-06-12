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
    var page = WinJS.UI.Pages.define("/html/scenario1_advertiser.html", {
        ready: function (element, options) {
            document.getElementById("btnStartAdvertisement").addEventListener("click", btnStartAdvertisement_Click, false);
            document.getElementById("btnStopAdvertisement").addEventListener("click", btnStopAdvertisement_Click, false);
            document.getElementById("btnSendMessage").addEventListener("click", btnSendMessage_Click, false);
            document.getElementById("btnCloseDevice").addEventListener("click", btnCloseDevice_Click, false);
            document.getElementById("btnAddIe").addEventListener("click", btnAddIe_Click, false);

            var connectedDevicesList = document.getElementById("lvConnectedDevices").winControl;
            connectedDevicesListBinding = new WinJS.Binding.List([]);

            connectedDevicesList.itemDataSource = connectedDevicesListBinding.dataSource;
            connectedDevicesList.forceLayout();
        }
    });

    var _publisher = null;
    var _listener = null;
    var _listenerSocket = null;
    var strServerPort = "50001";
    var customOui = [170, 187, 204];
    var customOuiType = 221;
    var connectedDevicesListBinding;

    function btnStartAdvertisement_Click() {
        if (_publisher === null) {
            _publisher = new Windows.Devices.WiFiDirect.WiFiDirectAdvertisementPublisher();
        }

        if (chkListener.checked === true) {
            if (_listener === null) {
                _listener = new Windows.Devices.WiFiDirect.WiFiDirectConnectionListener();
            }

            _listener.onconnectionrequested = OnConnectionRequested;
        }

        _publisher.advertisement.isAutonomousGroupOwnerEnabled = (chkPreferGroupOwnerMode.IsChecked === true);

        switch (cmbListenState.value) {
            case "None":
                _publisher.advertisement.listenStateDiscoverability = Windows.Devices.WiFiDirect.WiFiDirectAdvertisementListenStateDiscoverability.none;
                break;
            case "Normal":
                _publisher.advertisement.listenStateDiscoverability = Windows.Devices.WiFiDirect.WiFiDirectAdvertisementListenStateDiscoverability.normal;
                break;
            case "Intensive":
                _publisher.advertisement.listenStateDiscoverability = Windows.Devices.WiFiDirect.WiFiDirectAdvertisementListenStateDiscoverability.intensive;
                break;
        }

        _publisher.onstatuschanged = OnStatusChanged;

        _publisher.start();

        WinJS.log && WinJS.log("Advertisement started, waiting for StatusChanged callback...", "sample", "status");
    }

    function btnStopAdvertisement_Click() {
        try {
            if (_publisher !== null) {
                _publisher.stop();
                _publisher.onstatuschanged = null;

                WinJS.log && WinJS.log("Advertisement stopped successfully", "sample", "status");
            }

            if (_listener !== null) {
                _listener.onconnectionRequested = null;
            }
        }
        catch (err) {
            WinJS.log && WinJS.log("Error stopping Advertisement: " + err.toString(), "sample", "error");
        }
    }

    function btnAddIe_Click() {
        if (_publisher === null) {
            _publisher = new Windows.Devices.WiFiDirect.WiFiDirectAdvertisementPublisher();
        }

        var txtIE = document.getElementById("txtInformationElement");
        if (txtIE.value === "") {
            WinJS.log && WinJS.log("Please specifiy an IE", "sample", "error");
            return;
        }

        var ie = new Windows.Devices.WiFiDirect.WiFiDirectInformationElement();

        // IE blob
        var dataWriter = new Windows.Storage.Streams.DataWriter();
        dataWriter.unicodeEncoding = Windows.Storage.Streams.UnicodeEncoding.utf8
        dataWriter.byteOrder = Windows.Storage.Streams.ByteOrder.littleEndian;
        dataWriter.writeUInt32(dataWriter.measureString(txtIE.value));
        dataWriter.writeString(txtIE.value);
        ie.value = dataWriter.detachBuffer();

        // OUI
        var dataWriterOUI = new Windows.Storage.Streams.DataWriter();
        dataWriterOUI.writeBytes(customOui);
        ie.oui = dataWriterOUI.detachBuffer();

        // OUI Type
        ie.ouiType = customOuiType;

        _publisher.advertisement.informationElements.append(ie);
        txtIE.value = "";

        WinJS.log && WinJS.log("IE added successfully", "sample", "status");
    }

    function OnStatusChanged(e) {
        WinJS.log && WinJS.log("Advertisement: Status: " + e.status.toString() + " Error: " + e.error.toString(), "sample", "status");
        return;
    }

    function OnConnectionRequested(e) {
        var connectionRequest = e.getConnectionRequest();

        WinJS.log && WinJS.log("Connecting to " + connectionRequest.deviceInformation.name + "...", "sample", "status");

        var connectionParams = new Windows.Devices.WiFiDirect.WiFiDirectConnectionParameters();
        connectionParams.groupOwnerIntent = parseInt(txtGOIntent.value.toString());

        Windows.Devices.WiFiDirect.WiFiDirectDevice.fromIdAsync(connectionRequest.deviceInformation.id, connectionParams).then(
        function (wfdDevice) {
            if (wfdDevice === null) {
                WinJS.log && WinJS.log("Connection to " + connectionRequest.deviceInformation.name + " failed", "sample", "error");
                return;
            }

            // Register for the ConnectionStatusChanged event handler
            wfdDevice.onconnectionstatuschanged = OnConnectionStatusChanged;

            connectedDevicesListBinding.push(new WfdHelper.ConnectedDevice("dummy", wfdDevice, null));

            // Get the EndpointPair information.
            var endpointPairCollection = wfdDevice.getConnectionEndpointPairs();

            _listenerSocket = null;
            _listenerSocket = new Windows.Networking.Sockets.StreamSocketListener();
            _listenerSocket.onconnectionreceived = OnSocketConnectionReceived;

            _listenerSocket.bindEndpointAsync(endpointPairCollection[0].localHostName, strServerPort).then(
            function () {
                WinJS.log && WinJS.log("Devices connected on L2, listening on IP Address: " + endpointPairCollection[0].localHostName + " Port: " + strServerPort, "sample", "status");
                return;
            },
            function (err) {
                WinJS.log && WinJS.log("Failed to bind: " + err.toString(), "sample", "error");
                return;
            });
        },
        function (err) {
            WinJS.log && WinJS.log("Connect operation failed: " + err.toString(), "sample", "error");
            return;
        });
    }

    function OnConnectionStatusChanged(e) {
        WinJS.log && WinJS.log("Connection disconnected", "sample", "status");
    }

    function OnSocketConnectionReceived(e) {
        WinJS.log && WinJS.log("Connecting to remote side on L4 layer...", "sample", "status");
        var serverSocket = e.socket;

        try {
            var socketRW = new WfdHelper.SocketReaderWriter(serverSocket, "dummy");
            socketRW.ReadMessage();

            while (true) {
                var sessionId = socketRW.GetCurrentMessage();
                if (sessionId !== null) {
                    WinJS.log && WinJS.log("Connected with remote side on L4 layer", "sample", "status");

                    for (var idx = 0; idx < connectedDevicesListBinding.length; idx++) {
                        var item = connectedDevicesListBinding.getAt(idx);
                        if (item.displayName === "dummy") {
                            connectedDevicesListBinding.setAt(idx, new WfdHelper.ConnectedDevice(sessionId, item.wfdDevice, socketRW));
                            break;
                        }
                    }
                    break;
                }

                var wakeupTime = new Date().getTime() + 100;
                while (new Date().getTime() < wakeupTime);
            }
        }
        catch (err) {
            WinJS.log && WinJS.log("Connection failed: " + err.toString(), "sample", "error");
        }
    }

    function btnSendMessage_Click() {
        var connectedDevicesList = document.getElementById("lvConnectedDevices").winControl;

        if (connectedDevicesList.selection.count() == 0) {
            WinJS.log && WinJS.log("Please select a Session to send data", "sample", "error");
            return;
        }

        var messsage = document.getElementById("txtSendMessage");
        if (messsage.value === "") {
            WinJS.log && WinJS.log("Please type a message to send", "sample", "error");
            return;
        }

        connectedDevicesList.selection.getItems().done(function (items) {
            items[0].data.socketRW.WriteMessage(messsage.value);
        },
        function (err) {
            WinJS.log && WinJS.log("WriteMessage failed: " + err.toString(), "sample", "error");
            return;
        });
    }

    function btnCloseDevice_Click() {
        var connectedDevicesList = document.getElementById("lvConnectedDevices").winControl;

        if (connectedDevicesList.selection.count() == 0) {
            WinJS.log && WinJS.log("Please select a device to close", "sample", "error");
            return;
        }

        connectedDevicesList.selection.getItems().done(function (items) {
            if (items[0].data.socketRW !== null) items[0].data.socketRW.Dispose();
            if (items[0].data.wfdDevice !== null) items[0].data.wfdDevice.close();
            connectedDevicesListBinding.splice(items[0].index, 1);
        },
        function (err) {
            WinJS.log && WinJS.log("Close failed: " + err.toString(), "sample", "error");
            return;
        });
    }

})();
