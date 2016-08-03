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
    var pinCommand;
    var sendCommand;

    var page = WinJS.UI.Pages.define("/html/scenario5-expiringtilenotifications.html", {
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
        var tile = new SecondaryTile(_tileId, "Expiring Notification", "args", new Uri("ms-appx:///images/Square150x150Logo.png"), TileSize.default);
        tile.visualElements.square71x71Logo = new Uri("ms-appx:///images/Small.png");
        tile.visualElements.wide310x150Logo = new Uri("ms-appx:///images/WideLogo.png");
        tile.visualElements.square310x310Logo = new Uri("ms-appx:///images/LargeLogo.png");
        tile.visualElements.showNameOnSquare150x150Logo = true;
        tile.visualElements.showNameOnSquare310x310Logo = true;
        tile.visualElements.showNameOnWide310x150Logo = true;

        tile.requestCreateAsync().then(function complete() {
            pinCommand.disabled = false;
        });
    }

    function sendNotification() {

        // Set expiration time to 20 seconds.
        var expirationTime = new Date();
        expirationTime.setSeconds(expirationTime.getSeconds() + 20);
        var expirationTimeString = expirationTime.toLocaleTimeString();

        // Build TileText
        var lineOfText = new NotificationsExtensions.AdaptiveText();
        lineOfText.text = "This notification will expire at " + expirationTimeString;
        lineOfText.hintWrap = true;
        var adaptiveContent = new Tiles.TileBindingContentAdaptive();
        adaptiveContent.children.push(lineOfText);

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
        notification.expirationTime = expirationTime;
        TileUpdateManager.createTileUpdaterForSecondaryTile(_tileId).update(notification);
    }

})();