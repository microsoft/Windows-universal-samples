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

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using System;
using Windows.Web.Syndication;
using Windows.Web;
using System.Globalization;

namespace Syndication
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class S1_GetAFeed : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;
        SyndicationFeed currentFeed = null;
        int currentItemIndex;

        public S1_GetAFeed()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
        }

        private async void GetFeed_Click(object sender, RoutedEventArgs e)
        {
            OutputField.Text = "";

            // By default 'FeedUri' is disabled and URI validation is not required. When enabling the text box
            // validating the URI is required since it was received from an untrusted source (user input).
            // The URI is validated by calling Uri.TryCreate() that will return 'false' for strings that are not valid URIs.
            // Note that when enabling the text box users may provide URIs to machines on the intrAnet that require
            // the "Home or Work Networking" capability.
            Uri uri;
            if (!Uri.TryCreate(FeedUri.Text.Trim(), UriKind.Absolute, out uri))
            {
                rootPage.NotifyUser("Error: Invalid URI.", NotifyType.ErrorMessage);
                return;
            }

            SyndicationClient client = new SyndicationClient();
            client.BypassCacheOnRetrieve = true;

            // Although most HTTP servers do not require User-Agent header, others will reject the request or return
            // a different response if this header is missing. Use SetRequestHeader() to add custom headers.
            client.SetRequestHeader("User-Agent", "Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.2; WOW64; Trident/6.0)");

            rootPage.NotifyUser("Downloading feed...", NotifyType.StatusMessage);
            OutputField.Text = "Downloading feed: " + uri.ToString() + "\r\n";

            try
            {
                currentFeed = await client.RetrieveFeedAsync(uri);
                rootPage.NotifyUser("Feed download complete.", NotifyType.StatusMessage);

                DisplayFeed();
            }
            catch (Exception ex)
            {
                SyndicationErrorStatus status = SyndicationError.GetStatus(ex.HResult);
                if (status == SyndicationErrorStatus.InvalidXml)
                {
                    OutputField.Text += "An invalid XML exception was thrown. " +
                        "Please make sure to use a URI that points to a RSS or Atom feed.";
                }

                if (status == SyndicationErrorStatus.Unknown)
                {
                    WebErrorStatus webError = WebError.GetStatus(ex.HResult);

                    if (webError == WebErrorStatus.Unknown)
                    {
                        // Neither a syndication nor a web error. Rethrow.
                        throw;
                    }
                }

                rootPage.NotifyUser(ex.Message, NotifyType.ErrorMessage);
            }
        }

        private void DisplayFeed()
        {
            ISyndicationText title = currentFeed.Title;
            FeedTitleField.Text = title != null ? title.Text : "(no title)";

            currentItemIndex = 0;
            if (currentFeed.Items.Count > 0)
            {
                DisplayCurrentItem();
            }

            // List the items.
            OutputField.Text += "Items: " + currentFeed.Items.Count + "\r\n";
        }

        private void PreviousItem_Click(object sender, RoutedEventArgs e)
        {
            if (currentFeed != null && currentItemIndex > 0)
            {
                currentItemIndex--;
                DisplayCurrentItem();
            }
        }

        private void NextItem_Click(object sender, RoutedEventArgs e)
        {
            if (currentFeed != null && currentItemIndex < (currentFeed.Items.Count - 1))
            {
                currentItemIndex++;
                DisplayCurrentItem();
            }
        }

        private void DisplayCurrentItem()
        {
            SyndicationItem item = currentFeed.Items[currentItemIndex];

            // Display item number.
            IndexField.Text = String.Format(
                CultureInfo.InvariantCulture,
                "{0} of {1}",
                currentItemIndex + 1,
                currentFeed.Items.Count);

            // Display title.
            ItemTitleField.Text = item.Title != null ? item.Title.Text : "(no title)";

            // Display the main link.
            string link = string.Empty;
            if (item.Links.Count > 0)
            {
                link = item.Links[0].Uri.AbsoluteUri;
            }
            LinkField.Content = link;

            // Display item extensions.
            ExtensionsField.ItemsSource = item.ElementExtensions;

            // Display the body as HTML.
            string content = "(no content)";
            if (item.Content != null)
            {
                content = item.Content.Text;
            }
            else if (item.Summary != null)
            {
                content = item.Summary.Text;
            }
            ContentWebView.NavigateToString(content);
        }

        private async void LinkField_Click(object sender, RoutedEventArgs e)
        {
            await Windows.System.Launcher.LaunchUriAsync(new Uri((sender as HyperlinkButton).Content.ToString()));
        }
    }
}
