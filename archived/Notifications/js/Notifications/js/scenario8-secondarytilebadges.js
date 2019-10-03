//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var SecondaryTile = Windows.UI.StartScreen.SecondaryTile;
    var TileSize = Windows.UI.StartScreen.TileSize;
    var Uri = Windows.Foundation.Uri;

    var BadgeUpdateManager = Windows.UI.Notifications.BadgeUpdateManager;
    var BadgeNotification = Windows.UI.Notifications.BadgeNotification;
    var Badges = NotificationsExtensions.Badges;
    var GlyphValue = Badges.GlyphValue;

    var _tileId;

    // UI elements on the page
    var pinCommand;
    var clearCommand;
    var updateNumberCommand;
    var numberInput;
    var updateGlyphCommand;
    var glyphSelect;
    var restartCommand;

    var page = WinJS.UI.Pages.define("/html/scenario8-secondarytilebadges.html", {
        ready: function (element, options) {

            pinCommand = element.querySelector("#pincommand");
            pinCommand.addEventListener("click", pinTile, false);
            pinCommand.disabled = false;

            clearCommand = element.querySelector("#clearcommand");
            clearCommand.addEventListener("click", clearBadge, false);
            clearCommand.disabled = true;

            updateNumberCommand = element.querySelector("#updatenumbercommand");
            updateNumberCommand.addEventListener("click", updateBadgeAsNumber, false);
            updateNumberCommand.disabled = true;

            numberInput = element.querySelector("#numberinput");
            numberInput.disabled = true;

            updateGlyphCommand = element.querySelector("#updateglyphcommand");
            updateGlyphCommand.addEventListener("click", updateBadgeAsGlyph, false);
            updateGlyphCommand.disabled = true;

            glyphSelect = element.querySelector("#glyphselect");
            Object.keys(GlyphValue).forEach(function (glyphName) {
                var option = document.createElement("option");
                option.value = glyphName;
                option.innerText = glyphName;
                if (GlyphValue[glyphName] === GlyphValue.activity) {
                    option.selected = true;
                }
                glyphSelect.appendChild(option);
            });
            glyphSelect.disabled = true;

            restartCommand = element.querySelector("#restartcommand");
            restartCommand.addEventListener("click", restartScenario);
        }
    });

    function pinTile() {
        pinCommand.disabled = true;

        // Generate a unique tile ID for the purposes of the sample
        _tileId = new Date().getTime().toString();

        // Initialize and pin a new secondary tile that we will later update
        var tile = new SecondaryTile(_tileId, "Badges", "args", new Uri("ms-appx:///images/Square150x150Logo.png"), TileSize.default);
        tile.visualElements.square71x71Logo = new Uri("ms-appx:///images/Small.png");
        tile.visualElements.wide310x150Logo = new Uri("ms-appx:///images/WideLogo.png");
        tile.visualElements.square310x310Logo = new Uri("ms-appx:///images/LargeLogo.png");
        tile.visualElements.backgroundColor = Windows.UI.Colors.blue;
        tile.visualElements.showNameOnSquare150x150Logo = true;
        tile.visualElements.showNameOnSquare310x310Logo = true;
        tile.visualElements.showNameOnWide310x150Logo = true;

        tile.requestCreateAsync().then(function complete() {
            // Unlock the rest of the scenario
            clearCommand.disabled = false;
            updateNumberCommand.disabled = false;
            updateGlyphCommand.disabled = false;
            numberInput.disabled = false;
            glyphSelect.disabled = false;
        });
    }

    function clearBadge() {
        // Clear the badge from the secondary tile
        BadgeUpdateManager.createBadgeUpdaterForSecondaryTile(_tileId).clear();
    }
    function updateBadgeAsNumber() {
        var num = numberInput.value;

        var badgeXml = new Badges.BadgeNumericNotificationContent(num).getXml();

        // Create the badge notification
        var badge = new BadgeNotification(badgeXml);

        // Create the badge updater for our secondary tile, using our tile ID for the secondary tile
        var badgeUpdater = BadgeUpdateManager.createBadgeUpdaterForSecondaryTile(_tileId);

        // And update the badge
        badgeUpdater.update(badge);
    }

    function updateBadgeAsGlyph() {
        var glyph = GlyphValue[glyphSelect.value];

        var badgeXml = new Badges.BadgeGlyphNotificationContent(glyph).getXml();

        // Create the badge notification
        var badge = new BadgeNotification(badgeXml);

        // Create the badge updater for our secondary tile, using our tile ID for the secondary tile
        var badgeUpdater = BadgeUpdateManager.createBadgeUpdaterForSecondaryTile(_tileId);

        // And update the badge
        badgeUpdater.update(badge);
    }

    function restartScenario() {
        pinCommand.disabled = false;
        clearCommand.disabled = true;
        updateNumberCommand.disabled = true;
        updateGlyphCommand.disabled = true;
        numberInput.disabled = true;
        glyphSelect.disabled = true;
    }
})();