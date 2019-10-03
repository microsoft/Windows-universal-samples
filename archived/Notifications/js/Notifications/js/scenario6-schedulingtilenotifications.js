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
    var scheduleCommand;
    var restartCommand;

    var page = WinJS.UI.Pages.define("/html/scenario6-schedulingtilenotifications.html", {
        ready: function (element, options) {

            pinCommand = element.querySelector("#pincommand");
            pinCommand.addEventListener("click", pinTile);
            pinCommand.disabled = false;

            scheduleCommand = element.querySelector("#schedulecommand");
            scheduleCommand.addEventListener("click", scheduleNotifications);
            scheduleCommand.disabled = true;

            restartCommand = element.querySelector("#restartcommand");
            restartCommand.addEventListener("click", restartScenario);
        }
    });

    function pinTile() {

        pinCommand.disabled = true;

        // Generate a unique tile ID for the purposes of the sample
        _tileId = new Date().getTime().toString();

        // Initialize and pin a new secondary tile that we will later update
        var tile = new SecondaryTile(_tileId, "Scheduling Notifications", "args", new Uri("ms-appx:///images/Square150x150Logo.png"), TileSize.default);
        tile.visualElements.wide310x150Logo = new Uri("ms-appx:///images/Wide310x150Logo.png");
        tile.visualElements.square310x310Logo = new Uri("ms-appx:///images/Square310x310Logo.png");
        tile.visualElements.showNameOnSquare150x150Logo = true;
        tile.visualElements.showNameOnSquare310x310Logo = true;
        tile.visualElements.showNameOnWide310x150Logo = true;

        tile.requestCreateAsync().then(function complete() {
            // unlock the rest of the scenario
            scheduleCommand.disabled = false;
        });
    }

    // Schedule a single notification.
    function scheduleNotification(text, dueTime) {
        // Build TileText
        var lineOfText = new NotificationsExtensions.AdaptiveText();
        lineOfText.text = text;
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

        /// Create the notification object.
        var futureNotification = new Windows.UI.Notifications.ScheduledTileNotification(doc, dueTime);
        futureNotification.id = dueTime.getTime();

        // Add to the schedule.
        TileUpdateManager.createTileUpdaterForSecondaryTile(_tileId).addToSchedule(futureNotification);
    }

    function scheduleNotifications() {

        scheduleCommand.disabled = true;

        // Schedule 4 notifications over 20 seconds.
        var dateTime = new Date();

        dateTime.setSeconds(dateTime.getSeconds() + 1);
        scheduleNotification("20 seconds to go...", dateTime);

        dateTime.setSeconds(dateTime.getSeconds() + 10);
        scheduleNotification("10 seconds to go..", dateTime);

        dateTime.setSeconds(dateTime.getSeconds() + 5);
        scheduleNotification("5 seconds to go.", dateTime);

        dateTime.setSeconds(dateTime.getSeconds() + 5);
        scheduleNotification("Done :)", dateTime);
    }

    function restartScenario() {
        pinCommand.disabled = false;
        scheduleCommand.disabled = true;
    }

})();