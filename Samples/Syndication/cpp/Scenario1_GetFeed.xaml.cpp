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

//
// Scenario1.xaml.cpp
// Implementation of the Scenario1 class
//

#include "pch.h"
#include "Scenario1_GetFeed.xaml.h"

using namespace SDKTemplate::Syndication;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Web::Syndication;
using namespace Windows::Foundation;
using namespace Platform;
using namespace Platform::Collections;
using namespace Concurrency;

Scenario1::Scenario1()
{
    currentFeed = nullptr;

    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario1::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;
}

void SDKTemplate::Syndication::Scenario1::GetFeed_Click(Object^ sender, RoutedEventArgs^ e)
{
    OutputField->Text = "";

    // By default 'FeedUri' is disabled and URI validation is not required. When enabling the text box
    // validating the URI is required since it was received from an untrusted source (user input).
    // The URI is validated by calling TryGetUri() that will return 'false' for strings that are not valid URIs.
    // Note that when enabling the text box users may provide URIs to machines on the intrAnet that require the
    // "Home or Work Networking" capability.
    Uri^ uri;
    if (!rootPage->TryGetUri(FeedUri->Text, &uri))
    {
        return;
    }

    SyndicationClient^ client = ref new SyndicationClient();
    client->BypassCacheOnRetrieve = true;

    // Some servers require a user-agent.
    client->SetRequestHeader("user-agent", "Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.2; WOW64; Trident/6.0)");

    rootPage->NotifyUser("Downloading feed...", NotifyType::StatusMessage);
    OutputField->Text = "Downloading feed: " + uri->DisplayUri + "\r\n";

    task<SyndicationFeed^>(client->RetrieveFeedAsync(uri)).then([this] (SyndicationFeed^ feed)
    {
        currentFeed = feed;
        rootPage->NotifyUser("Feed download complete.", NotifyType::StatusMessage);

        DisplayFeed();
    }).then([this] (task<void> t)
    {
        try
        {
            t.get();
        }
        catch (Exception^ exception)
        {
            rootPage->NotifyUser(exception->Message, NotifyType::ErrorMessage);

            // Match HResult with a SyndicationErrorStatus value.
            // WebErrorStatus is also available WebErrorStatus for HTTP status error codes.
            SyndicationErrorStatus status = SyndicationError::GetStatus(exception->HResult);
            if (status == SyndicationErrorStatus::InvalidXml)
            {
                OutputField->Text += "An invalid XML exception was thrown. " +
                    "Please make sure to use a URI that points to a RSS or Atom feed.";
            }
        }
    });
}

void SDKTemplate::Syndication::Scenario1::DisplayFeed()
{
    ISyndicationText^ title = currentFeed->Title;
    FeedTitleField->Text = title != nullptr ? title->Text : "(no title)";

    currentItemIndex = 0;
    if (currentFeed->Items->Size > 0)
    {
        DisplayCurrentItem();
    }

    // List the items.
    OutputField->Text += "Items: " + currentFeed->Items->Size + "\r\n";
}

void SDKTemplate::Syndication::Scenario1::PreviousItem_Click(Object^ sender, RoutedEventArgs^ e)
{
    if (currentFeed != nullptr && currentItemIndex > 0)
    {
        currentItemIndex--;
        DisplayCurrentItem();
    }
}

void SDKTemplate::Syndication::Scenario1::NextItem_Click(Object^ sender, RoutedEventArgs^ e)
{
    if (currentFeed != nullptr && currentItemIndex < (currentFeed->Items->Size - 1))
    {
        currentItemIndex++;
        DisplayCurrentItem();
    }
}

void SDKTemplate::Syndication::Scenario1::DisplayCurrentItem()
{
    SyndicationItem^ item = currentFeed->Items->GetAt(currentItemIndex);

    // Display item number.
    IndexField->Text = (currentItemIndex + 1).ToString() + " of " + currentFeed->Items->Size.ToString();

    // Display title.
    ItemTitleField->Text = item->Title != nullptr ? item->Title->Text : "(no title)";

    // Display the main link.
    String^ link = nullptr;
    if (item->Links->Size > 0)
    {
        link = item->Links->GetAt(0)->Uri->AbsoluteUri;
    }
    LinkField->Content = link;

    // Display element extensions. The ElementExtensions collection contains all the additional child elements within
    // the current element that do not belong to the Atom or RSS standards (e.g., Dublin Core extension elements).
    auto items = ref new Vector<Platform::Object^>();
    for (ISyndicationNode^ elementExtension : item->ElementExtensions)
    {
        auto bindableNode = ref new BindableNode(
            elementExtension->NodeName,
            elementExtension->NodeNamespace,
            elementExtension->NodeValue);
        items->Append(bindableNode);
    }
    ExtensionsField->ItemsSource = items;

    // Display the body as HTML.
    String^ content = "(no content)";
    if (item->Content != nullptr)
    {
        content = item->Content->Text;
    }
    else if (item->Summary != nullptr)
    {
        content = item->Summary->Text;
    }
    ContentWebView->NavigateToString(content);
}

void SDKTemplate::Syndication::Scenario1::LinkField_Click(Object^ sender, RoutedEventArgs^ e)
{
    Windows::System::Launcher::LaunchUriAsync(ref new Uri(((HyperlinkButton^)sender)->Content->ToString()));
}
