//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    // This code runs in a background thread in a Web Worker.
    // Initialize the password vault, this may take less than a second
    // An optimistic initialization at this stage improves the UI performance
    // when any other call to the password vault may be made later.

    var vault = new Windows.Security.Credentials.PasswordVault();

    // any call to the password vault will load the vault
    var creds = vault.retrieveAll();
})();
