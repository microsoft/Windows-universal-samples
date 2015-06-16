//
// CustomTitleBar.xaml.cpp
// Implementation of the CustomTitleBar class
//

#include "pch.h"
#include "CustomTitleBar.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

CustomTitleBar::CustomTitleBar()
{
    InitializeComponent();
}

void CustomTitleBar::CustomTitleBar_Loaded(Object^ sender, RoutedEventArgs^ args)
{
    layoutMetricsChangedToken = coreTitleBar->LayoutMetricsChanged += ref new TypedEventHandler<CoreApplicationViewTitleBar^, Object^>(this, &CustomTitleBar::OnLayoutMetricsChanged);
    isVisibleChangedToken = coreTitleBar->IsVisibleChanged += ref new TypedEventHandler<CoreApplicationViewTitleBar^, Object^>(this, &CustomTitleBar::OnIsVisibleChanged);

    // The SizeChanged event is raised when the view enters or exits full screen mode.
    windowSizeChangedToken = Window::Current->SizeChanged += ref new WindowSizeChangedEventHandler(this, &CustomTitleBar::OnWindowSizeChanged);

    UpdateLayoutMetrics();
    UpdatePositionAndVisibility();
}

void CustomTitleBar::CustomTitleBar_Unloaded(Object^ sender, RoutedEventArgs^ args)
{
    coreTitleBar->LayoutMetricsChanged -= layoutMetricsChangedToken;
    coreTitleBar->IsVisibleChanged -= isVisibleChangedToken;
    Window::Current->SizeChanged -= windowSizeChangedToken;
}

void CustomTitleBar::OnLayoutMetricsChanged(CoreApplicationViewTitleBar^ sender, Object^ args)
{
    UpdateLayoutMetrics();
}

void CustomTitleBar::UpdateLayoutMetrics()
{
    PropertyChanged(this, ref new PropertyChangedEventArgs("CoreTitleBarHeight"));
    PropertyChanged(this, ref new PropertyChangedEventArgs("CoreTitleBarPadding"));
}

void CustomTitleBar::OnIsVisibleChanged(CoreApplicationViewTitleBar^ sender, Object^ args)
{
    UpdatePositionAndVisibility();
}

void CustomTitleBar::OnWindowSizeChanged(Object^ sender, WindowSizeChangedEventArgs^ args)
{
    UpdatePositionAndVisibility();
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
    if (ApplicationView::GetForCurrentView()->IsFullScreenMode)
    {
        // In full screen mode, the title bar overlays the content.
        // and might or might not be visible.
        TitleBar->Visibility = coreTitleBar->IsVisible ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;
        Grid::SetRow(TitleBar, 1);
    }
    else
    {
        // When not in full screen mode, the title bar is visible and does not overlay content.
        TitleBar->Visibility = Windows::UI::Xaml::Visibility::Visible;
        Grid::SetRow(TitleBar, 0);
    }
}

UIElement^ CustomTitleBar::SetPageContent(UIElement^ newContent)
{
    UIElement^ oldContent = pageContent;
    if (oldContent != nullptr)
    {
        pageContent = nullptr;
        RootGrid->Children->RemoveAtEnd();
    }
    pageContent = newContent;
    if (newContent != nullptr)
    {
        RootGrid->Children->Append(newContent);
        // The page content is row 1 in our grid. (See diagram above.)
        Grid::SetRow(static_cast<FrameworkElement^>(pageContent), 1);
    }
    return oldContent;
}

void CustomTitleBar::EnableControlsInTitleBar(bool enable)
{
    if (enable)
    {
        TitleBarControl->Visibility = Windows::UI::Xaml::Visibility::Visible;
        // Any clicks on the BackgroundElement will be treated as clicks on the title bar.
        Window::Current->SetTitleBar(BackgroundElement);
    }
    else
    {
        TitleBarControl->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
        Window::Current->SetTitleBar(nullptr);
    }
}

