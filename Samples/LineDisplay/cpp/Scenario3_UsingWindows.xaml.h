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

#include "Scenario3_UsingWindows.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::UI::Xaml::Data::Bindable]
    public ref class WindowInfo sealed
    {
    public:
        property Windows::Devices::PointOfService::LineDisplayWindow^ Window
        {
            Windows::Devices::PointOfService::LineDisplayWindow^ get()
            {
                return window;
            }

            void set(Windows::Devices::PointOfService::LineDisplayWindow^ value)
            {
                window = value;
            }
        }

        property int Id
        {
            int get()
            {
                return id;
            }

            void set(int value)
            {
                id = value;
            }
        }

        property Platform::String^ Name
        {
            Platform::String^ get()
            {
                return (Id == 0) ? "(Default)" : "";
            }
        }

        WindowInfo(Windows::Devices::PointOfService::LineDisplayWindow^ window, int id)
        {
            this->window = window;
            this->id = id;
        }
    private:
        Windows::Devices::PointOfService::LineDisplayWindow^ window;
        int id;
    };

    [Windows::Foundation::Metadata::WebHostHidden]
    [Windows::UI::Xaml::Data::Bindable]
    public ref class Scenario3_UsingWindows sealed
    {
    public:
        Scenario3_UsingWindows();

        property Windows::Foundation::Collections::IObservableVector<WindowInfo^>^ WindowList
        {
            Windows::Foundation::Collections::IObservableVector<WindowInfo^>^ get()
            {
                return windowList;
            }
        }

        // Public for binding purposes.
        bool IsNonNull(Platform::Object^ item)
        {
            return item != nullptr;
        }

        // The default window cannot be destroyed.
        bool CanDestroyWindow(Platform::Object^ item)
        {
            return (item != nullptr) && (safe_cast<WindowInfo^>(item)->Id != 0);
        }

        // Can create a new window if we haven't reached the maximum.
        bool CanCreateNewWindow(Windows::Foundation::Collections::IObservableVector<WindowInfo^>^ windowList)
        {
            return windowList->Size < maxWindows;
        }

    protected:
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        MainPage^ rootPage = MainPage::Current;
        Windows::Devices::PointOfService::ClaimedLineDisplay^ lineDisplay;

        Windows::Foundation::Collections::IObservableVector<WindowInfo^>^ windowList = ref new Platform::Collections::Vector<WindowInfo^>();

        int nextWindowId = 0;
        unsigned int maxWindows = 0;

        Concurrency::task<void> InitializeAsync();

        void NewWindowButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void RefreshWindowButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void DestroyWindowButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void DisplayTextButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
