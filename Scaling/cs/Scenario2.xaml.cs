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
using Windows.Graphics.Display;
using Windows.UI.Text;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario2 : Page
    {
        public Scenario2()
        {
            InitializeComponent();
            DisplayInformation displayInformation = DisplayInformation.GetForCurrentView();
            displayInformation.DpiChanged += DisplayProperties_DpiChanged;
            DefaultLayoutText.FontSize = PxFromPt(20);  // xaml fontsize is in pixels.
        }

        // Helpers to convert between points and pixels.
        double PtFromPx(double pixel)
        {
            return pixel * 72 / 96;
        }

        double PxFromPt(double pt)
        {
            return pt * 96 / 72;
        }

        void OutputSettings(double rawPixelsPerViewPixel, FrameworkElement rectangle, TextBlock viewPxText, TextBlock rawPxText, TextBlock fontTextBlock)
        {
            // Get the size of the rectangle in view pixels and calulate the size in raw pixels.
            double sizeInViewPx = rectangle.Width;
            double sizeInRawPx = sizeInViewPx * rawPixelsPerViewPixel;

            viewPxText.Text = sizeInViewPx.ToString("F1") + " view px";
            rawPxText.Text = sizeInRawPx.ToString("F0") + " raw px";

            double fontSize = PtFromPx(fontTextBlock.FontSize);
            fontTextBlock.Text = fontSize.ToString("F0") + "pt";
        }

        void ResetOutput()
        {
            ResolutionTextBlock.Text = Window.Current.Bounds.Width.ToString("F1") + "x" + Window.Current.Bounds.Height.ToString("F1");

            DisplayInformation displayInformation = DisplayInformation.GetForCurrentView();
            double rawPixelsPerViewPixel = displayInformation.RawPixelsPerViewPixel;

            // Set the override rectangle size and override text font size by taking our desired
            // size in raw pixels and converting it to view pixels.
            const double rectSizeInRawPx = 100;
            double rectSizeInViewPx = rectSizeInRawPx / rawPixelsPerViewPixel;
            OverrideLayoutRect.Width = rectSizeInViewPx;
            OverrideLayoutRect.Height = rectSizeInViewPx;

            double fontSizeInRawPx = PxFromPt(20);
            double fontSizeInViewPx = fontSizeInRawPx / rawPixelsPerViewPixel;
            OverrideLayoutText.FontSize = fontSizeInViewPx;

            // Output settings for controls with default scaling behavior.
            OutputSettings(rawPixelsPerViewPixel, DefaultLayoutRect, DefaultRelativePx, DefaultPhysicalPx, DefaultLayoutText);
            // Output settings for override controls.
            OutputSettings(rawPixelsPerViewPixel, OverrideLayoutRect, OverrideRelativePx, OverridePhysicalPx, OverrideLayoutText);
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
