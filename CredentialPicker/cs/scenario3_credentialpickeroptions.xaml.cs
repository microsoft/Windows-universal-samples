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
    public sealed partial class Scenario3_CredentialPickerOptions : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public Scenario3_CredentialPickerOptions()
        {
            this.InitializeComponent();
        }

        void SetError(string err)
        {
            rootPage.NotifyUser(err, NotifyType.ErrorMessage);
        }

        void SetPasswordExplainVisibility(bool isShown)
        {
            if (isShown)
            {
                PasswordExplain1.Visibility = Windows.UI.Xaml.Visibility.Visible;
                PasswordExplain2.Visibility = Windows.UI.Xaml.Visibility.Visible;
            }
            else
            {
                PasswordExplain1.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
                PasswordExplain2.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
            }
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

       
        private async void Launch_Click(object sender, RoutedEventArgs e)
        {
            if ((Target.Text == "") || (Message.Text == "") || (Caption.Text == ""))
            {
                return;
            }

            CredentialPickerOptions credPickerOptions = new CredentialPickerOptions();
            credPickerOptions.Message = Message.Text;
            credPickerOptions.Caption = Caption.Text;
            credPickerOptions.TargetName = Target.Text;
            credPickerOptions.AlwaysDisplayDialog = (AlwaysShowDialog.IsChecked == true);
            if (ProtocolSelection.SelectedItem == null)
            {
                //default protocol, if no selection
                credPickerOptions.AuthenticationProtocol = AuthenticationProtocol.Negotiate;
            }
            else
            {
                string selectedProtocol = ((ComboBoxItem)ProtocolSelection.SelectedItem).Content.ToString();
                if (selectedProtocol.Equals("Kerberos", StringComparison.CurrentCultureIgnoreCase))
                {
                    credPickerOptions.AuthenticationProtocol = AuthenticationProtocol.Kerberos;
                }
                else if (selectedProtocol.Equals("Negotiate", StringComparison.CurrentCultureIgnoreCase))
                {
                    credPickerOptions.AuthenticationProtocol = AuthenticationProtocol.Negotiate;
                }
                else if (selectedProtocol.Equals("NTLM", StringComparison.CurrentCultureIgnoreCase))
                {
                    credPickerOptions.AuthenticationProtocol = AuthenticationProtocol.Ntlm;
                }
                else if (selectedProtocol.Equals("CredSsp", StringComparison.CurrentCultureIgnoreCase))
                {
                    credPickerOptions.AuthenticationProtocol = AuthenticationProtocol.CredSsp;
                }
                else if (selectedProtocol.Equals("Basic", StringComparison.CurrentCultureIgnoreCase))
                {
                    credPickerOptions.AuthenticationProtocol = AuthenticationProtocol.Basic;
                }
                else if (selectedProtocol.Equals("Digest", StringComparison.CurrentCultureIgnoreCase))
                {
                    credPickerOptions.AuthenticationProtocol = AuthenticationProtocol.Digest;
                }
                else if (selectedProtocol.Equals("Custom", StringComparison.CurrentCultureIgnoreCase))
                {
                    if (CustomProtocol.Text != null && CustomProtocol.Text != String.Empty)
                    {
                        credPickerOptions.AuthenticationProtocol = AuthenticationProtocol.Custom;
                        credPickerOptions.CustomAuthenticationProtocol = CustomProtocol.Text;
                    }
                    else
                    {
                        rootPage.NotifyUser("Please enter a custom protocol", NotifyType.ErrorMessage);
                    }
                }
            }

            if (SaveCheckboxSelection.SelectedItem != null)
            {
                string checkboxState = ((ComboBoxItem)SaveCheckboxSelection.SelectedItem).Content.ToString();
                if (checkboxState.Equals("Hidden",StringComparison.CurrentCultureIgnoreCase))
                {
                    credPickerOptions.CredentialSaveOption = CredentialSaveOption.Hidden;
                }
                else if (checkboxState.Equals("Selected", StringComparison.CurrentCultureIgnoreCase))
                {
                    credPickerOptions.CredentialSaveOption = CredentialSaveOption.Selected;
                }
                else if (checkboxState.Equals("Unselected", StringComparison.CurrentCultureIgnoreCase))
                {
                    credPickerOptions.CredentialSaveOption = CredentialSaveOption.Unselected;
                }
            }

            var credPickerResults = await Windows.Security.Credentials.UI.CredentialPicker.PickAsync(credPickerOptions);
            SetResult(credPickerResults);
        }

        private void Protocol_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ComboBox box = sender as ComboBox;
            if (CustomProtocol == null || ProtocolSelection == null || ProtocolSelection.SelectedItem == null)
            {
                //return if this was triggered before all components are initialized
                return;
            }
            
            string selectedProtocol = ((ComboBoxItem)ProtocolSelection.SelectedItem).Content.ToString();

            if (selectedProtocol.Equals("Custom", StringComparison.CurrentCultureIgnoreCase))
            {
                CustomProtocol.IsEnabled = true;
            }
            else
            {
                CustomProtocol.IsEnabled = false;
                //Basic and Digest return plaintext passwords
                if (selectedProtocol.Equals("Basic",StringComparison.CurrentCultureIgnoreCase))
                {
                    SetPasswordExplainVisibility(false);
                }
                else if (selectedProtocol.Equals("Digest",StringComparison.CurrentCultureIgnoreCase))
                {
                    SetPasswordExplainVisibility(false);
                }
                else
                {
                    SetPasswordExplainVisibility(true);
                }
            }
        }


    }
}
