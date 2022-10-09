//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include "EdgeTappedListView.h"
#include "EdgeTappedListView.g.cpp"
#include "ListViewEdgeTappedEventArgs.g.cpp"
#include <mutex>

using namespace winrt::Windows::UI::Input;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Input;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Shapes;

namespace winrt::SDKTemplate::implementation
{
    Windows::UI::Xaml::DependencyProperty EdgeTappedListView::s_containerEventTokensProperty{ nullptr };

    void EdgeTappedListView::RegisterDependencyPropertiesOnce()
    {
        static std::once_flag flag;
        std::call_once(flag, [] {
            s_containerEventTokensProperty = DependencyProperty::RegisterAttached(
                L"ContainerEventTokens", xaml_typename<IInspectable>(),
                xaml_typename<SDKTemplate::EdgeTappedListView>(), nullptr);
            });
    }

    EdgeTappedListView::EdgeTappedListView()
    {
        RegisterDependencyPropertiesOnce();
        ContainerContentChanging({ get_weak(), &EdgeTappedListView::OnContainerContentChanging });
        leftEdgeBrush = Application::Current().Resources().Lookup(box_value(L"SystemControlHighlightAltListAccentLowBrush")).as<Brush>();
    }

    void EdgeTappedListView::OnContainerContentChanging(ListViewBase const&, ContainerContentChangingEventArgs const& args)
    {
        if (args.InRecycleQueue())
        {
            UIElement element = VisualTreeHelper::GetChild(args.ItemContainer(), 0).try_as<UIElement>();
            if (element != nullptr)
            {
                IInspectable prop = element.ReadLocalValue(s_containerEventTokensProperty);
                if (prop != nullptr)
                {
                    element.ClearValue(s_containerEventTokensProperty);
                    ContainerEventTokens* tokens = get_self<ContainerEventTokens>(prop);
                    element.PointerPressed(tokens->pointerPressedToken);
                    element.PointerReleased(tokens->pointerReleasedToken);
                    element.PointerCaptureLost(tokens->pointerCaptureLostToken);
                    element.PointerExited(tokens->pointerExitedToken);
                }
            }
        }
        else if (args.Phase() == 0)
        {
            UIElement element = VisualTreeHelper::GetChild(args.ItemContainer(), 0).try_as<UIElement>();
            if (element != nullptr)
            {
                com_ptr<ContainerEventTokens> tokens = make_self<ContainerEventTokens>();
                element.SetValue(s_containerEventTokensProperty, *tokens);
                tokens->pointerPressedToken = element.PointerPressed({ get_weak(), &EdgeTappedListView::Element_PointerPressed });
                tokens->pointerReleasedToken = element.PointerReleased({ get_weak(), &EdgeTappedListView::Element_PointerReleased });
                tokens->pointerCaptureLostToken = element.PointerCaptureLost({ get_weak(), &EdgeTappedListView::Element_PointerCaptureLost });
                tokens->pointerExitedToken = element.PointerExited({ get_weak(), &EdgeTappedListView::Element_PointerExited });
            }
        }
    }

    void EdgeTappedListView::Element_PointerPressed(IInspectable const& sender, PointerRoutedEventArgs const& e)
    {
        if (IsItemLeftEdgeTapEnabled())
        {
            if (e.Pointer().PointerDeviceType() == Windows::Devices::Input::PointerDeviceType::Touch)
            {
                UIElement element = sender.as<UIElement>();
                PointerPoint pointerPoint = e.GetCurrentPoint(element);
                if (pointerPoint.Position().X <= HIT_TARGET)
                {
                    listViewItemHighlighted = VisualTreeHelper::GetParent(element).as<ListViewItem>();
                    ShowVisual();
                }
            }
        }
    }

    void EdgeTappedListView::Element_PointerReleased(IInspectable const&, PointerRoutedEventArgs const&)
    {
        if (listViewItemHighlighted != nullptr)
        {
            ClearVisual();
        }
    }

    void EdgeTappedListView::Element_PointerCaptureLost(IInspectable const&, PointerRoutedEventArgs const&)
    {
        ClearVisual();
    }

    void EdgeTappedListView::Element_PointerExited(IInspectable const&, PointerRoutedEventArgs const&)
    {
        ClearVisual();
    }

    void EdgeTappedListView::ShowVisual()
    {
        if (listViewItemHighlighted != nullptr)
        {
            visualIndicator = listViewItemHighlighted.FindName(VISUAL_INDICATOR_NAME).as<Rectangle>();
            if (visualIndicator == nullptr)
            {
                visualIndicator = Rectangle();
                visualIndicator.Name(VISUAL_INDICATOR_NAME);
                visualIndicator.Height(listViewItemHighlighted.ActualHeight());
                visualIndicator.HorizontalAlignment(HorizontalAlignment::Left);
                visualIndicator.Width(VISUAL_INDICATOR_WIDTH);
                visualIndicator.Fill(leftEdgeBrush);
                visualIndicator.Margin(Thickness{ -(listViewItemHighlighted.Padding().Left), 0, 0, 0 });
                Panel panel = listViewItemHighlighted.ContentTemplateRoot().try_as<Panel>();
                if (panel != nullptr)
                {
                    Grid grid = panel.try_as<Grid>();
                    if (grid != nullptr)
                    {
                        visualIndicator.SetValue(Grid::RowSpanProperty(), box_value(grid.RowDefinitions().Size()));
                    }
                    panel.Children().Append(visualIndicator);
                }
            }
            else
            {
                visualIndicator.Opacity(1.0);
            }
        }
    }

    void EdgeTappedListView::ClearVisual()
    {
        if (listViewItemHighlighted != nullptr)
        {
            if (visualIndicator != nullptr)
            {
                visualIndicator.Opacity(0.0);
                if (itemLeftEdgeTappedEvent)
                {
                    itemLeftEdgeTappedEvent(*this, make<ListViewEdgeTappedEventArgs>(listViewItemHighlighted));
                }
            }
            listViewItemHighlighted = nullptr;
        }
    }
}
