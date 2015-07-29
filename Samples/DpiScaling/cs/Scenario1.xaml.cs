//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.Collections.Generic;
using Windows.Graphics.Display;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Xaml.Media.Imaging;

namespace SDKTemplate
{
    public sealed partial class Scenario1 : Page
    {
        private const String imageBase = "http://www.contoso.com/imageScale{0}.png";

        public Scenario1()
        {
            this.InitializeComponent();
            DisplayInformation displayInformation = DisplayInformation.GetForCurrentView();
            displayInformation.DpiChanged += DisplayProperties_DpiChanged;
        }

        private void ResetOutput()
        {
            DisplayInformation displayInformation = DisplayInformation.GetForCurrentView();
            String scaleValue = (displayInformation.RawPixelsPerViewPixel * 100.0).ToString("F0");
            ScalingText.Text = scaleValue + "%";
            ManualLoadURL.Text = String.Format(imageBase, scaleValue);
            LogicalDPIText.Text = displayInformation.LogicalDpi.ToString() + " DPI";
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            ResetOutput();
        }

        void DisplayProperties_DpiChanged(DisplayInformation sender, object args)
        {
            ResetOutput();
        }
    }
}
