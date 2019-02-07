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
#include "pch.h"
#include "App.h"

namespace winrt::SDKTemplate
{
    struct SampleState
    {
        static implementation::App* CurrentApp;
        static Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> SecondaryViews;
        static Windows::UI::Core::CoreDispatcher MainDispatcher;
        static int32_t MainViewId;
        static void UpdateTitle(hstring newTitle, int32_t viewId);
    };

    inline void App_Construct(implementation::App* app) { SampleState::CurrentApp = app; }
    bool App_OverrideOnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs const& e);
    fire_and_forget App_OnActivated(Windows::ApplicationModel::Activation::IActivatedEventArgs const&);
}
