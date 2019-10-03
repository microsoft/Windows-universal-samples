//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var System = Windows.System;
    var User = System.User;
    var KnownUserProperties = System.KnownUserProperties;
    var UserPictureSize = System.UserPictureSize;

    var userTypeNames = [ "LocalUser", "RemoteUser", "LocalGuest", "RemoteGuest" ];
    var authenticationStatusNames = [ "Unauthenticated", "LocallyAuthenticated", "RemotelyAuthenticated" ];

    var nextUserNumber = 1;
    var userList;
    var resultsText;
    var profileImage;

    var page = WinJS.UI.Pages.define("/html/scenario1-findUsers.html", {
        ready: function (element, options) {
            userList = document.getElementById("userList");
            resultsText = document.getElementById("resultsText");
            profileImage = document.getElementById("profileImage");

            document.getElementById("showPropertiesButton").addEventListener("click", showProperties, false);
                
            return fillUsersAsync();
        }
    });

    function fillUsersAsync() {
        nextUserNumber = 1;
        User.findAllAsync().then(function (users) {
            WinJS.Promise.join(users.map(function (user) {
                return user.getPropertyAsync(KnownUserProperties.displayName);
            })).then(function (results) {
                for (var i = 0; i < users.length; i++) {
                    // Choose a generic name if we do not have access to the actual name.
                    var displayName = results[i];
                    if (displayName == "") {
                        displayName = "User #" + nextUserNumber;
                        ++nextUserNumber;
                    }

                    var newOption = document.createElement("option");
                    newOption.text = displayName;
                    newOption.value = users[i].nonRoamableId;
                    newOption.selected = false;
                    userList.add(newOption);
                }
            });
        });
    }

    function showProperties() {
        if (userList.selectedIndex >= 0) {
            resultsText.innerText = "";
            profileImage.src = "";
            WinJS.log && WinJS.log("", "sample", "status");

            var userId = userList[userList.selectedIndex].value;
            try {
                var user = User.getFromId(userId);
            } catch (ex) {
                WinJS.log && WinJS.log(ex.message, "sample", "error");
                return;
            }

            // Build a list of all the properties we want.
            var desiredProperties = [
                    KnownUserProperties.firstName,
                    KnownUserProperties.lastName,
                    KnownUserProperties.providerName,
                    KnownUserProperties.accountName,
                    KnownUserProperties.guestHost,
                    KnownUserProperties.principalName,
                    KnownUserProperties.domainName,
                    KnownUserProperties.sessionInitiationProtocolUri
            ];

            // Issue a bulk query for all of the properties above.
            user.getPropertiesAsync(desiredProperties).then(function (values) {
                // Generate the results.
                var result = "NonRoamableId: " + user.nonRoamableId + "\n";
                result += "Type: " + userTypeNames[user.type] + "\n";
                result += "AuthenticationStatus: " + authenticationStatusNames[user.authenticationStatus] + "\n";
                desiredProperties.forEach(function (propertyName) {
                    result += propertyName + ": "+ values[propertyName] + "\n";
                });

                resultsText.innerText = result;
            }).then(function () {
                return user.getPictureAsync(UserPictureSize.size64x64);
            }).then(function (streamReference) {
                return streamReference && streamReference.openReadAsync();
            }).then(function (stream) {
                if (stream) {
                    profileImage.src = URL.createObjectURL(stream, { oneTimeOnly: true });
                    // Close the stream once the image is loaded.
                    profileImage.onload = function () { stream.close(); };
                }
            });
        }
    }
})();
