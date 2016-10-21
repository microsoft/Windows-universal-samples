//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var SecondaryTile = Windows.UI.StartScreen.SecondaryTile;
    var TileSize = Windows.UI.StartScreen.TileSize;
    var Uri = Windows.Foundation.Uri;

    var TileNotification = Windows.UI.Notifications.TileNotification;
    var TileUpdateManager = Windows.UI.Notifications.TileUpdateManager;
    var Tiles = NotificationsExtensions.Tiles;

    var _tileId;

    // UI elements on the page
    var sendCommand;
    var pinCommand;


    var page = WinJS.UI.Pages.define("/html/scenario4-secondarytilenotifications.html", {
        ready: function (element, options) {

            pinCommand = element.querySelector("#pincommand");
            pinCommand.addEventListener("click", pinTile);

            sendCommand = element.querySelector("#sendcommand");
            sendCommand.addEventListener("click", sendNotification);
        }
    });

    function pinTile() {

        pinCommand.disabled = true;

        // Generate a unique tile ID for the purposes of the sample
        _tileId = new Date().getTime().toString();

        // Initialize and pin a new secondary tile.
        var tile = new SecondaryTile(_tileId, "Secondary notifications", "args", new Uri("ms-appx:///images/Square150x150Logo.png"), TileSize.default);
        tile.visualElements.wide310x150Logo = new Uri("ms-appx:///images/Wide310x150Logo.png");
        tile.visualElements.square310x310Logo = new Uri("ms-appx:///images/Square310x310Logo.png");
        tile.visualElements.showNameOnSquare150x150Logo = true;
        tile.visualElements.showNameOnSquare310x310Logo = true;
        tile.visualElements.showNameOnWide310x150Logo = true;

        tile.requestCreateAsync().then(function complete() {
            pinCommand.disabled = false;
        });
    }

    function sendNotification() {

        var nowTimeString = new Date().toLocaleString();

        var textConfigs = [
            { text: "New secondary tile notification", hintWrap: true },
            { text: nowTimeString, hintWrap: true, hintStyle: NotificationsExtensions.AdaptiveTextStyle.captionSubtle }
        ];

        // Build and append content from textConfigs, line by line.
        var adaptiveContent = new Tiles.TileBindingContentAdaptive();
        textConfigs.forEach(function (lineConfig) {
            var lineOfText = new NotificationsExtensions.AdaptiveText();
            for (var key in lineConfig) {
                lineOfText[key] = lineConfig[key];
            }
            adaptiveContent.children.push(lineOfText);
        });

        // Specify templates and send Notification.
        var tileContent = new Tiles.TileContent();
        tileContent.visual = new Tiles.TileVisual();
        tileContent.visual.branding = Tiles.TileBranding.nameAndLogo;
        tileContent.visual.tileMedium = new Tiles.TileBinding();
        tileContent.visual.tileMedium.content = adaptiveContent;
        tileContent.visual.tileWide = new Tiles.TileBinding();
        tileContent.visual.tileWide.content = adaptiveContent;
        tileContent.visual.tileLarge = new Tiles.TileBinding();
        tileContent.visual.tileLarge.content = adaptiveContent;

        var doc = tileContent.getXml();
        var notification = new TileNotification(doc);
        TileUpdateManager.createTileUpdaterForSecondaryTile(_tileId).update(notification);
    }

})();