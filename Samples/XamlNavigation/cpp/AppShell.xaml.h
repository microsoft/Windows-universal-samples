//
// AppShell.xaml.h
// Declaration of the AppShell class
//

#pragma once

#include "NavMenuItem.h"
#include "NavMenuListView.h"
#include "AppShell.g.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Navigation;

namespace NavigationMenuSample
{
    /// <summary>
    /// The "chrome" layer of the app that provides top-level navigation with
    /// proper keyboarding navigation.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class AppShell sealed
    {
    public:
        AppShell();
        property Windows::UI::Xaml::Controls::Frame^ AppFrame
        {
            Windows::UI::Xaml::Controls::Frame^ get();
        }
        void OpenNavePane();

    internal:
        /// <summary>
        /// An event to notify listeners when the hamburger button may occlude other content in the app.
        /// The custom "PageHeader" user control is using this.
        /// </summary>
        event TypedEventHandler<AppShell^, Rect>^ TogglePaneButtonRectChanged;
        property Rect TogglePaneButtonRect
        {
            Rect get() { return _togglePaneButtonRect;  }
        private:
            void set(Rect value) { _togglePaneButtonRect = value; }
        }

        static property AppShell^ Current
        {
            AppShell^ get()
            {
                return _current;
            }
        }

    private:
        void OnLoaded(Object ^sender, RoutedEventArgs ^e);
        void AppShell_KeyDown(Object^ sender, KeyRoutedEventArgs^ e);
        void SystemNavigationManager_BackRequested(Object^, Windows::UI::Core::BackRequestedEventArgs^ e);
        void NavMenuList_ItemInvoked(Object^ sender, ListViewItem^ e);
        void OnNavigatingToPage(Object^ sender, NavigatingCancelEventArgs^ e);
        void RootSplitView_PaneClosed(SplitView^ sender, Object^ args);
        void TogglePaneButton_Checked(Object^ sender, RoutedEventArgs^ e);
        void TogglePaneButton_Unchecked(Object^ sender, RoutedEventArgs^ e);
        void CheckTogglePaneButtonSizeChanged();
        void RootSplitViewDisplayModeChangedCallback(DependencyObject^ sender, DependencyProperty^ dp);
        void NavMenuItemContainerContentChanging(ListViewBase^ sender, ContainerContentChangingEventArgs^ args);

        Vector<NavMenuItem^>^ navlist;
        Rect _togglePaneButtonRect;

        static AppShell^ _current;
    };
}
