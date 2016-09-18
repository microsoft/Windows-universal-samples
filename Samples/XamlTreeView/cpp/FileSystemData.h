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
    [Windows::UI::Xaml::Data::Bindable]
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class FileSystemData sealed
    {
    public:
        FileSystemData(Platform::String^ name)
        {
            this->Name = name;
        }

        property Platform::String^ Name
        {
            Platform::String^ get() { return name; }
            void set(Platform::String^ value) { name = value; }
        }

        property bool IsFolder
        {
            bool get() { return isFolder; }
            void set(bool value) { isFolder = value; }
        }
    private:
        Platform::String^ name = nullptr;
        bool isFolder = false;
    };
}

