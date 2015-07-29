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

#include "ProjectionViewPage.g.h"

#include "ViewLifetimeControl.h"

namespace SDKSample
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class ProjectionViewPage sealed
    {
    public:
        ProjectionViewPage();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        void SwapViews_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void StopProjecting_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void thisViewControl_Released(Platform::Object^ sender, Platform::Object^ e);
        SDKSample::SecondaryViewsHelpers::ViewLifetimeControl^ thisViewControl;
        Windows::UI::Core::CoreDispatcher^ mainDispatcher;
        int mainViewId;
        Windows::Foundation::EventRegistrationToken releasedToken;
    };
}
