#include "pch.h"
#include "OcrFileImage.h"
#include "OcrFileImage.g.cpp"
#include <sstream>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Globalization;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Media;
using namespace Windows::Media::Ocr;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Navigation;
using namespace SDKTemplate;

namespace winrt::SDKTemplate::implementation
{
    OcrFileImage::OcrFileImage()
    {
        InitializeComponent();
    }

    /// <summary>
       /// Invoked when this page is about to be displayed in a Frame.
       /// Updates OCR available languages and loads sample image.
       /// </summary>
       /// <param name="e"></param>
    fire_and_forget OcrFileImage::OnNavigatedTo(NavigationEventArgs const&)
    {
        auto lifetime = get_strong();

        UpdateAvailableLanguages();

        co_await LoadSampleImageAsync();
    }


    /// <summary>
    /// Updates list of ocr languages available on device.
    /// </summary>
    void OcrFileImage::UpdateAvailableLanguages()
    {
        if (!UserLanguageToggle().IsOn())
        {
            std::vector<IInspectable> items;
            for (auto&& language : OcrEngine::AvailableRecognizerLanguages())
            {
                ComboBoxItem item;
                item.Content(box_value(language.DisplayName()));
                item.Tag(language);
                items.push_back(item);
            }
            // Check if any OCR language is available on device.
            if (items.size() > 0)
            {   
                LanguageList().ItemsSource(winrt::single_threaded_vector<IInspectable>(std::move(items)));
                LanguageList().SelectedIndex(0);
                LanguageList().IsEnabled(true);
            }
            else
            {
                // Prevent OCR if no OCR languages are available on device.
                UserLanguageToggle().IsEnabled(false);
                LanguageList().IsEnabled(false);
                ExtractButton().IsEnabled(false);
                rootPage.NotifyUser(L"No available OCR languages.", NotifyType::ErrorMessage);
            }
        }
        else
        {
            LanguageList().ItemsSource(nullptr);
            LanguageList().IsEnabled(false);

            rootPage.NotifyUser(
                L"Run OCR in first OCR available language from UserProfile.GlobalizationPreferences.Languages list.",
                NotifyType::StatusMessage);
        }
    }

    /// <summary>
    /// Loads image from file to bitmap and displays it in UI.
    /// </summary>
    /// <param name="file"></param>
    /// <returns></returns>
    IAsyncAction OcrFileImage::LoadImageAsync(StorageFile const& file)
    {
        auto lifetime = get_strong();

        auto stream = co_await file.OpenAsync(winrt::Windows::Storage::FileAccessMode::Read);

        auto decoder = co_await BitmapDecoder::CreateAsync(stream);

        bitmap = co_await decoder.GetSoftwareBitmapAsync(BitmapPixelFormat::Bgra8, BitmapAlphaMode::Premultiplied);

        auto imgSource = WriteableBitmap(bitmap.PixelWidth(), bitmap.PixelHeight());
        bitmap.CopyToBuffer(imgSource.PixelBuffer());
        PreviewImage().Source(imgSource);
    }

    IAsyncAction OcrFileImage::LoadSampleImageAsync()
    {
        auto lifetime = get_strong();

        StorageFile file = co_await Windows::ApplicationModel::Package::Current().InstalledLocation().GetFileAsync(L"Assets\\splash-sdk.png");
        co_await LoadImageAsync(file);
    }

    /// <summary>
    /// Clears extracted text and text overlay from previous OCR.
    /// </summary>
    void OcrFileImage::ClearResults()
    {
        TextOverlay().RenderTransform(nullptr);
        ExtractedTextBox().Text({});
        TextOverlay().Children().Clear();
        wordBoxes.clear();
    }

    /// <summary>
    ///  Update word box transform to match current UI size.
    /// </summary>
    void OcrFileImage::UpdateWordBoxTransform()
    {
        // Used for text overlay.
        // Prepare scale transform for words since image is not displayed in original size
        ScaleTransform scaleTransform;
        scaleTransform.CenterX(0);
        scaleTransform.CenterY(0);
        scaleTransform.ScaleX(PreviewImage().ActualWidth() / bitmap.PixelWidth());
        scaleTransform.ScaleY(PreviewImage().ActualHeight() / bitmap.PixelHeight());

        for (auto&& item : wordBoxes)
        {
            item->Transform(scaleTransform);
        }
    }

    /// <summary>
    /// This is event handler for 'Extract' button.
    /// Recognizes text from image and displays it.
    /// </summary>
    /// <param name="e"></param>
    fire_and_forget OcrFileImage::ExtractButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        ClearResults();

        // Check if OcrEngine supports image resolution.

        if (static_cast<uint32_t>(bitmap.PixelWidth()) > OcrEngine::MaxImageDimension() ||
            static_cast<uint32_t>(bitmap.PixelHeight()) > OcrEngine::MaxImageDimension())
        {
            rootPage.NotifyUser(
                L"Bitmap dimensions (" + to_hstring(bitmap.PixelWidth()) + L"x" + to_hstring(bitmap.PixelHeight()) + L"are too big for OCR. Max image dimension is " + to_hstring(OcrEngine::MaxImageDimension()) + L".",
                NotifyType::ErrorMessage);

            return;
        }

        OcrEngine ocrEngine = nullptr;

        if (UserLanguageToggle().IsOn())
        {
            // Try to create OcrEngine for first supported language from UserProfile.GlobalizationPreferences.Languages list.
            // If none of the languages are available on device, method returns null.
            ocrEngine = OcrEngine::TryCreateFromUserProfileLanguages();
        }
        else
        {
            // Try to create OcrEngine for specified language.
            // If language is not supported on device, method returns null.
            auto lang = LanguageList().SelectedValue().as<ComboBoxItem>().Tag().as<Windows::Globalization::Language>();
            ocrEngine = OcrEngine::TryCreateFromLanguage(lang);
        }

        if (ocrEngine != nullptr)
        {
            // Recognize text from image.
            auto ocrResult = co_await ocrEngine.RecognizeAsync(bitmap);

            // Display recognized text.
            ExtractedTextBox().Text(ocrResult.Text());

            auto angle = unbox_value_or<double>(ocrResult.TextAngle(), 0.0);
            if (angle != 0.0)
            {
                // If text is detected under some angle in this sample scenario we want to
                // overlay word boxes over original image, so we rotate overlay boxes.
                
                RotateTransform rotateTransform;

                rotateTransform.Angle(angle);
                rotateTransform.CenterX(PreviewImage().ActualWidth() / 2);
                rotateTransform.CenterY(PreviewImage().ActualWidth() / 2);

                TextOverlay().RenderTransform(rotateTransform);
      
            }

            // Create overlay boxes over recognized words.
            for (auto&& line : ocrResult.Lines())
            {
                // Determine if line is horizontal or vertical.
                // Vertical lines are supported only in Chinese Traditional and Japanese languages.
                Rect lineRect = RectHelper::Empty();
                for (auto&& word : line.Words())
                {
                    lineRect = RectHelper::Union(lineRect, word.BoundingRect());
                }
                bool isVerticalLine = lineRect.Height > lineRect.Width;
                auto style = isVerticalLine ? HighlightedWordBoxVerticalLineStyle() : HighlightedWordBoxHorizontalLineStyle();

                for (auto&& word : line.Words())
                {
                    com_ptr<WordOverlay> wordBoxOverlay = make_self<WordOverlay>(word);

                    // Keep references to word boxes.
                    wordBoxes.push_back(wordBoxOverlay);

                    // Create a box to highlight the word.
                    TextOverlay().Children().Append(wordBoxOverlay->CreateBorder(style));
                }
            }

            // Rescale word boxes to match current UI size.
            UpdateWordBoxTransform();

            rootPage.NotifyUser(
                L"Image is OCRed for " + ocrEngine.RecognizerLanguage().DisplayName() + L" language.",
                NotifyType::StatusMessage);
        }
        else
        {
            rootPage.NotifyUser(L"Selected language is not available.", NotifyType::ErrorMessage);
        }
    }

    /// <summary>
    /// Occures when user language toggle state is changed.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void OcrFileImage::UserLanguageToggle_Toggled(IInspectable const&, RoutedEventArgs const&)
    {
        UpdateAvailableLanguages();
    }

    /// <summary>
    /// This is event handler for 'Sample' button.
    /// It loads sample image and displays it in UI.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    fire_and_forget OcrFileImage::SampleButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        ClearResults();

        co_await LoadSampleImageAsync();

        rootPage.NotifyUser(L"Loaded sample image.", NotifyType::StatusMessage);
    }

    /// <summary>
    /// This is event handler for 'Load' button.
    /// It opens file picked and load selected image in UI..
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    IAsyncAction OcrFileImage::LoadButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        FileOpenPicker picker;
        picker.SuggestedStartLocation(PickerLocationId::PicturesLibrary);
        picker.FileTypeFilter().ReplaceAll({ L".jpg", L".jpeg", L".png" });

        auto file = co_await picker.PickSingleFileAsync();
        if (file != nullptr)
        {
            ClearResults();

            co_await LoadImageAsync(file);

            rootPage.NotifyUser(
                L"Loaded image from file: " + file.Name() + L" (" + to_hstring(bitmap.PixelWidth()) + L"x" + to_hstring(bitmap.PixelHeight()) + L").",
                NotifyType::StatusMessage);
        }
    }

    /// <summary>
    /// Occurs when selected language is changed in available languages combo box.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void OcrFileImage::LanguageList_SelectionChanged(IInspectable const&, RoutedEventArgs const&)
    {
        ClearResults();
    }

    /// <summary>
    /// Occurs when displayed image size changes.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void OcrFileImage::PreviewImage_SizeChanged(IInspectable const&, RoutedEventArgs const&)
    {
        // Update word overlay boxes.
        UpdateWordBoxTransform();

        // Update image rotation center.
        auto rotate = TextOverlay().RenderTransform().try_as<RotateTransform>();
        if (rotate != nullptr)
        {
            rotate.CenterX(PreviewImage().ActualWidth() / 2);
            rotate.CenterY(PreviewImage().ActualHeight() / 2);
        }
    }
}
