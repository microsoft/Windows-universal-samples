//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var Uri = Windows.Foundation.Uri;
    var StartScreen = Windows.UI.StartScreen;
    var JumpList = StartScreen.JumpList;
    var JumpListItem = StartScreen.JumpListItem;

    var page = WinJS.UI.Pages.define("/html/scenario3-customitems.html", {
        ready: function (element, options) {
            DisplayNameLocalizedResource.addEventListener("change", onDisplayNameLocalizedResourceChange);
            DescriptionLocalizedResource.addEventListener("change", onDescriptionLocalizedResourceChange);
            GroupNameLocalizedResource.addEventListener("change", onGroupNameLocalizedResourceChange);
            AddToJumpList.addEventListener("click", onAddToJumpListClick);
        }
    });

    function onDisplayNameLocalizedResourceChange() {
        DisplayName.disabled = DisplayNameLocalizedResource.checked;
        DisplayName.value = "ms-resource:///Resources/CustomJumpListItemDisplayName";
    }

    function onDescriptionLocalizedResourceChange() {
        Description.disabled = DescriptionLocalizedResource.checked;
        Description.value = "ms-resource:///Resources/CustomJumpListItemDescription";
    }

    function onGroupNameLocalizedResourceChange() {
        GroupName.disabled = GroupNameLocalizedResource.checked;
        GroupName.value = "ms-resource:///Resources/CustomJumpListGroupName";
    }

    function onAddToJumpListClick() {
        JumpList.loadCurrentAsync().done(function (jumpList) {
            
            var item = JumpListItem.createWithArguments(Arguments.value, DisplayName.value);
            item.description = Description.value;
            item.groupName = GroupName.value;
            item.logo = new Uri("ms-appx:///images/smallTile-sdk.png");

            jumpList.items.append(item);

            jumpList.saveAsync();
        });
    }
})();
