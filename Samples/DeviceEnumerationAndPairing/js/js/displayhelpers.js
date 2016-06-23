//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    
    var bt = Windows.Devices.Bluetooth;
    var rfcommChatServiceUuid = "34B1CF4D-1069-4AD6-89B6-E161D79BE4D8";
    
    var commonArray = new Array(

        // Pre-canned device class selectors
        { displayName: "All Device Interfaces (default)", deviceClassSelector: Windows.Devices.Enumeration.DeviceClass.all },
        { displayName: "Audio Capture", deviceClassSelector: Windows.Devices.Enumeration.DeviceClass.audioCapture },
        { displayName: "Audio Render", deviceClassSelector: Windows.Devices.Enumeration.DeviceClass.audioRender },
        { displayName: "Image Scanner", deviceClassSelector: Windows.Devices.Enumeration.DeviceClass.imageScanner },
        { displayName: "Location", deviceClassSelector: Windows.Devices.Enumeration.DeviceClass.location },
        { displayName: "Portable Storage", deviceClassSelector: Windows.Devices.Enumeration.DeviceClass.portableStorageDevice },
        { displayName: "Video Capture", deviceClassSelector: Windows.Devices.Enumeration.DeviceClass.videoCapture },

        // A few examples of selectors built dynamically by windows runtime apis. 
        { displayName: "Human Interface (HID)", selector: Windows.Devices.HumanInterfaceDevice.HidDevice.getDeviceSelector(0, 0) },
        { displayName: "Activity Sensor", selector: Windows.Devices.Sensors.ActivitySensor.getDeviceSelector() },
        { displayName: "Pedometer", selector: Windows.Devices.Sensors.Pedometer.getDeviceSelector() },
        { displayName: "Proximity", selector: Windows.Networking.Proximity.ProximityDevice.getDeviceSelector() },
        { displayName: "Proximity Sensor", selector: Windows.Devices.Sensors.ProximitySensor.getDeviceSelector() }
    );

    // Currently Bluetooth APIs don't provide a selector to get ALL devices that are both paired and non-paired.  Typically you wouldn't need this for common scenarios, but it's convenient to demonstrate the
    // various sample scenarios. 
    var bluetooth = { displayName: "Bluetooth", selector: "System.Devices.Aep.ProtocolId:=\"{e0cbf06c-cd8b-4647-bb8a-263b43f0f974}\"", kind: Windows.Devices.Enumeration.DeviceInformationKind.associationEndpoint };
    var bluetoothUnpairedOnly = { displayName: "Bluetooth (unpaired)", selector: Windows.Devices.Bluetooth.BluetoothDevice.getDeviceSelectorFromPairingState(false) };
    var bluetoothPairedOnly = { displayName: "Bluetooth (paired)", selector: Windows.Devices.Bluetooth.BluetoothDevice.getDeviceSelectorFromPairingState(true) };
    var bluetoothLE = { displayName: "Bluetooth LE", selector: "System.Devices.Aep.ProtocolId:=\"{bb7bb05e-5972-42b5-94fc-76eaa7084d49}\"", kind: Windows.Devices.Enumeration.DeviceInformationKind.associationEndpoint };
    var bluetoothLEUnpairedOnly = { displayName: "Bluetooth LE (unpaired)", selector: Windows.Devices.Bluetooth.BluetoothLEDevice.getDeviceSelectorFromPairingState(false) };
    var bluetoothLEPairedOnly = { displayName: "Bluetooth LE (paired)", selector: Windows.Devices.Bluetooth.BluetoothLEDevice.getDeviceSelectorFromPairingState(true) };
    var wiFiDirect = { displayName: "Wi-Fi Direct", selector: Windows.Devices.WiFiDirect.WiFiDirectDevice.getDeviceSelector(Windows.Devices.WiFiDirect.WiFiDirectDeviceSelectorType.associationEndpoint) };
    var wiFiDirectPairedOnly = { displayName: "Wi-Fi Direct (paired)", selector: Windows.Devices.WiFiDirect.WiFiDirectDevice.getDeviceSelector() };
    var pointOfServicePrinter = { displayName: "Point of Service Printer", selector: Windows.Devices.PointOfService.PosPrinter.getDeviceSelector() };
    var videoCasting = { displayName: "Video Casting", selector: Windows.Media.Casting.CastingDevice.getDeviceSelector(Windows.Media.Casting.CastingPlaybackTypes.video) };
    var dialAllApps = { displayName: "DIAL (All Apps)", selector: Windows.Media.DialProtocol.DialDevice.getDeviceSelector("") };

    // WSD and UPnP are unique in that there are currently no general WSD or UPnP APIs to USE the devices once you've discovered them. 
    // You can pair the devices using DeviceInformation.Pairing.PairAsync etc, and you can USE them with the sockets APIs. However, since
    // there's no specific API right now, there's no *.GetDeviceSelector available.  That's why we just simply build the device selector
    // ourselves and specify the correct DeviceInformationKind (AssociationEndpoint). 
    var wsd = { displayName: "Web Services on Devices (WSD)", selector: "System.Devices.Aep.ProtocolId:=\"{782232aa-a2f9-4993-971b-aedc551346b0}\"", kind: Windows.Devices.Enumeration.DeviceInformationKind.associationEndpoint };
    var upnp = { displayName: "UPnP", selector: "System.Devices.Aep.ProtocolId:=\"{0e261de4-12f0-46e6-91ba-428607ccef64}\"", kind: Windows.Devices.Enumeration.DeviceInformationKind.associationEndpoint };
    
    var devicePickerArray = commonArray.slice();
    devicePickerArray.push(bluetoothPairedOnly);
    devicePickerArray.push(bluetoothUnpairedOnly);
    devicePickerArray.push(bluetoothLEPairedOnly);
    devicePickerArray.push(bluetoothLEUnpairedOnly);
    devicePickerArray.push(wiFiDirect);
    devicePickerArray.push(pointOfServicePrinter);
    devicePickerArray.push(videoCasting);
    devicePickerArray.push(dialAllApps);
    var devicePickerSelectors = new WinJS.Binding.List(devicePickerArray);

    var findAllAsyncArray = commonArray.slice();    
    findAllAsyncArray.push(bluetoothPairedOnly);
    findAllAsyncArray.push(bluetoothLEPairedOnly);
    findAllAsyncArray.push(wiFiDirectPairedOnly);
    var findAllAsyncSelectors = new WinJS.Binding.List(findAllAsyncArray);

    var deviceWatcherArray = commonArray.slice();
    deviceWatcherArray.push(bluetooth);
    deviceWatcherArray.push(bluetoothLE);
    deviceWatcherArray.push(wiFiDirect);
    deviceWatcherArray.push(pointOfServicePrinter);
    deviceWatcherArray.push(videoCasting);
    deviceWatcherArray.push(dialAllApps);
    deviceWatcherArray.push(wsd);
    deviceWatcherArray.push(upnp);
    var deviceWatcherSelectors = new WinJS.Binding.List(deviceWatcherArray);

    var backgroundDeviceWatcherArray = commonArray.slice();
    backgroundDeviceWatcherArray.push(bluetoothPairedOnly);
    backgroundDeviceWatcherArray.push(bluetoothLEPairedOnly);
    backgroundDeviceWatcherArray.push(wiFiDirectPairedOnly);
    backgroundDeviceWatcherArray.push(pointOfServicePrinter);
    backgroundDeviceWatcherArray.push(videoCasting);
    backgroundDeviceWatcherArray.push(dialAllApps);
    backgroundDeviceWatcherArray.push(wsd);
    backgroundDeviceWatcherArray.push(upnp);
    var backgroundDeviceWatcherSelectors = new WinJS.Binding.List(backgroundDeviceWatcherArray);

    var pairingArray = new Array();
    pairingArray.push(bluetooth);
    pairingArray.push(bluetoothLE);
    pairingArray.push(wiFiDirect);
    pairingArray.push(pointOfServicePrinter);
    pairingArray.push(videoCasting);
    pairingArray.push(wsd);
    pairingArray.push(upnp);
    var pairingSelectors = new WinJS.Binding.List(pairingArray);
    
    var kindArray = [
        { displayName: "DeviceContainer", deviceInformationKind: Windows.Devices.Enumeration.DeviceInformationKind.deviceContainer },
        { displayName: "Device", deviceInformationKind: Windows.Devices.Enumeration.DeviceInformationKind.device },
        { displayName: "DeviceInterface (default)", deviceInformationKind: Windows.Devices.Enumeration.DeviceInformationKind.deviceInterface },
        { displayName: "DeviceInterfaceClass", deviceInformationKind: Windows.Devices.Enumeration.DeviceInformationKind.deviceInterfaceClass },
        { displayName: "AssociationEndpointContainer", deviceInformationKind: Windows.Devices.Enumeration.DeviceInformationKind.associationEndpointContainer },
        { displayName: "AssociationEndpoint", deviceInformationKind: Windows.Devices.Enumeration.DeviceInformationKind.associationEndpoint },
        { displayName: "AssociationEndpointService", deviceInformationKind: Windows.Devices.Enumeration.DeviceInformationKind.associationEndpointService },
    ];
    var kindList = new WinJS.Binding.List(kindArray);

    // Define a bindable "display" version of the DeviceInformation class. WinJS.Binding.as won't work directly
    // on the DeviceInformation object, so we use this new class and make it a mix of observable and non-observable
    // properties. Notice  the main properties are made observable, but deviceInfo is not.
    var DeviceInformationDisplay = WinJS.Class.define(
        function (deviceInfo) {
            this._initObservable();

            this.id = deviceInfo.id;
            this.kind = kindStringFromType(deviceInfo.kind);
            this.name = deviceInfo.name;
            if (null != deviceInfo.pairing) {
                this.canPair = deviceInfo.pairing.canPair;
                this.isPaired = deviceInfo.pairing.isPaired;
            }
            this.deviceInfo = deviceInfo;

            // These two properties are only valid for scenario 6, but won't hurt other scenarios
            this.interfaceClass = deviceInfo.properties["System.Devices.InterfaceClassGuid"];
            this.itemNameDisplay = deviceInfo.properties["System.ItemNameDisplay"];
        }, {
            update: function (deviceInfoUpdate) {
                this.deviceInfo.update(deviceInfoUpdate);
                this.name = this.deviceInfo.name;

                if (null != this.deviceInfo.pairing) {
                    this.canPair = this.deviceInfo.pairing.canPair;
                    this.isPaired = this.deviceInfo.pairing.isPaired;
                }

                // These two properties are only valid for scenario 6, but won't hurt other scenarios
                this.interfaceClass = this.deviceInfo.properties["System.Devices.InterfaceClassGuid"];
                this.itemNameDisplay = this.deviceInfo.properties["System.ItemNameDisplay"];
            }
        }
        );

    var DeviceInformationDisplayMix = WinJS.Class.mix(
        DeviceInformationDisplay,
        WinJS.Binding.mixin,
        WinJS.Binding.expandProperties({ id: "", kind: "", name: "", canPair: "", isPaired: "", interfaceClass: "", itemNameDisplay: "" }));

    var publicMembers =
        {
            devicePickerSelectors: devicePickerSelectors,
            deviceWatcherSelectors: deviceWatcherSelectors,
            backgroundDeviceWatcherSelectors: backgroundDeviceWatcherSelectors,
            findAllAsyncSelectors: findAllAsyncSelectors,
            pairingSelectors: pairingSelectors,
            kindList: kindList,
            DeviceInformationDisplay: DeviceInformationDisplayMix
        };

    WinJS.Namespace.define("DisplayHelpers", publicMembers);
})();


function kindStringFromType(kindType) {
    var DevEnum = Windows.Devices.Enumeration;

    switch (kindType) {
        case DevEnum.DeviceInformationKind.associationEndpoint:
            return "AssociationEndpoint";
        case DevEnum.DeviceInformationKind.associationEndpointContainer:
            return "AssociationEndpointContainer";
        case DevEnum.DeviceInformationKind.associationEndpointService:
            return "AssociationEndpointService";
        case DevEnum.DeviceInformationKind.device:
            return "Device";
        case DevEnum.DeviceInformationKind.deviceContainer:
            return "DeviceContainer";
        case DevEnum.DeviceInformationKind.deviceInterface:
            return "DeviceInterface";
        case DevEnum.DeviceInformationKind.deviceInterfaceClass:
            return "DeviceInterfaceClass";
    }
};
