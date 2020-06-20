#pragma once
#include "pch.h"
#include "MainPage.h"
#include "WordOverlay.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct WordOverlay : WordOverlayT<WordOverlay>
    {
        WordOverlay(Windows::Media::Ocr::OcrWord const& ocrWord);
        Windows::UI::Xaml::Thickness WordPosition() { return { m_wordBoundingBox.X, m_wordBoundingBox.Y, 0, 0 };  }
        double WordWidth() { return m_wordBoundingBox.Width; }
        double WordHeight() { return m_wordBoundingBox.Height; }
        event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(event_token const& token) noexcept;

        void Transform(Windows::UI::Xaml::Media::ScaleTransform const& scale);
        Windows::UI::Xaml::Controls::Border CreateBorder(Windows::UI::Xaml::Style const& style, Windows::UI::Xaml::UIElement const& child = nullptr);

    private:
        Windows::UI::Xaml::Data::Binding CreateBinding(hstring const& propertyName);
        void UpdateProps(Windows::Foundation::Rect const& wordBoundingBox);
        void OnPropertyChanged(hstring const& PropertyName);

    private:
        Windows::Media::Ocr::OcrWord word{ nullptr };
        Windows::Foundation::Rect m_wordBoundingBox;
        event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
    };
}
