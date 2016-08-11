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

using Windows.Foundation;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.Input.Inking;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace SDKTemplate
{
    /// <summary>
    /// This page shows the code to control multiple InkCanvas controls with only one Inktoolbar.
    /// </summary>
    public sealed partial class Scenario6 : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario6()
        {
            this.InitializeComponent();

            // Initialize the InkCanvas controls
            inkCanvas.InkPresenter.InputDeviceTypes =
            inkCanvas2.InkPresenter.InputDeviceTypes = CoreInputDeviceTypes.Mouse | CoreInputDeviceTypes.Pen;

            inkCanvas.InkPresenter.UnprocessedInput.PointerEntered += UnprocessedInput_PointerEntered;
            inkCanvas2.InkPresenter.UnprocessedInput.PointerEntered += UnprocessedInput_PointerEntered1;
        }

        private void UnprocessedInput_PointerEntered(InkUnprocessedInput sender, PointerEventArgs args)
        {
            border1.BorderBrush = new SolidColorBrush(Colors.Blue);
            border2.BorderBrush = new SolidColorBrush(Colors.Gray);
            inkToolbar.TargetInkCanvas = inkCanvas;
        }

        private void UnprocessedInput_PointerEntered1(InkUnprocessedInput sender, PointerEventArgs args)
        {
            border1.BorderBrush = new SolidColorBrush(Colors.Gray);
            border2.BorderBrush = new SolidColorBrush(Colors.Blue);
            inkToolbar.TargetInkCanvas = inkCanvas2;
        }

        private void OnSizeChanged(object sender, SizeChangedEventArgs e)
        {
            SetCanvasSize();
        }

        private void SetCanvasSize()
        {
            outputGrid.Width = RootGrid.ActualWidth / 2;
            outputGrid.Height = RootGrid.ActualHeight / 2;
            inkCanvas.Width = RootGrid.ActualWidth / 2;
            inkCanvas.Height = RootGrid.ActualHeight / 2;
            outputGrid2.Width = RootGrid.ActualWidth / 2;
            outputGrid2.Height = RootGrid.ActualHeight / 2;
            inkCanvas2.Width = RootGrid.ActualWidth / 2;
            inkCanvas2.Height = RootGrid.ActualHeight / 2;
        }

        private void ToggleLogs(object sender, RoutedEventArgs e)
        {
            if (toggleLog.IsOn)
            {
                LogBorder.Visibility = Visibility.Visible;
                inkToolbar.ActiveToolChanged += InkToolbar_ActiveToolChanged;
                inkToolbar.InkDrawingAttributesChanged += InkToolbar_InkDrawingAttributesChanged;
                inkToolbar.EraseAllClicked += InkToolbar_EraseAllClicked;
                inkToolbar.IsRulerButtonCheckedChanged += InkToolbar_IsRulerButtonCheckedChanged;
            }
            else
            {
                LogBorder.Visibility = Visibility.Collapsed;
                textLogs.Text = string.Empty;
                inkToolbar.ActiveToolChanged -= InkToolbar_ActiveToolChanged;
                inkToolbar.InkDrawingAttributesChanged -= InkToolbar_InkDrawingAttributesChanged;
                inkToolbar.EraseAllClicked -= InkToolbar_EraseAllClicked;
                inkToolbar.IsRulerButtonCheckedChanged -= InkToolbar_IsRulerButtonCheckedChanged;

            }
        }

        private void InkToolbar_IsRulerButtonCheckedChanged(InkToolbar sender, object args)
        {
            textLogs.Text = "IsRulerButtonCheckedChanged\n" + textLogs.Text;
        }

        private void InkToolbar_EraseAllClicked(InkToolbar sender, object args)
        {
            textLogs.Text = "EraseAllClicked\n" + textLogs.Text;
        }

        private void InkToolbar_InkDrawingAttributesChanged(InkToolbar sender, object args)
        {
            textLogs.Text = "InkDrawingAttributesChanged\n" + textLogs.Text;
        }

        private void InkToolbar_ActiveToolChanged(InkToolbar sender, object args)
        {
            textLogs.Text = "ActiveToolChanged\n" + textLogs.Text;
        }
    }
}