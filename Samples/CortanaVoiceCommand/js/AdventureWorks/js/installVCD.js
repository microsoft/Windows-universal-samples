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

    var wap = Windows.ApplicationModel.Package;
    var voiceCommandManager = Windows.ApplicationModel.VoiceCommands.VoiceCommandDefinitionManager;

    self.onmessage = function(e) {
        // Pass in the PhraseList and current UI language as an object via e.data 

        wap.current.installedLocation.getFileAsync("AdventureworksCommands.xml").then(function (file) {
            return voiceCommandManager.installCommandDefinitionsFromStorageFileAsync(file);
        }).then(function () {
            var commandSetName = "AdventureWorksCommandSet_" + e.data.language.toLowerCase();
            if (voiceCommandManager.installedCommandDefinitions.hasKey(commandSetName)) {
                var vcd = voiceCommandManager.installedCommandDefinitions.lookup(commandSetName);
                vcd.setPhraseListAsync("destination", e.data.phraseList).done();
            } else {
                WinJS.log && WinJS.log("VCD not installed yet?", "", "warning");
            }
        }).done(function () {
            // We're all done, close the webworker.
            close();
        });
    }

})();