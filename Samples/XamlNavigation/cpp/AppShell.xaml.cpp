//
// AppShell.xaml.cpp
// Implementation of the AppShell class
//

#include "pch.h"
#include "BasicPage.xaml.h"
#include "BasicSubPage.xaml.h"
#include "CommandBarPage.xaml.h"
#include "DrillInPage.xaml.h"
#include "NavMenuItem.h"
#include "NavMenuListView.h"
#include "AppShell.xaml.h"

using namespace Platform;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

using namespace NavigationMenuSample::Controls;

namespace NavigationMenuSample
{
    AppShell^ AppShell::_current;

    /// <summary>
    /// Initializes a new instance of the AppShell, sets the static 'Current' reference,
    /// adds callbacks for Back requests and changes in the SplitView's DisplayMode, and
    /// provide the nav menu list with the data to display.
    /// </summary>
    AppShell::AppShell()
    {
        InitializeComponent();

        Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &AppShell::OnLoaded);

        RootSplitView->RegisterPropertyChangedCallback(
            SplitView::DisplayModeProperty,
            ref new DependencyPropertyChangedCallback(this, &AppShell::RootSplitViewDisplayModeChangedCallback));

        SystemNavigationManager::GetForCurrentView()->BackRequested +=
            ref new EventHandler<Windows::UI::Core::BackRequestedEventArgs^>(this, &AppShell::SystemNavigationManager_BackRequested);

        // Declare the top level nav items
        navlist = ref new Vector<NavMenuItem^>();
        navlist->Append(
            ref new NavMenuItem (
                "Basic Page",
                Symbol::Contact,
                TypeName(Views::BasicPage::typeid)));
        navlist->Append(
            ref new NavMenuItem (
                "CommandBar Page",
                Symbol::Edit,
                TypeName(Views::CommandBarPage::typeid)));
        navlist->Append(
            ref new NavMenuItem(
                "Drill In Page",
                Symbol::Favorite,
                TypeName(Views::DrillInPage::typeid)));

        NavMenuList->ItemsSource = navlist;
    }

    Frame^ AppShell::AppFrame::get()
    {
        return frame;
    }

    void AppShell::OnLoaded(Object^ sender, Windows::UI::Xaml::RoutedEventArgs ^e)
    {
        AppShell::_current = this;

        TogglePaneButton->Focus(Windows::UI::Xaml::FocusState::Programmatic);
    }

    /// <summary>
    /// Default keyboard focus movement for any unhandled keyboarding
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void AppShell::AppShell_KeyDown(Object^ sender, KeyRoutedEventArgs^ e)
    {
        FocusNavigationDirection direction = FocusNavigationDirection::None;
        switch (e->Key)
        {
        case Windows::System::VirtualKey::Left:
        case Windows::System::VirtualKey::GamepadDPadLeft:
        case Windows::System::VirtualKey::GamepadLeftThumbstickLeft:
        case Windows::System::VirtualKey::NavigationLeft:
            direction = FocusNavigationDirection::Left;
            break;
        case Windows::System::VirtualKey::Right:
        case Windows::System::VirtualKey::GamepadDPadRight:
        case Windows::System::VirtualKey::GamepadLeftThumbstickRight:
        case Windows::System::VirtualKey::NavigationRight:
            direction = FocusNavigationDirection::Right;
            break;

        case Windows::System::VirtualKey::Up:
        case Windows::System::VirtualKey::GamepadDPadUp:
        case Windows::System::VirtualKey::GamepadLeftThumbstickUp:
        case Windows::System::VirtualKey::NavigationUp:
            direction = FocusNavigationDirection::Up;
            break;

        case Windows::System::VirtualKey::Down:
        case Windows::System::VirtualKey::GamepadDPadDown:
        case Windows::System::VirtualKey::GamepadLeftThumbstickDown:
        case Windows::System::VirtualKey::NavigationDown:
            direction = FocusNavigationDirection::Down;
            break;
        }

        if (direction != FocusNavigationDirection::None)
        {
            Control^ control = dynamic_cast<Control^>(FocusManager::FindNextFocusableElement(direction));
            if (control != nullptr)
            {
                control->Focus(Windows::UI::Xaml::FocusState::Programmatic);
            }
        }
    }

    void AppShell::SystemNavigationManager_BackRequested(Object^ sender, Windows::UI::Core::BackRequestedEventArgs^ e)
    {
        if (!e->Handled && AppFrame->CanGoBack)
        {
            e->Handled = true;
            AppFrame->GoBack();
        }
    }

    /// <summary>
    /// Navigate to the Page for the selected <paramref name="listViewItem"/>.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="listViewItem"></param>
    void AppShell::NavMenuList_ItemInvoked(Object^ sender, ListViewItem^ listViewItem)
    {
        for (NavMenuItem^ i : navlist)
        {
            i->IsSelected = false;
        }

        auto item = (NavMenuItem^)((NavMenuListView^)(sender))->ItemFromContainer(listViewItem);

        if (item != nullptr)
        {
            item->IsSelected = true;
            if (item->DestPage.Name != AppFrame->CurrentSourcePageType.Name)
            {
                AppFrame->Navigate(item->DestPage, item->Arguments);
            }
        }
    }

    void AppShell::OnNavigatingToPage(Object^ sender, NavigatingCancelEventArgs^ e)
    {
        if (e->NavigationMode == NavigationMode::Back)
        {
            NavMenuItem^ item = nullptr;
            for (auto p : navlist)
            {
                if (p->DestPage.Name == e->SourcePageType.Name)
                {
                    item = p;
                    break;
                }
            }

            if (item == nullptr && AppFrame->BackStackDepth > 0)
            {
                // In cases where a page drills into sub-pages then we'll highlight the most recent
                // navigation menu item that appears in the BackStack
                for (auto entry : AppFrame->BackStack)
                {
                    Object^ p = nullptr;
                    for (auto p : navlist)
                    {
                        if (p->DestPage.Name == entry->SourcePageType.Name)
                        {
                            item = p;
                            break;
                        }
                    }
                }
            }

            for (NavMenuItem^ i : navlist)
            {
                i->IsSelected = false;
            }
            if (item != nullptr)
            {
                item->IsSelected = true;
            }

            auto container = (ListViewItem^)NavMenuList->ContainerFromItem(item);

            // While updating the selection state of the item prevent it from taking keyboard focus.  If a
            // user is invoking the back button via the keyboard causing the selected nav menu item to change
            // then focus will remain on the back button.
            if (container != nullptr) container->IsTabStop = false;
            NavMenuList->SetSelectedItem(container);
            if (container != nullptr) container->IsTabStop = true;
        }
    }

    /// <summary>
    /// Public method to allow pages to open SplitView's pane.
    /// Used for custom app shortcuts like navigating left from page's left-most item
    /// </summary>
    void AppShell::OpenNavePane()
    {
        TogglePaneButton->IsChecked = true;
        NavPaneDivider->Visibility = Windows::UI::Xaml::Visibility::Visible;
    }

    // <summary>
    /// Hide divider when nav pane is closed.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    void AppShell::RootSplitView_PaneClosed(SplitView^ sender, Object^ args)
    {
        NavPaneDivider->Visibility = Windows::UI::Xaml::Visibility::Collapsed;

        // Prevent focus from moving to elements when they're not visible on screen
        FeedbackNavPaneButton->IsTabStop = false;
        SettingsNavPaneButton->IsTabStop = false;
    }

    /// <summary>
    /// Callback when the SplitView's Pane is toggled opened.
    /// Restores divider's visibility and ensures that margins around the floating hamburger are correctly set.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void AppShell::TogglePaneButton_Checked(Object^ sender, RoutedEventArgs^ e)
    {
        NavPaneDivider->Visibility = Windows::UI::Xaml::Visibility::Visible;
        CheckTogglePaneButtonSizeChanged();

        FeedbackNavPaneButton->IsTabStop = true;
        SettingsNavPaneButton->IsTabStop = true;
    }

    /// <summary>
    /// Callback when the SplitView's Pane is toggled closed.  When the Pane is not visible
    /// then the floating hamburger may be occluding other content in the app unless it is aware.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void AppShell::TogglePaneButton_Unchecked(Object^ sender, RoutedEventArgs^ e)
    {
        CheckTogglePaneButtonSizeChanged();
    }

    /// <summary>
    /// Ensure that we update the reported size of the TogglePaneButton when the SplitView's
    /// DisplayMode changes.
    /// </summary>
    void AppShell::RootSplitViewDisplayModeChangedCallback(DependencyObject^ sender, DependencyProperty^ dp)
    {
        CheckTogglePaneButtonSizeChanged();
    }

    /// <summary>
    /// Check for the conditions where the navigation pane does not occupy the space under the floating
    /// hamburger button and trigger the event.
    /// </summary>
    void AppShell::CheckTogglePaneButtonSizeChanged()
    {
        if (RootSplitView->DisplayMode == SplitViewDisplayMode::Inline ||
            RootSplitView->DisplayMode == SplitViewDisplayMode::Overlay)
        {
            auto transform = TogglePaneButton->TransformToVisual(this);
            auto rect = transform->TransformBounds(Rect(0, 0, (float)TogglePaneButton->ActualWidth, (float)TogglePaneButton->ActualHeight));
            _togglePaneButtonRect = rect;
        }
        else
        {
            _togglePaneButtonRect = Rect();
        }

        TogglePaneButtonRectChanged(this, TogglePaneButtonRect);
    }

    /// <summary>
    /// Enable accessibility on each nav menu item by setting the AutomationProperties.Name on each container
    /// using the associated Label of each item.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    void AppShell::NavMenuItemContainerContentChanging(ListViewBase^ sender, ContainerContentChangingEventArgs^ args)
    {
        if (!args->InRecycleQueue && args->Item != nullptr && dynamic_cast<NavMenuItem^>(args->Item) != nullptr)
        {
            args->ItemContainer->SetValue(Windows::UI::Xaml::Automation::AutomationProperties::NameProperty, ((NavMenuItem^)args->Item)->Label);
        }
        else
        {
            args->ItemContainer->ClearValue(Windows::UI::Xaml::Automation::AutomationProperties::NameProperty);
        }
    }
}
