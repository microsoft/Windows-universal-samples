//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";


    function updateCarrierDetectStateView() {

        if (SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.carrierDetectState) {
            document.getElementById("CarrierDetectStateValue").textContent = "On";
        } else {
            document.getElementById("CarrierDetectStateValue").textContent = "Off";
        }
    }

    function updateDataSetReadyView() {

        if (SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.dataSetReadyState) {
            document.getElementById("DataSetReadyValue").textContent = "On";
        } else {
            document.getElementById("DataSetReadyValue").textContent = "Off";
        }
    }

    function updateBreakStateSignalView() {

        if (SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.breakSignalState) {
            document.getElementById("BreakStateSignalValue").textContent = "On";
            document.getElementById("BreakStateSignalToggleSwitch").winControl.checked = true;
        } else {
            document.getElementById("BreakStateSignalValue").textContent = "Off";
            document.getElementById("BreakStateSignalToggleSwitch").winControl.checked = false;
        }
    }

    function breakSignalStateToggled() {
        if (document.getElementById("BreakStateSignalToggleSwitch").winControl.checked) {
            document.getElementById("BreakStateSignalValue").textContent = "On";
            SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.breakSignalState = true;
        } else {
            SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.breakSignalState = false;
            document.getElementById("BreakStateSignalValue").textContent = "Off";
        }
        updateBreakStateSignalView();
    }

    function updateDataTerminalReadyEnabledView() {

        if (SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.isDataTerminalReadyEnabled) {
            document.getElementById("DataTerminalReadyEnabledValue").textContent = "On";
            document.getElementById("DataTerminalReadyEnabledToggleSwitch").winControl.checked = true;
        } else {
            document.getElementById("DataTerminalReadyEnabledValue").textContent = "Off";
            document.getElementById("DataTerminalReadyEnabledToggleSwitch").winControl.checked = false;
        }
    }

    function dataTerminalReadyEnabledToggled() {
        if (document.getElementById("DataTerminalReadyEnabledToggleSwitch").winControl.checked) {
            document.getElementById("DataTerminalReadyEnabledValue").textContent = "On";
            SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.isDataTerminalReadyEnabled = true;
        } else {
            SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.isDataTerminalReadyEnabled = false;
            document.getElementById("DataTerminalReadyEnabledValue").textContent = "Off";
        }
        updateBreakStateSignalView();
    }

    function updateReadyToSendEnabledView() {

        if (SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.isReadyToSendEnabled) {
            document.getElementById("ReadyToSendEnabledValue").textContent = "On";
            document.getElementById("ReadyToSendEnabledToggleSwitch").winControl.checked = true;
        } else {
            document.getElementById("ReadyToSendEnabledValue").textContent = "Off";
            document.getElementById("ReadyToSendEnabledToggleSwitch").winControl.checked = false;
        }
    }

    function readyToSendEnabledToggled() {
        if (document.getElementById("ReadyToSendEnabledToggleSwitch").winControl.checked) {
            document.getElementById("ReadyToSendEnabledValue").textContent = "On";
            SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.isReadyToSendEnabled = true;
        } else {
            SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.isReadyToSendEnabled = false;
            document.getElementById("ReadyToSendEnabledValue").textContent = "Off";
        }
        updateBreakStateSignalView();
    }

    function updateBaudRateView() {
        document.getElementById("BaudRateLabel").textContent = SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.baudRate;
    }

    function baudRateButtonClicked() {
        var input = document.getElementById("BaudRateInput");
        var baudRate = parseInt(input.value, 10);
        SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.baudRate = baudRate;
        updateBaudRateView();
    }

    function updateParityView() {
        var selectionItem = document.getElementById("ParityInput");
        var label = document.getElementById("ParityLabel");
        var currentParity = SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.parity;

        if (currentParity == Windows.Devices.SerialCommunication.SerialParity.none) {
            selectionItem.selectedIndex = 0;
            label.textContent = "None";
        } else if (currentParity == Windows.Devices.SerialCommunication.SerialParity.odd) {
            selectionItem.selectedIndex = 1;
            label.textContent = "Odd";
        } else if (currentParity == Windows.Devices.SerialCommunication.SerialParity.even) {
            selectionItem.selectedIndex = 2;
            label.textContent = "Even";
        } else if (currentParity == Windows.Devices.SerialCommunication.SerialParity.mark) {
            selectionItem.selectedIndex = 3;
            label.textContent = "Mark";
        } else if (currentParity == Windows.Devices.SerialCommunication.SerialParity.space) {
            selectionItem.selectedIndex = 4;
            label.textContent = "Space";
        }
    }

    function parityInputChanged() {
        var selectionItem = document.getElementById("ParityInput");
        if (selectionItem.selectedIndex == 0) {
            SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.parity = Windows.Devices.SerialCommunication.SerialParity.none;
        } else if (selectionItem.selectedIndex == 1) {
            SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.parity = Windows.Devices.SerialCommunication.SerialParity.odd;
        } else if (selectionItem.selectedIndex == 2) {
            SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.parity = Windows.Devices.SerialCommunication.SerialParity.even;
        } else if (selectionItem.selectedIndex == 3) {
            SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.parity = Windows.Devices.SerialCommunication.SerialParity.mark;
        } else if (selectionItem.selectedIndex == 4) {
            SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.parity = Windows.Devices.SerialCommunication.SerialParity.space;
        }
        updateParityView();
    }


    function updateStopBitCountView() {
        var selectionItem = document.getElementById("StopBitCountInput");
        var label = document.getElementById("StopBitCountLabel");
        var currentStopBitCount = SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.stopBits;

        if (currentStopBitCount == Windows.Devices.SerialCommunication.SerialStopBitCount.one) {
            selectionItem.selectedIndex = 0;
            label.textContent = "One";
        } else if (currentStopBitCount == Windows.Devices.SerialCommunication.SerialStopBitCount.onePointFive) {
            selectionItem.selectedIndex = 1;
            label.textContent = "OnePointFive";
        } else if (currentStopBitCount == Windows.Devices.SerialCommunication.SerialStopBitCount.two) {
            selectionItem.selectedIndex = 2;
            label.textContent = "Two";
        }
    }

    function stopBitCountInputChanged() {
        var selectionItem = document.getElementById("StopBitCountInput");
        if (selectionItem.selectedIndex == 0) {
            SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.stopBits = Windows.Devices.SerialCommunication.SerialStopBitCount.one;
        } else if (selectionItem.selectedIndex == 1) {
            SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.stopBits = Windows.Devices.SerialCommunication.SerialStopBitCount.onePointFive;
        } else if (selectionItem.selectedIndex == 2) {
            SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.stopBits = Windows.Devices.SerialCommunication.SerialStopBitCount.two;
        }
        updateStopBitCountView();
    }


    function updateHandShakeView() {
        var selectionItem = document.getElementById("HandShakeInput");
        var label = document.getElementById("HandShakeLabel");
        var currentHandShake = SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.handshake;

        if (currentHandShake == Windows.Devices.SerialCommunication.SerialHandshake.none) {
            selectionItem.selectedIndex = 0;
            label.textContent = "None";
        } else if (currentHandShake == Windows.Devices.SerialCommunication.SerialHandshake.requestToSend) {
            selectionItem.selectedIndex = 1;
            label.textContent = "RequestToSend";
        } else if (currentHandShake == Windows.Devices.SerialCommunication.SerialHandshake.xonXOff) {
            selectionItem.selectedIndex = 2;
            label.textContent = "XOnXOff";
        } else if (currentHandShake == Windows.Devices.SerialCommunication.SerialHandshake.requestToSendXOnXOff) {
            selectionItem.selectedIndex = 3;
            label.textContent = "RequestToSendXOnXOff";
        }
    }

    function handShakeInputChanged() {
        var selectionItem = document.getElementById("HandShakeInput");
        if (selectionItem.selectedIndex == 0) {
            SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.handshake = Windows.Devices.SerialCommunication.SerialHandshake.none;
        } else if (selectionItem.selectedIndex == 1) {
            SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.handshake = Windows.Devices.SerialCommunication.SerialHandshake.requestToSend;
        } else if (selectionItem.selectedIndex == 2) {
            SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.handshake = Windows.Devices.SerialCommunication.SerialHandshake.xonXOff;
        } else if (selectionItem.selectedIndex == 3) {
            SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.handshake = Windows.Devices.SerialCommunication.SerialHandshake.requestToSendXOnXOff;
        }
        updateHandShakeView();
    }


    function updateDataBitsView() {
        var selectionItem = document.getElementById("DataBitsInput");
        var label = document.getElementById("DataBitsLabel");
        var currentDataBits = SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.dataBits;

        switch (currentDataBits) {
            case 0:
                selectionItem.selectedIndex = 0;
                label.textContent = "0";
                break;

            case 1:
                selectionItem.selectedIndex = 1;
                label.textContent = "1";
                break;

            case 2:
                selectionItem.selectedIndex = 2;
                label.textContent = "2";
                break;

            case 3:
                selectionItem.selectedIndex = 3;
                label.textContent = "3";
                break;

            case 4:
                selectionItem.selectedIndex = 4;
                label.textContent = "4";
                break;

            case 5:
                selectionItem.selectedIndex = 5;
                label.textContent = "5";
                break;

            case 6:
                selectionItem.selectedIndex = 6;
                label.textContent = "6";
                break;

            case 7:
                selectionItem.selectedIndex = 7;
                label.textContent = "7";
                break;

            case 8:
                selectionItem.selectedIndex = 8;
                label.textContent = "8";
                break;

            default:
                break;
        }

    }

    function dataBitsInputChanged() {
        var selectionItem = document.getElementById("DataBitsInput");
        SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.dataBits = selectionItem.selectedIndex;
        updateDataBitsView();
    }

    var page = WinJS.UI.Pages.define("/html/Scenario2_configureDevice.html", {
        ready: function (element, options) {

            if (SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.isDeviceConnected == false) {
                document.getElementById("scenarioContent").hidden = true;
                WinJS.log && WinJS.log("Device is NOT connected", "sample", "status");
                return;
            }

            document.getElementById("scenarioContent").hidden = false;

            updateCarrierDetectStateView();
            updateDataSetReadyView();

            document.getElementById("BreakStateSignalToggleSwitch").addEventListener("change", breakSignalStateToggled, false);
            document.getElementById("DataTerminalReadyEnabledToggleSwitch").addEventListener("change", dataTerminalReadyEnabledToggled, false);
            document.getElementById("ReadyToSendEnabledToggleSwitch").addEventListener("change", readyToSendEnabledToggled, false);
            document.getElementById("BaudRateButton").addEventListener("click", baudRateButtonClicked, false);
            document.getElementById("ParityInput").addEventListener("change", parityInputChanged, false);
            document.getElementById("StopBitCountInput").addEventListener("change", stopBitCountInputChanged, false);
            document.getElementById("HandShakeInput").addEventListener("change", handShakeInputChanged, false);
            document.getElementById("DataBitsInput").addEventListener("change", dataBitsInputChanged, false);

            updateBreakStateSignalView();
            updateDataTerminalReadyEnabledView();
            updateReadyToSendEnabledView();
            updateBaudRateView();
            updateParityView();
            updateStopBitCountView();
            updateHandShakeView();
            dataBitsInputChanged();
        }
    })

    function doSomething() {
        WinJS.log && WinJS.log("Error message here", "sample", "error");
    }
})();
