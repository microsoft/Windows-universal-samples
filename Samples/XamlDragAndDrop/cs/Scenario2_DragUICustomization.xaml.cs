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

using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Media.Imaging;
using System;
using Windows.Graphics.Imaging;
using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.ApplicationModel.DataTransfer;

namespace DragAndDropSampleManaged
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario2_DragUICustomization : Page
    {
        public Scenario2_DragUICustomization()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Start of the Drag and Drop operation: we set some content and change the DragUI
        /// depending on the selected options
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        private async void SourceGrid_DragStarting(Windows.UI.Xaml.UIElement sender, Windows.UI.Xaml.DragStartingEventArgs args)
        {
            args.Data.SetText(SourceTextBox.Text);
            if ((bool)DataPackageRB.IsChecked)
            {
                // Standard icon will be used as the DragUIContent
                args.DragUI.SetContentFromDataPackage();
            }
            else if ((bool)CustomContentRB.IsChecked)
            {
                // Generate a bitmap with only the TextBox
                // We need to take the deferral as the rendering won't be completed synchronously
                var deferral = args.GetDeferral();
                var rtb = new RenderTargetBitmap();
                await rtb.RenderAsync(SourceTextBox);
                var buffer = await rtb.GetPixelsAsync();
                var bitmap = SoftwareBitmap.CreateCopyFromBuffer(buffer,
                    BitmapPixelFormat.Bgra8,
                    rtb.PixelWidth,
                    rtb.PixelHeight,
                    BitmapAlphaMode.Premultiplied);
                args.DragUI.SetContentFromSoftwareBitmap(bitmap);
                deferral.Complete();
            }
            // else just show the dragged UIElement
        }

        /// <summary>
        /// Entering the Target, we'll change its background and optionally change the DragUI as well
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void TargetTextBox_DragEnter(object sender, Windows.UI.Xaml.DragEventArgs e)
        {
            /// Change the background of the target
            VisualStateManager.GoToState(this, "Inside", true);
            bool hasText = e.DataView.Contains(StandardDataFormats.Text);
            e.AcceptedOperation = hasText ? DataPackageOperation.Copy : DataPackageOperation.None;
            if (hasText)
            {
                e.DragUIOverride.Caption = "Drop here to insert text";
                // Now customize the content
                if ((bool)HideRB.IsChecked)
                {
                    e.DragUIOverride.IsGlyphVisible = false;
                    e.DragUIOverride.IsContentVisible = false;
                }
                else if ((bool)CustomRB.IsChecked)
                {
                    var bitmap = new BitmapImage(new Uri("ms-appx:///Assets/dropcursor.png", UriKind.RelativeOrAbsolute));
                    // Anchor will define how to position the image relative to the pointer
                    Point anchor = new Point(0,52); // lower left corner of the image
                    e.DragUIOverride.SetContentFromBitmapImage(bitmap, anchor);
                    e.DragUIOverride.IsGlyphVisible = false;
                    e.DragUIOverride.IsCaptionVisible = false;
                }
                // else keep the DragUI Content set by the source
            }
        }

        /// <summary>
        /// DragLeave: Restore previous background
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void TargetTextBox_DragLeave(object sender, Windows.UI.Xaml.DragEventArgs e)
        {
            VisualStateManager.GoToState(this, "Outside", true);
        }
        
        /// <summary>
        /// Drop: restore the background and append the dragged text
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void TargetTextBox_Drop(object sender, Windows.UI.Xaml.DragEventArgs e)
        {
            VisualStateManager.GoToState(this, "Outside", true);
            bool hasText = e.DataView.Contains(StandardDataFormats.Text);
            // if the result of the drop is not too important (and a text copy should have no impact on source)
            // we don't need to take the deferral and this will complete the operation faster
            e.AcceptedOperation = hasText ? DataPackageOperation.Copy : DataPackageOperation.None;
            if (hasText)
            {
                var text = await e.DataView.GetTextAsync();
                TargetTextBox.Text += text;
            }
        }
    }
}
