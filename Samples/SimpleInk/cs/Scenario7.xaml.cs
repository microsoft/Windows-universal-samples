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
    ///   This scenario demonstrates how to work with InkPresenterStencil objects in a ScrollViewer.
    ///   -- Repositioning the InkPresenterStencil on demand
    ///   -- Integrating a custom button with stencil functionality alongside the InkToolbar
    /// </summary>
    public sealed partial class Scenario7 : Page
    {
        public Scenario7()
        {
            this.InitializeComponent();

            inkCanvas.InkPresenter.InputDeviceTypes = CoreInputDeviceTypes.Mouse | CoreInputDeviceTypes.Pen;

            // Customize the ruler
            var ruler = new InkPresenterRuler(inkCanvas.InkPresenter);
            ruler.BackgroundColor = Windows.UI.Colors.PaleTurquoise;
            ruler.ForegroundColor = Windows.UI.Colors.MidnightBlue;
            ruler.Length = 800;
            ruler.AreTickMarksVisible = false;
            ruler.IsCompassVisible = false;

            // Customize the protractor
            var protractor = new InkPresenterProtractor(inkCanvas.InkPresenter);
            protractor.BackgroundColor = Windows.UI.Colors.Bisque;
            protractor.ForegroundColor = Windows.UI.Colors.DarkGreen;
            protractor.AccentColor = Windows.UI.Colors.Firebrick;
            protractor.AreRaysVisible = false;
            protractor.AreTickMarksVisible = false;
            protractor.IsAngleReadoutVisible = false;
            protractor.IsCenterMarkerVisible = false;
        }

        private void InkToolbar_IsStencilButtonCheckedChanged(InkToolbar sender, InkToolbarIsStencilButtonCheckedChangedEventArgs args)
        {
            var stencilButton = (InkToolbarStencilButton)inkToolbar.GetMenuButton(InkToolbarMenuKind.Stencil);
            BringIntoViewButton.IsEnabled = stencilButton.IsChecked.Value;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Make the ink canvas larger than the window, so that we can demonstrate
            // scrolling and zooming.
            inkCanvas.Width = Window.Current.Bounds.Width * 2;
            inkCanvas.Height = Window.Current.Bounds.Height * 2;
        }

        void OnBringIntoView(object sender, RoutedEventArgs e)
        {
            // Set stencil origin to Scrollviewer Viewport origin.
            // The purpose of this behavior is to allow the user to "grab" the
            // stencil and bring it into view no matter where the scrollviewer viewport
            // happens to be.  Note that this is accomplished by a simple translation
            // that adjusts to the zoom factor.  The additional ZoomFactor term is to
            // ensure the scale of the InkPresenterStencil is invariant to Zoom.
            Matrix3x2 viewportTransform =
                Matrix3x2.CreateScale(ScrollViewer.ZoomFactor) *
                Matrix3x2.CreateTranslation(
                   (float)ScrollViewer.HorizontalOffset,
                   (float)ScrollViewer.VerticalOffset) *
                Matrix3x2.CreateScale(1.0f / ScrollViewer.ZoomFactor);

            var stencilButton = (InkToolbarStencilButton)inkToolbar.GetMenuButton(InkToolbarMenuKind.Stencil);
            switch (stencilButton.SelectedStencil)
            {
                case InkToolbarStencilKind.Protractor:
                    stencilButton.Protractor.Transform = viewportTransform;
                    break;

                case InkToolbarStencilKind.Ruler:
                    stencilButton.Ruler.Transform = viewportTransform;
                    break;
            }
        }
    }
}
