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
#include "Scenario2_XmlWriter.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    namespace XmlLite
    {
        /// <summary>
        /// An empty page that can be used on its own or navigated to within a Frame.
        /// </summary>
        [Windows::Foundation::Metadata::WebHostHidden]
        public ref class Scenario2 sealed
        {
        public:
            Scenario2();

        private:
            MainPage^ rootPage;
            void DoSomething_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

            HRESULT WriteXml(Windows::Storage::Streams::IRandomAccessStream^ pStream);
        };
    }
}
