//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

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

    var page = WinJS.UI.Pages.define("/html/scenario7-primarytilebadges.html", {
        ready: function (element, options) {

            clearCommand = element.querySelector("#clearcommand");
            clearCommand.addEventListener("click", clearBadge, false);

            updateNumberCommand = element.querySelector("#updatenumbercommand");
            updateNumberCommand.addEventListener("click", updateBadgeAsNumber, false);

            numberInput = element.querySelector("#numberinput");

            updateGlyphCommand = element.querySelector("#updateglyphcommand");
            updateGlyphCommand.addEventListener("click", updateBadgeAsGlyph, false);

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
        }
    });

    function clearBadge() {
        // Clear the badge from the primary tile.
        BadgeUpdateManager.createBadgeUpdaterForApplication().clear();
    }
    function updateBadgeAsNumber() {
        var num = numberInput.value;

        var badgeXml = new Badges.BadgeNumericNotificationContent(num).getXml();

        // Create the badge notification
        var badge = new BadgeNotification(badgeXml);

        // Create the badge updater for the application.
        var badgeUpdater = BadgeUpdateManager.createBadgeUpdaterForApplication();

        // And update the badge
        badgeUpdater.update(badge);
    }
    function updateBadgeAsGlyph() {
        var glyph = GlyphValue[glyphSelect.value];

        var badgeXml = new Badges.BadgeGlyphNotificationContent(glyph).getXml();

        // Create the badge notification
        var badge = new BadgeNotification(badgeXml);

        // Create the badge updater for our application.
        var badgeUpdater = BadgeUpdateManager.createBadgeUpdaterForApplication();

        // And update the badge
        badgeUpdater.update(badge);
    }

})();