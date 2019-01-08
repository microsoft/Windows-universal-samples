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
#include "pch.h"
#include "DecodeEngine.h"

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Devices::PointOfService;
using namespace Windows::Globalization;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Media::Ocr;
using namespace Windows::Storage;

namespace BarcodeDecoder
{
    DecodeEngine::DecodeEngine()
    {
        // Create OCR engine from the language set in scenario 1 configuration
        auto localSettings = ApplicationData::Current->LocalSettings;
        auto decoderLanguageKey = ref new String(L"decoderLanguage");
        if (localSettings->Values->HasKey(decoderLanguageKey))
        {
            String^ languageName = safe_cast<String^>(localSettings->Values->Lookup(decoderLanguageKey));
            for (Language^ lang : OcrEngine::AvailableRecognizerLanguages)
            {
                if (lang->DisplayName == languageName)
                {
                    m_ocrEngine = OcrEngine::TryCreateFromLanguage(lang);
                    break;
                }
            }
        }

        if (m_ocrEngine == nullptr)
        {
            m_ocrEngine = OcrEngine::TryCreateFromUserProfileLanguages();
        }

        SupportedSymbologies = ref new Platform::Collections::Vector<UINT>();
        SupportedSymbologies->Append(BarcodeSymbologies::OcrA);

        ActiveSymbologies = ref new Platform::Collections::Vector<UINT>();
        ActiveSymbologies->Append(BarcodeSymbologies::OcrA);
    }

    task<DecodeResult^> DecodeEngine::DecodeAsync(SoftwareBitmap^ bitmap)
    {
        // The only thing we decode is OCR.
        uint32 index;
        if (!ActiveSymbologies->IndexOf(BarcodeSymbologies::OcrA, &index))
        {
            return task_from_result<DecodeResult^>(nullptr);
        }

        return create_task(m_ocrEngine->RecognizeAsync(bitmap)).then([this, bitmap](OcrResult^ ocrResult) -> DecodeResult^
        {
            // Get the text in the first non-empty line as result
            for (auto line : ocrResult->Lines)
            {
                if (!line->Text->IsEmpty())
                {
                    auto result = ref new DecodeResult();
                    result->Symbology = BarcodeSymbologies::OcrA;
                    result->Text = line->Text;
                    return result;
                }
            }
            return nullptr;
        });
    }
}
