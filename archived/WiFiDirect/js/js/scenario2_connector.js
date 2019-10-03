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
    var page = WinJS.UI.Pages.define("/html/scenario2_connector.html", {
        ready: function (element, options) {
            document.getElementById("btnGetDevices").addEventListener("click", btnGetDevices_Click, false);
            document.getElementById("btnIe").addEventListener("click", btnIe_Click, false);
            document.getElementById("btnFromId").addEventListener("click", btnFromId_Click, false);
            document.getElementById("btnSendMessage").addEventListener("click", btnSendMessage_Click, false);
            document.getElementById("btnCloseDevice").addEventListener("click", btnCloseDevice_Click, false);

            var discoveredDevicesList = document.getElementById("lvDiscoveredDevices").winControl;
            discoveredDevicesListBinding = new WinJS.Binding.List([]);

            discoveredDevicesList.itemDataSource = discoveredDevicesListBinding.dataSource;
            discoveredDevicesList.forceLayout();

            var connectedDevicesList = document.getElementById("lvConnectedDevices").winControl;
            connectedDevicesListBinding = new WinJS.Binding.List([]);

            connectedDevicesList.itemDataSource = connectedDevicesListBinding.dataSource;
            connectedDevicesList.forceLayout();
        }
    });

    var strServerPort = "50001";

    var customOui = [170, 187, 204];
    var customOuiType = 221;
    var wfaOui = [80, 111, 154];
    var msftOui = [0, 80, 242];

    var discoveredDevicesListBinding;
    var connectedDevicesListBinding;

    function btnGetDevices_Click() {
        WinJS.log && WinJS.log("Finding Devices......", "sample", "status");

        discoveredDevicesListBinding.splice(0, discoveredDevicesListBinding.length);

        var deviceSelector;
        switch (cmbDeviceSelector.value) {
            case "AssociationEndpoint":
                deviceSelector = Windows.Devices.WiFiDirect.WiFiDirectDevice.getDeviceSelector(Windows.Devices.WiFiDirect.WiFiDirectDeviceSelectorType.associationEndpoint);
                break;

            default:
                deviceSelector = Windows.Devices.WiFiDirect.WiFiDirectDevice.getDeviceSelector(Windows.Devices.WiFiDirect.WiFiDirectDeviceSelectorType.deviceInterface);
                break;
        }

        var targetWfdProperties = ["System.Devices.WiFiDirect.InformationElements"];
        Windows.Devices.Enumeration.DeviceInformation.findAllAsync(deviceSelector, targetWfdProperties).done(
            function (deviceInfoCollection) {
                if (deviceInfoCollection.size === 0) {
                    WinJS.log && WinJS.log("No devices found", "sample", "error");
                    return;
                }

                deviceInfoCollection.forEach(function (item, idx) {
                    discoveredDevicesListBinding.push(new WfdHelper.DiscoveredDevice(item));
                });

                WinJS.log && WinJS.log(deviceInfoCollection.size.toString() + " devices found", "sample", "status");
            });
    }

    function btnIe_Click() {
        var discoveredDevicesList = document.getElementById("lvDiscoveredDevices").winControl;

        if (discoveredDevicesList.selection.count() === 0) {
            WinJS.log && WinJS.log("No device selected, please select one", "sample", "error");
            return;
        }

        discoveredDevicesList.selection.getItems().done(function (items) {
            var deviceInfo = items[0].data.deviceInfo;
            var iECollection = Windows.Devices.WiFiDirect.WiFiDirectInformationElement.createFromDeviceInformation(deviceInfo);

            if (iECollection.size === 0) {
                WinJS.log && WinJS.log("No Information Elements found", "sample", "error");
                return;
            }

            var strIeOutput = "";
            var strIe = "N/A";

            iECollection.forEach(function (ie) {
                var oui = new Uint8Array(ie.oui.length);
                var dataReader = Windows.Storage.Streams.DataReader.fromBuffer(ie.oui);
                dataReader.readBytes(oui);
                dataReader.close();

                var bOuiStr = oui.toString(16);
                if (bOuiStr === msftOui.toString()) {
                    strIeOutput = strIeOutput.concat("Microsoft IE: ");
                }
                else if (bOuiStr === wfaOui.toString()) {
                    strIeOutput = strIeOutput.concat("WFA IE: ");
                }
                else if (bOuiStr === customOui.toString()) {
                    strIeOutput = strIeOutput.concat("Custom IE: ");

                    var dataReader = Windows.Storage.Streams.DataReader.fromBuffer(ie.value);
                    dataReader.unicodeEncoding = Windows.Storage.Streams.UnicodeEncoding.utf8;
                    dataReader.byteOrder = Windows.Storage.Streams.ByteOrder.littleEndian;

                    strIe = dataReader.readString(dataReader.readUInt32());
                }
                else {
                    strIeOutput = strIeOutput.concat("Unknown IE: ");
                }

                strIeOutput = strIeOutput + "OUI Type: " + ie.ouiType.toString() + " OUI: " + bOuiStr + " IE Data: " + strIe + "\n";
            });

            WinJS.log && WinJS.log(strIeOutput, "sample", "status");
        });
    }

    function btnFromId_Click() {
        var discoveredDevicesList = document.getElementById("lvDiscoveredDevices").winControl;

        if (discoveredDevicesList.selection.count() === 0) {
            WinJS.log && WinJS.log("No device selected, please select one", "sample", "error");
            return;
        }


        discoveredDevicesList.selection.getItems().done(function (items) {
            var deviceInfo = items[0].data.deviceInfo;

            WinJS.log && WinJS.log("Connecting to " + deviceInfo.name + "...", "sample", "status");

            var connectionParams = new Windows.Devices.WiFiDirect.WiFiDirectConnectionParameters();
            connectionParams.groupOwnerIntent = parseInt(txtGOIntent.value.toString());

            Windows.Devices.WiFiDirect.WiFiDirectDevice.fromIdAsync(deviceInfo.id, connectionParams).then(function (wfdDevice) {
                if (wfdDevice === null) {
                    WinJS.log && WinJS.log("Connection to " + deviceInfo.name + " failed", "sample", "error");
                    return;
                }

                // Register for the ConnectionStatusChanged event handler
                wfdDevice.onconnectionstatuschanged = OnConnectionStatusChanged;

                // Get the EndpointPair information.
                var endpointPairCollection = wfdDevice.getConnectionEndpointPairs();

                WinJS.log && WinJS.log("Devices connected on L2 layer, connecting to IP Address: " + endpointPairCollection[0].remoteHostName +
                " Port: " + strServerPort, "sample", "status");

                // Wait for server to start listening on a socket
                var wakeupTime = new Date().getTime() + 2000;
                while (new Date().getTime() < wakeupTime);

                // Connect to Advertiser on L4 layer
                var clientSocket = new Windows.Networking.Sockets.StreamSocket();
                var endPointPair = endpointPairCollection[0];
                endPointPair.remoteServiceName = strServerPort;
                clientSocket.connectAsync(endPointPair).done(function () {
                    var sessionId = Math.random().toString();

                    var socketRW = new WfdHelper.SocketReaderWriter(clientSocket, sessionId);
                    connectedDevicesListBinding.push(new WfdHelper.ConnectedDevice(sessionId, wfdDevice, socketRW));

                    socketRW.ReadMessage();
                    socketRW.WriteMessage(sessionId);

                    WinJS.log && WinJS.log("Connected with remote side on L4 layer", "sample", "status");
                },
                function (err) {
                    WinJS.log && WinJS.log("Socket connection failed: " + err.toString(), "sample", "error");
                    return;
                });
            },
            function (err) {
                WinJS.log && WinJS.log("Connect operation failed: " + err.toString(), "sample", "error");
                return;
            });
        });
    }

    function OnConnectionStatusChanged(e) {
        WinJS.log && WinJS.log("Connection disconnected", "sample", "status");
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
            WinJS.log && WinJS.log("Send failed: " + +err.toString(), "sample", "error");
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
            WinJS.log && WinJS.log("Close failed: " + +err.toString(), "sample", "error");
        });
    }
})();
