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

    var sampleTitle = "Magnetic Stripe Reader";

    var scenarios = [
        { url: "/html/Scenario1_BankCards.html", title: "BankCardDataReceived Event" },
        { url: "/html/Scenario2_AamvaCards.html", title: "AamvaCardDataReceived Event" }
    ];

    var MagneticStripeReader = Windows.Devices.PointOfService.MagneticStripeReader;

    function getFirstMagneticStripeReaderAsync(connectionTypes) {
        if (connectionTypes === undefined) {
            // By default, use all connections types.
            connectionTypes = Windows.Devices.PointOfService.PosConnectionTypes.all;
        }
        return DeviceHelpers.getFirstDeviceAsync(MagneticStripeReader.getDeviceSelector(connectionTypes), (id) => MagneticStripeReader.fromIdAsync(id));
    }

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),
        getFirstMagneticStripeReaderAsync: getFirstMagneticStripeReaderAsync
    });
})();
