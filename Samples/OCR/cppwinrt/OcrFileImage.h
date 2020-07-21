#pragma once

#include "pch.h"
#include "MainPage.h"
#include "WordOverlay.h"
#include "OcrFileImage.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct OcrFileImage : OcrFileImageT<OcrFileImage>
    {
        OcrFileImage();

        fire_and_forget OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        fire_and_forget ExtractButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void UserLanguageToggle_Toggled(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

        fire_and_forget SampleButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        Windows::Foundation::IAsyncAction LoadButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void LanguageList_SelectionChanged(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void PreviewImage_SizeChanged(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        Windows::Foundation::IAsyncAction LoadSampleImageAsync();
        Windows::Foundation::IAsyncAction LoadImageAsync(Windows::Storage::StorageFile const& file);

        void UpdateAvailableLanguages();
        void ClearResults();
        void UpdateWordBoxTransform();

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
        Windows::Graphics::Imaging::SoftwareBitmap bitmap{ nullptr };
        std::vector<com_ptr<WordOverlay>> wordBoxes;

    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct OcrFileImage : OcrFileImageT<OcrFileImage, implementation::OcrFileImage>
    {
    };
}
