//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var TileNotification = Windows.UI.Notifications.TileNotification;
    var TileUpdateManager = Windows.UI.Notifications.TileUpdateManager;
    var Tiles = NotificationsExtensions.Tiles;

    // UI elements on the page
    var sendCommand;
    var clearCommand;

    var page = WinJS.UI.Pages.define("/html/scenario3-primarytilenotifications.html", {
        ready: function (element, options) {

            sendCommand = element.querySelector("#sendcommand");
            sendCommand.addEventListener("click", sendNotification);
            sendCommand.disabled = false;

            clearCommand = element.querySelector("#clearcommand");
            clearCommand.addEventListener("click", clearNotification);

        }
    });

    function sendNotification() {

        var nowTimeString = new Date().toLocaleString();

        var textConfigs = [
            { text: "New primary tile notification", hintWrap: true },
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
        TileUpdateManager.createTileUpdaterForApplication().update(notification);
    }

    function clearNotification() {
        TileUpdateManager.createTileUpdaterForApplication().clear();
    }

})();