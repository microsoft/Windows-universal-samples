//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

//
// Scenario5_GroupedTask.xaml.h
// Declaration of the GroupedBackgroundTask class
//

#pragma once

#include <ppltasks.h>
#include "pch.h"
#include "Scenario6_GroupedTask.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class GroupedBackgroundTask sealed
    {
    public:
        GroupedBackgroundTask();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
    private:
        SDKTemplate::MainPage^ rootPage;
        BackgroundTaskRegistrationGroup^ group = nullptr;

        void AttachProgressAndCompletedHandlers(Windows::ApplicationModel::Background::IBackgroundTaskRegistration^ task);
        void RegisterGroupedBackgroundTask(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void UnregisterGroupedTask(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void UnregisterUngroupedTasks(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnProgress(Windows::ApplicationModel::Background::BackgroundTaskRegistration^ task, Windows::ApplicationModel::Background::BackgroundTaskProgressEventArgs^ args);
        void OnCompleted(Windows::ApplicationModel::Background::BackgroundTaskRegistration^ task, Windows::ApplicationModel::Background::BackgroundTaskCompletedEventArgs^ args);
        void UpdateUI();
    };
}
