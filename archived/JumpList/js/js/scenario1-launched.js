//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var ActivationKind = Windows.ApplicationModel.Activation.ActivationKind;

    var page = WinJS.UI.Pages.define("/html/scenario1-launched.html", {
        ready: function (element, options) {
            if (options &&
                options.activationKind == ActivationKind.launch &&
                options.activatedEventArgs.length > 0 &&
                options.activatedEventArgs[0].arguments != "") {
                ActivatedBy.innerText = "arguments: " + options.activatedEventArgs[0].arguments;
            }
            else if (options &&
                options.activationKind == ActivationKind.file &&
                options.activatedEventArgs.length > 0 &&
                options.activatedEventArgs[0].files.length > 0) {
                ActivatedBy.innerText = "file: " + options.activatedEventArgs[0].files[0].name;
            }
            else {
                ActivatedBy.innerText = "manual navigation.";
            }
            
        }
    });

    function showData()
    {
        
    }
})();
