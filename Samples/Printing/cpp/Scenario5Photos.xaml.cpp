// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario5Photos.xaml.h"
#include "PageToPrint.xaml.h"
#include "PreviewUnavailable.xaml.h"
#include <robuffer.h>
#include <wrl\client.h>
#include <sstream>

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Graphics::Printing;
using namespace Windows::Graphics::Printing::OptionDetails;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Documents;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;

/// <summary>
/// This is the event handler for PrintManager.PrintTaskRequested.
/// In order to ensure a good user experience, the system requires that the app handle the PrintTaskRequested event within the time specified
/// by PrintTaskRequestedEventArgs->Request->Deadline.
/// Therefore, we use this handler to only create the print task.
/// The print settings customization can be done when the print document source is requested.
/// </summary>
/// <param name="sender">The print manager for which a print task request was made.</param>
/// <param name="e">The print taks request associated arguments.</param>
void PhotosPrintHelper::PrintTaskRequested(PrintManager^ sender, PrintTaskRequestedEventArgs^ e)
{
    PrintTask^ printTask = e->Request->CreatePrintTask("C++ Printing SDK Sample", ref new PrintTaskSourceRequestedHandler([=](PrintTaskSourceRequestedArgs^ args)
    {
        // Set the document source.
        args->SetSource(PrintDocumentSource);
    }));

    // Choose the printer options to be shown.
    // The order in which the options are appended determines the order in which they appear in the UI
    PrintTaskOptionDetails^ printDetailedOptions = PrintTaskOptionDetails::GetFromPrintTaskOptions(printTask->Options);
    printDetailedOptions->DisplayedOptions->Clear();
    printDetailedOptions->DisplayedOptions->Append(Windows::Graphics::Printing::StandardPrintTaskOptions::MediaSize);
    printDetailedOptions->DisplayedOptions->Append(Windows::Graphics::Printing::StandardPrintTaskOptions::Copies);

    // Create a new list option.
    PrintCustomItemListOptionDetails^ photoSize = printDetailedOptions->CreateItemListOption("photoSize", "Photo Size");
    photoSize->AddItem("SizeFullPage", "Full Page");
    photoSize->AddItem("Size4x6", "4 x 6 in");
    photoSize->AddItem("Size5x7", "5 x 7 in");
    photoSize->AddItem("Size8x10", "8 x 10 in");

    // Add the custom option to the option list.
    printDetailedOptions->DisplayedOptions->Append("photoSize");

    PrintCustomItemListOptionDetails^ scaling = printDetailedOptions->CreateItemListOption("scaling", "Scaling");
    scaling->AddItem("ShrinkToFit", "Shrink To Fit");
    scaling->AddItem("Crop", "Crop");

    // Add the custom option to the option list.
    printDetailedOptions->DisplayedOptions->Append("scaling");

    // Set default orientation to landscape.
    printTask->Options->Orientation = PrintOrientation::Landscape;

    // Register for print task option changed notifications.
    printDetailedOptions->OptionChanged += ref new TypedEventHandler<PrintTaskOptionDetails^, PrintTaskOptionChangedEventArgs^>(this, &PhotosPrintHelper::PrintDetailedOptionsOptionChanged);

    // Print Task event handler is invoked when the print job is completed.
    printTask->Completed += ref new TypedEventHandler<PrintTask^, PrintTaskCompletedEventArgs^>([=](PrintTask^ sender, PrintTaskCompletedEventArgs^ e)
    {
        auto callback = ref new Windows::UI::Core::DispatchedHandler([=]()
        {
            ClearPageCollection();

            // Reset image options to default values.
            m_photoScale = Scaling::ShrinkToFit;
            m_photoSize = PhotoSize::SizeFullPage;

            // Reset the current page description ( modify any size)
            m_currentPageDescription = PageDescription();

            // Notify the user when the print operation fails.
            if (e->Completion == Windows::Graphics::Printing::PrintTaskCompletion::Failed)
            {
                MainPage::Current->NotifyUser(ref new String(L"Failed to print."), NotifyType::ErrorMessage);
            }
        });

        ScenarioPage->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, callback);
    });
}

/// <summary>
/// Option change event handler
/// </summary>
/// <param name="sender">The print task option details for which an option changed.</param>
/// <param name="args">The event arguments containing the id of the changed option.</param>
void PhotosPrintHelper::PrintDetailedOptionsOptionChanged(PrintTaskOptionDetails^ sender, PrintTaskOptionChangedEventArgs^ args)
{
    bool invalidatePreview = false;

    // For this scenario we are interested only when the 2 custom options change (photoSize & scaling) in order to trigger a preview refresh.
    // Default options that change page aspect will trigger preview invalidation (refresh) automatically.
    // It is safe to ignore verifying other options and(or) combinations here because during Paginate event(CreatePrintPreviewPages) we check if the PageDescription changed.
    if (args->OptionId == nullptr)
    {
        return;
    }

    String^ optionId = safe_cast<String^>(args->OptionId);
    if (optionId == "photoSize")
    {
        IPrintOptionDetails^ photoSizeOption = sender->Options->Lookup(optionId);
        String^ photoSizeValue = safe_cast<String^>(photoSizeOption->Value);

        if (photoSizeValue == "SizeFullPage")
        {
            m_photoSize = PhotoSize::SizeFullPage;
        }
        else if (photoSizeValue == "Size4x6")
        {
            m_photoSize = PhotoSize::Size4x6;
        }
        else if (photoSizeValue == "Size5x7")
        {
            m_photoSize = PhotoSize::Size5x7;
        }
        else if (photoSizeValue == "Size8x10")
        {
            m_photoSize = PhotoSize::Size8x10;
        }
        invalidatePreview = true;
    }

    if (optionId == "scaling")
    {
        IPrintOptionDetails^ scalingOption = sender->Options->Lookup(optionId);
        String^ scalingValue = safe_cast<String^>(scalingOption->Value);

        if (scalingValue == "Crop")
        {
            m_photoScale = Scaling::Crop;
        }
        if (scalingValue == "ShrinkToFit")
        {
            m_photoScale = Scaling::ShrinkToFit;
        }
        invalidatePreview = true;
    }

    // Invalidate the preview if one of the 2 options (photoSize, scaling) changed.
    if (invalidatePreview)
    {
        RefreshPreview();
    }
}

/// <summary>
/// This is the event handler for Pagination.
/// </summary>
/// <param name="sender">The document for which pagination occurs.</param>
/// <param name="e">The pagination event arguments containing the print options.</param>
void PhotosPrintHelper::CreatePrintPreviewPages(Object^ sender, PaginateEventArgs^ e)
{
    // A new "session" starts with each paginate event.
    InterlockedIncrement64(&m_requestCount);

    // Get printer's page description.
    PrintTaskOptionDetails^ printDetailedOptions = PrintTaskOptionDetails::GetFromPrintTaskOptions(e->PrintTaskOptions);
    PrintPageDescription printPageDescription = e->PrintTaskOptions->GetPageDescription(0);

    // Reset the error state
    printDetailedOptions->Options->Lookup("photoSize")->ErrorText = "";

    // Compute the printing page description (page size & center printable area)
    PageDescription pageDescription;
    pageDescription.PageSize = printPageDescription.PageSize;

    pageDescription.Margin.Width = (std::max)(printPageDescription.ImageableRect.Left,
        printPageDescription.ImageableRect.Right - printPageDescription.PageSize.Width);

    pageDescription.Margin.Height = (std::max)(printPageDescription.ImageableRect.Top,
        printPageDescription.ImageableRect.Bottom - printPageDescription.PageSize.Height);

    pageDescription.ViewablePageSize.Width = printPageDescription.PageSize.Width - pageDescription.Margin.Width * 2;
    pageDescription.ViewablePageSize.Height = printPageDescription.PageSize.Height - pageDescription.Margin.Height * 2;

    // Compute print photo area.
    switch (m_photoSize)
    {
    case PhotoSize::Size4x6:
        pageDescription.PictureViewSize.Width = 4.0f * DPI96;
        pageDescription.PictureViewSize.Height = 6.0f * DPI96;
        break;
    case PhotoSize::Size5x7:
        pageDescription.PictureViewSize.Width = 5.0f * DPI96;
        pageDescription.PictureViewSize.Height = 7.0f * DPI96;
        break;
    case PhotoSize::Size8x10:
        pageDescription.PictureViewSize.Width = 8.0f * DPI96;
        pageDescription.PictureViewSize.Height = 10.0f * DPI96;
        break;
    case PhotoSize::SizeFullPage:
        pageDescription.PictureViewSize.Width = pageDescription.ViewablePageSize.Width;
        pageDescription.PictureViewSize.Height = pageDescription.ViewablePageSize.Height;
        break;
    }

    // Try to maximize the photo's size based on its aspect-ratio.
    if ((pageDescription.ViewablePageSize.Width > pageDescription.ViewablePageSize.Height) && (m_photoSize != PhotoSize::SizeFullPage))
    {
        auto swap = pageDescription.PictureViewSize.Width;
        pageDescription.PictureViewSize.Width = pageDescription.PictureViewSize.Height;
        pageDescription.PictureViewSize.Height = swap;
    }

    pageDescription.IsContentCropped = m_photoScale == Scaling::Crop;

    // Recreate content only when :
    // - there is no current page description
    // - the current page description doesn't match the new one
    if (m_currentPageDescription != pageDescription)
    {
        ClearPageCollection();
        PrintDocument^ printDocument = safe_cast<PrintDocument^>(sender);

        if (pageDescription.PictureViewSize.Width > pageDescription.ViewablePageSize.Width ||
            pageDescription.PictureViewSize.Height > pageDescription.ViewablePageSize.Height)
        {
            printDetailedOptions->Options->Lookup("photoSize")->ErrorText = "Photo doesn’t fit on the selected paper";

            // Inform preview that it has only 1 page to show.
            printDocument->SetPreviewPageCount(1, PreviewPageCountType::Intermediate);

            // Add a custom "preview" unavailable page
            m_csPrintSync.lock();
            m_pageCollection[0] = ref new PreviewUnavailable(pageDescription.PageSize, pageDescription.ViewablePageSize);
            m_csPrintSync.unlock();
        }
        else
        {
            // Inform preview that is has #NumberOfPhotos pages to show.
            printDocument->SetPreviewPageCount(NumberOfPhotos, PreviewPageCountType::Intermediate);
        }

        m_currentPageDescription = pageDescription;
    }
}

/// <summary>
/// This is the event handler for PrintDocument.GetPrintPreviewPage. It provides a specific print page preview,
/// in the form of an UIElement, to an instance of PrintDocument.
/// PrintDocument subsequently converts the UIElement into a page that the Windows print system can deal with.
/// </summary>
/// <param name="sender">The print documet.</param>
/// <param name="e">Arguments containing the requested page preview.</param>
void PhotosPrintHelper::GetPrintPreviewPage(Object^ sender, GetPreviewPageEventArgs^ e)
{
    PrintDocument^ printDocument = safe_cast<PrintDocument^>(sender);

    // Store a local copy of the request count to use later to determine if the computed page is out of date.
    // If the page preview is unavailable an async operation will generate the content.
    // When the operation completes there is a chance that a pagination request was already made therefore making this page outdated.
    // If the page is outdated throw away the result (don't call SetPreviewPage) since a new GetPrintPreviewPage will server that request.
    LONGLONG requestNumber = 0;
    InterlockedExchange64(&requestNumber, m_requestCount);
    int pageNumber = e->PageNumber;

    UIElement^ page;

    // Try to get the page if it was previously generated.
    m_csPrintSync.lock();
    auto result = m_pageCollection.find(pageNumber - 1);
    if (result != m_pageCollection.end())
    {
        page = result->second;
    }
    m_csPrintSync.unlock();


    // Detect if the page is available.
    if (page != nullptr)
    {
        // Send the page to preview.
        printDocument->SetPreviewPage(pageNumber, page);
    }
    else
    {
        // The page is not available yet.
        GeneratePageAsync(pageNumber, m_currentPageDescription).then([=](UIElement^ page)
        {
            // If the ticket changed discard the result since the content is outdated.
            LONGLONG localRequestNumber = requestNumber;
            if (InterlockedCompareExchange64(&localRequestNumber, requestNumber, m_requestCount) == m_requestCount)
            {
                // Store the page in the collection in case a pagination request is made but the content doesn't need to be regenerated.
                m_csPrintSync.lock();
                m_pageCollection[pageNumber - 1] = page;

                // Add the newly created page to the printing root which is part of the visual tree and force it to go
                // through layout so that the linked containers correctly distribute the content inside them.
                PrintCanvas->Children->Append(page);
                PrintCanvas->InvalidateMeasure();
                PrintCanvas->UpdateLayout();
                m_csPrintSync.unlock();

                // Send the page to preview.
                printDocument->SetPreviewPage(pageNumber, page);
            }
        });
    }
}

/// <summary>
/// This is the event handler for PrintDocument.AddPages. It provides all pages to be printed, in the form of
/// UIElements, to an instance of PrintDocument. PrintDocument subsequently converts the UIElements
/// into a pages that the Windows print system can deal with.
/// </summary>
/// <param name="sender">The print document.</param>
/// <param name="e">Arguments containing the print task options.</param>
void PhotosPrintHelper::AddPrintPages(Object^ sender, AddPagesEventArgs^ e)
{
    PrintDocument^ printDocument = safe_cast<PrintDocument^>(sender);

    // A list of tasks that will generate content for pages that are not ready at this point.
    std::vector<concurrency::task<void>> createPageTasks;

    for (int i = 0; i < NumberOfPhotos; ++i)
    {
        UIElement^ page = nullptr;

        m_csPrintSync.lock();
        auto value = m_pageCollection.find(i);
        if (value != m_pageCollection.end())
        {
            page = value->second;
        }
        m_csPrintSync.unlock();

        if (page != nullptr)
        {
            m_pageCollection[i] = page;
        }

        // If the page is not ready create a task that will generate its content.
        createPageTasks.push_back(GeneratePageAsync(i + 1, m_currentPageDescription).then([=](UIElement^ generatedPage)
        {
            m_pageCollection[i] = generatedPage;
        }));
    }

    // Add the pages to the print document when content has been generated for all of them.
    concurrency::when_all(createPageTasks.begin(), createPageTasks.end()).then([=]
    {
        std::for_each(m_pageCollection.begin(), m_pageCollection.end(), [=](std::pair<int, UIElement^> keyValue)
        {
            printDocument->AddPage(keyValue.second);
        });

        // Indicate that all of the print pages have been provided.
        printDocument->AddPagesComplete();
    });

    // Reset the current page description as soon as possible since the PrintTask.Completed event might fire later (long running job)
    m_currentPageDescription = PageDescription();
}

/// <summary>
/// Helper function used to triger a preview refresh
/// </summary>
void PhotosPrintHelper::RefreshPreview()
{
    // Invalidate the preview in order to refresh cotent and trigger a pagination request.
    auto callback = ref new Windows::UI::Core::DispatchedHandler([this]()
    {
        CurrentPrintDocument->InvalidatePreview();
    });

    ScenarioPage->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, callback);
}

/// <summary>
/// Helper function that clears the page collection and also the pages attached to the "visual root".
/// </summary>
void PhotosPrintHelper::ClearPageCollection()
{
    m_csPrintSync.lock();

    m_pageCollection.clear();
    PrintCanvas->Children->Clear();

    m_csPrintSync.unlock();
}

/// <summary>
/// Generic swap of 2 values
/// </summary>
/// <typeparam name="T">typename</typeparam>
/// <param name="v1">Value 1</param>
/// <param name="v2">Value 2</param>
template<class T> static void PhotosPrintHelper::Swap(T& v1, T& v2)
{
    T swap = v1;
    v1 = v2;
    v2 = swap;
}

/// <summary>
/// Generates a page containing a photo.
/// The image will be rotated if detected that there is a gain from that regarding size (try to maximize photo size).
/// </summary>
/// <param name="photoNumber">The photo number.</param>
/// <param name="pageDescription">The description of the printer page.</param>
/// <returns>A task that will return the page.</returns>
concurrency::task<UIElement^> PhotosPrintHelper::GeneratePageAsync(int photoNumber, PageDescription pageDescription)
{
    Canvas^ page = ref new Canvas();
    page->Width = pageDescription.PageSize.Width;
    page->Height = pageDescription.PageSize.Height;

    Canvas^ viewablePage = ref new Canvas();
    viewablePage->Width = pageDescription.ViewablePageSize.Width;
    viewablePage->Height = pageDescription.ViewablePageSize.Height;

    viewablePage->SetValue(Canvas::LeftProperty, pageDescription.Margin.Width);
    viewablePage->SetValue(Canvas::TopProperty, pageDescription.Margin.Height);

    // The image "frame" which also acts as a viewport
    Grid^ photoView = ref new Grid();
    photoView->Width = pageDescription.PictureViewSize.Width;
    photoView->Height = pageDescription.PictureViewSize.Height;

    // Center the frame.
    photoView->SetValue(Canvas::LeftProperty, (viewablePage->Width - photoView->Width) / 2);
    photoView->SetValue(Canvas::TopProperty, (viewablePage->Height - photoView->Height) / 2);

    std::wostringstream imageUriStream;
    imageUriStream << "ms-appx:///Assets/photo" << photoNumber << ".jpg";

    String^ imageUri = ref new String(imageUriStream.str().data());

    // Return an async task that will complete when the image is fully loaded.
    return LoadBitmapAsync(ref new Uri(imageUri), pageDescription.PageSize.Width > pageDescription.PageSize.Height).then([=](WriteableBitmap^ bitmap) -> UIElement^
    {
        if (bitmap != nullptr)
        {
            Image^ image = ref new Image();
            image->Source = bitmap;
            image->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Center;
            image->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Center;

            // Use the real image size when croping or if the image is smaller then the target area (prevent a scale-up).
            if (m_photoScale == Scaling::Crop || (bitmap->PixelWidth <= pageDescription.PictureViewSize.Width &&
                bitmap->PixelHeight <= pageDescription.PictureViewSize.Height))
            {
                image->Stretch = Stretch::None;
                image->Width = bitmap->PixelWidth;
                image->Height = bitmap->PixelHeight;
            }

            // Add the newly created image to the visual root.
            photoView->Children->Append(image);
            viewablePage->Children->Append(photoView);
            page->Children->Append(viewablePage);
        }

        // Return the page with the image centered.
        return page;
    });
}

/// <summary>
/// Loads an image from an uri source and performs a rotation based on the print target aspect.
/// </summary>
/// <param name="source">The location of the image.</param>
/// <param name="landscape">A flag that indicates if the target (printer page) is in landscape mode.</param>
/// <returns>A task that will return the loaded bitmap.</returns>
concurrency::task<WriteableBitmap^> PhotosPrintHelper::LoadBitmapAsync(Uri^ source, bool landscape)
{
    return concurrency::create_task(StorageFile::GetFileFromApplicationUriAsync(source)).then([](StorageFile^ file)
    {
        return file->OpenAsync(FileAccessMode::Read);
    }).then([](IRandomAccessStream^ stream)
    {
        return BitmapDecoder::CreateAsync(stream);
    }).then([landscape](BitmapDecoder^ bitmapDecoder)
    {
        BitmapTransform^ transform = ref new BitmapTransform();
        transform->Rotation = BitmapRotation::None;

        unsigned int width = bitmapDecoder->PixelWidth;
        unsigned int height = bitmapDecoder->PixelHeight;

        // Rotate the image to match the printing target orientation.
        if (landscape && width < height)
        {
            transform->Rotation = BitmapRotation::Clockwise270Degrees;
            Swap(width, height);
        }
        else if (!landscape && width > height)
        {
            transform->Rotation = BitmapRotation::Clockwise90Degrees;
            Swap(width, height);
        }

        return concurrency::create_task(bitmapDecoder->GetPixelDataAsync(
            BitmapPixelFormat::Bgra8,    // WriteableBitmap uses BGRA format.
            BitmapAlphaMode::Straight,
            transform,
            ExifOrientationMode::IgnoreExifOrientation,    // This sample ignores Exif orientation.
            ColorManagementMode::DoNotColorManage)).then([width, height](PixelDataProvider^ pixelData) -> WriteableBitmap^
        {
            WriteableBitmap^ bitmap = ref new WriteableBitmap(static_cast<int>(width), static_cast<int>(height));

            IBuffer^ bitmapBuffer = bitmap->PixelBuffer;
            Microsoft::WRL::ComPtr<IUnknown> spUnknown = reinterpret_cast<IUnknown*>(bitmapBuffer);
            Microsoft::WRL::ComPtr<IBufferByteAccess> spBufferAcces;

            if (SUCCEEDED(spUnknown.As(&spBufferAcces)))
            {
                byte *buffer;
                if (SUCCEEDED(spBufferAcces->Buffer(&buffer)))
                {
                    // Copy the pixels in the WriteableBitmap.
                    auto pixelDataBuffer = pixelData->DetachPixelData();
                    memcpy_s(buffer, bitmapBuffer->Length, pixelDataBuffer->begin(), pixelDataBuffer->Length);
                }
            }

            return bitmap;
        });
    });
}

Scenario5Photos::Scenario5Photos()
{
    InitializeComponent();
}

void Scenario5Photos::OnPrintButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    printHelper->ShowPrintUIAsync();
}

void Scenario5Photos::OnNavigatedTo(NavigationEventArgs^ e)
{
    if (PrintManager::IsSupported())
    {
        // Tell the user how to print
        MainPage::Current->NotifyUser("Print contract registered with customization, use the Print button to print.", NotifyType::StatusMessage);
    }
    else
    {
        // Remove the print button
        InvokePrintingButton->Visibility = Windows::UI::Xaml::Visibility::Collapsed;

        // Inform user that Printing is not supported
        MainPage::Current->NotifyUser("Printing is not supported.", NotifyType::ErrorMessage);

        // Printing-related event handlers will never be called if printing
        // is not supported, but it's okay to register for them anyway.
    }

    // Initalize common helper class and register for printing
    printHelper = ref new PhotosPrintHelper(this);
    printHelper->RegisterForPrinting();
}

void Scenario5Photos::OnNavigatedFrom(NavigationEventArgs^ e)
{
    if (printHelper)
    {
        printHelper->UnregisterForPrinting();
    }
}
