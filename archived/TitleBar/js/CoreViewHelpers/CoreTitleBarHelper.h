#pragma once

namespace CoreViewHelpers
{
    ref class CoreTitleBarHelper;

    public interface struct ICoreTitleBarHelper
    {
        event Windows::Foundation::TypedEventHandler<CoreTitleBarHelper^, Platform::Object^>^ IsVisibleChanged;
        event Windows::Foundation::TypedEventHandler<CoreTitleBarHelper^, Platform::Object^>^ LayoutMetricsChanged;
    };

    // The CoreTitleBarHelper class is non-marshalable, so all accesses are from the same thread.
    // Therefore, it is okay to keep a reference to a non-marshalable object in a member variable.
#pragma warning(push)
#pragma warning(disable:4451)

    [Windows::Foundation::Metadata::Threading(Windows::Foundation::Metadata::ThreadingModel::STA)]
    [Windows::Foundation::Metadata::MarshalingBehavior(Windows::Foundation::Metadata::MarshalingType::None)]
    public ref class CoreTitleBarHelper sealed : [Windows::Foundation::Metadata::Default] ICoreTitleBarHelper
    {
    public:
        static CoreTitleBarHelper^ GetForCurrentView();

        property bool ExtendViewIntoTitleBar
        {
            bool get()
            {
                return titleBar->ExtendViewIntoTitleBar;
            }
            void set(bool value)
            {
                titleBar->ExtendViewIntoTitleBar = value;
            }
        }

        property double Height
        {
            double get()
            {
                return titleBar->Height;
            }
        }

        property double IsVisible
        {
            double get()
            {
                return titleBar->IsVisible;
            }
        }

        property double SystemOverlayLeftInset
        {
            double get()
            {
                return titleBar->SystemOverlayLeftInset;
            }
        }

        property double SystemOverlayRightInset
        {
            double get()
            {
                return titleBar->SystemOverlayRightInset;
            }
        }

        virtual event Windows::Foundation::TypedEventHandler<CoreTitleBarHelper^, Platform::Object^>^ IsVisibleChanged;
        virtual event Windows::Foundation::TypedEventHandler<CoreTitleBarHelper^, Platform::Object^>^ LayoutMetricsChanged;

    private:
        Windows::ApplicationModel::Core::CoreApplicationViewTitleBar^ titleBar = Windows::ApplicationModel::Core::CoreApplication::GetCurrentView()->TitleBar;

        /// <summary>
        /// This class is not activatable. Use the GetForCurrentView pattern to obtain an instance.
        /// </summary>
        CoreTitleBarHelper();
        ~CoreTitleBarHelper();

        void OnIsVisibleChanged(Windows::ApplicationModel::Core::CoreApplicationViewTitleBar^ sender, Platform::Object^ args);
        void OnLayoutMetricsChanged(Windows::ApplicationModel::Core::CoreApplicationViewTitleBar^ sender, Platform::Object^ args);

        Windows::Foundation::EventRegistrationToken isVisibleChangedToken{};
        Windows::Foundation::EventRegistrationToken layoutMetricsChangedToken{};

        static Platform::String^ KeyName;
    };
#pragma warning(pop)
}
