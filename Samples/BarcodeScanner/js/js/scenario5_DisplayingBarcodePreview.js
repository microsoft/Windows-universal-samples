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

    // Types
    var BarcodeScanner = Windows.Devices.PointOfService.BarcodeScanner;
    var BarcodeSymbologies = Windows.Devices.PointOfService.BarcodeSymbologies;
    var BarcodeSymbologyDecodeLengthKind = Windows.Devices.PointOfService.BarcodeSymbologyDecodeLengthKind;
    var DeviceEnumeration = Windows.Devices.Enumeration;
    var Capture = Windows.Media.Capture;

    // Page elements
    var startSoftwareTriggerButton;
    var stopSoftwareTriggerButton;
    var scenarioOutputScanDataType;
    var scenarioOutputScanData;
    var scenarioOutputScanDataFormatted;
    var scannerResultsBlock;
    var previewControl;

    // Will be defined when the page is initialized, otherwise we won't be able to find element
    var scannerListViewControl = null;

    var watcher = null;
    var observableScanners = new WinJS.Binding.List([]);
    var selectedScanner = null;
    var claimedScanner = null;

    var mediaCapture = null;
    var displayRequest = new Windows.System.Display.DisplayRequest();

    var isSelectionChanging = false;
    var pendingSelectionDeviceId;
    var isStopPending = false;

    var page = WinJS.UI.Pages.define("/html/scenario5_DisplayingBarcodePreview.html", {

        ready: function (element, options) {
            scenarioOutputScanDataType = document.getElementById("scenarioOutputScanDataType");
            scenarioOutputScanData = document.getElementById("scenarioOutputScanData");
            scenarioOutputScanDataFormatted = document.getElementById("scenarioOutputScanDataFormatted");
            scannerResultsBlock = document.getElementById("scannerResultsBlock");
            previewControl = document.getElementById("cameraPreview");

            var showPreviewButton = document.getElementById("showPreviewButton");
            showPreviewButton.addEventListener("click", showPreviewButton_Clicked, false);

            var hidePreviewButton = document.getElementById("hidePreviewButton");
            hidePreviewButton.addEventListener("click", hidePreviewButton_Clicked, false);

            startSoftwareTriggerButton = document.getElementById("startSoftwareTriggerButton");
            startSoftwareTriggerButton.addEventListener("click", startSoftwareTriggerButton_Clicked, false);

            stopSoftwareTriggerButton = document.getElementById("stopSoftwareTriggerButton");
            stopSoftwareTriggerButton.addEventListener("click", stopSoftwareTriggerButton_Clicked, false);

            var scannerListView = document.getElementById("scannerListView");
            scannerListView.addEventListener("selectionchanged", scannerSelectionChanged, false);
            scannerListViewControl = scannerListView.winControl;
            scannerListViewControl.itemDataSource = observableScanners.dataSource;

            watcher = DeviceEnumeration.DeviceInformation.createWatcher(BarcodeScanner.getDeviceSelector(), null);
            watcher.addEventListener("added", watcher_Added);
            watcher.addEventListener("updated", watcher_Updated);
            watcher.addEventListener("removed", watcher_Removed);
            watcher.start();
        },

        unload: function () {
            watcher.stop();
            observableScanners.length = 0;

            if (isSelectionChanging) {
                // If selection is changing, then let it know to stop media capture
                // when it's done.
                isStopPending = true;
            }
            else {
                // If selection is not changing, then it's safe to stop immediately.
                closeScannerResourcesAsync();
            }
        }
    });

    function watcher_Added(deviceInfo) {
        observableScanners.push({
            name: `${deviceInfo.name} (${deviceInfo.id})`,
            id: deviceInfo.id
        });
    }

    function watcher_Updated(deviceInfoUpdate) {
        // We don't do anything here, but this event needs to be handled to enable realtime updates.
        // See https://aka.ms/devicewatcher_added.
    }

    function watcher_Removed(deviceInfoUpdate) {
        // We don't do anything here, but this event needs to be handled to enable realtime updates.
        // See https://aka.ms/devicewatcher_added.
    }

    async function startMediaCaptureAsync(videoDeviceId) {
        mediaCapture = new Capture.MediaCapture();
        mediaCapture.addEventListener("failed", mediaCapture_Failed);

        var settings = new Capture.MediaCaptureInitializationSettings();
        settings.videoDeviceId = videoDeviceId;
        settings.streamingCaptureMode = Capture.StreamingCaptureMode.video;
        settings.sharingMode = Capture.MediaCaptureSharingMode.sharedReadOnly;

        var captureInitialized = false;
        try {
            await mediaCapture.initializeAsync(settings);
            captureInitialized = true;
        } catch (e) {
            WinJS.log("Failed to initialize the camera preview with: " + e.message, "sample", "error");
        }

        if (captureInitialized) {
            // Prevent the device from sleeping while the preview is running.
            displayRequest.requestActive();

            previewControl.src = URL.createObjectURL(mediaCapture);
            previewControl.play();
            scannerResultsBlock.dataset.isPreviewing = true;
        } else {
            previewControl.src = null;
            mediaCapture.close();
            mediaCapture = null;
        }
    }

    function closeScannerResourcesAsync() {
        if (claimedScanner) {
            claimedScanner.close();
            claimedScanner = null;
        }

        if (selectedScanner) {
            selectedScanner.close();
            selectedScanner = null;
        }

        startSoftwareTriggerButton.disabled = false;
        stopSoftwareTriggerButton.disabled = true;

        if (previewControl.src) {
            previewControl.pause();
            previewControl.src = null;

            // Allow the display to go to sleep.
            displayRequest.requestRelease();
        }

        if (mediaCapture) {
            mediaCapture.removeEventListener("failed", mediaCapture_Failed);
            mediaCapture.close();
            mediaCapture = null;
        }

        scannerResultsBlock.dataset.isPreviewing = false;
    }

    function mediaCapture_Failed(e) {
        WinJS.log("Media capture failed. Make sure the camera is still connected.", "sample", "error");
    }

    function showPreviewButton_Clicked() {
        claimedScanner && claimedScanner.showVideoPreviewAsync();
    }

    function hidePreviewButton_Clicked() {
        claimedScanner && claimedScanner.hideVideoPreview();
    }

    async function startSoftwareTriggerButton_Clicked() {
        if (claimedScanner) {
            await claimedScanner.startSoftwareTriggerAsync();
            startSoftwareTriggerButton.disabled = true;
            stopSoftwareTriggerButton.disabled = false;
        }
    }

    async function stopSoftwareTriggerButton_Clicked() {
        if (claimedScanner) {
            await claimedScanner.stopSoftwareTriggerAsync();
            startSoftwareTriggerButton.disabled = false;
            stopSoftwareTriggerButton.disabled = true;
        }
    }

    async function scannerSelectionChanged() {

        var items = await scannerListViewControl.selection.getItems();
        if (!items.length) {
            return;
        }

        var deviceId = items[0].data.id;

        if (isSelectionChanging) {
            pendingSelectionDeviceId = deviceId;
            return;
        }

        do {
            await selectScannerAsync(deviceId);

            // Stop takes precedence over updating the selection.
            if (isStopPending) {
                await closeScannerResourcesAsync();
                break;
            }
            deviceId = pendingSelectionDeviceId;
            pendingSelectionDeviceId = null;
        } while (deviceId);
    }

    async function selectScannerAsync(scannerDeviceId) {
        isSelectionChanging = true;

        await closeScannerResourcesAsync();

        selectedScanner = await BarcodeScanner.fromIdAsync(scannerDeviceId);

        if (selectedScanner) {
            claimedScanner = await selectedScanner.claimScannerAsync();
            if (claimedScanner) {
                scannerResultsBlock.dataset.isScannerClaimed = true;
                await claimedScanner.enableAsync();
                claimedScanner.addEventListener("closed", claimedScannerClosed);
                startSoftwareTriggerButton.disabled = false;
                claimedScanner.addEventListener("datareceived", dataReceived);

                scannerResultsBlock.dataset.scannerSupportsPreview = (selectedScanner.videoDeviceId != "");
                if (selectedScanner.videoDeviceId) {
                    await startMediaCaptureAsync(selectedScanner.videoDeviceId);
                }
            } else {
                WinJS.log("Failed to claim the selected barcode scanner.", "sample", "error");
            }
        } else {
            WinJS.log("Failed to create a barcode scanner object", "sample", "error");
        }

        isSelectionChanging = false;
    }

    function claimedScannerClosed() {
        // Resources associated to the claimed barcode scanner can be cleaned up here
    }

    function dataReceived(args) {
        var tempScanType = BarcodeSymbologies.getName(args.report.scanDataType);

        scenarioOutputScanDataType.textContent = tempScanType;
        scenarioOutputScanData.textContent = getDataString(args.report.scanData);
        scenarioOutputScanDataFormatted.textContent = getDataLabelString(args.report.scanDataLabel, args.report.scanDataType);
    }
})();
