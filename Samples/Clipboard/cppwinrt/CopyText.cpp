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

#include "pch.h"
#include "CopyText.h"
#include "CopyText.g.cpp"

using namespace winrt;
using namespace Windows::ApplicationModel::DataTransfer;
using namespace Windows::Data::Html;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

namespace winrt::SDKTemplate::implementation
{
    CopyText::CopyText()
    {
        InitializeComponent();
        Description().NavigateToString(htmlFragment);
    }

    void CopyText::CopyButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        OutputText().Text(L"");
        OutputResourceMapKeys().Text(L"");
        OutputHtml().NavigateToString(L"<HTML></HTML>");

        // Set the content to DataPackage as html format
        hstring htmlFormat = HtmlFormatHelper::CreateHtmlFormat(this->htmlFragment);
        auto dataPackage = DataPackage();
        dataPackage.SetHtmlFormat(htmlFormat);

        // Set the content to DataPackage as (plain) text format
        hstring plainText = HtmlUtilities::ConvertToText(this->htmlFragment);
        dataPackage.SetText(plainText);

        // Populate resourceMap with StreamReference objects corresponding to local image files embedded in HTML
        auto imgUri = Uri(imgSrc);
        auto imgRef = RandomAccessStreamReference::CreateFromUri(imgUri);
        dataPackage.ResourceMap().Insert(imgSrc, imgRef);

        // Set the DataPackage to clipboard.
        if (Clipboard::SetContentWithOptions(dataPackage, nullptr))
        {
            rootPage.NotifyUser(L"Text and HTML formats have been copied to clipboard.", NotifyType::StatusMessage);
        }
        else
        {
            // Copying data to Clipboard can potentially fail - for example, if another application is holding Clipboard open
            rootPage.NotifyUser(L"Error copying content to clipboard.", NotifyType::ErrorMessage);
        }
    }

    fire_and_forget CopyText::PasteButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        rootPage.NotifyUser(L"", NotifyType::StatusMessage);
        OutputText().Text(L"");
        OutputResourceMapKeys().Text(L"");
        OutputHtml().NavigateToString(L"<HTML></HTML>");

        auto dataPackageView = Clipboard::GetContent();
        if (dataPackageView.Contains(StandardDataFormats::Text()))
        {
            try
            {
                auto text = co_await dataPackageView.GetTextAsync();
                OutputText().Text(L"Text: \n" + text);
            }
            catch (hresult_error const& ex)
            {
                rootPage.NotifyUser(L"Error retrieving Text format from Clipboard: " + ex.message(), NotifyType::ErrorMessage);
            }
        }
        else
        {
            OutputText().Text(L"Text: \nText format is not available in clipboard");
        }

        if (dataPackageView.Contains(StandardDataFormats::Html()))
        {
            DisplayResourceMapAsync(dataPackageView);

            hstring htmlFormat = L"";
            try
            {
                htmlFormat = co_await dataPackageView.GetHtmlFormatAsync();
            }
            catch (hresult_error const& ex)
            {
                rootPage.NotifyUser(L"Error retrieving HTML format from Clipboard: " + ex.message(), NotifyType::ErrorMessage);
            }

            if (!htmlFormat.empty())
            {
                hstring fragment = HtmlFormatHelper::GetStaticFragment(htmlFormat);
                OutputHtml().NavigateToString(L"HTML:<br/ > " + fragment);
            }
        }
        else
        {
            OutputHtml().NavigateToString(L"HTML:<br/ > HTML format is not available in clipboard");
        }
    }

    // Note: this sample is not trying to resolve and render the HTML using resource map.
    // Please refer to the Clipboard JavaScript sample for an example of how to use resource map
    // for local images display within an HTML format. This sample will only demonstrate how to
    // get a resource map object and extract its key values
    fire_and_forget CopyText::DisplayResourceMapAsync(DataPackageView dataPackageView)
    {
        auto lifetime = get_strong();

        std::wostringstream output;
        output << std::endl << L"Resource map: ";

        IMapView<hstring, RandomAccessStreamReference> resMap = nullptr;
        try
        {
            resMap = co_await dataPackageView.GetResourceMapAsync();
        }
        catch (hresult_error const& ex)
        {
            rootPage.NotifyUser(L"Error retrieving Resource map from Clipboard: " + ex.message(), NotifyType::ErrorMessage);
        }

        if (resMap)
        {
            if (resMap.Size() > 0)
            {
                for (auto&& item : resMap)
                {
                    output << std::endl << L"Key: " << std::wstring_view(item.Key());
                }
            }
            else
            {
                output << std::endl << L"Resource map is empty";
            }
        }
        OutputResourceMapKeys().Text(output.str());
    }
}

