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
#include "Scenario1_Render.h"
#include "Scenario1_Render.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Data::Pdf;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::Storage::Pickers;
using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Media::Imaging;


namespace winrt::SDKTemplate::implementation
{
    Scenario1_Render::Scenario1_Render()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario1_Render::LoadDocument(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        LoadButton().IsEnabled(false);

        pdfDocument = nullptr;
        Output().Source(nullptr);
        PageNumberBox().Text(L"1");
        RenderingPanel().Visibility(Visibility::Collapsed);

        FileOpenPicker picker;

        picker.FileTypeFilter().Append(L".pdf");
        StorageFile file = co_await picker.PickSingleFileAsync();

        if (file != nullptr)
        {
            ProgressControl().Visibility(Visibility::Visible);
            try
            {
                pdfDocument = co_await PdfDocument::LoadFromFileAsync(file, PasswordBox().Password());
            }
            catch (winrt::hresult_error const& ex)
            {
                switch (ex.to_abi())
                {
                case __HRESULT_FROM_WIN32(ERROR_WRONG_PASSWORD):
                    rootPage.NotifyUser(L"Document is password-protected and password is incorrect.", NotifyType::ErrorMessage);
                    break;

                case E_FAIL:
                    rootPage.NotifyUser(L"Document is not a valid PDF.", NotifyType::ErrorMessage);
                    break;

                default:
                    // File I/O errors are reported as exceptions.
                    rootPage.NotifyUser(ex.message(), NotifyType::ErrorMessage);
                    break;
                }
            }

            if (pdfDocument != nullptr)
            {
                RenderingPanel().Visibility(Visibility::Visible);
                if (pdfDocument.IsPasswordProtected())
                {
                    rootPage.NotifyUser(L"Document is password protected.", NotifyType::StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser(L"Document is not password protected.", NotifyType::StatusMessage);
                }
                PageCountText().Text(winrt::to_hstring(pdfDocument.PageCount()));
            }
            ProgressControl().Visibility(Visibility::Collapsed);
        }
        LoadButton().IsEnabled(true);
    }

    fire_and_forget Scenario1_Render::ViewPage(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        rootPage.NotifyUser(L"", NotifyType::StatusMessage);

        // If the text is not a valid number, then wcstoul returns 0, which is an invalid
        // page number, so we don't need to special-case that possibility.
        unsigned long pageNumber = wcstoul(PageNumberBox().Text().data(), nullptr, 10);

        if ((pageNumber < 1) || (pageNumber > pdfDocument.PageCount()))
        {
            rootPage.NotifyUser(L"Invalid page number.", NotifyType::ErrorMessage);
            return;
        }

        Output().Source(nullptr);
        ProgressControl().Visibility(Visibility::Visible);

        // Convert from 1-based page number to 0-based page index.
        unsigned long pageIndex = pageNumber - 1;

        PdfPage page = pdfDocument.GetPage(pageIndex);

        InMemoryRandomAccessStream stream;

        switch (Options().SelectedIndex())
        {
            // View actual size.
        case 0:
            co_await page.RenderToStreamAsync(stream);
            break;

            // View half size on beige background.
        case 1:
        {
            auto options1 = PdfPageRenderOptions();
            options1.BackgroundColor(Windows::UI::Colors::Beige());
            options1.DestinationHeight(static_cast<unsigned int>(page.Size().Height / 2));
            options1.DestinationWidth(static_cast<unsigned int>(page.Size().Width / 2));
            co_await page.RenderToStreamAsync(stream, options1);
            break;
        }

        // Crop to center.
        case 2:
        {
            auto options2 = PdfPageRenderOptions();
            auto rect = page.Dimensions().TrimBox();
            options2.SourceRect({ rect.X + rect.Width / 4, rect.Y + rect.Height / 4, rect.Width / 2, rect.Height / 2 });
            co_await page.RenderToStreamAsync(stream, options2);
            break;
        }
        }

        BitmapImage src;
        Output().Source(src);
        co_await src.SetSourceAsync(stream);

        ProgressControl().Visibility(Visibility::Collapsed);
    }
}
