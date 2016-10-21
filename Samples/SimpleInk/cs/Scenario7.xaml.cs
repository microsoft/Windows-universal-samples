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

using System.Numerics;
using Windows.UI.Core;
using Windows.UI.Input.Inking;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    ///   This scenario demonstrates how to work with InkPresenterRuler in a ScrollViewer.
    ///   -- Repositioning the InkPresenterRuler on demand
    ///   -- Integrating a custom button with ruler functionality alongside the InkToolbar
    /// </summary>
    public sealed partial class Scenario7 : Page
    {
        private MainPage rootPage = MainPage.Current;

        InkPresenterRuler ruler;

        public Scenario7()
        {
            this.InitializeComponent();

            InkCanvas.InkPresenter.InputDeviceTypes = CoreInputDeviceTypes.Mouse | CoreInputDeviceTypes.Pen;

            ruler = new InkPresenterRuler(InkCanvas.InkPresenter);

            // Customize Ruler
            ruler.BackgroundColor = Windows.UI.Colors.PaleTurquoise;
            ruler.ForegroundColor = Windows.UI.Colors.MidnightBlue;
            ruler.Length = 800;
        }

        private void InkToolbar_IsRulerButtonCheckedChanged(InkToolbar sender, object args)
        {
            var rulerButton = (InkToolbarRulerButton)InkToolbar.GetToggleButton(InkToolbarToggle.Ruler);
            BringIntoViewButton.IsEnabled = rulerButton.IsChecked.Value;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Make the ink canvas larger than the window, so that we can demonstrate
            // scrolling and zooming.
            InkCanvas.Width = Window.Current.Bounds.Width * 2;
            InkCanvas.Height = Window.Current.Bounds.Height * 2;
        }

        void OnBringIntoView(object sender, RoutedEventArgs e)
        {
            // Set Ruler Origin to Scrollviewer Viewport origin.
            // The purpose of this behavior is to allow the user to "grab" the
            // ruler and bring it into view no matter where the scrollviewer viewport
            // happens to be.  Note that this is accomplished by a simple translation
            // that adjusts to the zoom factor.  The additional ZoomFactor term is to
            // make ensure the scale of the InkPresenterRuler is invariant to Zoom.
            Matrix3x2 viewportTransform =
                Matrix3x2.CreateScale(ScrollViewer.ZoomFactor) *
                Matrix3x2.CreateTranslation(
                   (float)ScrollViewer.HorizontalOffset,
                   (float)ScrollViewer.VerticalOffset) *
                Matrix3x2.CreateScale(1.0f / ScrollViewer.ZoomFactor);

            ruler.Transform = viewportTransform;
        }
    }
}