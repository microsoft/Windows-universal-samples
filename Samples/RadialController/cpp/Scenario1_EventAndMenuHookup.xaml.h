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

#pragma once

#include "Scenario1_EventAndMenuHookup.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_EventAndMenuHookup sealed
    {
    public:
        Scenario1_EventAndMenuHookup();
        void OnItemInvoked(unsigned int selectedItemIndex);

        property Windows::UI::Input::RadialController^ Controller
        {
            Windows::UI::Input::RadialController^ get()
            {
                return controller;
            }
        }

    protected:
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        MainPage^ rootPage;

        Windows::UI::Input::RadialController^ controller;

        int activeItemIndex;
        std::vector<Windows::UI::Input::RadialControllerMenuItem^> menuItems;
        std::vector<Windows::UI::Xaml::Controls::Slider^> sliders;
        std::vector<Windows::UI::Xaml::Controls::ToggleSwitch^> toggles;

        void InitializeController();
        void CreateMenuItems();
        void SetItemInvokedCallback(Windows::UI::Input::RadialControllerMenuItem^ menuItem, unsigned int index);

        void AddItem(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args);
        void RemoveItem(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args);
        void SelectItem(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args);
        void SelectPreviouslySelectedItem(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args);

        bool IsItemInMenu(Windows::UI::Input::RadialControllerMenuItem^ item);
        unsigned int GetItemIndex(Windows::UI::Input::RadialControllerMenuItem^ item);
        Windows::UI::Input::RadialControllerMenuItem^ GetRadialControllerMenuItemFromSender(Platform::Object^ sender);

        void PrintSelectedItem(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args);
        void PrintSelectedItem();
        Platform::String^ GetSelectedMenuItemName();

        void OnLogSizeChanged(Platform::Object ^sender, Platform::Object ^args);
        void OnControlAcquired(Windows::UI::Input::RadialController ^sender, Windows::UI::Input::RadialControllerControlAcquiredEventArgs ^args);
        void OnControlLost(Windows::UI::Input::RadialController ^sender, Platform::Object ^args);

        void OnScreenContactStarted(Windows::UI::Input::RadialController ^sender, Windows::UI::Input::RadialControllerScreenContactStartedEventArgs ^args);
        void OnScreenContactContinued(Windows::UI::Input::RadialController ^sender, Windows::UI::Input::RadialControllerScreenContactContinuedEventArgs ^args);
        void OnScreenContactEnded(Windows::UI::Input::RadialController ^sender, Platform::Object ^args);

        void OnButtonClicked(Windows::UI::Input::RadialController ^sender, Windows::UI::Input::RadialControllerButtonClickedEventArgs ^args);
        void OnRotationChanged(Windows::UI::Input::RadialController ^sender, Windows::UI::Input::RadialControllerRotationChangedEventArgs ^args);
        void LogContactInfo(Windows::UI::Input::RadialControllerScreenContact^ contact);
    };
}
