//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var parameters = [ "httpStatus", "failureName", "failureUrl" ];

    function id(elementId) {
        return document.getElementById(elementId); 
    }
    
    function initialize() {
        for (var i = 0; i < 3; i++) {
            var parameter = parameters[i];
            var parameterValue = getQueryParameter(parameter);
            if (parameterValue) {
                id(parameter + "Value").innerHTML = unescape(parameterValue);
            } else {
                id(parameter + "Value").innerHTML = "N/A";
            }
        }
    }

    function getQueryParameter(variable) {
        var query = window.location.search.substring(1);
        var vars = query.split("&");
        for (var i = 0; i < vars.length; i++) {
            var pair = vars[i].split("=");
            if (pair[0] === variable) {
                return pair[1];
            }
        }
        return "";
    }

    document.addEventListener("DOMContentLoaded", initialize, false);
})();
