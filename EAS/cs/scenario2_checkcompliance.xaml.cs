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

using SDKTemplate;
using System;
using Windows.Security.ExchangeActiveSyncProvisioning;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace EAS
{
    public sealed partial class Scenario2_CheckCompliance : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario2_CheckCompliance()
        {
            this.InitializeComponent();
        }

        private void Launch_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                EasClientSecurityPolicy RequestedPolicy = new EasClientSecurityPolicy();
                EasComplianceResults ComplianceResult = null;

                if (RequireEncryptionValue == null)
                {
                    RequestedPolicy.RequireEncryption = false;
                }
                else
                {
                    if (RequireEncryptionValue.SelectedIndex == 1)
                    {
                        RequestedPolicy.RequireEncryption = true;
                    }
                    else
                    {
                        RequestedPolicy.RequireEncryption = false;
                    }
                }

                if (MinPasswordLengthValue == null || MinPasswordLengthValue.Text.Length == 0)
                {
                    RequestedPolicy.MinPasswordLength = 0;
                }
                else
                {
                    RequestedPolicy.MinPasswordLength = Convert.ToByte(MinPasswordLengthValue.Text);
                }

                if (DisallowConvenienceLogonValue == null)
                {
                    RequestedPolicy.DisallowConvenienceLogon = false;
                }
                else
                {
                    if (DisallowConvenienceLogonValue.SelectedIndex == 1)
                    {
                        RequestedPolicy.DisallowConvenienceLogon = true;
                    }
                    else
                    {
                        RequestedPolicy.DisallowConvenienceLogon = false;
                    }
                }

                if (MinPasswordComplexCharactersValue == null || MinPasswordComplexCharactersValue.Text.Length == 0)
                {
                    RequestedPolicy.MinPasswordComplexCharacters = 0;
                }
                else
                {
                    RequestedPolicy.MinPasswordComplexCharacters = Convert.ToByte(MinPasswordComplexCharactersValue.Text);
                }

                if (PasswordExpirationValue == null || PasswordExpirationValue.Text.Length == 0)
                {
                    RequestedPolicy.PasswordExpiration = TimeSpan.Parse("0");
                }
                else
                {
                    RequestedPolicy.PasswordExpiration = TimeSpan.FromDays(Convert.ToDouble(PasswordExpirationValue.Text));
                }

                if (PasswordHistoryValue == null || PasswordHistoryValue.Text.Length == 0)
                {
                    RequestedPolicy.PasswordHistory = 0;
                }
                else
                {
                    RequestedPolicy.PasswordHistory = Convert.ToByte(PasswordHistoryValue.Text);
                }

                if (MaxPasswordFailedAttemptsValue == null || MaxPasswordFailedAttemptsValue.Text.Length == 0)
                {
                    RequestedPolicy.MaxPasswordFailedAttempts = 0;
                }
                else
                {
                    RequestedPolicy.MaxPasswordFailedAttempts = Convert.ToByte(MaxPasswordFailedAttemptsValue.Text);
                }

                if (MaxInactivityTimeLockValue == null || MaxInactivityTimeLockValue.Text.Length == 0)
                {
                    RequestedPolicy.MaxInactivityTimeLock = TimeSpan.Parse("0");
                }
                else
                {
                    RequestedPolicy.MaxInactivityTimeLock = TimeSpan.FromSeconds(Convert.ToDouble(MaxInactivityTimeLockValue.Text));
                }

                ComplianceResult = RequestedPolicy.CheckCompliance();

                RequireEncryptionResult.Text = ComplianceResult.RequireEncryptionResult.ToString();
                EncryptionProviderTypeResult.Text = ComplianceResult.EncryptionProviderType.ToString();
                MinPasswordLengthResult.Text = ComplianceResult.MinPasswordLengthResult.ToString();
                DisallowConvenienceLogonResult.Text = ComplianceResult.DisallowConvenienceLogonResult.ToString();
                MinPasswordComplexCharactersResult.Text = ComplianceResult.MinPasswordComplexCharactersResult.ToString();
                PasswordExpirationResult.Text = ComplianceResult.PasswordExpirationResult.ToString();
                PasswordHistoryResult.Text = ComplianceResult.PasswordHistoryResult.ToString();
                MaxPasswordFailedAttemptsResult.Text = ComplianceResult.MaxPasswordFailedAttemptsResult.ToString();
                MaxInactivityTimeLockResult.Text = ComplianceResult.MaxInactivityTimeLockResult.ToString();
            }
            catch (Exception Error)
            {
                rootPage.NotifyUser(Error.Message, NotifyType.ErrorMessage);
            }
        }

        private void Reset_Click(object sender, RoutedEventArgs e)
        {
            RequireEncryptionValue.SelectedIndex = 0;
            MinPasswordLengthValue.Text = "";
            DisallowConvenienceLogonValue.SelectedIndex = 0;
            MinPasswordComplexCharactersValue.Text = "";
            PasswordExpirationValue.Text = "";
            PasswordHistoryValue.Text = "";
            MaxPasswordFailedAttemptsValue.Text = "";
            MaxInactivityTimeLockValue.Text = "";
            RequireEncryptionResult.Text = "";
            EncryptionProviderTypeResult.Text = "";
            MinPasswordLengthResult.Text = "";
            DisallowConvenienceLogonResult.Text = "";
            MinPasswordComplexCharactersResult.Text = "";
            PasswordExpirationResult.Text = "";
            PasswordHistoryResult.Text = "";
            MaxPasswordFailedAttemptsResult.Text = "";
            MaxInactivityTimeLockResult.Text = "";
        }
    }
}
