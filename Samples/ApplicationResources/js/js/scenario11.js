//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario11.html", {
        ready: function (element, options) {
            document.getElementById("scenario11Show").addEventListener("click", show, false);
        },

        unload: function () {
            // Clearing qualifier value overrides set on the default context for the current
            // view so that an override set here doesn't impact other scenarios. See comments
            // below for additional information.
            var resourceNS = Windows.ApplicationModel.Resources.Core;
            resourceNS.ResourceContext.getForCurrentView().reset();
        }
    });

    function show() {

        // A langauge override will be set on the default context for the current view.
        // When navigating between different scenarios in this sample, the content for each
        // scenario is loaded into a host page that remains constant. The view (meaning, the 
        // CoreWindow) remains the same, and so it is the same default context that's in use. 
        // Thus, an override set here can impact behavior in other scenarios.
        //
        // However, the description for the scenario may give the impression that a value 
        // being set is local to this scenario. Also, when entering this scenario, the combo 
        // box always gets set to the first item, which can add to the confusion. To avoid 
        // confusion when using this sample, the override that gets set here will be cleared 
        // when the user navigates away from this scenario (in the unload event handler for
        // the page). In a real app, whether and when to clear an override will depend on
        // the desired behaviour and the design of the app.

        var resourceNS = Windows.ApplicationModel.Resources.Core;
        var resourceMap = resourceNS.ResourceManager.current.mainResourceMap.getSubtree('Resources');

        var context = resourceNS.ResourceContext.getForCurrentView();
        var languagesVector = new Array(document.getElementById('scenario11Select').value); // Context values can be a list
        context.languages = languagesVector;

        var str = resourceMap.getValue('scenario3Message', context).valueAsString;

        WinJS.log && WinJS.log(str, "sample", "status");
    }
})();
