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
            return addNextUserAsync(0);

            function addNextUserAsync(i) {
                if (i < users.length) {
                    var user = users[i];
                    return user.getPropertyAsync(KnownUserProperties.displayName).then(function (displayName) {
                        // Choose a generic name if we do not have access to the actual name.
                        if (displayName == "") {
                            displayName = "User #" + nextUserNumber;
                            ++nextUserNumber;
                        }

                        var newOption = document.createElement("option");
                        newOption.text = displayName;
                        newOption.value = user.nonRoamableId;
                        newOption.selected = false;
                        userList.add(newOption);

                        return addNextUser(i + 1);
                    });
                }
            }
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
            // NOTE: Bulk queries do not work from JS right now.
            // user.getPropertiesAsync(desiredProperties).then(function (values) {
            simulateGetPropertiesAsync(user, desiredProperties).then(function (values) {
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

    // Bulk queries do not work from JS right now, so we simulate it by
    // issuing a series of single-property queries.
    function simulateGetPropertiesAsync(user, desiredProperties) {
        var values = {};
        return addNextPropertyAsync(0);

        function addNextPropertyAsync(i) {
            if (i < desiredProperties.length) {
                var propertyName = desiredProperties[i];
                return user.getPropertyAsync(propertyName).then(function (result) {
                    values[propertyName] = result;
                    return addNextPropertyAsync(i + 1);
                });
            } else {
                return values;
            }
        }
    }

})();
