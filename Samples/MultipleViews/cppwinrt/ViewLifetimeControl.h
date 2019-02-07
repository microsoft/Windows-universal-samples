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

#include "ViewLifetimeControl.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct ViewLifetimeControl : ViewLifetimeControlT<ViewLifetimeControl>
    {
        ViewLifetimeControl(Windows::UI::Core::CoreWindow const& newWindow);

        // Dispatcher for this view. Kept here for sending messages between this view and the main view.
        Windows::UI::Core::CoreDispatcher Dispatcher()
        {
            // This property never changes, so there's no need to lock
            return m_dispatcher;
        }

        // Each view has a unique Id, found using the ApplicationView.Id property or
        // ApplicationView.GetApplicationViewIdForCoreWindow method. This id is used in all of the ApplicationViewSwitcher
        // and ProjectionManager APIs. 
        int32_t Id()
        {
            // This property never changes, so there's no need to lock
            return m_viewId;
        }

        hstring Title();
        void Title(hstring const& value);

        int32_t StartViewInUse();
        int32_t StopViewInUse();

        event_token Released(Windows::Foundation::TypedEventHandler<SDKTemplate::ViewLifetimeControl, Windows::Foundation::IInspectable> const& handler);
        void Released(event_token const& token);
        event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(event_token const& token);

        static SDKTemplate::ViewLifetimeControl CreateForCurrentView();

    private:
        // Dispatcher for this view. Kept here for sending messages between this view and the main view.
        Windows::UI::Core::CoreDispatcher m_dispatcher{ nullptr };

        // Window for this particular view. Used to register and unregister for events
        agile_ref<Windows::UI::Core::CoreWindow> m_window;

        // Each view has a unique Id, found using the ApplicationView.Id property or
        // ApplicationView.GetApplicationViewIdForCoreWindow method. This id is used in all of the ApplicationViewSwitcher
        // and ProjectionManager APIs. 
        int32_t m_viewId;

        // The title for the view shown in the list of recently used apps (by setting the title on 
        // ApplicationView)
        hstring m_title;

        event<Windows::Foundation::TypedEventHandler<SDKTemplate::ViewLifetimeControl, Windows::Foundation::IInspectable>> m_internalReleased;
        event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;

        // Tracks if this ViewLifetimeControl object is still valid. If this is true, then the view is in the process
        // of closing itself down
        bool m_released = false;

        // This class uses reference counts to make sure the secondary views isn't closed prematurely.
        // Whenever the main view is about to interact with the secondary view, it should take a reference
        // by calling "StartViewInUse" on this object. When finished interacting, it should release the reference
        // by calling "StopViewInUse". You can see examples of this throughout the sample, especially in
        // scenario 1.
        int m_refCount = 0;

        event_token m_consolidatedToken;

        std::mutex m_mutex;

        void RegisterForEvents();
        void UnregisterForEvents();
        void FinalizeRelease();
        void ViewConsolidated(Windows::UI::ViewManagement::ApplicationView const& sender, Windows::UI::ViewManagement::ApplicationViewConsolidatedEventArgs const& e);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct ViewLifetimeControl : ViewLifetimeControlT<ViewLifetimeControl, implementation::ViewLifetimeControl>
    {
    };
}
