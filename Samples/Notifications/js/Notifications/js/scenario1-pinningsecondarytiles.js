//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var SecondaryTile = Windows.UI.StartScreen.SecondaryTile;
    var Uri = Windows.Foundation.Uri;
    var Colors = Windows.UI.Colors;

    // UI elements on the page
    var pinCommand;
    var displayNameInput;
    var checkBoxSquare71x71Logo;
    var checkBoxSquare150x150Logo;
    var checkBoxWide310x150Logo;
    var checkBoxSquare310x310Logo;
    var checkBoxShowNameOnSquare150x150Logo;
    var checkBoxShowNameOnWide310x150Logo;
    var checkBoxShowNameOnSquare310x310Logo;


    var page = WinJS.UI.Pages.define("/html/scenario1-pinningsecondarytiles.html", {
        ready: function (element, options) {
            pinCommand = element.querySelector("#pincommand");
            pinCommand.addEventListener("click", pinTiles);

            displayNameInput = element.querySelector("#displayname");
            displayNameInput.focus();

            checkBoxSquare71x71Logo = element.querySelector("#checkboxsquare71x71logo");
            checkBoxSquare150x150Logo = element.querySelector("#checkboxsquare150x150logo");
            checkBoxWide310x150Logo = element.querySelector("#checkboxwide310x150logo");
            checkBoxSquare310x310Logo = element.querySelector("#checkboxsquare310x310logo");
            checkBoxShowNameOnSquare150x150Logo = element.querySelector("#checkboxshownameonsquare150x150logo");
            checkBoxShowNameOnWide310x150Logo = element.querySelector("#checkboxshownameonwide310x150logo");
            checkBoxShowNameOnSquare310x310Logo = element.querySelector("#checkboxshownameonsquare310x310logo");
        }
    });

    function pinTiles() {
        pinCommand.disabled = true;

        var tile = new SecondaryTile(new Date().getTime());
        tile.arguments = "args";
        tile.displayName = displayNameInput.value;

        if (checkBoxSquare150x150Logo.checked) {
            tile.visualElements.square150x150Logo = new Uri("ms-appx:///images/Square150x150Logo.png");
        }
        if (checkBoxSquare71x71Logo.checked) {
            tile.visualElements.square71x71Logo = new Uri("ms-appx:///images/Square150x150Logo.png");
        }
        if (checkBoxWide310x150Logo.checked) {
            tile.visualElements.wide310x150Logo = new Uri("ms-appx:///images/Wide310x150Logo.png");
        }
        if (checkBoxSquare310x310Logo.checked) {
            tile.visualElements.square310x310Logo = new Uri("ms-appx:///images/Square310x310Logo.png");
        }

        tile.visualElements.backgroundColor = Colors.purple;
        tile.visualElements.showNameOnSquare150x150Logo = checkBoxShowNameOnSquare150x150Logo.checked;
        tile.visualElements.showNameOnSquare310x310Logo = checkBoxShowNameOnSquare310x310Logo.checked;
        tile.visualElements.showNameOnWide310x150Logo = checkBoxShowNameOnWide310x150Logo.checked;

        var asyncOp = tile.requestCreateAsync();

        asyncOp.done(function () {
            pinCommand.disabled = false;
        });
    }

})();