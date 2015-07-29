//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#pragma once
#include "Scenario1_OvenClient.g.h"

namespace SDKSample
{
    namespace ProxyStubsForWinRTComponents
    {
        public ref class OvenClient sealed
        {
        public:
            OvenClient();

        private:
            void Start_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void BreadCompletedHandler1(Microsoft::SDKSamples::Kitchen::Oven^ oven, Microsoft::SDKSamples::Kitchen::Bread^ bread);
            void BreadCompletedHandler2(Microsoft::SDKSamples::Kitchen::Oven^ oven, Microsoft::SDKSamples::Kitchen::Bread^ bread);
            void BreadCompletedHandler3(Microsoft::SDKSamples::Kitchen::Oven^ oven, Microsoft::SDKSamples::Kitchen::Bread^ bread);
        };
    }
}
