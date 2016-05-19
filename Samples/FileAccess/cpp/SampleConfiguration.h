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

        property Platform::String^ MruToken
        {
            Platform::String^ get()
            {
                return mruToken;
            }
            void set(Platform::String^ value)
            {
                mruToken = value;
            }
        }

        property Platform::String^ FalToken
        {
            Platform::String^ get()
            {
                return falToken;
            }
            void set(Platform::String^ value)
            {
                falToken = value;
            }
        }

    internal:
        void Initialize();
        void ValidateFile();
        void NotifyUserFileNotExist();
        void HandleIoException(Platform::COMException^ e, Platform::String^ description);

    private:
        static Platform::Array<Scenario>^ scenariosInner;
        Windows::Storage::StorageFile^ sampleFile;
        Platform::String^ mruToken;
        Platform::String^ falToken;
    };

    public value struct Scenario
    {
        Platform::String^ Title;
        Platform::String^ ClassName;
    };
}
