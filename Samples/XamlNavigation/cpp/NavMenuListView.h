#pragma once

using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;

namespace NavigationMenuSample
{
    namespace Controls
    {
        /// <summary>
        /// A specialized ListView to represent the items in the navigation menu.
        /// </summary>
        /// <remarks>
        /// This class handles the following:
        /// 1. Sizes the panel that hosts the items so they fit in the hosting pane.  Otherwise, the keyboard
        ///    may appear cut off on one side b/c the Pane clips instead of affecting layout.
        /// 2. Provides a single selection experience where keyboard focus can move without changing selection.
        ///    Both the 'Space' and 'Enter' keys will trigger selection.  The up/down arrow keys can move
        ///    keyboard focus without triggering selection.  This is different than the default behavior when
        ///    SelectionMode == Single.  The default behavior for a ListView in single selection requires using
        ///    the Ctrl + arrow key to move keyboard focus without triggering selection.  Users won't expect
        ///    this type of keyboarding model on the nav menu.
        /// </remarks>
        public ref class NavMenuListView sealed : public ListView
        {
        public:
            NavMenuListView();
            event EventHandler<ListViewItem^>^ ItemInvoked;
            void SetSelectedItem(ListViewItem^ item);

        protected:
            virtual void OnApplyTemplate() override;
            virtual void OnKeyDown(KeyRoutedEventArgs^ e) override;

        private:
            SplitView^ _splitViewHost;
            void ItemClickHandler(Object^ sender, ItemClickEventArgs^ e);
            void InvokeItem(Object^ focusedItem);
            void OnLoaded(Object ^sender, RoutedEventArgs ^e);
            void OnPaneToggled();
            void IsOpenPanePropertyChangedCallback(DependencyObject^ sender, DependencyProperty^ args);
            void DisplayModePropertyChangedCallback(DependencyObject^ sender, DependencyProperty^ args);
            void TryMoveFocus(FocusNavigationDirection direction);
        };
    }
}
