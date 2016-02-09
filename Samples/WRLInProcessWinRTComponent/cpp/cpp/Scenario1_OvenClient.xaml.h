//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#pragma once
#include "Scenario1_OvenClient.g.h"

namespace SDKTemplate
{
    namespace WRLInProcessWinRTComponent
    {
        public ref class OvenClient sealed
        {
        public:
            OvenClient();

        private:
            void Start_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void BreadCompletedHandler1(Microsoft::SDKSamples::Kitchen::Oven^ oven, Microsoft::SDKSamples::Kitchen::BreadBakedEventArgs^ args);
            void BreadCompletedHandler2(Microsoft::SDKSamples::Kitchen::Oven^ oven, Microsoft::SDKSamples::Kitchen::BreadBakedEventArgs^ args);
            void BreadCompletedHandler3(Microsoft::SDKSamples::Kitchen::Oven^ oven, Microsoft::SDKSamples::Kitchen::BreadBakedEventArgs^ args);

            Microsoft::SDKSamples::Kitchen::Oven^ _myOven; // keep around since this is an event source
        };
    }
}
