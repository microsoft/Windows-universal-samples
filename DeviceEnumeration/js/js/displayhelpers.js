//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    
    var bt = Windows.Devices.Bluetooth;
    var rfcommChatServiceUuid = "34B1CF4D-1069-4AD6-89B6-E161D79BE4D8";
    
    var selectorsArray = [

        // Pre-canned device class selectors
        { displayName: "All Device Interfaces", deviceClassSelector: Windows.Devices.Enumeration.DeviceClass.all },
        { displayName: "Audio Capture Devices", deviceClassSelector: Windows.Devices.Enumeration.DeviceClass.audioCapture },
        { displayName: "Audio Render Devices", deviceClassSelector: Windows.Devices.Enumeration.DeviceClass.audioRender },
        { displayName: "Image Scanner Devices", deviceClassSelector: Windows.Devices.Enumeration.DeviceClass.imageScanner },
        { displayName: "Location Devices", deviceClassSelector: Windows.Devices.Enumeration.DeviceClass.location },
        { displayName: "Portable Storage Devices", deviceClassSelector: Windows.Devices.Enumeration.DeviceClass.portableStorageDevice },
        { displayName: "Video Capture Devices", deviceClassSelector: Windows.Devices.Enumeration.DeviceClass.videoCapture },

        // A few examples of selectors built dynamically by windows runtime apis. 
        { displayName: "Proximity Devices", selector: Windows.Networking.Proximity.ProximityDevice.getDeviceSelector() },
        { displayName: "Human Interface Devices", selector: Windows.Devices.HumanInterfaceDevice.HidDevice.getDeviceSelector(0,0) },
        { displayName: "Bluetooth Rfcomm Chat Services", selector: bt.Rfcomm.RfcommDeviceService.getDeviceSelector(bt.Rfcomm.RfcommServiceId.fromUuid(rfcommChatServiceUuid)) },
        { displayName: "Bluetooth Gatt Heart Rate Devices", selector: bt.GenericAttributeProfile.GattDeviceService.getDeviceSelectorFromUuid(bt.GenericAttributeProfile.GattServiceUuids.heartRate) },
        { displayName: "WiFi Direct Devices", selector: Windows.Devices.WiFiDirect.WiFiDirectDevice.getDeviceSelector() }
    ];

    var selectorsList = new WinJS.Binding.List(selectorsArray);

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

    var protocolArray = [

        { displayName: "Wi-Fi Direct", selector: "System.Devices.Aep.ProtocolId:=\"{0407d24e-53de-4c9a-9ba1-9ced54641188}\"" },
        { displayName: "WSD", selector: "System.Devices.Aep.ProtocolId:=\"{782232aa-a2f9-4993-971b-aedc551346b0}\"" },
        { displayName: "UPnP", selector: "System.Devices.Aep.ProtocolId:=\"{0e261de4-12f0-46e6-91ba-428607ccef64}\"" },
        { displayName: "Point of Service", selector: "System.Devices.Aep.ProtocolId:=\"{D4BF61B3-442E-4ADA-882D-FA7B70C832D9}\"" }
    ];

    var protocolList = new WinJS.Binding.List(protocolArray);

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
            selectorsList: selectorsList,
            kindList: kindList,
            protocolList: protocolList,
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
