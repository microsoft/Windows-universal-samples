//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#pragma once
#include "Scenario2_OvenClientWRL.g.h"
#include "..\Server\Microsoft.SDKSamples.Kitchen.h"

namespace SDKTemplate
{
    namespace WRLOutOfProcessWinRTComponent
    {
        public ref class OvenClientWRL sealed
        {
        public:
            OvenClientWRL();

        private:
            void Start_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

            // This scenario uses the Oven as an event source so its lifetime must be maintained
            Microsoft::WRL::ComPtr<ABI::Microsoft::SDKSamples::Kitchen::IOven> _myOven;
        };
    }
}
