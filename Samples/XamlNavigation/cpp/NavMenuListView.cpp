#include "pch.h"
#include "NavMenuListView.h"

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::System;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Documents;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Animation;

namespace NavigationMenuSample
{
    namespace Controls
    {
        NavMenuListView::NavMenuListView()
        {
            this->SelectionMode = ListViewSelectionMode::Single;
            this->SingleSelectionFollowsFocus = false;
            this->IsItemClickEnabled = true;
            this->ItemClick += ref new ItemClickEventHandler(this, &NavMenuListView::ItemClickHandler);

            // Locate the hosting SplitView control
            this->Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &NavigationMenuSample::Controls::NavMenuListView::OnLoaded);
        }

        void NavMenuListView::OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
        {
            auto parent = VisualTreeHelper::GetParent(this);
            while (parent != nullptr && dynamic_cast<SplitView^>(parent) == nullptr)
            {
                parent = VisualTreeHelper::GetParent((DependencyObject^)parent);
            }

            if (parent != nullptr)
            {
                _splitViewHost = dynamic_cast<SplitView^>(parent);


                _splitViewHost->RegisterPropertyChangedCallback(
                    SplitView::IsPaneOpenProperty,
                    ref new DependencyPropertyChangedCallback(this, &NavMenuListView::IsOpenPanePropertyChangedCallback));

                _splitViewHost->RegisterPropertyChangedCallback(
                    SplitView::DisplayModeProperty,
                    ref new DependencyPropertyChangedCallback(this, &NavMenuListView::DisplayModePropertyChangedCallback));

                // Call once to ensure we're in the correct state
                OnPaneToggled();
            }
        }

        void NavMenuListView::IsOpenPanePropertyChangedCallback(DependencyObject^ sender, DependencyProperty^ args)
        {
            OnPaneToggled();
        }

        void NavMenuListView::DisplayModePropertyChangedCallback(DependencyObject^ sender, DependencyProperty^ args)
        {
            OnPaneToggled();
        }

        void NavMenuListView::OnApplyTemplate()
        {
            ListView::OnApplyTemplate();

            // Remove the entrance animation on the item containers.
            for (int i = 0; i < (int)ItemContainerTransitions->Size; i++)
            {
                if (dynamic_cast<EntranceThemeTransition^>(ItemContainerTransitions->GetAt(i)) != nullptr)
                {
                    ItemContainerTransitions->RemoveAt(i);
                }
            }
        }

        /// <summary>
        /// Mark the <paramref name="item"/> as selected and ensures everything else is not.
        /// If the <paramref name="item"/> is null then everything is unselected.
        /// </summary>
        /// <param name="item"></param>
        void NavMenuListView::SetSelectedItem(ListViewItem^ item)
        {
            int index = -1;
            if (item != nullptr)
            {
                index = IndexFromContainer(item);
            }

            for (int i = 0; i < (int)Items->Size; i++)
            {
                auto lvi = (ListViewItem^)ContainerFromIndex(i);
                if (i != index)
                {
                    lvi->IsSelected = false;
                }
                else if (i == index)
                {
                    lvi->IsSelected = true;
                }
            }
        }

	/// <summary>
        /// Custom keyboarding logic to enable movement via the arrow keys without triggering selection
        /// until a 'Space' or 'Enter' key is pressed.
        /// </summary>
        /// <param name="e"></param>
        void NavMenuListView::OnKeyDown(KeyRoutedEventArgs^ e)
        {
            auto focusedItem = FocusManager::GetFocusedElement();

            auto shiftKeyState = CoreWindow::GetForCurrentThread()->GetKeyState(VirtualKey::Shift);
            auto shiftKeyDown = (shiftKeyState & CoreVirtualKeyStates::Down) == CoreVirtualKeyStates::Down;

            switch (e->Key)
            {
            case VirtualKey::Up:
                this->TryMoveFocus(FocusNavigationDirection::Up);
                e->Handled = true;
                break;

            case VirtualKey::Down:
                this->TryMoveFocus(FocusNavigationDirection::Down);
                e->Handled = true;
                break;

            case VirtualKey::Space:
            case VirtualKey::Enter:
                // Fire our event using the item with current keyboard focus
                InvokeItem(focusedItem);
                e->Handled = true;
                break;

            default:
                ListView::OnKeyDown(e);
                break;
            }
        }

        /// <summary>
        /// This method is a work-around until the bug in FocusManager.TryMoveFocus is fixed.
        /// </summary>
        /// <param name="direction"></param>
        void NavMenuListView::TryMoveFocus(FocusNavigationDirection direction)
        {
            if (direction == FocusNavigationDirection::Next || direction == FocusNavigationDirection::Previous)
            {
                FocusManager::TryMoveFocus(direction);
            }
            else
            {
                auto control = dynamic_cast<Control^>(FocusManager::FindNextFocusableElement(direction));
                if (control != nullptr)
                {
                    control->Focus(Windows::UI::Xaml::FocusState::Programmatic);
                }
            }
        }

        void NavMenuListView::ItemClickHandler(Object^ sender, ItemClickEventArgs^ e)
        {
            // Triggered when the item is selected using something other than a keyboard
            auto item = ContainerFromItem(e->ClickedItem);
            InvokeItem(item);
        }

        void NavMenuListView::InvokeItem(Object^ focusedItem)
        {
            auto item = dynamic_cast<ListViewItem^>(focusedItem);
            SetSelectedItem(item);
            ItemInvoked(this, item);

            if (_splitViewHost->IsPaneOpen && (
                _splitViewHost->DisplayMode == SplitViewDisplayMode::CompactOverlay ||
                _splitViewHost->DisplayMode == SplitViewDisplayMode::Overlay))
            {
                _splitViewHost->IsPaneOpen = false;
            }
            if (item != nullptr)
            {
                item->Focus(Windows::UI::Xaml::FocusState::Programmatic);
            }
        }

        /// <summary>
        /// Re-size the ListView's Panel when the SplitView is compact so the items
        /// will fit within the visible space and correctly display a keyboard focus rect.
        /// </summary>
        void NavMenuListView::OnPaneToggled()
        {
            if (_splitViewHost->IsPaneOpen)
            {
                ItemsPanelRoot->ClearValue(FrameworkElement::WidthProperty);
                ItemsPanelRoot->ClearValue(FrameworkElement::HorizontalAlignmentProperty);
            }
            else if (_splitViewHost->DisplayMode == SplitViewDisplayMode::CompactInline ||
                _splitViewHost->DisplayMode == SplitViewDisplayMode::CompactOverlay)
            {
                ItemsPanelRoot->SetValue(FrameworkElement::WidthProperty, _splitViewHost->CompactPaneLength);
                ItemsPanelRoot->SetValue(FrameworkElement::HorizontalAlignmentProperty, Windows::UI::Xaml::HorizontalAlignment::Left);
            }
        }
    }
}

