//
// CustomTitleBar.xaml.h
// Declaration of the CustomTitleBar class
//

#pragma once

#include "CustomTitleBar.g.h"

namespace SDKTemplate
{
    // The CustomTitleBar class derives from DependencyObject, which has UI thread affinity.
    // Therefore, it is okay to keep a reference to a non-marshalable object in a member variable.
#pragma warning(push)
#pragma warning(disable:4451)

    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class CustomTitleBar sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        CustomTitleBar();

#pragma region Data binding
        virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;

        // The SystemOverlayLeftInset and SystemOverlayRightInset values are
        // in terms of physical left and right. Our OverlayLogicalLeftInset and
        // OverlayLogicalRightInset properties flip the values if the flow direction
        // is RTL.
        property Windows::UI::Xaml::Thickness CoreTitleBarPadding
        {
            Windows::UI::Xaml::Thickness get()
            {
                // The SystemOverlayLeftInset and SystemOverlayRightInset values are
                // in terms of physical left and right. Therefore, we need to flip
                // then when our flow direction is RTL.
                if (FlowDirection == Windows::UI::Xaml::FlowDirection::LeftToRight)
                {
                    return Windows::UI::Xaml::ThicknessHelper::FromLengths(coreTitleBar->SystemOverlayLeftInset, 0, coreTitleBar->SystemOverlayRightInset, 0);
                }
                else
                {
                    return Windows::UI::Xaml::ThicknessHelper::FromLengths(coreTitleBar->SystemOverlayRightInset, 0, coreTitleBar->SystemOverlayLeftInset, 0);
                }
            }
        }

        property double CoreTitleBarHeight
        {
            double get()
            {
                return coreTitleBar->Height;
            }
        }

#pragma endregion

        Windows::UI::Xaml::UIElement^ SetPageContent(Windows::UI::Xaml::UIElement^ newContent);
        void EnableControlsInTitleBar(bool enable);

    private:
        void CustomTitleBar_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args);
        void CustomTitleBar_Unloaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args);
        void OnLayoutMetricsChanged(Windows::ApplicationModel::Core::CoreApplicationViewTitleBar^ sender, Platform::Object^ args);
        void OnIsVisibleChanged(Windows::ApplicationModel::Core::CoreApplicationViewTitleBar^ sender, Platform::Object^ args);
        void OnWindowSizeChanged(Platform::Object^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ args);
        void UpdateLayoutMetrics();
        void UpdatePositionAndVisibility();

    private:
        Windows::ApplicationModel::Core::CoreApplicationViewTitleBar^ coreTitleBar = Windows::ApplicationModel::Core::CoreApplication::GetCurrentView()->TitleBar;
        Windows::UI::Xaml::UIElement^ pageContent = nullptr;

        Windows::Foundation::EventRegistrationToken layoutMetricsChangedToken{};
        Windows::Foundation::EventRegistrationToken isVisibleChangedToken{};
        Windows::Foundation::EventRegistrationToken windowSizeChangedToken{};

    };
#pragma warning(pop)
}
