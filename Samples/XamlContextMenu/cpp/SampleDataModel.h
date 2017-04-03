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
    public ref class SampleDataModel sealed
    {
    public:
        property Platform::String^ Title {
            Platform::String^ get() { return m_title; }
        }
        property Platform::String^ ImagePath {
            Platform::String^ get() { return m_imagePath; }
        }

        SampleDataModel(Platform::String^ title, Platform::String^ imagePath)
            : m_title(title), m_imagePath(imagePath)
        {
        }

        static Windows::Foundation::Collections::IVector<SampleDataModel^>^ GetSampleData()
        {
            return ref new Platform::Collections::Vector<SampleDataModel^>{
                ref new SampleDataModel("Cliff", "Assets/cliff.jpg"),
                ref new SampleDataModel("Grapes", "Assets/grapes.jpg"),
                ref new SampleDataModel("Rainier", "Assets/Rainier.jpg"),
                ref new SampleDataModel("Sunset", "Assets/Sunset.jpg"),
                ref new SampleDataModel("Treetops", "Assets/Treetops.jpg"),
                ref new SampleDataModel("Valley", "Assets/Valley.jpg"),
            };
        }

    private:
        Platform::String^ m_title;
        Platform::String^ m_imagePath;
    };
}
