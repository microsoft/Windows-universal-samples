//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

//
// CopyText.xaml.cpp
// Implementation of the CopyText class
//

#include "pch.h"
#include "CopyText.xaml.h"
#include "MainPage.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage::Streams;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Data;
using namespace Windows::Data::Html;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

// App specific namespace references
using namespace concurrency;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::DataTransfer;

CopyText::CopyText()
{
    InitializeComponent();

    imgSrc = "ms-appx-web:///assets/windows-sdk.png";

    htmlDescription = "Use Clipboard to copy and paste text in different formats, including plain text, and formatted text (HTML).<br />"
        + "To <b>copy</b>, add text or HTML formats to a <i>DataPackage</i>, and then place <i>DataPackage</i> into the Clipboard.<br />"
        + "To handle local image files in the formatted text (such as the one below), use a resourceMap collection.<br />"
        + "<br /><img id=\"LocalImage\" src=\"" + imgSrc + "\" /><br /><br />"
        + "To <b>paste</b>, get <i>DataPackageView</i> from the Clipboard, and retrieve the content of the desired text format from it.<br />"
        + "The <i>DataPackage</i> class is also used to share or send content between applications. See the Share SDK sample for more information.";

    Description->NavigateToString(htmlDescription);

    textDescription = HtmlUtilities::ConvertToText(htmlDescription);
}

void CopyText::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page. This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;
}

#pragma region CopyText Click handlers
void CopyText::CopyButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    OutputHtml->NavigateToString("<HTML></HTML>");
    OutputResourceMapKeys->Text = "";
    OutputText->Text = "";

    auto dataPackage = ref new DataPackage();
    String^ htmlFormat = HtmlFormatHelper::CreateHtmlFormat(htmlDescription);

    // Set the content as CF_HTML html format.
    dataPackage->SetHtmlFormat(htmlFormat);

    // Set the content as CF_TEXT text format.
    dataPackage->SetText(textDescription);

    // Populate resourceMap with StreamReference objects corresponding to local image files embedded in HTML
    auto imgUri = ref new Uri(imgSrc);
    auto imgRef = RandomAccessStreamReference::CreateFromUri(imgUri);
    dataPackage->ResourceMap->Insert(imgSrc, imgRef);

    try
    {
        // Set the contents in the clipboard
        DataTransfer::Clipboard::SetContent(dataPackage);
        rootPage->NotifyUser("Text and HTML formats have been copied to clipboard. ", NotifyType::StatusMessage);
    }
    catch (Exception^ ex)
    {
        // Copying data to the Clipboard can potentially fail - for example, if another application is holding the Clipboard open
        rootPage->NotifyUser("Error copying Text and HTML formats to Clipboard: " + ex->Message + ".", NotifyType::ErrorMessage);
    }
}

void CopyText::PasteButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    OutputHtml->NavigateToString("<HTML></HTML>");
    OutputResourceMapKeys->Text = "";
    OutputText->Text = "";

    rootPage->NotifyUser("Clipboard content displayed below: ", NotifyType::StatusMessage);

    DataPackageView^ dataPackageView = DataTransfer::Clipboard::GetContent();

    if (dataPackageView->Contains(StandardDataFormats::Html))
    {
        create_task(dataPackageView->GetHtmlFormatAsync()).then(
            [this](task<String^> htmlTask)
        {
            String^ htmlFormat;
            try
            {
                htmlFormat = htmlTask.get();
            }
            catch (Exception^ ex)
            {
                rootPage->NotifyUserBackgroundThread("Failed GetHtmlFormatAsync - " + ex->Message, NotifyType::ErrorMessage);
            }

            if (htmlFormat != nullptr)
            {
                String^ htmlFragment = HtmlFormatHelper::GetStaticFragment(htmlFormat);

                // Note: Windows Store apps may be unable to render some schemes (e.g., 'ms-clipboard-file') that reference local content.
                // As a result, images may look broken. To learn the different URL schemes used by Windows Store apps and how to use them
                // to refer to content, see MSDN - http://msdn.microsoft.com/en-us/library/windows/apps/hh781215.aspx
                OutputHtml->NavigateToString("HTML:<br />" + htmlFragment);
            }
        });

        // Note: this sample is not trying to resolve and render the HTML using resource map.
        // Please refer to the Clipboard JavaScript sample for an example of how to use resource map
        // for local images display within an HTML format. This sample will only demonstrate how to
        // get a resource map object and extract its key values
        create_task(dataPackageView->GetResourceMapAsync()).then(
            [this](task<IMapView<String^, RandomAccessStreamReference^>^> resourceMapTask)
        {
            IMapView<String^, RandomAccessStreamReference^>^ resourceMap = resourceMapTask.get();

            // Check if there are any local images in the resource map.
            if (resourceMap->Size > 0)
            {
                OutputResourceMapKeys->Text = "\n\nResource Map:";
                std::for_each(begin(resourceMap), end(resourceMap),
                    [this](IKeyValuePair<String^, RandomAccessStreamReference^>^ item)
                {
                    OutputResourceMapKeys->Text += "\n\tKey: " + item->Key;
                });
            }
            else
            {
                rootPage->NotifyUserBackgroundThread("The resource map does not contain any file", NotifyType::StatusMessage);
            }
        });
    }
    else
    {
        rootPage->NotifyUser("HTML format is not available in clipboard", NotifyType::StatusMessage);
    }

    if (dataPackageView->Contains(StandardDataFormats::Text))
    {
        create_task(dataPackageView->GetTextAsync()).then(
            [this](task<String^> textTask)
        {
            try
            {
                OutputText->Text = "\nText: \n" + textTask.get();
            }
            catch (Exception^ ex)
            {
                rootPage->NotifyUserBackgroundThread("Failed GetTextAsync - " + ex->Message, NotifyType::ErrorMessage);
            }
        });
    }
    else
    {
        rootPage->NotifyUser("Text format is not available in clipboard", NotifyType::StatusMessage);
    }
}

#pragma endregion
