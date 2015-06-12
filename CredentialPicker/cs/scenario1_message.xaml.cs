//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using SDKTemplate;
using System;
using Windows.Security.Credentials.UI;

namespace CredentialPicker
{
    public sealed partial class Scenario1_Message : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public Scenario1_Message()
        {
            this.InitializeComponent();
        }

        void SetError(string err)
        {
            rootPage.NotifyUser(err, NotifyType.ErrorMessage);
        }

        void SetResult(CredentialPickerResults res)
        {
            if (res.ErrorCode == 0)
            {
                Status.Text = String.Format("OK");
            }
            else
            {
                Status.Text = String.Format("Error returned from CredPicker API: {0}", res.ErrorCode);
            }
            Domain.Text = res.CredentialDomainName;
            Username.Text = res.CredentialUserName;
            Password.Text = res.CredentialPassword;
            CredentialSaved.Text = (res.CredentialSaved ? "true" : "false");
            switch (res.CredentialSaveOption)
            {
                case CredentialSaveOption.Hidden:
                    CheckboxState.Text = "Hidden";
                    break;
                case CredentialSaveOption.Selected:
                    CheckboxState.Text = "Selected";
                    break;
                case CredentialSaveOption.Unselected:
                    CheckboxState.Text = "Unselected";
                    break;
            }
        }


        #region Sample Click Handlers - modify if you need them, delete them otherwise
        private async void Launch_Click(object sender, RoutedEventArgs e)
        {
            if ((Target.Text.Length != 0) && (Message.Text.Length != 0))
            {
                try
                {
                    CredentialPickerResults credPickerResults = await Windows.Security.Credentials.UI.CredentialPicker.PickAsync(Target.Text, Message.Text);
                    SetResult(credPickerResults);
                }
                catch(Exception ex)
                {
                    SetError(ex.GetType().ToString() + ": " + ex.Message);
                }
            }
           
        }
        #endregion
    }
}
