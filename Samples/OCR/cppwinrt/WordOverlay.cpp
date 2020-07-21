#include "pch.h"
#include "WordOverlay.h"
#include "WordOverlay.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Media::Ocr;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Data;
using namespace winrt::Windows::UI::Xaml::Media;

namespace winrt::SDKTemplate::implementation
{
    WordOverlay::WordOverlay(OcrWord const& ocrWord)
    {
        word = ocrWord;

        UpdateProps(word.BoundingRect());
    }

    event_token WordOverlay::PropertyChanged(PropertyChangedEventHandler const& handler)
    {
        return m_propertyChanged.add(handler);
    }

    void WordOverlay::PropertyChanged(event_token const& token) noexcept
    {
        m_propertyChanged.remove(token);
    }

    void WordOverlay::Transform(ScaleTransform const& scale)
    {
        // Scale word box bounding rect and update properties.
        UpdateProps(scale.TransformBounds(word.BoundingRect()));
    }

    Border WordOverlay::CreateBorder(Style const& style, UIElement const& child)
    {
        Border overlay;
        overlay.Child(child);
        overlay.Style(style);

        // Bind word boxes to UI.
        overlay.SetBinding(FrameworkElement::MarginProperty(), CreateBinding(L"WordPosition"));
        overlay.SetBinding(FrameworkElement::WidthProperty(), CreateBinding(L"WordWidth"));
        overlay.SetBinding(FrameworkElement::HeightProperty(), CreateBinding(L"WordHeight"));

        return overlay;
    }

    Binding WordOverlay::CreateBinding(hstring const& propertyName)
    {
        Binding positionBinding;

        positionBinding.Path(PropertyPath{ propertyName });
        positionBinding.Source(*this);

        return positionBinding;
    }

    void WordOverlay::UpdateProps(Rect const& wordBoundingBox)
    {
        m_wordBoundingBox = wordBoundingBox;

        OnPropertyChanged(L"WordPosition");
        OnPropertyChanged(L"WordWidth");
        OnPropertyChanged(L"WordHeight");
    }

    void WordOverlay::OnPropertyChanged(hstring const& PropertyName)
    {
        m_propertyChanged(*this, PropertyChangedEventArgs(PropertyName));
    }
}
