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

    var sampleTitle = "Cash Drawer";

    var scenarios = [
        { url: "/html/Scenario1_OpenDrawer.html", title: "Drawer Claim and Open" },
        { url: "/html/Scenario2_CloseDrawer.html", title: "Wait for Drawer Close" },
        { url: "/html/Scenario3_MultipleDrawers.html", title: "Drawer Retain" }
    ];

    var CashDrawer = Windows.Devices.PointOfService.CashDrawer;

    function getFirstCashDrawerAsync(connectionTypes) {
        if (connectionTypes === undefined) {
            // By default, use all connections types.
            connectionTypes = Windows.Devices.PointOfService.PosConnectionTypes.all;
        }
        return DeviceHelpers.getFirstDeviceAsync(CashDrawer.getDeviceSelector(connectionTypes), (id) => CashDrawer.fromIdAsync(id));
    }

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),
        getFirstCashDrawerAsync: getFirstCashDrawerAsync
    });
})();
