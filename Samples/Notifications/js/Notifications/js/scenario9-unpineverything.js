//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var SecondaryTile = Windows.UI.StartScreen.SecondaryTile;

    // UI elements on the page
    var unpinCommand;

    // UI controls on the page
    var secondaryTilesListView;

    var page = WinJS.UI.Pages.define("/html/scenario9-unpineverything.html", {
        ready: function (element, options) {

            unpinCommand = element.querySelector("#unpincommand");
            unpinCommand.addEventListener("click", unpinTiles);

            secondaryTilesListView = element.querySelector("#scenariocontrol").winControl;

            unpinCommand.disabled = true;
            setListViewDataAsync(secondaryTilesListView).done(function () {
                unpinCommand.disabled = false;
            });
        }
    });

    function uriToString(uri) {
        return uri ? uri.displayUri : "null";
    }

    function setListViewDataAsync(listView) {
        return SecondaryTile.findAllAsync().then(function (tiles) {
            var data = tiles.map(function(tile) {

                var bgColor = tile.visualElements.backgroundColor;
                var bgColorString = "{a: " + bgColor.a + ", b: " + bgColor.b + ", g: " + bgColor.g + ", r: " + bgColor.r + "}";

                var foregroundTextString = tile.visualElements.foregroundText;
                switch (foregroundTextString) {
                    case Windows.UI.StartScreen.ForegroundText.dark:
                        foregroundTextString = "dark";
                        break;
                    case Windows.UI.StartScreen.ForegroundText.light:
                        foregroundTextString = "light";
                }

                return {
                    displayName: tile.displayName,
                    tileId: tile.tileId,
                    arguments: tile.arguments,
                    visualElements: {
                        backgroundColor: bgColorString,
                        foregroundText: foregroundTextString,
                        showNameOnSquare150x150Logo: tile.visualElements.showNameOnSquare150x150Logo,
                        showNameOnSquare310x310Logo: tile.visualElements.showNameOnSquare310x310Logo,
                        showNameOnWide310x150Logo: tile.visualElements.showNameOnWide310x150Logo,
                        square30x30Logo: uriToString(tile.visualElements.square30x30Logo),
                        square44x44Logo: uriToString(tile.visualElements.square44x44Logo),
                        square70x70Logo: uriToString(tile.visualElements.square70x70Logo),
                        square71x71Logo: uriToString(tile.visualElements.square71x71Logo),
                        square150x150Logo: uriToString(tile.visualElements.square150x150Logo),
                        square310x310Logo: uriToString(tile.visualElements.square310x310Logo),
                        wide310x150Logo: uriToString(tile.visualElements.wide310x150Logo),
                    }
                };
            });

            listView.itemDataSource = new WinJS.Binding.List(data).dataSource;
        });
    }

    function unpinTiles() {
        unpinCommand.disabled = true;

        SecondaryTile.findAllAsync().then(function (tiles) {
            return WinJS.Promise.join(tiles.map(function (tile) {
                return tile.requestDeleteAsync();
            }));
        }).then(function () {
            return setListViewDataAsync(secondaryTilesListView);
        }).done(function () {
            unpinCommand.disabled = false;
        });
    }

})();