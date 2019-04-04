// Copyright (c) Microsoft. All rights reserved.

#pragma once 
#include "pch.h"

namespace SDKTemplate
{
    value struct Scenario;

    partial ref class MainPage
    {
    internal:
        static property Platform::String^ FEATURE_NAME
        {
            Platform::String^ get()
            {
                return ref new Platform::String(L"File access C++ sample");
            }
        }

        static property Platform::Array<Scenario>^ scenarios
        {
            Platform::Array<Scenario>^ get()
            {
                return scenariosInner;
            }
        }

        static property Platform::String^ Filename
        {
            Platform::String^ get()
            {
                return ref new Platform::String(L"sample.dat");
            }
        }

        property Windows::Storage::StorageFile^ SampleFile
        {
            Windows::Storage::StorageFile^ get()
            {
                return sampleFile;
            }
            void set(Windows::Storage::StorageFile^ value)
            {
                sampleFile = value;
            }
        }

    internal:
        void ValidateFile();
        void NotifyUserFileNotExist();
        void HandleIoException(Platform::COMException^ e, Platform::String^ description);
        static Windows::Storage::Streams::IBuffer^ GetBufferFromString(Platform::String^ string);
        static Platform::String^ GetStringFromBuffer(Windows::Storage::Streams::IBuffer^ buffer);
    private:
        static Platform::Array<Scenario>^ scenariosInner;
        Windows::Storage::StorageFile^ sampleFile;
    };

    public value struct Scenario
    {
        Platform::String^ Title;
        Platform::String^ ClassName;
    };
}
