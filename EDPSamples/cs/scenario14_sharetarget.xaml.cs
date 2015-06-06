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
using System.Threading.Tasks;
using Windows.ApplicationModel;
using Windows.ApplicationModel.DataTransfer;
using Windows.ApplicationModel.DataTransfer.ShareTarget;
using Windows.Data.Json;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.Text;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Documents;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using Windows.Security.EnterpriseData;

namespace EdpSample
{
    public sealed partial class Scenario14_ShareTarget : Page
    {
        private MainPage rootPage;
        ShareOperation m_shareOperation;
        private string m_sharedDataTitle;
        private string m_sharedDataDescription;
        private string m_sharedDataPackageFamilyName;
        private Uri m_sharedDataContentSourceWebLink;
        private Uri m_sharedDataContentSourceApplicationLink;
        private Color m_sharedDataLogoBackgroundColor;
        private IRandomAccessStreamReference m_sharedDataSquare30x30Logo;
        private string shareQuickLinkId;
        private string m_sharedText;
        private Uri m_sharedWebLink;
        private Uri m_sharedApplicationLink;
        private IReadOnlyList<IStorageItem> m_sharedStorageItems;
        private string m_sharedCustomData;
        private string m_sharedHtmlFormat;
        private IReadOnlyDictionary<string, RandomAccessStreamReference> m_sharedResourceMap;
        private IRandomAccessStreamReference m_sharedBitmapStreamRef;
        private IRandomAccessStreamReference m_sharedThumbnailStreamRef;
        private const string dataFormatName = "http://schema.org/Book";

        public Scenario14_ShareTarget()
        {
            this.InitializeComponent();
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;

            // It is recommended to only retrieve the m_shareOperation object in the activation handler, return as
            // quickly as possible, and retrieve all data from the share target asynchronously.

            m_shareOperation = (ShareOperation)e.Parameter;

            await Task.Factory.StartNew(async () =>
            {
                // Retrieve the data package properties.
                m_sharedDataTitle = m_shareOperation.Data.Properties.Title;
                m_sharedDataDescription = m_shareOperation.Data.Properties.Description;
                m_sharedDataPackageFamilyName = m_shareOperation.Data.Properties.PackageFamilyName;
                m_sharedDataContentSourceWebLink = m_shareOperation.Data.Properties.ContentSourceWebLink;
                m_sharedDataContentSourceApplicationLink = m_shareOperation.Data.Properties.ContentSourceApplicationLink;
                m_sharedDataLogoBackgroundColor = m_shareOperation.Data.Properties.LogoBackgroundColor;
                m_sharedDataSquare30x30Logo = m_shareOperation.Data.Properties.Square30x30Logo;
                m_sharedThumbnailStreamRef = m_shareOperation.Data.Properties.Thumbnail;
                shareQuickLinkId = m_shareOperation.QuickLinkId;

                // Retrieve the data package content.
                // The GetWebLinkAsync(), GetTextAsync(), GetStorageItemsAsync(), etc. APIs will throw if there was an error retrieving the data from the source app.
                // In this sample, we just display the error. It is recommended that a share target app handles these in a way appropriate for that particular app.
                if (m_shareOperation.Data.Contains(StandardDataFormats.WebLink))
                {
                    try
                    {
                        m_sharedWebLink = await m_shareOperation.Data.GetWebLinkAsync();
                    }
                    catch (Exception ex)
                    {
                        NotifyUserBackgroundThread("Failed GetWebLinkAsync - " + ex.Message, NotifyType.ErrorMessage);
                    }
                }
                if (m_shareOperation.Data.Contains(StandardDataFormats.ApplicationLink))
                {
                    try
                    {
                        m_sharedApplicationLink = await m_shareOperation.Data.GetApplicationLinkAsync();
                    }
                    catch (Exception ex)
                    {
                        NotifyUserBackgroundThread("Failed GetApplicationLinkAsync - " + ex.Message, NotifyType.ErrorMessage);
                    }
                }
                if (m_shareOperation.Data.Contains(StandardDataFormats.Text))
                {
                    try
                    {
                        m_sharedText = await m_shareOperation.Data.GetTextAsync();
                    }
                    catch (Exception ex)
                    {
                        NotifyUserBackgroundThread("Failed GetTextAsync - " + ex.Message, NotifyType.ErrorMessage);
                    }
                }
                if (m_shareOperation.Data.Contains(StandardDataFormats.StorageItems))
                {
                    try
                    {
                        m_sharedStorageItems = await m_shareOperation.Data.GetStorageItemsAsync();
                    }
                    catch (Exception ex)
                    {
                        NotifyUserBackgroundThread("Failed GetStorageItemsAsync - " + ex.Message, NotifyType.ErrorMessage);
                    }
                }
                if (m_shareOperation.Data.Contains(dataFormatName))
                {
                    try
                    {
                        m_sharedCustomData = await m_shareOperation.Data.GetTextAsync(dataFormatName);
                    }
                    catch (Exception ex)
                    {
                        NotifyUserBackgroundThread("Failed GetTextAsync(" + dataFormatName + ") - " + ex.Message, NotifyType.ErrorMessage);
                    }
                }
                if (m_shareOperation.Data.Contains(StandardDataFormats.Html))
                {
                    try
                    {
                        m_sharedHtmlFormat = await m_shareOperation.Data.GetHtmlFormatAsync();
                    }
                    catch (Exception ex)
                    {
                        NotifyUserBackgroundThread("Failed GetHtmlFormatAsync - " + ex.Message, NotifyType.ErrorMessage);
                    }

                    try
                    {
                        m_sharedResourceMap = await m_shareOperation.Data.GetResourceMapAsync();
                    }
                    catch (Exception ex)
                    {
                        NotifyUserBackgroundThread("Failed GetResourceMapAsync - " + ex.Message, NotifyType.ErrorMessage);
                    }
                }
                if (m_shareOperation.Data.Contains(StandardDataFormats.Bitmap))
                {
                    try
                    {
                        m_sharedBitmapStreamRef = await m_shareOperation.Data.GetBitmapAsync();
                    }
                    catch (Exception ex)
                    {
                        NotifyUserBackgroundThread("Failed GetBitmapAsync - " + ex.Message, NotifyType.ErrorMessage);
                    }
                }

                // In this sample, we just display the m_shared data content.

                // Get back to the UI thread using the dispatcher.
                await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, async () =>
                {
                   DataPackageTitle.Text = m_sharedDataTitle;
                   DataPackageDescription.Text = m_sharedDataDescription;
                   DataPackagePackageFamilyName.Text = m_sharedDataPackageFamilyName;
                    if (m_sharedDataContentSourceWebLink != null)
                    {
                       DataPackageContentSourceWebLink.Text = m_sharedDataContentSourceWebLink.AbsoluteUri;
                    }
                    if (m_sharedDataContentSourceApplicationLink != null)
                    {
                       DataPackageContentSourceApplicationLink.Text = m_sharedDataContentSourceApplicationLink.AbsoluteUri;
                    }

                    if (m_sharedDataSquare30x30Logo != null)
                    {
                        IRandomAccessStreamWithContentType logoStream = await m_sharedDataSquare30x30Logo.OpenReadAsync();
                        BitmapImage bitmapImage = new BitmapImage();
                        bitmapImage.SetSource(logoStream);
                        LogoHolder.Source = bitmapImage;
                        LogoBackground.Background = new SolidColorBrush(m_sharedDataLogoBackgroundColor);
                        LogoArea.Visibility = Visibility.Visible;
                    }

                    if (!String.IsNullOrEmpty(m_shareOperation.QuickLinkId))
                    {
                        SelectedQuickLinkId.Text = shareQuickLinkId;
                    }

                    if (m_sharedThumbnailStreamRef != null)
                    {
                        IRandomAccessStreamWithContentType thumbnailStream = await m_sharedThumbnailStreamRef.OpenReadAsync();
                        BitmapImage bitmapImage = new BitmapImage();
                        bitmapImage.SetSource(thumbnailStream);
                        ThumbnailHolder.Source = bitmapImage;
                        ThumbnailArea.Visibility = Visibility.Visible;
                    }

                    if (m_sharedWebLink != null)
                    {
                        AddContentValue("WebLink: ", m_sharedWebLink.AbsoluteUri);
                    }
                    if (m_sharedApplicationLink != null)
                    {
                        AddContentValue("ApplicationLink: ", m_sharedApplicationLink.AbsoluteUri);
                    }
                    if (m_sharedText != null)
                    {
                        AddContentValue("Text: ", m_sharedText);
                    }
                    if (m_sharedStorageItems != null)
                    {
                        // Display the name of the files being m_shared.
                        StringBuilder fileNames = new StringBuilder();
                        for (int index = 0; index < m_sharedStorageItems.Count; index++)
                        {
                            fileNames.Append(m_sharedStorageItems[index].Name);
                            if (index < m_sharedStorageItems.Count - 1)
                            {
                                fileNames.Append(", ");
                            }
                        }
                        fileNames.Append(".");

                        AddContentValue("StorageItems: ", fileNames.ToString());
                    }
                    if (m_sharedCustomData != null)
                    {
                        // This is an area to be especially careful parsing data from the source app to avoid buffer overruns.
                        // This sample doesn't perform data validation but will catch any exceptions thrown.
                        try
                        {
                            StringBuilder receivedStrings = new StringBuilder();
                            JsonObject customObject = JsonObject.Parse(m_sharedCustomData);
                            if (customObject.ContainsKey("type"))
                            {
                                if (customObject["type"].GetString() == "http://schema.org/Book")
                                {
                                    // This sample expects the custom format to be of type http://schema.org/Book
                                    receivedStrings.AppendLine("Type: " + customObject["type"].Stringify());
                                    JsonObject properties = customObject["properties"].GetObject();
                                    receivedStrings.AppendLine("Image: " + properties["image"].Stringify());
                                    receivedStrings.AppendLine("Name: " + properties["name"].Stringify());
                                    receivedStrings.AppendLine("Book Format: " + properties["bookFormat"].Stringify());
                                    receivedStrings.AppendLine("Author: " + properties["author"].Stringify());
                                    receivedStrings.AppendLine("Number of Pages: " + properties["numberOfPages"].Stringify());
                                    receivedStrings.AppendLine("Publisher: " + properties["publisher"].Stringify());
                                    receivedStrings.AppendLine("Date Published: " + properties["datePublished"].Stringify());
                                    receivedStrings.AppendLine("In Language: " + properties["inLanguage"].Stringify());
                                    receivedStrings.Append("ISBN: " + properties["isbn"].Stringify());

                                    AddContentValue("Custom format data:" + Environment.NewLine, receivedStrings.ToString());
                                }
                                else
                                {
                                    rootPage.NotifyUser("The custom format from the source app is not of type http://schema.org/Book", NotifyType.ErrorMessage);
                                }
                            }
                            else
                            {
                                rootPage.NotifyUser("The custom format from the source app doesn't contain a type", NotifyType.ErrorMessage);
                            }
                        }
                        catch (Exception ex)
                        {
                            rootPage.NotifyUser("Failed to parse the custom data - " + ex.Message, NotifyType.ErrorMessage);
                        }
                    }
                    if (m_sharedHtmlFormat != null)
                    {
                        string htmlFragment = HtmlFormatHelper.GetStaticFragment(m_sharedHtmlFormat);
                        if (!String.IsNullOrEmpty(htmlFragment))
                        {
                            AddContentValue("HTML: ");
                            ShareWebView.Visibility = Visibility.Visible;
                            ShareWebView.NavigateToString("<html><body>" + htmlFragment + "</body></html>");
                        }
                        else
                        {
                            rootPage.NotifyUser("GetStaticFragment failed to parse the HTML from the source app", NotifyType.ErrorMessage);
                        }

                        // Check if there are any local images in the resource map.
                        if (m_sharedResourceMap.Count > 0)
                        {
                            //ResourceMapValue.Text = "";
                            foreach (KeyValuePair<string, RandomAccessStreamReference> item in m_sharedResourceMap)
                            {
                                 ResourceMapValue.Text += "\nKey: " + item.Key;
                            }
                              ResourceMapArea.Visibility = Visibility.Visible;
                        }
                    }
                    if (m_sharedBitmapStreamRef != null)
                    {
                        IRandomAccessStreamWithContentType bitmapStream = await m_sharedBitmapStreamRef.OpenReadAsync();
                        BitmapImage bitmapImage = new BitmapImage();
                        bitmapImage.SetSource(bitmapStream);
                        ImageHolder.Source = bitmapImage;
                        ImageArea.Visibility = Visibility.Visible;
                    }
                });
            });
        }

        private void QuickLinkSectionLabel_Tapped(object sender, TappedRoutedEventArgs e)
        {
            // Trigger the appropriate Checked/Unchecked event if the user taps on the text instead of the checkbox.
            AddQuickLink.IsChecked = !AddQuickLink.IsChecked;
        }

        private void AddQuickLink_Checked(object sender, RoutedEventArgs e)
        {
            QuickLinkCustomization.Visibility = Visibility.Visible;
        }

        private void AddQuickLink_Unchecked(object sender, RoutedEventArgs e)
        {
            QuickLinkCustomization.Visibility = Visibility.Collapsed;
        }

        async void ReportCompleted_Click(object sender, RoutedEventArgs e)
        {
           if (AddQuickLink.IsChecked.Equals(true))
            {
                QuickLink quickLinkInfo = new QuickLink
                {
                    Id = QuickLinkId.Text,
                    Title = QuickLinkTitle.Text,

                    // For QuickLinks, the supported FileTypes and DataFormats are set independently from the manifest.
                    SupportedFileTypes = { "*" },
                    SupportedDataFormats =
                    {
                        StandardDataFormats.Text,
                        StandardDataFormats.WebLink,
                        StandardDataFormats.ApplicationLink,
                        StandardDataFormats.Bitmap,
                        StandardDataFormats.StorageItems,
                        StandardDataFormats.Html,
                        dataFormatName
                    }
                };

                try
                {
                    StorageFile iconFile = await Package.Current.InstalledLocation.CreateFileAsync("assets\\user.png", CreationCollisionOption.OpenIfExists);
                    quickLinkInfo.Thumbnail = RandomAccessStreamReference.CreateFromFile(iconFile);
                    m_shareOperation.ReportCompleted(quickLinkInfo);
                }
                catch (Exception)
                {
                    // Even if the QuickLink cannot be created it is important to call ReportCompleted. Otherwise, if this is a long-running share,
                    // the app will stick around in the long-running share progress list.
                    m_shareOperation.ReportCompleted();
                    throw;
                }
            }
            else
            {
                m_shareOperation.ReportCompleted();
            }
        }

        private void DismissUI_Click(object sender, RoutedEventArgs e)
        {
            m_shareOperation.DismissUI();
        }

        private void LongRunningShareLabel_Tapped(object sender, TappedRoutedEventArgs e)
        {
            // Trigger the appropriate Checked/Unchecked event if the user taps on the text instead of the checkbox.
            ExpandLongRunningShareSection.IsChecked = !ExpandLongRunningShareSection.IsChecked;
        }

        private void ExpandLongRunningShareSection_Checked(object sender, RoutedEventArgs e)
        {
            ExtendedSharingArea.Visibility = Visibility.Visible;
        }

        private void ExpandLongRunningShareSection_Unchecked(object sender, RoutedEventArgs e)
        {
            ExtendedSharingArea.Visibility = Visibility.Collapsed;
        }

        private void ReportStarted_Click(object sender, RoutedEventArgs e)
        {
            m_shareOperation.ReportStarted();
            rootPage.NotifyUser("Started...", NotifyType.StatusMessage);
        }

        private void ReportDataRetrieved_Click(object sender, RoutedEventArgs e)
        {
            m_shareOperation.ReportDataRetrieved();
            rootPage.NotifyUser("Data retrieved...", NotifyType.StatusMessage);
        }

        private void ReportSubmittedBackgroundTask_Click(object sender, RoutedEventArgs e)
        {
            m_shareOperation.ReportSubmittedBackgroundTask();
            rootPage.NotifyUser("Submitted background task...", NotifyType.StatusMessage);
        }

        private void ReportErrorButton_Click(object sender, RoutedEventArgs e)
        {
            m_shareOperation.ReportError(ReportError.Text);
            
        }

   

        async private void NotifyUserBackgroundThread(string message, NotifyType type)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                rootPage.NotifyUser(message, type);
            });
        }

        private void AddContentValue(string title, string description = null)
        {
            Run contentType = new Run();
            contentType.FontWeight = FontWeights.Bold;
            contentType.Text = title;
            ContentValue.Inlines.Add(contentType);

            if (description != null)
            {
                Run contentValue = new Run();
                contentValue.Text = description + Environment.NewLine;
                ContentValue.Inlines.Add(contentValue);
            }
        }
    }

   
}
