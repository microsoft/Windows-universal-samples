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
using Windows.ApplicationModel.DataTransfer;
using Windows.Graphics.Imaging;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class HistoryAndRoaming : Page
    {
        MainPage rootPage = MainPage.Current;

        public HistoryAndRoaming()
        {
            InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            Clipboard.HistoryEnabledChanged += OnHistoryEnabledChanged;
            Clipboard.RoamingEnabledChanged += OnRoamingEnabledChanged;
            CheckHistoryAndRoaming();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            Clipboard.HistoryEnabledChanged -= OnHistoryEnabledChanged;
            Clipboard.RoamingEnabledChanged -= OnRoamingEnabledChanged;
        }

        async void CopyButton_Click(object sender, RoutedEventArgs e)
        {
            // Generate a bitmap for our clipboard content.
            RenderTargetBitmap target = new RenderTargetBitmap();
            await target.RenderAsync(ClipboardContent);
            IBuffer buffer = await target.GetPixelsAsync();
            SoftwareBitmap bitmap = SoftwareBitmap.CreateCopyFromBuffer(buffer, BitmapPixelFormat.Bgra8, target.PixelWidth, target.PixelHeight, BitmapAlphaMode.Premultiplied);

            // Save the bitmap to a stream.
            InMemoryRandomAccessStream stream = new InMemoryRandomAccessStream();
            BitmapEncoder encoder = await BitmapEncoder.CreateAsync(BitmapEncoder.PngEncoderId, stream);
            encoder.SetSoftwareBitmap(bitmap);
            await encoder.FlushAsync();
            stream.Seek(0);

            // Create a DataPackage to hold the bitmap and text.
            var dataPackage = new DataPackage();
            dataPackage.SetBitmap(RandomAccessStreamReference.CreateFromStream(stream));
            dataPackage.SetText(ClipboardContent.Text);

            ClipboardContentOptions options = new ClipboardContentOptions();

            // Customize clipboard history.
            if (IncludeAllInHistory.IsChecked.Value)
            {
                // This is the default, but set it explicitly for expository purposes.
                options.IsAllowedInHistory = true;
            }
            else if (IncludeTextInHistory.IsChecked.Value)
            {
                // This is the default, but set it explicitly for expository purposes.
                options.IsAllowedInHistory = true;
                // Allow only text to go into the history.
                options.HistoryFormats.Add(StandardDataFormats.Text);
            }
            else
            {
                // Exclude from history.
                options.IsAllowedInHistory = false;
            }

            // Customize clipboard roaming.
            if (AllowAllToRoam.IsChecked.Value)
            {
                // This is the default, but set it explicitly for expository purposes.
                options.IsRoamable = true;
            }
            else if (AllowTextToRoam.IsChecked.Value)
            {
                // This is the default, but set it explicitly for expository purposes.
                options.IsRoamable = true;
                // Allow only text to roam.
                options.RoamingFormats.Add(StandardDataFormats.Text);
            }
            else
            {
                // Exclude from roaming.
                options.IsRoamable = false;
            }

            // Set the data package to the clipboard with our custom options.
            if (Clipboard.SetContentWithOptions(dataPackage, options))
            {
                rootPage.NotifyUser("Text and bitmap have been copied to clipboard.", NotifyType.StatusMessage);
            }
            else
            {
                // Copying data to Clipboard can potentially fail - for example, if another application is holding Clipboard open
                rootPage.NotifyUser("Error copying content to clipboard.", NotifyType.ErrorMessage);
            }
        }

        void OnHistoryEnabledChanged(object sender, object e)
        {
            CheckHistoryAndRoaming();
        }

        void OnRoamingEnabledChanged(object sender, object e)
        {
            CheckHistoryAndRoaming();
        }

        static void SelectVisibleElement(UIElement ifTrue, UIElement ifFalse, bool value)
        {
            ifTrue.Visibility = value ? Visibility.Visible : Visibility.Collapsed;
            ifFalse.Visibility = !value ? Visibility.Visible : Visibility.Collapsed;
        }

        async void CheckHistoryAndRoaming()
        {
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                SelectVisibleElement(ClipboardHistoryBlock, ClipboardHistoryUnavailableBlock, Clipboard.IsHistoryEnabled());
                SelectVisibleElement(ClipboardRoamingBlock, ClipboardRoamingUnavailableBlock, Clipboard.IsRoamingEnabled());
            });
        }
    }
}
