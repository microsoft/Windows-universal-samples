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

        public ref class BufferConverter sealed : Windows::UI::Xaml::Data::IValueConverter
        {
        public:
            BufferConverter() {}
            virtual ~BufferConverter() {}

            virtual Platform::Object^ Convert(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object^ parameter, Platform::String^ language)
            {
                Platform::String^ metadata = "";
                Windows::Storage::Streams::IBuffer^ buffer = dynamic_cast<Windows::Storage::Streams::IBuffer^>(value);
                if (buffer != nullptr)
                {
                    auto metadataReader = Windows::Storage::Streams::DataReader::FromBuffer(buffer);
                    metadata = metadataReader->ReadString(buffer->Length);
                    metadata = "(" + metadata + ")";
                }
                return metadata;
            }

            virtual Platform::Object^ ConvertBack(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object^ parameter, Platform::String^ language)
            {
                throw ref new Platform::NotImplementedException();
            }
        };
    }
}
