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

using SDKTemplate.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using Windows.ApplicationModel.DataTransfer;
using Windows.UI.Core;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "AdaptiveStreaming";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Simplest Adaptive Streaming", ClassType=typeof(Scenario1_SimplestAdaptiveStreaming)},
            new Scenario() { Title="Event Handlers", ClassType=typeof(Scenario2_EventHandlers)},
            new Scenario() { Title="Network Request Modification", ClassType=typeof(Scenario3_RequestModification)},
            new Scenario() { Title="Adaptive Streaming Tuning", ClassType=typeof(Scenario4_Tuning)},
            new Scenario() { Title="Metadata", ClassType=typeof(Scenario5_Metadata)},
            new Scenario() { Title="Ad Insertion", ClassType=typeof(Scenario6_AdInsertion)},
        };

        public static IReadOnlyList<AdaptiveContentModel> ContentManagementSystemStub = AdaptiveContentModel.GetKnownAzureMediaServicesModels();
        public static AdaptiveContentModel FindContentById(int id)
        {
            return ContentManagementSystemStub.First(model => model.Id == id);
        }
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }

    public class BitrateHelper
    {
        // Each bitrate is assigned a symbol as follows:
        // The bottom quartile gets 1 bar, the second quartile gets 2 bars,
        // the third quartile gets 3 bars, and the top quartile gets 4 bars.
        private Dictionary<uint, Symbol> symbolAssignment = new Dictionary<uint, Symbol>();

        public BitrateHelper(IEnumerable<uint> availableBitRates)
        {
            uint[] sortedBitRates = availableBitRates.ToArray();
            Array.Sort(sortedBitRates);
            int i = 0;
            while (i < sortedBitRates.Length * 1 / 4)
            {
                symbolAssignment.Add(sortedBitRates[i++], Symbol.OneBar);
            }
            while (i < sortedBitRates.Length * 2 / 4)
            {
                symbolAssignment.Add(sortedBitRates[i++], Symbol.TwoBars);
            }
            while (i < sortedBitRates.Length * 3 / 4)
            {
                symbolAssignment.Add(sortedBitRates[i++], Symbol.ThreeBars);
            }
            while (i < sortedBitRates.Length * 4 / 4)
            {
                symbolAssignment.Add(sortedBitRates[i++], Symbol.FourBars);
            }
        }

        public Symbol GetBitrateSymbol(uint currentBitrate)
        {
            Symbol symbol = Symbol.ZeroBars;
            if (symbolAssignment.TryGetValue(currentBitrate, out symbol))
            {
                return symbol;
            }
            return Symbol.ZeroBars;
        }
    }
}