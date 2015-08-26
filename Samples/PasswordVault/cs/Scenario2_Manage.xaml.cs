//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using Windows.Security.Credentials;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario2_Manage : Page
    {
        private MainPage rootPage = MainPage.Current;

        const int ElementNotFound = unchecked((int)0x80070490);

        public Scenario2_Manage()
        {
            this.InitializeComponent();
        }

        private void RetrieveCredentials()
        {
            var resource = InputResourceValue.Text;
            var userName = InputUserNameValue.Text;

            // Clear previous output.
            RetrievedCredentials.Items.Clear();
            rootPage.NotifyUser("", NotifyType.StatusMessage);

            var vault = new PasswordVault();
            IReadOnlyList<PasswordCredential> creds = null;

            // The credential retrieval functions raise an "Element not found"
            // exception if there were no matches.
            try
            {
                // Perform the desired queryuserName
                if (resource == "" && userName == "")
                {
                    // Both resource and user name are empty: Use retrieveAll().
                    creds = vault.RetrieveAll();
                }
                else if (userName == "")
                {
                    // Resource is provided but no user name: Use findAllByResource().
                    creds = vault.FindAllByResource(resource);
                }
                else if (resource == "")
                {
                    // User name is provided but no resource: Use findByUserName().
                    creds = vault.FindAllByUserName(userName);
                }
                else
                {
                    // Both resource and user name are provided: Use retrieve().
                    PasswordCredential cred = vault.Retrieve(resource, userName);
                    // Add it to our results if the retrieval was successful.
                    if (cred != null)
                    {
                        creds = new List<PasswordCredential>() { cred };
                    }
                }
            }
            catch (Exception e)
            {
                if (e.HResult != ElementNotFound)
                {
                    throw;
                }
            }

            // Display the results. Note that the password field is initially blank.
            // You must call retrievePassword() to get the password.

            if (creds == null || creds.Count == 0)
            {
                rootPage.NotifyUser("No matching credentials", NotifyType.ErrorMessage);
            }
            else
            {
                ItemCollection items = RetrievedCredentials.Items;
                foreach (PasswordCredential cred in creds)
                {
                    var item = new TextBlock();
                    item.DataContext = cred;
                    item.Text = cred.Resource + ": " + cred.UserName;
                    items.Add(item);
                }
                rootPage.NotifyUser("Credentials found: " + items.Count.ToString(), NotifyType.StatusMessage);
            }
        }

        private void RevealPasswords()
        {
            IList<Object> selectedItems = RetrievedCredentials.SelectedItems;
            if (selectedItems.Count > 0)
            {
                int retrievedCount = 0;
                foreach (object o in selectedItems)
                {
                    TextBlock item = (TextBlock)o;
                    PasswordCredential cred = (PasswordCredential)item.DataContext;

                    // The credential retrieval functions raise an "Element not found"
                    // exception if the credential is no longer in the PasswordVault.
                    // (For example, if the user manually removed the credential via the
                    // Control Panel.)
                    try
                    {
                        cred.RetrievePassword();
                        item.Text = cred.Resource + ": " + cred.UserName + " (" + cred.Password + ")";
                        retrievedCount++;
                    }
                    catch (Exception e)
                    {
                        if (e.HResult != ElementNotFound)
                        {
                            throw;
                        }
                        item.Text = cred.Resource + ": " + cred.UserName + " (password unavailable)";
                    }

                }
                rootPage.NotifyUser("Passwords retrieved: " + retrievedCount.ToString(), NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("No credentials selected", NotifyType.ErrorMessage);
            }
        }

        private void RemoveCredentials()
        {
            IList<object> selectedItems = RetrievedCredentials.SelectedItems;
            if (selectedItems.Count > 0)
            {
                var vault = new PasswordVault();
                int removedCount = 0;

                // Take a snapshot of the selectedOptions collection because we will be
                // changing the collection during the loop.
                var snapshot = new List<object>(selectedItems);

                foreach (object o in snapshot)
                {
                    TextBlock item = (TextBlock)o;
                    PasswordCredential cred = (PasswordCredential)item.DataContext;
                    RetrievedCredentials.Items.Remove(o);

                    try
                    {
                        vault.Remove(cred);
                        removedCount++;
                    }
                    catch (Exception e)
                    {
                        // Ignore exception if the credential was already removed.
                        if (e.HResult != ElementNotFound)
                        {
                            throw;
                        }
                    }
                }
                rootPage.NotifyUser("Credentials removed: " + removedCount.ToString(), NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("No credentials selected", NotifyType.ErrorMessage);
            }
        }
    }
}
