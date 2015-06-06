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

using Smartcard;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.Devices.SmartCards;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Smart Card";

        public const int ADMIN_KEY_LENGTH_IN_BYTES = 24;

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Create and Provision a TPM Virtual Smart Card", ClassType=typeof(Scenario1_ProvisionTPM)},
            new Scenario() { Title="Change Smart Card PIN", ClassType=typeof(Scenario2_ChangePIN)},
            new Scenario() { Title="Reset Smart Card PIN", ClassType=typeof(Scenario3_ResetPIN)},
            new Scenario() { Title="Change Smart Card Admin Key", ClassType=typeof(Scenario4_ChangeAdminKey)},
            new Scenario() { Title="Verify Response", ClassType=typeof(Scenario5_VerifyResponse)},
            new Scenario() { Title="Delete TPM Virtual Smart Card", ClassType=typeof(Scenario6_DeleteTPM)},
            new Scenario() { Title="List All Smart Cards", ClassType=typeof(Scenario7_ListAllCards)},
            new Scenario() { Title="Transmit APDU to Smart Card", ClassType=typeof(Scenario8_TransmitAPDU)}
        };

        public Windows.Storage.Streams.IBuffer AdminKey { get; set; }

        public String SmartCardReaderDeviceId { get; set; }

        public async Task<SmartCard> GetSmartCard()
        {
            SmartCardReader reader = await SmartCardReader.FromIdAsync(SmartCardReaderDeviceId);
            IReadOnlyList<SmartCard> cards = await reader.FindAllCardsAsync();

            if (1 != cards.Count)
            {
                throw new InvalidOperationException("Reader has an unexpected number of cards (" + cards.Count + ")");
            }

            return cards[0];
        }

        public bool ValidateTPMSmartCard()
        {
            if (string.IsNullOrEmpty(SmartCardReaderDeviceId))
            {
                return false;
            }
            return true;
        }
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
