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
#include "Scenario1_Render.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Data::Pdf;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media::Imaging;

Scenario1_Render::Scenario1_Render()
{
    InitializeComponent();
}

void Scenario1_Render::LoadDocument()
{
    LoadButton->IsEnabled = false;

    pdfDocument = nullptr;
    Output->Source = nullptr;
    PageNumberBox->Text = "1";
    RenderingPanel->Visibility = Windows::UI::Xaml::Visibility::Collapsed;

    auto picker = ref new FileOpenPicker();
    picker->FileTypeFilter->Append(".pdf");
    create_task(picker->PickSingleFileAsync()).then([this](StorageFile^ file)
    {
        if (file != nullptr)
        {
            ProgressControl->Visibility = Windows::UI::Xaml::Visibility::Visible;
            return create_task(PdfDocument::LoadFromFileAsync(file, PasswordBox->Password));
        }
        else
        {
            return task_from_result(static_cast<PdfDocument^>(nullptr));
        }
    }).then([this](task<PdfDocument^> task)
    {
        try
        {
            pdfDocument = task.get();
        }
        catch (Exception^ ex)
        {
            switch (ex->HResult)
            {
            case __HRESULT_FROM_WIN32(ERROR_WRONG_PASSWORD):
                rootPage->NotifyUser("Document is password-protected and password is incorrect.", NotifyType::ErrorMessage);
                break;

            case E_FAIL:
                rootPage->NotifyUser("Document is not a valid PDF.", NotifyType::ErrorMessage);
                break;

            default:
                // File I/O errors are reported as exceptions.
                rootPage->NotifyUser(ex->Message, NotifyType::ErrorMessage);
                break;
            }
        }

        if (pdfDocument != nullptr)
        {
            RenderingPanel->Visibility = Windows::UI::Xaml::Visibility::Visible;
            if (pdfDocument->IsPasswordProtected)
            {
                rootPage->NotifyUser("Document is password protected.", NotifyType::StatusMessage);
            }
            else
            {
                rootPage->NotifyUser("Document is not password protected.", NotifyType::StatusMessage);
            }
            PageCountText->Text = pdfDocument->PageCount.ToString();
        }
        ProgressControl->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
        LoadButton->IsEnabled = true;
    });
}

void Scenario1_Render::ViewPage()
{
    rootPage->NotifyUser("", NotifyType::StatusMessage);

    // If the text is not a valid number, then wcstoul returns 0, which is an invalid
    // page number, so we don't need to special-case that possibility.
    unsigned long pageNumber = wcstoul(PageNumberBox->Text->Data(), nullptr, 10);

    if ((pageNumber < 1) || (pageNumber > pdfDocument->PageCount))
    {
        rootPage->NotifyUser("Invalid page number.", NotifyType::ErrorMessage);
        return;
    }

    Output->Source = nullptr;
    ProgressControl->Visibility = Windows::UI::Xaml::Visibility::Visible;

    // Convert from 1-based page number to 0-based page index.
    unsigned long pageIndex = pageNumber - 1;

    PdfPage^ page = pdfDocument->GetPage(pageIndex);

    auto stream = ref new InMemoryRandomAccessStream();

    IAsyncAction^ renderAction;

    switch (Options->SelectedIndex)
    {
    // View actual size.
    case 0:
        renderAction = page->RenderToStreamAsync(stream);
        break;

    // View half size on beige background.
    case 1:
    {
        auto options1 = ref new PdfPageRenderOptions();
        options1->BackgroundColor = Windows::UI::Colors::Beige;
        options1->DestinationHeight = static_cast<unsigned int>(page->Size.Height / 2);
        options1->DestinationWidth = static_cast<unsigned int>(page->Size.Width / 2);
        renderAction = page->RenderToStreamAsync(stream, options1);
        break;
    }

    // Crop to center.
    case 2:
    {
        auto options2 = ref new PdfPageRenderOptions();
        auto rect = page->Dimensions->TrimBox;
        options2->SourceRect = Rect(rect.X + rect.Width / 4, rect.Y + rect.Height / 4, rect.Width / 2, rect.Height / 2);
        renderAction = page->RenderToStreamAsync(stream, options2);
        break;
    }
    }

    create_task(renderAction).then([this, stream]()
    {
        // Put the stream in the image.
        auto src = ref new BitmapImage();
        Output->Source = src;
        return create_task(src->SetSourceAsync(stream));
    }).then([this]()
    {
        ProgressControl->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    });
}
