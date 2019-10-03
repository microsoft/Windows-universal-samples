//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var output;
    var namedResourceTemplate;
    var candidateTemplate;
    var qualifierTemplate;

    var page = WinJS.UI.Pages.define("/html/scenario13.html", {
        ready: function (element, options) {
            output = document.getElementById('output');
            
            namedResourceTemplate = document.getElementById('named-resource-template').winControl;
            candidateTemplate = document.getElementById('candidate-template').winControl;
            qualifierTemplate = document.getElementById('qualifier-template').winControl;

            document.getElementById("scenario13Show").addEventListener("click", show, false);
        }
    });

    function show() {

        WinJS.Utilities.empty(output);

        var context = createContext();

        // Resources actually reside within Scenario13 Resource Map
        var resourceIds = [
            '/Scenario13/languageOnly',
            '/Scenario13/scaleOnly',
            '/Scenario13/contrastOnly',
            '/Scenario13/homeregionOnly',
            '/Scenario13/multiDimensional',
        ];

        resourceIds.forEach(function (resourceId) {
            renderNamedResource(resourceId, context);
            
        });
    }

    function createContext() {
        // use a cloned context for this scenario so that qualifier values set
        // in this scenario don't impact behaviour in other scenarios that
        // use a default context for the view (crossover effects between
        // the scenarios will not be expected)
        var context = Windows.ApplicationModel.Resources.Core.ResourceContext.getForCurrentView().clone();

        // Set the specific context for lookup of resources
        var qualifierValues = context.qualifierValues;
        qualifierValues["language"] = document.getElementById('scenario13SelectLanguage').value;
        qualifierValues["contrast"] = document.getElementById('scenario13SelectContrast').value;
        qualifierValues["scale"] = document.getElementById('scenario13SelectScale').value;
        qualifierValues["homeregion"] = document.getElementById('scenario13SelectHomeRegion').value;

        return context;
    }

    function renderNamedResource(resourceId, context) {
        var observable = { resourceId: resourceId };
        namedResourceTemplate.render(observable).done(function (namedResourceEl) {
            var candidatesEl = namedResourceEl.querySelector('.resource-candidates');

            // Lookup resource in the mainResourceMap (the one for this package)
            var resource = Windows.ApplicationModel.Resources.Core.ResourceManager.current.mainResourceMap.lookup(resourceId);

            // Returns all possible resources candidates resolved against the context
            // in order of appropriateness
            var candidates = resource.resolveAll(context);

            candidates.forEach(function (candidate, index) {
                               
                renderCandidate(candidate, index).done(function (candidateEl) {
                    candidatesEl.appendChild(candidateEl);
                });
            });

            output.appendChild(namedResourceEl);
        });
    }

    function renderCandidate(candidate, index) {
        var observable = {
            index: index + 1,
            isMatch: candidate.isMatch,
            isDefault: candidate.isDefault,
            value: candidate.valueAsString
        };
        return candidateTemplate.render(observable).then(function (candidateEl) {
            var qualifiersEl = candidateEl.querySelector('.candidate-qualifiers');

            // Get all the various qualifiers for the candidate (i.e. language, scale, contrast)
            candidate.qualifiers.forEach(function (qualifier) {
                var obj = {
                    qualifierName: qualifier.qualifierName,
                    qualifierValue: qualifier.qualifierValue
                };
                qualifierTemplate.render(obj).done(function (qualifierEl) {
                    qualifiersEl.appendChild(qualifierEl);
                });
            });

            return candidateEl;

        });
    }

})();
