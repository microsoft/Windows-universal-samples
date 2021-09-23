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
using Windows.Devices.Haptics;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "PenHaptics C# Sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Ink Canvas Tactile Feedback", ClassType=typeof(Scenario1_InkCanvasTactileFeedback)},
            new Scenario() { Title="Query Tactile Feedback Support", ClassType=typeof(Scenario2_SupportedTactileFeedback)},
            new Scenario() { Title="Inking Feedback", ClassType=typeof(Scenario3_InkingFeedback)},
            new Scenario() { Title="Interaction Feedback", ClassType=typeof(Scenario4_InteractionFeedback)},
            new Scenario() { Title="Inking and Interaction Feedback", ClassType=typeof(Scenario5_InkingAndInteractionFeedback)},
        };

        // Mapping between waveform names and waveform values.
        public static IReadOnlyDictionary<string, ushort> WaveformNamesMap { get; } = new Dictionary<string, ushort>()
            {
                ["BrushContinuous"] = KnownSimpleHapticsControllerWaveforms.BrushContinuous,
                ["BuzzContinuous"] = KnownSimpleHapticsControllerWaveforms.BuzzContinuous,
                ["ChiselMarkerContinuous"] = KnownSimpleHapticsControllerWaveforms.ChiselMarkerContinuous,
                ["Click"] = KnownSimpleHapticsControllerWaveforms.Click,
                ["EraserContinuous"] = KnownSimpleHapticsControllerWaveforms.EraserContinuous,
                ["Error"] = KnownSimpleHapticsControllerWaveforms.Error,
                ["GalaxyPenContinuous"] = KnownSimpleHapticsControllerWaveforms.GalaxyPenContinuous,
                ["Hover"] = KnownSimpleHapticsControllerWaveforms.Hover,
                ["InkContinuous"] = KnownSimpleHapticsControllerWaveforms.InkContinuous,
                ["MarkerContinuous"] = KnownSimpleHapticsControllerWaveforms.MarkerContinuous,
                ["PencilContinuous"] = KnownSimpleHapticsControllerWaveforms.PencilContinuous,
                ["Press"] = KnownSimpleHapticsControllerWaveforms.Press,
                ["Release"] = KnownSimpleHapticsControllerWaveforms.Release,
                ["RumbleContinuous"] = KnownSimpleHapticsControllerWaveforms.RumbleContinuous,
                ["Success"] = KnownSimpleHapticsControllerWaveforms.Success,
            };

        // Returns the first SimpleHapticsControllerFeedback supported by the provided SimpleHapticsController
        // which supports the specified waveform.
        public static SimpleHapticsControllerFeedback FindSupportedFeedback(SimpleHapticsController hapticsController, ushort waveform)
        {
            foreach (SimpleHapticsControllerFeedback feedback in hapticsController.SupportedFeedback)
            {
                if (feedback.Waveform == waveform)
                {
                    return feedback;
                }
            }
            return null;
        }
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
