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

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using SDKTemplate;
using System.Threading.Tasks;
using System;
using Windows.Security.Credentials;
using System.Collections.Generic;

namespace PasswordVaultSample
{
    public abstract class InitializePasswordVault
    {
        public static void Initialize()
        {
            Task.Factory.StartNew(() => { InitializePasswordVault.LoadPasswordVault(); });
        }

        private static void LoadPasswordVault()
        {
            // any call to the password vault will load the vault
            PasswordVault vault = new PasswordVault();
            vault.RetrieveAll();
        }
    }

    public sealed partial class Scenario1_AddReadRemoveCredentials : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario1_AddReadRemoveCredentials()
        {
            this.InitializeComponent();

            // Initialize the password vault, this may take less than a second
            // An optimistic initialization at this stage improves the UI performance
            // when any other call to the password vault may be made later on
            InitializePasswordVault.Initialize();
        }

        private void Save_Click(object sender, RoutedEventArgs e)
        {
            String result = "";
            if (InputResourceValue.Text == "" || InputUserNameValue.Text == "" || InputPasswordValue.Password == "")
            {
                rootPage.NotifyUser("Inputs are missing. Resource, User name and Password are required", NotifyType.ErrorMessage);
            }
            else
            {
                try
                {
                    //Add a credential to PasswordVault by supplying resource, username, and password
                    PasswordVault vault = new PasswordVault();
                    PasswordCredential cred = new PasswordCredential(InputResourceValue.Text, InputUserNameValue.Text, InputPasswordValue.Password);
                    vault.Add(cred);

                    //Output credential added to debug spew
                    rootPage.NotifyUser("Credential saved successfully. " + "Resource: " + cred.Resource.ToString() + " Username: " + cred.UserName.ToString() + " Password: " + cred.Password.ToString() + ".", NotifyType.StatusMessage);
                }
                catch (Exception Error) // No stored credentials, so none to delete
                {
                    rootPage.NotifyUser(Error.Message, NotifyType.ErrorMessage);
                }
            }

        }

        private void Read_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                //Read a credential from PasswordVault by supplying resource or username
                Windows.Security.Credentials.PasswordVault vault = new Windows.Security.Credentials.PasswordVault();
                IReadOnlyList<PasswordCredential> creds = null;
                PasswordCredential cred = null;

                //If both resource and username are empty, you can use RetrieveAll() to enumerate all credentials
                if (InputUserNameValue.Text == "" && InputResourceValue.Text == "")
                {
                    rootPage.NotifyUser("Retrieving all credentials since resource or username are not specified.", NotifyType.StatusMessage);
                    creds = vault.RetrieveAll();
                }

                //If there is only resouce, you can use FindAllByResource() to enumerate by resource. 
                //Note: the password will not be returned, you need to call retrieveAll with returned resouce and username to get the credential with password
                else if (InputUserNameValue.Text == "")
                {
                    rootPage.NotifyUser("Retrieve credentials by resouces that you provided", NotifyType.StatusMessage);
                    creds = vault.FindAllByResource(InputResourceValue.Text);
                }

                //If there is only username, you can use findbyusername() to enumerate by resource. 
                //Note: the password will not be returned, you need to call retrieveAll with returned resouce and username to get the credential with password 
                else if (InputResourceValue.Text == "")
                {
                    rootPage.NotifyUser("Retrieve credentials by username that you provided", NotifyType.StatusMessage);
                    creds = vault.FindAllByUserName(InputUserNameValue.Text);
                }

                //Read by explicit resource and username name, result will be a single credential if it exists. Password will be returned.
                else
                {
                    cred = vault.Retrieve(InputResourceValue.Text, InputUserNameValue.Text);
                }

                //Output credential added to debug spew
                if (creds != null)
                {
                    rootPage.NotifyUser("There are " + creds.Count + " credential(s) found.", NotifyType.StatusMessage);
                    foreach (PasswordCredential c in creds)
                    {
                        try
                        {
                            PasswordCredential c1 = vault.Retrieve(c.Resource.ToString(), c.UserName.ToString());
                            rootPage.NotifyUser("Credential read successfully. " + "Resource: " + c.Resource.ToString() + ", " + "Username: " + c.UserName.ToString() + "Password: " + c1.Password.ToString() + ".", NotifyType.StatusMessage);
                        }
                        catch (Exception Error)
                        {
                            rootPage.NotifyUser(Error.Message, NotifyType.ErrorMessage);
                        }
                    }
                }

                else if (cred != null)
                {
                    rootPage.NotifyUser("Credential read successfully. " + "Resource: " + cred.Resource + ", " + "Username: " + cred.UserName + "Password: " + cred.Password.ToString() + ".", NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser("Credential not found.", NotifyType.StatusMessage);
                }
            }
            catch (Exception Error)
            {
                if (Error.HResult == -2147023728)
                {
                    rootPage.NotifyUser("Credential not found.", NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser(Error.Message, NotifyType.ErrorMessage);
                }
            }
        }

        private void Delete_Click(object sender, RoutedEventArgs e)
        {
            if (InputResourceValue.Text == "" || InputUserNameValue.Text == "")
            {
                rootPage.NotifyUser("Inputs are missing. Resource and Username are required.", NotifyType.ErrorMessage);
            }
            else
            {
                try
                {
                    // This is the code to remove a credential from PasswordVault by supplying resource or username
                    PasswordVault vault = new PasswordVault();
                    PasswordCredential cred = vault.Retrieve(InputResourceValue.Text, InputUserNameValue.Text);
                    vault.Remove(cred);
                    rootPage.NotifyUser("Credential removed successfully. Resource: " + InputResourceValue.Text + " Username: " + InputUserNameValue.Text + ".", NotifyType.StatusMessage);
                }
                catch (Exception Error) // No stored credentials, so none to delete
                {
                    rootPage.NotifyUser(Error.Message, NotifyType.ErrorMessage);
                }
            }

        }

        private void Reset_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                InputResourceValue.Text = "";
                InputUserNameValue.Text = "";
                InputPasswordValue.Password = "";

                PasswordVault vault = new PasswordVault();
                IReadOnlyList<PasswordCredential> creds = vault.RetrieveAll();
                foreach (PasswordCredential c in creds)
                {
                    try
                    {
                        vault.Remove(c);
                    }
                    catch (Exception Error)
                    {
                        rootPage.NotifyUser(Error.Message, NotifyType.ErrorMessage);
                    }
                }

                rootPage.NotifyUser("Scenario has been reset. All credentials are removed.", NotifyType.StatusMessage);
            }
            catch (Exception Error)
            {
                // Bad Parameter, Machine info Unavailable errors are to be handled here.
                rootPage.NotifyUser(Error.Message, NotifyType.ErrorMessage);
            }
        }
    }
}
