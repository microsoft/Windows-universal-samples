#include "pch.h"
#include "CustomTitleBar.h"
#include "CustomTitleBar.g.cpp"

using namespace winrt;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Data;

namespace winrt::SDKTemplate::implementation
{
    CustomTitleBar::CustomTitleBar()
    {
        InitializeComponent();
    }

    // The SystemOverlayLeftInset and SystemOverlayRightInset values are
    // in terms of physical left and right. Our OverlayLogicalLeftInset and
    // OverlayLogicalRightInset properties flip the values if the flow direction
    // is RTL.
    Thickness CustomTitleBar::CoreTitleBarPadding()
    {
        // The SystemOverlayLeftInset and SystemOverlayRightInset values are
        // in terms of physical left and right. Therefore, we need to flip
        // then when our flow direction is RTL.
        if (FlowDirection() == FlowDirection::LeftToRight)
        {
            return ThicknessHelper::FromLengths(coreTitleBar.SystemOverlayLeftInset(), 0, coreTitleBar.SystemOverlayRightInset(), 0);
        }
        else
        {
            return Windows::UI::Xaml::ThicknessHelper::FromLengths(coreTitleBar.SystemOverlayRightInset(), 0, coreTitleBar.SystemOverlayLeftInset(), 0);
        }
    }

    void CustomTitleBar::CustomTitleBar_Loaded(IInspectable const&, RoutedEventArgs const&)
    {
        layoutMetricsChangedToken = coreTitleBar.LayoutMetricsChanged([this](auto&&, auto&&) { UpdateLayoutMetrics(); });
        isVisibleChangedToken = coreTitleBar.IsVisibleChanged([this](auto&&, auto&&) { UpdatePositionAndVisibility(); });

        // The SizeChanged event is raised when the view enters or exits full screen mode.
        windowSizeChangedToken = Window::Current().SizeChanged([this](auto&&, auto&&) { UpdatePositionAndVisibility(); });

        UpdateLayoutMetrics();
        UpdatePositionAndVisibility();
    }

    void CustomTitleBar::CustomTitleBar_Unloaded(IInspectable const&, RoutedEventArgs const&)
    {
        coreTitleBar.LayoutMetricsChanged(layoutMetricsChangedToken);
        coreTitleBar.IsVisibleChanged(isVisibleChangedToken);
        Window::Current().SizeChanged(windowSizeChangedToken);
    }

    void CustomTitleBar::UpdateLayoutMetrics()
    {
        propertyChanged(*this, PropertyChangedEventArgs(L"CoreTitleBarHeight"));
        propertyChanged(*this, PropertyChangedEventArgs(L"CoreTitleBarPadding"));
    }

    // We wrap the normal contents of the MainPage inside a grid
    // so that we can place a title bar on top of it.
    //
    // When not in full screen mode, the grid looks like this:
    //
    //      Row 0: Custom-rendered title bar
    //      Row 1: Rest of content
    //
    // In full screen mode, the the grid looks like this:
    //
    //      Row 0: (empty)
    //      Row 1: Custom-rendered title bar
    //      Row 1: Rest of content
    //
    // The title bar is either Visible or Collapsed, depending on the value of
    // the IsVisible property.
    void CustomTitleBar::UpdatePositionAndVisibility()
    {
        if (ApplicationView::GetForCurrentView().IsFullScreenMode())
        {
            // In full screen mode, the title bar overlays the content.
            // and might or might not be visible.
            TitleBar().Visibility(coreTitleBar.IsVisible() ? Visibility::Visible : Visibility::Collapsed);
            Grid::SetRow(TitleBar(), 1);
        }
        else
        {
            // When not in full screen mode, the title bar is visible and does not overlay content.
            TitleBar().Visibility(Visibility::Visible);
            Grid::SetRow(TitleBar(), 0);
        }
    }

    UIElement CustomTitleBar::SetPageContent(UIElement const& newContent)
    {
        UIElement oldContent = pageContent;
        if (oldContent != nullptr)
        {
            pageContent = nullptr;
            RootGrid().Children().RemoveAtEnd();
        }
        pageContent = newContent;
        if (newContent != nullptr)
        {
            RootGrid().Children().Append(newContent);
            // The page content is row 1 in our grid. (See diagram above.)
            Grid::SetRow(pageContent.as<FrameworkElement>(), 1);
        }
        return oldContent;
    }

    void CustomTitleBar::EnableControlsInTitleBar(bool enable)
    {
        if (enable)
        {
            TitleBarControl().Visibility(Visibility::Visible);
            // Any clicks on the BackgroundElement will be treated as clicks on the title bar.
            Window::Current().SetTitleBar(BackgroundElement());
        }
        else
        {
            TitleBarControl().Visibility(Visibility::Collapsed);
            Window::Current().SetTitleBar(nullptr);
        }
    }
}
