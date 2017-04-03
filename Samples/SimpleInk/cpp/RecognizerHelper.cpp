#include "pch.h"
#include "RecognizerHelper.h"

namespace SDKTemplate
{
    std::map<Platform::String^, Platform::String^> RecognizerHelper::Bcp47ToRecognizerNameDictionary =
    {
        { L"en-US", L"Microsoft English (US) Handwriting Recognizer" },
        { L"en-GB", L"Microsoft English (UK) Handwriting Recognizer" },
        { L"en-CA", L"Microsoft English (Canada) Handwriting Recognizer" },
        { L"en-AU", L"Microsoft English (Australia) Handwriting Recognizer" },
        { L"de-DE", L"Microsoft-Handschrifterkennung - Deutsch" },
        { L"de-CH", L"Microsoft-Handschrifterkennung - Deutsch (Schweiz)" },
        { L"es-ES", L"Reconocimiento de escritura a mano en español de Microsoft" },
        { L"es-MX", L"Reconocedor de escritura en Español (México) de Microsoft" },
        { L"es-AR", L"Reconocedor de escritura en Español (Argentina) de Microsoft" },
        { L"fr", L"Reconnaissance d'écriture Microsoft - Français" },
        { L"fr-FR", L"Reconnaissance d'écriture Microsoft - Français" },
        { L"ja", L"Microsoft 日本語手書き認識エンジン" },
        { L"ja-JP", L"Microsoft 日本語手書き認識エンジン" },
        { L"it", L"Riconoscimento grafia italiana Microsoft" },
        { L"nl-NL", L"Microsoft Nederlandstalige handschriftherkenning" },
        { L"nl-BE", L"Microsoft Nederlandstalige (België) handschriftherkenning" },
        { L"zh", L"Microsoft 中文(简体)手写识别器" },
        { L"zh-CN", L"Microsoft 中文(简体)手写识别器" },
        { L"zh-Hans-CN", L"Microsoft 中文(简体)手写识别器" },
        { L"zh-Hant", L"Microsoft 中文(繁體)手寫辨識器" },
        { L"zh-TW", L"Microsoft 中文(繁體)手寫辨識器" },
        { L"ru", L"Microsoft система распознавания русского рукописного ввода" },
        { L"pt-BR", L"Reconhecedor de Manuscrito da Microsoft para Português (Brasil)" },
        { L"pt-PT", L"Reconhecedor de escrita manual da Microsoft para português" },
        { L"ko", L"Microsoft 한글 필기 인식기" },
        { L"pl", L"System rozpoznawania polskiego pisma odręcznego firmy Microsoft" },
        { L"sv", L"Microsoft Handskriftstolk för svenska" },
        { L"cs", L"Microsoft rozpoznávač rukopisu pro český jazyk" },
        { L"da", L"Microsoft Genkendelse af dansk håndskrift" },
        { L"nb", L"Microsoft Håndskriftsgjenkjenner for norsk" },
        { L"nn", L"Microsoft Håndskriftsgjenkjenner for nynorsk" },
        { L"fi", L"Microsoftin suomenkielinen käsinkirjoituksen tunnistus" },
        { L"ro", L"Microsoft recunoaştere grafie - Română" },
        { L"hr", L"Microsoftov hrvatski rukopisni prepoznavač" },
        { L"sr-Latn", L"Microsoft prepoznavač rukopisa za srpski (latinica)" },
        { L"sr", L"Microsoft препознавач рукописа за српски (ћирилица)" },
        { L"ca", L"Reconeixedor d'escriptura manual en català de Microsoft" },
    };

    Platform::String^ RecognizerHelper::LanguageTagToRecognizerName(Platform::String^ bcp47tag)
    {
        Platform::String^ recognizerName;
        recognizerName = "";

        auto it = Bcp47ToRecognizerNameDictionary.find(bcp47tag);
        if (it != Bcp47ToRecognizerNameDictionary.end())
        {
            recognizerName = it->second;
        }

        return recognizerName;
    }
}
