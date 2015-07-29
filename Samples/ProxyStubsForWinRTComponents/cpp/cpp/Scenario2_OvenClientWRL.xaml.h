//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#pragma once
#include "Scenario2_OvenClientWRL.g.h"

namespace SDKSample
{
    namespace ProxyStubsForWinRTComponents
    {
        public ref class OvenClientWRL sealed
        {
        public:
            OvenClientWRL();

        private:
            void Start_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        };
    }
}
