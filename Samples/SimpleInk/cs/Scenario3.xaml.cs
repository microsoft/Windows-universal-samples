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
using Windows.Foundation;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.UI.Core;
using Windows.UI.Input.Inking;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    /// <summary>
    /// This page demonstrates the usage of the InkPresenter APIs. It shows the following functionality:
    /// - Load/Save ink files
    /// - Usage of drawing attributes
    /// - Input type switching to enable/disable touch
    /// - Pen tip transform, highlighter, pencil, and different pen colors and sizes
    /// </summary>
    public sealed partial class Scenario3 : Page
    {
        private MainPage rootPage = MainPage.Current;

        const int minPenSize = 2;
        const int penSizeIncrement = 2;
        int penSize;

        public Scenario3()
        {
            this.InitializeComponent();

            penSize = minPenSize + penSizeIncrement * PenThickness.SelectedIndex;

            InkDrawingAttributes drawingAttributes = new InkDrawingAttributes();
            drawingAttributes.Color = Windows.UI.Colors.Red;
            drawingAttributes.Size = new Size(penSize, penSize);
            drawingAttributes.IgnorePressure = false;
            drawingAttributes.FitToCurve = true;

            inkCanvas.InkPresenter.UpdateDefaultDrawingAttributes(drawingAttributes);
            inkCanvas.InkPresenter.InputDeviceTypes = CoreInputDeviceTypes.Mouse | CoreInputDeviceTypes.Pen;
            inkCanvas.InkPresenter.StrokesCollected += InkPresenter_StrokesCollected;
            inkCanvas.InkPresenter.StrokesErased += InkPresenter_StrokesErased;
        }

        private void OnSizeChanged(object sender, SizeChangedEventArgs e)
        {
            HelperFunctions.UpdateCanvasSize(RootGrid, outputGrid, inkCanvas);
        }


        private void InkPresenter_StrokesErased(InkPresenter sender, InkStrokesErasedEventArgs args)
        {
            rootPage.NotifyUser(args.Strokes.Count + " stroke(s) erased!", NotifyType.StatusMessage);
        }

        private void InkPresenter_StrokesCollected(InkPresenter sender, InkStrokesCollectedEventArgs args)
        {
            rootPage.NotifyUser(args.Strokes.Count + " stroke(s) collected!", NotifyType.StatusMessage);
        }

        void OnPenColorChanged(object sender, RoutedEventArgs e)
        {
            if (inkCanvas != null)
            {
                InkDrawingAttributes drawingAttributes = inkCanvas.InkPresenter.CopyDefaultDrawingAttributes();

                // Use button's background to set new pen's color
                var btnSender = sender as Button;
                var brush = btnSender.Background as Windows.UI.Xaml.Media.SolidColorBrush;

                drawingAttributes.Color = brush.Color;
                inkCanvas.InkPresenter.UpdateDefaultDrawingAttributes(drawingAttributes);
            }
        }

        void OnPenThicknessChanged(object sender, RoutedEventArgs e)
        {
            if (inkCanvas != null)
            {
                InkDrawingAttributes drawingAttributes = inkCanvas.InkPresenter.CopyDefaultDrawingAttributes();
                penSize = minPenSize + penSizeIncrement * PenThickness.SelectedIndex;
                string value = ((ComboBoxItem)PenType.SelectedItem).Content.ToString();
                if (value == "Highlighter" || value == "Calligraphy")
                {
                    // Make the pen tip rectangular for highlighter and calligraphy pen
                    drawingAttributes.Size = new Size(penSize, penSize * 2);
                }
                else
                {
                    // Otherwise, use a square pen tim.
                    drawingAttributes.Size = new Size(penSize, penSize);
                }
                inkCanvas.InkPresenter.UpdateDefaultDrawingAttributes(drawingAttributes);
            }
        }

        void OnPenTypeChanged(object sender, RoutedEventArgs e)
        {
            if (inkCanvas != null)
            {
                InkDrawingAttributes drawingAttributes = inkCanvas.InkPresenter.CopyDefaultDrawingAttributes();
                string value = ((ComboBoxItem)PenType.SelectedItem).Content.ToString();

                if (value == "Ballpoint")
                {
                    if (drawingAttributes.Kind != InkDrawingAttributesKind.Default)
                    {
                        var newDrawingAttributes = new InkDrawingAttributes();
                        newDrawingAttributes.Color = drawingAttributes.Color;
                        drawingAttributes = newDrawingAttributes;
                    }
                    drawingAttributes.Size = new Size(penSize, penSize);
                    drawingAttributes.PenTip = PenTipShape.Circle;
                    drawingAttributes.DrawAsHighlighter = false;
                    drawingAttributes.PenTipTransform = System.Numerics.Matrix3x2.Identity;
                }
                else if (value == "Highlighter")
                {
                    if (drawingAttributes.Kind != InkDrawingAttributesKind.Default)
                    {
                        var newDrawingAttributes = new InkDrawingAttributes();
                        newDrawingAttributes.Color = drawingAttributes.Color;
                        drawingAttributes = newDrawingAttributes;
                    }
                    // Make the pen rectangular for highlighter
                    drawingAttributes.Size = new Size(penSize, penSize * 2);
                    drawingAttributes.PenTip = PenTipShape.Rectangle;
                    drawingAttributes.DrawAsHighlighter = true;
                    drawingAttributes.PenTipTransform = System.Numerics.Matrix3x2.Identity;
                }
                else if (value == "Calligraphy")
                {
                    if (drawingAttributes.Kind != InkDrawingAttributesKind.Default)
                    {
                        var newDrawingAttributes = new InkDrawingAttributes();
                        newDrawingAttributes.Color = drawingAttributes.Color;
                        drawingAttributes = newDrawingAttributes;
                    }
                    drawingAttributes.Size = new Size(penSize, penSize * 2);
                    drawingAttributes.PenTip = PenTipShape.Rectangle;
                    drawingAttributes.DrawAsHighlighter = false;

                    // Set a 45 degree rotation on the pen tip
                    double radians = 45.0 * Math.PI / 180;
                    drawingAttributes.PenTipTransform = System.Numerics.Matrix3x2.CreateRotation((float)radians);
                }
                else if (value == "Windows.UI.Xaml.Controls.TextBlock")
                {
                    if (drawingAttributes.Kind != InkDrawingAttributesKind.Pencil)
                    {
                        var newDrawingAttributes = InkDrawingAttributes.CreateForPencil();
                        newDrawingAttributes.Color = drawingAttributes.Color;
                        newDrawingAttributes.Size = drawingAttributes.Size;
                        drawingAttributes = newDrawingAttributes;
                    }
                }
                inkCanvas.InkPresenter.UpdateDefaultDrawingAttributes(drawingAttributes);
            }
        }

        void OnClear(object sender, RoutedEventArgs e)
        {
            inkCanvas.InkPresenter.StrokeContainer.Clear();
            rootPage.NotifyUser("Cleared Canvas", NotifyType.StatusMessage);
        }

        async void OnSaveAsync(object sender, RoutedEventArgs e)
        {
            // We don't want to save an empty file
            if (inkCanvas.InkPresenter.StrokeContainer.GetStrokes().Count > 0)
            {
                var savePicker = new Windows.Storage.Pickers.FileSavePicker();
                savePicker.SuggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.PicturesLibrary;
                savePicker.FileTypeChoices.Add("Gif with embedded ISF", new System.Collections.Generic.List<string> { ".gif" });

                Windows.Storage.StorageFile file = await savePicker.PickSaveFileAsync();
                if (null != file)
                {
                    try
                    {
                        using (IRandomAccessStream stream = await file.OpenAsync(FileAccessMode.ReadWrite))
                        {
                            await inkCanvas.InkPresenter.StrokeContainer.SaveAsync(stream);
                        }
                        rootPage.NotifyUser(inkCanvas.InkPresenter.StrokeContainer.GetStrokes().Count + " stroke(s) saved!", NotifyType.StatusMessage);
                    }
                    catch (Exception ex)
                    {
                        rootPage.NotifyUser(ex.Message, NotifyType.ErrorMessage);
                    }
                }
            }
            else
            {
                rootPage.NotifyUser("There is no ink to save.", NotifyType.ErrorMessage);
            }
        }

        async void OnLoadAsync(object sender, RoutedEventArgs e)
        {
            var openPicker = new Windows.Storage.Pickers.FileOpenPicker();
            openPicker.SuggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.PicturesLibrary;
            openPicker.FileTypeFilter.Add(".gif");
            openPicker.FileTypeFilter.Add(".isf");
            Windows.Storage.StorageFile file = await openPicker.PickSingleFileAsync();
            if (null != file)
            {
                using (var stream = await file.OpenSequentialReadAsync())
                {
                    try
                    {
                        await inkCanvas.InkPresenter.StrokeContainer.LoadAsync(stream);
                    }
                    catch (Exception ex)
                    {
                        rootPage.NotifyUser(ex.Message, NotifyType.ErrorMessage);
                    }
                }

                rootPage.NotifyUser(inkCanvas.InkPresenter.StrokeContainer.GetStrokes().Count + " stroke(s) loaded!", NotifyType.StatusMessage);
            }
        }

        private void TouchInkingCheckBox_Checked(object sender, RoutedEventArgs e)
        {
            inkCanvas.InkPresenter.InputDeviceTypes |= CoreInputDeviceTypes.Touch;
            rootPage.NotifyUser("Enable Touch Inking", NotifyType.StatusMessage);
        }

        private void TouchInkingCheckBox_Unchecked(object sender, RoutedEventArgs e)
        {
            inkCanvas.InkPresenter.InputDeviceTypes &= ~CoreInputDeviceTypes.Touch;
            rootPage.NotifyUser("Disable Touch Inking", NotifyType.StatusMessage);
        }

        private void ErasingModeCheckBox_Checked(object sender, RoutedEventArgs e)
        {
            inkCanvas.InkPresenter.InputProcessingConfiguration.Mode = InkInputProcessingMode.Erasing;
            rootPage.NotifyUser("Enable Erasing Mode", NotifyType.StatusMessage);
        }

        private void ErasingModeCheckBox_Unchecked(object sender, RoutedEventArgs e)
        {
            inkCanvas.InkPresenter.InputProcessingConfiguration.Mode = InkInputProcessingMode.Inking;
            rootPage.NotifyUser("Disable Erasing Mode", NotifyType.StatusMessage);
        }
    }
}