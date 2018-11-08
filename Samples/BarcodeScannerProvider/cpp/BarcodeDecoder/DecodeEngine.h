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

namespace BarcodeDecoder
{
    ref class DecodeResult
    {
    internal:
        property UINT Symbology
        {
            UINT get() { return m_symboloy; }
        internal:
            void set(_In_ UINT value) { m_symboloy = value; }
        }

        property Platform::String^ Text
        {
            Platform::String^ get() { return m_text; }
        internal:
            void set(_In_ Platform::String^ value) { m_text = value; }
        }

        static bool Equals(DecodeResult^ obj1, DecodeResult^ obj2)
        {
            if (obj1 == nullptr)
            {
                return (obj2 == nullptr);
            }
            else if (obj2 == nullptr)
            {
                return false;
            }
            return (obj1->Symbology == obj2->Symbology) && (obj1->Text == obj2->Text);
        }

    private:
        UINT m_symboloy;
        Platform::String^ m_text;
    };

    ref class DecodeEngine
    {
    internal:
        DecodeEngine();

         property Windows::Foundation::Collections::IVector<UINT>^ SupportedSymbologies
        {
            Windows::Foundation::Collections::IVector<UINT>^ get() { return m_supportedSymbologies; }
        internal:
            void set(_In_ Windows::Foundation::Collections::IVector<UINT>^ value) { m_supportedSymbologies = value; }
        }

        property Windows::Foundation::Collections::IVector<UINT>^ ActiveSymbologies
        {
            Windows::Foundation::Collections::IVector<UINT>^ get() { return m_activeSymbologies; }
            void set(_In_ Windows::Foundation::Collections::IVector<UINT>^ value) { m_activeSymbologies = value; }
        }

        concurrency::task<DecodeResult^> DecodeAsync(Windows::Graphics::Imaging::SoftwareBitmap^ bitmap);

    private:
        Windows::Media::Ocr::OcrEngine^ m_ocrEngine = nullptr;
        Windows::Foundation::Collections::IVector<UINT>^ m_supportedSymbologies;
        Windows::Foundation::Collections::IVector<UINT>^ m_activeSymbologies;
    };
}
