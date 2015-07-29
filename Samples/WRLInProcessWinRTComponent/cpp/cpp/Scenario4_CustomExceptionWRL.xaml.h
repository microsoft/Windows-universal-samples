//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#pragma once
#include "Scenario4_CustomExceptionWRL.g.h"

namespace SDKTemplate
{
    namespace WRLInProcessWinRTComponent
    {
        public ref class CustomExceptionWRL sealed
        {
        public:
            CustomExceptionWRL();

        private:
            void Start_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        };
    }
}
