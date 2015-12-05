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
using System.Diagnostics;
using System.Text;
using Windows.Storage;
using Windows.Storage.BulkAccess;
using Windows.Storage.FileProperties;
using Windows.Storage.Search;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
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
            int max_files = 100000000;
            if ( MaxFilesText.Text.Length > 0 )
            {
                var good = int.TryParse(MaxFilesText.Text, out max_files);
            }

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
            queryOptions.FolderDepth = FolderDepth.Deep;

            // Set up property prefetch - use the PropertyPrefetchOptions for top-level properties
            // and a list for additional properties.
            List<string> propertyNames = new List<string>();
            propertyNames.Add(CopyrightProperty);
            propertyNames.Add(ColorSpaceProperty);
            queryOptions.SetPropertyPrefetch(PropertyPrefetchOptions.ImageProperties, propertyNames);

            // Set up thumbnail prefetch if needed, e.g. when creating a picture gallery view.
            const uint requestedSize = 190;
            const ThumbnailMode thumbnailMode = ThumbnailMode.PicturesView;
            const ThumbnailOptions thumbnailOptions = ThumbnailOptions.UseCurrentScale;

            if (PrefetchCheck.IsChecked == true)
            {
                queryOptions.SetThumbnailPrefetch(thumbnailMode, requestedSize, thumbnailOptions);
            }

            // Set up the query and retrieve files.
            StorageFolder testFolder     = KnownFolders.PicturesLibrary;
//            testFolder = await testFolder.GetFolderAsync("EnumTest");

            var query = testFolder.CreateItemQueryWithOptions(queryOptions);

            ScanProgressText.Text = "scanning...";

            // Do the scan a bit at a time to help us discern where we fail if we do.
            List<IStorageItem> fileList = new List<IStorageItem>();

            uint start_index = 0;
            int list_count = 0;
            do
            {
                var tmp_list = await query.GetItemsAsync(start_index, 100);
                list_count = tmp_list.Count;
                if ( list_count > 0 )
                {
                    // quick copy the tmp_list into the master list
                    fileList.AddRange(tmp_list);
                    start_index += (uint)list_count;

                    ScanProgressText.Text = String.Format("found {0} files", fileList.Count);
                }
            } while (( list_count > 0) && ( fileList.Count < max_files )) ;
            // this check can be changed to shorten the test case

            List<IStorageItem> badItems = new List<IStorageItem>();
            List<IStorageItem> goodItems = new List<IStorageItem>();

            StorageItemThumbnail tmp_thumb;

            foreach (IStorageItem file in fileList)
            {
                OutputPanel.Children.Add(CreateHeaderTextBlock(file.Name));

                // GetImagePropertiesAsync will return synchronously when prefetching has been able to
                // retrieve the properties in advance.
                var onlyFile = file as StorageFile;
                var onlyFold = file as StorageFolder;
                if (onlyFile != null)
                {
                    var properties = await onlyFile.Properties.GetImagePropertiesAsync();
                    OutputPanel.Children.Add(CreateLineItemTextBlock("Dimensions: " + properties.Width + "x" + properties.Height));

                    // Similarly, extra properties are retrieved asynchronously but may
                    // return immediately when prefetching has fulfilled its task.
                    IDictionary<string, object> extraProperties = await onlyFile.Properties.RetrievePropertiesAsync(propertyNames);
                    var propValue = extraProperties[CopyrightProperty];
                    OutputPanel.Children.Add(CreateLineItemTextBlock("Copyright: " + GetPropertyDisplayValue(propValue)));
                    propValue = extraProperties[ColorSpaceProperty];
                    OutputPanel.Children.Add(CreateLineItemTextBlock("Color space: " + GetPropertyDisplayValue(propValue)));

#if true
                    // Thumbnails can also be retrieved and used.
                    using (tmp_thumb = await onlyFile.GetThumbnailAsync(thumbnailMode, requestedSize, thumbnailOptions))
                    {
                        if (tmp_thumb != null)
                        {
                            var img = CreateThumbnailImage(tmp_thumb);
                            OutputPanel.Children.Add(img);

                            goodItems.Add(file);
                        }
                        else
                        {
                            badItems.Add(file);
                        }
                    }
#endif
                }
                else if ( onlyFold != null )
                {
                    // Thumbnails can also be retrieved and used.
                    using (tmp_thumb = await onlyFold.GetThumbnailAsync(thumbnailMode, requestedSize, thumbnailOptions))
                    {
                        if (tmp_thumb != null)
                        {
                            var img = CreateThumbnailImage(tmp_thumb);
                            OutputPanel.Children.Add(img);
                            goodItems.Add(file);
                        }
                        else
                        {
                            badItems.Add(file);
                            Debug.WriteLine(String.Format("thumb for {0} is null", onlyFold.Name));
                        }
                    }
                }
            }

            Debug.WriteLine(String.Format("we found {0} good items, {1} bad ones"
                           , goodItems.Count
                           , badItems.Count));

            // rescan folders.  This wouldn't happen this way, but would be similar in the case of a 
            // virtual data source.
            foreach ( IStorageItem file in goodItems)
            {
                var tmpFold = file as StorageFolder;
                var tmpFile = file as StorageFile;
                if (tmpFold != null)
                {
                    // should be no problem
                    try
                    {
                        using (var thumbnail = await tmpFold.GetThumbnailAsync(thumbnailMode, requestedSize, thumbnailOptions))
                        {
                            if (thumbnail == null)
                            {
                                Debug.WriteLine(String.Format("folder {0} failed on subsequent scan", file.Name));
                            }
                        }
                    }
                    catch( Exception except )
                    {
                        Debug.WriteLine(String.Format("folder {0} exception \"{1}\"", file.Name, except.Message));
                    }
                }
                else if ( tmpFile != null )
                {
                    // should be no problem
                    try
                    {
                        using (var thumbnail = await tmpFile.GetThumbnailAsync(thumbnailMode, requestedSize, thumbnailOptions))
                        {
                            if (thumbnail == null)
                            {
                                Debug.WriteLine(String.Format("file {0} failed on subsequent scan", file.Name));
                            }
                        }
                    }
                    catch ( Exception except )
                    {
                        Debug.WriteLine(String.Format("file {0} exception \"{1}\"", file.Name, except.Message));
                    }
                }
            }
        }

        Image CreateThumbnailImage(StorageItemThumbnail thumbnail)
        {
            var image = new Image();
            var bm = new BitmapImage();
            bm.SetSource(thumbnail);
            image.Source = bm;
            image.Width = 190;
            return image;
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
