//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var SecondaryTile = Windows.UI.StartScreen.SecondaryTile;
    var TileSize = Windows.UI.StartScreen.TileSize;
    var Uri = Windows.Foundation.Uri;
    var MessageDialog = Windows.UI.Popups.MessageDialog;
    var _tileId;

    // UI elements on the page
    var pinCommand;
    var updateCommand;
    var restartCommand;

    var page = WinJS.UI.Pages.define("/html/scenario2-updatesecondarytile.html", {
        ready: function (element, options) {

            pinCommand = element.querySelector("#pincommand");
            pinCommand.addEventListener("click", pinTile);
            pinCommand.disabled = false;

            updateCommand = element.querySelector("#updatecommand");
            updateCommand.addEventListener("click", updateTile);
            updateCommand.disabled = true;

            restartCommand = element.querySelector("#restartcommand");
            restartCommand.addEventListener("click", restartScenario);
        }
    });

    function pinTile() {

        pinCommand.disabled = true;

        // Generate a unique tile ID for the purposes of the sample
        _tileId = new Date().getTime().toString();

        // Initialize and pin a new secondary tile that we will later update
        var tile = new SecondaryTile(_tileId, "Original", "args", new Uri("ms-appx:///images/cancel.png"), TileSize.default);
        tile.visualElements.square71x71Logo = new Uri("ms-appx:///images/cancel.png");
        tile.visualElements.wide310x150Logo = new Uri("ms-appx:///images/cancel.png");
        tile.visualElements.square310x310Logo = new Uri("ms-appx:///images/cancel.png");
        tile.visualElements.showNameOnSquare150x150Logo = true;
        tile.visualElements.showNameOnSquare310x310Logo = true;
        tile.visualElements.showNameOnWide310x150Logo = true;

        tile.requestCreateAsync().then(function complete() {
            // unlock the rest of the scenario
            updateCommand.disabled = false;
        });
    }

    function updateTile() {
        updateCommand.disabled = true;

        // Grab the existing tile
        SecondaryTile.findAllAsync().then(function complete(tiles) {

            var tile = tiles.find(function (tile) {
                return tile.tileId === _tileId;
            });

            if (!tile) {
                new MessageDialog("The secondary tile that was previously pinned could not be found. Has it been removed from Start?", "Error").showAsync();
            } else {

                // Change its name and logo
                tile.displayName = "Updated";
                tile.visualElements.square150x150Logo = new Uri("ms-appx:///images/check.png");
                tile.visualElements.square71x71Logo = new Uri("ms-appx:///images/check.png");
                tile.visualElements.square310x310Logo = new Uri("ms-appx:///images/check.png");
                tile.visualElements.wide310x150Logo = new Uri("ms-appx:///images/check.png");

                // And request its properties to be updated
                tile.updateAsync();
            }

        });
    }

    function restartScenario() {
        pinCommand.disabled = false;
        updateCommand.disabled = true;
    }

})();