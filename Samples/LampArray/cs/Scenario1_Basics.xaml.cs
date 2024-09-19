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
using System.Linq;
using Windows.Devices.Enumeration;
using Windows.Devices.Lights;
using Windows.System;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario1_Basics : Page
    {
        private MainPage rootPage = MainPage.Current;

        private DeviceWatcher m_deviceWatcher;

        // Currently attached LampArrays
        private readonly List<LampArrayInfo> m_attachedLampArrays = new List<LampArrayInfo>();

        public Scenario1_Basics()
        {
            InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            UpdateLampArrayList();

            // Start watching for newly attached LampArrays.
            m_deviceWatcher = DeviceInformation.CreateWatcher(LampArray.GetDeviceSelector());

            m_deviceWatcher.Added += Watcher_Added;
            m_deviceWatcher.Removed += Watcher_Removed;

            m_deviceWatcher.Start();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            m_deviceWatcher.Stop();
        }

        private async void Watcher_Added(DeviceWatcher sender, DeviceInformation args)
        {
            var info = new LampArrayInfo(args.Id, args.Name, await LampArray.FromIdAsync(args.Id));

            // DeviceWatcher events are invoked on a background thread.
            // We need to switch to the foreground thread to update our app UI
            // and access member variables without race conditions.
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                if (info.lampArray == null)
                {
                    // A LampArray was found, but Windows couldn't initialize it.
                    // This suggests a device error.
                    rootPage.NotifyUser($"Problem with LampArray {info.displayName}.", NotifyType.ErrorMessage);
                    return;
                }

                // Set up the AvailabilityChanged event callback for being notified whether this process can control
                // RGB lighting for the newly attached LampArray.
                info.lampArray.AvailabilityChanged += LampArray_AvailabilityChanged;

                // Remember this new LampArray and add it to the list.
                m_attachedLampArrays.Add(info);
                UpdateLampArrayList();
            });
        }

        private async void Watcher_Removed(DeviceWatcher sender, DeviceInformationUpdate args)
        {
            // DeviceWatcher events are invoked on a background thread.
            // We need to switch to the foreground thread to update our app UI
            // and access member variables without race conditions.
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                m_attachedLampArrays.RemoveAll(info => info.id == args.Id);
                UpdateLampArrayList();
            });
        }

        // The AvailabilityChanged event will fire when this calling process gains or loses control of RGB lighting
        // for the specified LampArray.
        private async void LampArray_AvailabilityChanged(LampArray sender, object args)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                UpdateLampArrayList();
            });
        }

        private void UpdateLampArrayList()
        {
            string message = $"Attached LampArrays: {m_attachedLampArrays.Count}\n";
            foreach (LampArrayInfo info in m_attachedLampArrays)
            {
                message += $"\t{info.displayName} ({info.lampArray.LampArrayKind.ToString()}, {info.lampArray.LampCount} lamps, " +
                           $"{(info.lampArray.IsAvailable ? "Available" : "Unavailable")})\n";
            }
            LampArraysSummary.Text = message;
        }

        private void Apply_Clicked(object sender, RoutedEventArgs e)
        {
            foreach (LampArrayInfo info in m_attachedLampArrays)
            {
                ApplySettingsToLampArray(info.lampArray);
            }
        }

        private void ApplySettingsToLampArray(LampArray lampArray)
        {
            // Apply the light pattern.
            if (OffButton.IsChecked.Value)
            {
                // Set all Lamps on the LampArray to black, turning them off.
                lampArray.SetColor(Colors.Black);
            }
            else if (SetColorButton.IsChecked.Value)
            {
                // Set all Lamps on the LampArray to green.
                lampArray.SetColor(Colors.Green);
            }
            else if (GradientButton.IsChecked.Value)
            {
                SetGradientPatternToLampArray(lampArray);
            }
            else if (WasdButton.IsChecked.Value)
            {
                SetWasdPatternToLampArray(lampArray);
            }

            // Apply the brightness. Convert the percentage value in the slider to the range [0, 1] expected by BrightnessLevel.
            lampArray.BrightnessLevel = BrightnessSlider.Value / BrightnessSlider.Maximum;
        }

        private void SetGradientPatternToLampArray(LampArray lampArray)
        {
            // Create a static gradient on the LampArray from black to red, blending from left to right.

            // For this effect, we want to set the colors of all Lamps. Create an array containing all the Lamp indices
            // and an array of Colors. Both arrays must be of the same length. We will use these for SetColorsForIndices later.
            int[] indices = Enumerable.Range(0, lampArray.LampCount).ToArray();
            var colors = new Color[lampArray.LampCount];

            // A LampArray provides information about its size using a three-dimensional logical bounding box,
            // as well as the location of each of its Lamps within that bounding box.
            // The origin point of the bounding box is the leftmost (X), farthest (Y), uppermost (Z) corner relative to the user.
            // LampArray bounding boxes and Lamp positions are measured in meters.

            // Because our gradient will blend from left to right, we will set the Lamp colors according to each Lamp's X position.
            // We will make two passes over the LampArray. First, calculate the minimum and maximum X position values of all Lamps.
            // This is to "trim" the margins of the bounding box where no Lamps are present.
            double maxX = 0;
            double minX = lampArray.BoundingBox.X;

            foreach (int i in indices)
            {
                LampInfo lampInfo = lampArray.GetLampInfo(i);

                if (lampInfo.Position.X > maxX)
                {
                    maxX = lampInfo.Position.X;
                }

                if (lampInfo.Position.X < minX)
                {
                    minX = lampInfo.Position.X;
                }
            }

            // In the second pass, we will calculate the gradient colors for each Lamp.
            foreach (int i in indices)
            {
                LampInfo lampInfo = lampArray.GetLampInfo(i);

                // Calculate the X position for this Lamp relative to the rightmost Lamp. We will scale the Lamp's color by this relative value below.
                // (We also want to avoid dividing by zero here in case all Lamps share the same X position, including the single-Lamp case.)
                double xProgress = 1.0;
                if (maxX != minX)
                {
                    xProgress = (lampInfo.Position.X - minX) / (maxX - minX);
                }

                // Apply that value to the expected color for the lamp.
                // Maximize the Alpha value to give the color full opacity.
                colors[i].A = byte.MaxValue;

                // Set the color's R value based on the Lamp's X position.
                // As the X position of the Lamp increases (i.e., is further to the right),
                // the Red value of the Lamp's color will also increase.
                // This means that the leftmost Lamp(s) will be black (turned off), and the rightmost Lamp(s) will be the brightest red.
                colors[i].R = (byte)(xProgress * byte.MaxValue);
            }

            // Apply the colors to the LampArray.
            lampArray.SetColorsForIndices(colors, indices);
        }

        private void SetWasdPatternToLampArray(LampArray lampArray)
        {
            // Set a background color of blue for the whole LampArray.
            lampArray.SetColor(Colors.Blue);

            // Highlight the WASD keys in white, if the LampArray supports addressing its Lamps using a virtual key to Lamp mapping.
            // This is typically found on keyboard LampArrays. Other LampArrays will not usually support virtual key based lighting.
            if (lampArray.SupportsVirtualKeys)
            {
                Color[] colors = Enumerable.Repeat(Colors.White, 4).ToArray();
                VirtualKey[] virtualKeys = { VirtualKey.W, VirtualKey.A, VirtualKey.S, VirtualKey.D };

                lampArray.SetColorsForKeys(colors, virtualKeys);
            }
        }
    }
}
