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

namespace SDKTemplate
{
    namespace Helpers
    {

        public ref class ServiceStatusConverter sealed : Windows::UI::Xaml::Data::IValueConverter
        {
        public:
            ServiceStatusConverter() {}
            virtual ~ServiceStatusConverter() {}

            virtual Platform::Object^ Convert(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object^ parameter, Platform::String^ language)
            {
                return static_cast<Windows::Devices::WiFiDirect::Services::WiFiDirectServiceStatus>(value).ToString();
            }

            virtual Platform::Object^ ConvertBack(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object^ parameter, Platform::String^ language)
            {
                Platform::String^ statusString = dynamic_cast<Platform::String^>(value);
                if (statusString == Windows::Devices::WiFiDirect::Services::WiFiDirectServiceStatus::Available.ToString())
                {
                    return Windows::Devices::WiFiDirect::Services::WiFiDirectServiceStatus::Available;
                }
                else if (statusString == Windows::Devices::WiFiDirect::Services::WiFiDirectServiceStatus::Busy.ToString())
                {
                    return Windows::Devices::WiFiDirect::Services::WiFiDirectServiceStatus::Busy;
                }
                else if (statusString == Windows::Devices::WiFiDirect::Services::WiFiDirectServiceStatus::Custom.ToString())
                {
                    return Windows::Devices::WiFiDirect::Services::WiFiDirectServiceStatus::Custom;
                }
                else
                {
                    return Windows::Devices::WiFiDirect::Services::WiFiDirectServiceStatus::Custom;
                }
            }
        };
    }
}
