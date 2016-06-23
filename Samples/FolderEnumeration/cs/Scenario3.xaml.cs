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
using System.Text;
using Windows.Storage;
using Windows.Storage.BulkAccess;
using Windows.Storage.FileProperties;
using Windows.Storage.Search;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario3 : Page
    {
        public Scenario3()
        {
            this.InitializeComponent();
        }

        const string CopyrightProperty = "System.Copyright";
        const string ColorSpaceProperty = "System.Image.ColorSpace";

        private string GetPropertyDisplayValue(object property)
        {
            string value;
            if (property == null || string.IsNullOrEmpty(property.ToString()))
            {
                value = "none";
            }
            else
            {
                value = property.ToString();
            }

            return value;
        }

        private async void GetFilesButton_Click(object sender, RoutedEventArgs e)
        {
            // Reset output.
            OutputPanel.Children.Clear();

            // Set up file type filter.
            List<string> fileTypeFilter = new List<string>();
            fileTypeFilter.Add(".jpg");
            fileTypeFilter.Add(".png");
            fileTypeFilter.Add(".bmp");
            fileTypeFilter.Add(".gif");

            // Create query options.
            var queryOptions = new QueryOptions(CommonFileQuery.OrderByName, fileTypeFilter);

            // Set up property prefetch - use the PropertyPrefetchOptions for top-level properties
            // and a list for additional properties.
            List<string> propertyNames = new List<string>();
            propertyNames.Add(CopyrightProperty);
            propertyNames.Add(ColorSpaceProperty);
            queryOptions.SetPropertyPrefetch(PropertyPrefetchOptions.ImageProperties, propertyNames);

            // Set up thumbnail prefetch if needed, e.g. when creating a picture gallery view.
            /*
            const uint requestedSize = 190;
            const ThumbnailMode thumbnailMode = ThumbnailMode.PicturesView;
            const ThumbnailOptions thumbnailOptions = ThumbnailOptions.UseCurrentScale;
            queryOptions.SetThumbnailPrefetch(thumbnailMode, requestedSize, thumbnailOptions);
            */

            // Set up the query and retrieve files.
            StorageFolder picturesFolder = await KnownFolders.GetFolderForUserAsync(null /* current user */, KnownFolderId.PicturesLibrary);
            var query = picturesFolder.CreateFileQueryWithOptions(queryOptions);
            IReadOnlyList<StorageFile> fileList = await query.GetFilesAsync();
            foreach (StorageFile file in fileList)
            {
                OutputPanel.Children.Add(CreateHeaderTextBlock(file.Name));

                // GetImagePropertiesAsync will return synchronously when prefetching has been able to
                // retrieve the properties in advance.
                var properties = await file.Properties.GetImagePropertiesAsync();
                OutputPanel.Children.Add(CreateLineItemTextBlock("Dimensions: " + properties.Width + "x" + properties.Height));

                // Similarly, extra properties are retrieved asynchronously but may
                // return immediately when prefetching has fulfilled its task.
                IDictionary<string, object> extraProperties = await file.Properties.RetrievePropertiesAsync(propertyNames);
                var propValue = extraProperties[CopyrightProperty];
                OutputPanel.Children.Add(CreateLineItemTextBlock("Copyright: " + GetPropertyDisplayValue(propValue)));
                propValue = extraProperties[ColorSpaceProperty];
                OutputPanel.Children.Add(CreateLineItemTextBlock("Color space: " + GetPropertyDisplayValue(propValue)));

                // Thumbnails can also be retrieved and used.
                // var thumbnail = await file.GetThumbnailAsync(thumbnailMode, requestedSize, thumbnailOptions);
            }
        }

        TextBlock CreateHeaderTextBlock(string contents)
        {
            TextBlock textBlock = new TextBlock();
            textBlock.Text = contents;
            textBlock.Style = (Style)Application.Current.Resources["SampleHeaderTextStyle"];
            textBlock.TextWrapping = TextWrapping.Wrap;
            return textBlock;
        }

        TextBlock CreateLineItemTextBlock(string contents)
        {
            TextBlock textBlock = new TextBlock();
            textBlock.Text = contents;
            textBlock.Style = (Style)Application.Current.Resources["BasicTextStyle"];
            textBlock.TextWrapping = TextWrapping.Wrap;
            Thickness margin = textBlock.Margin;
            margin.Left = 20;
            textBlock.Margin = margin;
            return textBlock;
        }
    }
}
