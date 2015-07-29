//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#pragma once
#include "Scenario3_CustomException.g.h"

namespace SDKTemplate
{
    namespace WRLInProcessWinRTComponent
    {
        public ref class CustomException sealed
        {
        public:
            CustomException();

        private:
            void Start_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        };
    }
}
