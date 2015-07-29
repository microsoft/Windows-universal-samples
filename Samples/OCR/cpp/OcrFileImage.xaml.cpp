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
#include "OcrFileImage.xaml.h"
#include <ppltasks.h>

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

OcrFileImage::OcrFileImage() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// Updates OCR available languages and loads sample image.
/// </summary>
/// <param name="e"></param>
void OcrFileImage::OnNavigatedTo(NavigationEventArgs^ e)
{
    UpdateAvailableLanguages();
    LoadSampleImage();
}

/// <summary>
/// Updates list of ocr languages available on device.
/// </summary>
void OcrFileImage::UpdateAvailableLanguages()
{
    if (!UserLanguageToggle->IsOn)
    {
        // Check if any OCR language is available on device.
        if (OcrEngine::AvailableRecognizerLanguages->Size > 0)
        {
            auto languages = ref new Vector<String^>();
            for (const auto& lang : OcrEngine::AvailableRecognizerLanguages)
            {
                languages->Append(lang->DisplayName);
                languageMap[lang->DisplayName] = lang;
            }

            LanguageList->ItemsSource = languages;
            LanguageList->SelectedIndex = 0;
            LanguageList->IsEnabled = true;
        }
        else
        {
            // Prevent OCR if no OCR languages are available on device.
            UserLanguageToggle->IsEnabled = false;
            LanguageList->IsEnabled = false;
            ExtractButton->IsEnabled = false;

            rootPage->NotifyUser("No available OCR languages.", NotifyType::ErrorMessage);
        }
    }
    else
    {
        LanguageList->ItemsSource = nullptr;
        LanguageList->IsEnabled = false;

        rootPage->NotifyUser(
            "Run OCR in first OCR available language from UserProfile.GlobalizationPreferences.Languages list.",
            NotifyType::StatusMessage);
    }
}

/// <summary>
/// Loads image from file to bitmap and displays it in UI.
/// </summary>
/// <param name="file"></param>
/// <returns></returns>
task<void> OcrFileImage::LoadImage(StorageFile^ file)
{
    return create_task(file->OpenAsync(FileAccessMode::Read)).then([this](IRandomAccessStream^ stream)
    {
        return BitmapDecoder::CreateAsync(stream);
    }).then([this](BitmapDecoder^ decoder) -> IAsyncOperation<SoftwareBitmap^>^
    {
        return decoder->GetSoftwareBitmapAsync();
    }).then([this](SoftwareBitmap^ imageBitmap)
    {
        WriteableBitmap^ wb = ref new WriteableBitmap(imageBitmap->PixelWidth, imageBitmap->PixelHeight);
        imageBitmap->CopyToBuffer(wb->PixelBuffer);
        PreviewImage->Source = wb;

        this->bitmap = imageBitmap;
    });
}

void OcrFileImage::LoadSampleImage()
{
    // Load sample "Hello World" image.
    auto getFileOp = Windows::ApplicationModel::Package::Current->InstalledLocation->GetFileAsync("Assets\\splash-sdk.png");
    create_task(getFileOp).then([this](StorageFile^ file)
    {
        return LoadImage(file);
    }).then([this]()
    {
        // Use dispatcher to update UI.
        Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
        {
            rootPage->NotifyUser("Loaded sample image.", NotifyType::StatusMessage);
        }));
    });
}

/// <summary>
/// Clears extracted text and text overlay from previous OCR.
/// </summary>
void OcrFileImage::ClearResults()
{
    isAngleDetected = false;
    TextOverlay->RenderTransform = nullptr;
    ExtractedTextBox->Text = "";
    TextOverlay->Children->Clear();
    wordBoxes.clear();
}

/// <summary>
///  Update word box transform to match current UI size.
/// </summary>
void OcrFileImage::UpdateWordBoxTransform()
{
    // Used for text overlay.
    // Prepare scale transform for words since image is not displayed in original size.
    ScaleTransform^ scaleTrasform = ref new ScaleTransform();
    scaleTrasform->CenterX = 0;
    scaleTrasform->CenterY = 0;
    scaleTrasform->ScaleX = PreviewImage->ActualWidth / bitmap->PixelWidth;
    scaleTrasform->ScaleY = PreviewImage->ActualHeight / bitmap->PixelHeight;

    for (auto& item : wordBoxes)
    {
        item->Transform(scaleTrasform);
    }
}

/// <summary>
/// This is event handler for 'Extract' button.
/// Recognizes text from image and displays it.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void OcrFileImage::ExtractButton_Tapped(Platform::Object^ sender, TappedRoutedEventArgs^ e)
{
    ClearResults();

    if (bitmap->PixelWidth > static_cast<int>(OcrEngine::MaxImageDimension) || bitmap->PixelHeight > static_cast<int>(OcrEngine::MaxImageDimension))
    {
        rootPage->NotifyUser(
            "Bitmap dimensions (" + bitmap->PixelWidth + "x" + bitmap->PixelHeight + ") are too big for OCR. " +
                "Max image dimension is " + OcrEngine::MaxImageDimension + ".",
            NotifyType::ErrorMessage);

        return;
    }

    OcrEngine^ ocrEngine = nullptr;

    if (UserLanguageToggle->IsOn)
    {
        // Try to create OcrEngine for first supported language from UserProfile.GlobalizationPreferences.Languages list.
        // If none of the languages are available on device, method returns null.
        ocrEngine = OcrEngine::TryCreateFromUserProfileLanguages();
    }
    else
    {
        // Try to create OcrEngine for specified language.
        // If language is not supported on device, method returns null.
        Windows::Globalization::Language^ language = languageMap[safe_cast<String^>(LanguageList->SelectedValue)];
        ocrEngine = OcrEngine::TryCreateFromLanguage(language);
    }

    if (ocrEngine != nullptr)
    {
        // Recognize text from image.
        auto recognizeOp = ocrEngine->RecognizeAsync(bitmap);
        create_task(recognizeOp).then([this, ocrEngine](OcrResult^ ocrResult)
        {
            // Display recognized text.
            ExtractedTextBox->Text = ocrResult->Text;

            if (ocrResult->TextAngle != nullptr)
            {
                // If text is detected under some angle in this sample scenario we want to
                // overlay word boxes over original image, so we rotate overlay boxes.
                RotateTransform^ rotateTransform = ref new RotateTransform();
                rotateTransform->Angle = ocrResult->TextAngle->Value;
                rotateTransform->CenterX = PreviewImage->ActualWidth / 2;
                rotateTransform->CenterY = PreviewImage->ActualHeight / 2;
                TextOverlay->RenderTransform = rotateTransform;

                isAngleDetected = true;
            }

            // Create overlay boxes over recognized words.
            for (const auto& line : ocrResult->Lines)
            {
                Rect lineRect = line->Words->GetAt(0u)->BoundingRect;
                for (const auto& word : line->Words)
                {
                    lineRect.Union(word->BoundingRect);
                }

                // Determine if line is horizontal or vertical.
                // Vertical lines are supported only in Chinese Traditional and Japanese languages.
                bool isVerticalLine = lineRect.Height > lineRect.Width;

                for (const auto& word : line->Words)
                {
                    WordOverlay^ wordBoxOverlay = ref new WordOverlay(word);

                    // Keep references to word boxes.
                    wordBoxes.push_back(wordBoxOverlay);

                    // Define overlay style.
                    auto overlay = ref new Border();

                    if (isVerticalLine)
                    {
                        overlay->Style = (Windows::UI::Xaml::Style^) this->Resources->Lookup("HighlightedWordBoxVerticalLine");
                    }
                    else
                    {
                        overlay->Style = (Windows::UI::Xaml::Style^) this->Resources->Lookup("HighlightedWordBoxHorizontalLine");
                    }

                    // Bind word boxes to UI.
                    overlay->SetBinding(FrameworkElement::MarginProperty, wordBoxOverlay->CreateWordPositionBinding());
                    overlay->SetBinding(FrameworkElement::WidthProperty, wordBoxOverlay->CreateWordWidthBinding());
                    overlay->SetBinding(FrameworkElement::HeightProperty, wordBoxOverlay->CreateWordHeightBinding());

                    // Put the filled textblock in the results grid.
                    TextOverlay->Children->Append(overlay);
                }
            }

            // Rescale word boxes to match current UI size.
            UpdateWordBoxTransform();

            // Use dispatcher to update UI.
            Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, ocrEngine]()
            {
                rootPage->NotifyUser(
                    "Image is OCRed for " + ocrEngine->RecognizerLanguage->DisplayName + " language.",
                    NotifyType::StatusMessage);
            }));
            
        });
    }
    else
    {
        rootPage->NotifyUser("Selected language is not available.", NotifyType::ErrorMessage);
    }
}

/// <summary>
/// Occures when user language toogle state is changed.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void OcrFileImage::UserLanguageToggle_Toggled(Platform::Object^ sender, RoutedEventArgs^ e)
{
    UpdateAvailableLanguages();
}

/// <summary>
/// This is event handler for 'Sample' button.
/// It loads image with 'Hello World' text and displays it in UI.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void OcrFileImage::SampleButton_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
    ClearResults();

    // Load sample "Hello World" image.
    LoadSampleImage();
}

/// <summary>
/// This is event handler for 'Load' button.
/// It opens file picked and load selected image in UI.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void OcrFileImage::LoadButton_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
    FileOpenPicker^ picker = ref new FileOpenPicker();
    picker->FileTypeFilter->Append(".jpg");
    picker->FileTypeFilter->Append(".jpeg");
    picker->FileTypeFilter->Append(".png");
    picker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;

    create_task(picker->PickSingleFileAsync()).then([this](StorageFile^ file)
    {
        if (file != nullptr)
        {
            ClearResults();

            create_task(LoadImage(file)).then([this, file]()
            {
                // Use dispatcher to update UI.
                Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, file]()
                {
                    rootPage->NotifyUser("Loaded image from file: " + file->Name + ".", NotifyType::StatusMessage);
                }));
            });
        }
    });
}

/// <summary>
/// Occures when selected language is changed in available languages combo box.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void OcrFileImage::LanguageList_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
    ClearResults();

    auto lang = safe_cast<String^>(LanguageList->SelectedValue);
    if (lang != nullptr)
    {
        rootPage->NotifyUser(
            "Selected OCR language is " + lang + ".\n" +
            OcrEngine::AvailableRecognizerLanguages->Size + " OCR language(s) are available.\n" +
            "Check combo box for full list.",
            NotifyType::StatusMessage);
    }
}

/// <summary>
/// Occurs when displayed image size changes.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void OcrFileImage::PreviewImage_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e)
{
    // Update word overlay boxes.
    UpdateWordBoxTransform();

    // Update image rotation center.
    if (isAngleDetected)
    {
        const auto& rotate = static_cast<RotateTransform^>(TextOverlay->RenderTransform);

        rotate->CenterX = PreviewImage->ActualWidth / 2;
        rotate->CenterY = PreviewImage->ActualHeight / 2;
    }
}