using System;
using System.Collections.Generic;

namespace SDKTemplate
{
    static class RecognizerHelper
    {
        private static Dictionary<string, string> Bcp47ToRecognizerNameDictionary =
            new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase)
            {
                { "en-US", "Microsoft English (US) Handwriting Recognizer" },
                { "en-GB", "Microsoft English (UK) Handwriting Recognizer" },
                { "en-CA", "Microsoft English (Canada) Handwriting Recognizer" },
                { "en-AU", "Microsoft English (Australia) Handwriting Recognizer" },
                { "de-DE", "Microsoft-Handschrifterkennung - Deutsch" },
                { "de-CH", "Microsoft-Handschrifterkennung - Deutsch (Schweiz)" },
                { "es-ES", "Reconocimiento de escritura a mano en español de Microsoft" },
                { "es-MX", "Reconocedor de escritura en Español (México) de Microsoft" },
                { "es-AR", "Reconocedor de escritura en Español (Argentina) de Microsoft" },
                { "fr", "Reconnaissance d'écriture Microsoft - Français" },
                { "fr-FR", "Reconnaissance d'écriture Microsoft - Français" },
                { "ja", "Microsoft 日本語手書き認識エンジン" },
                { "ja-JP", "Microsoft 日本語手書き認識エンジン" },
                { "it", "Riconoscimento grafia italiana Microsoft" },
                { "nl-NL", "Microsoft Nederlandstalige handschriftherkenning" },
                { "nl-BE", "Microsoft Nederlandstalige (België) handschriftherkenning" },
                { "zh", "Microsoft 中文(简体)手写识别器" },
                { "zh-CN", "Microsoft 中文(简体)手写识别器" },
                { "zh-Hans-CN", "Microsoft 中文(简体)手写识别器" },
                { "zh-Hant", "Microsoft 中文(繁體)手寫辨識器" },
                { "zh-TW", "Microsoft 中文(繁體)手寫辨識器" },
                { "ru", "Microsoft система распознавания русского рукописного ввода" },
                { "pt-BR", "Reconhecedor de Manuscrito da Microsoft para Português (Brasil)" },
                { "pt-PT", "Reconhecedor de escrita manual da Microsoft para português" },
                { "ko", "Microsoft 한글 필기 인식기" },
                { "pl", "System rozpoznawania polskiego pisma odręcznego firmy Microsoft" },
                { "sv", "Microsoft Handskriftstolk för svenska" },
                { "cs", "Microsoft rozpoznávač rukopisu pro český jazyk" },
                { "da", "Microsoft Genkendelse af dansk håndskrift" },
                { "nb", "Microsoft Håndskriftsgjenkjenner for norsk" },
                { "nn", "Microsoft Håndskriftsgjenkjenner for nynorsk" },
                { "fi", "Microsoftin suomenkielinen käsinkirjoituksen tunnistus" },
                { "ro", "Microsoft recunoaştere grafie - Română" },
                { "hr", "Microsoftov hrvatski rukopisni prepoznavač" },
                { "sr-Latn", "Microsoft prepoznavač rukopisa za srpski (latinica)" },
                { "sr", "Microsoft препознавач рукописа за српски (ћирилица)" },
                { "ca", "Reconeixedor d'escriptura manual en català de Microsoft" },
            };

        public static string LanguageTagToRecognizerName(string bcp47tag)
        {
            string recognizerName;
            if (!Bcp47ToRecognizerNameDictionary.TryGetValue(bcp47tag, out recognizerName))
            {
                recognizerName = string.Empty;
            }

            return recognizerName;
        }
    }
}
