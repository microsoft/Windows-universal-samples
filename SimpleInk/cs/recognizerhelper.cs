using System;
using System.Collections.Generic;

namespace simpleInk
{
    class RecognizerHelper
    {
        private static Dictionary<string, string> Bcp47ToRecognizerNameDictionary = null;

        private static void EnsureDictionary()
        {
            if (Bcp47ToRecognizerNameDictionary == null)
            {
                Bcp47ToRecognizerNameDictionary = new Dictionary<string, string>(StringComparer.CurrentCultureIgnoreCase);

                Bcp47ToRecognizerNameDictionary.Add("en-US", "Microsoft English (US) Handwriting Recognizer");
                Bcp47ToRecognizerNameDictionary.Add("en-GB", "Microsoft English (UK) Handwriting Recognizer");
                Bcp47ToRecognizerNameDictionary.Add("en-CA", "Microsoft English (Canada) Handwriting Recognizer");
                Bcp47ToRecognizerNameDictionary.Add("en-AU", "Microsoft English (Australia) Handwriting Recognizer");
                Bcp47ToRecognizerNameDictionary.Add("de-DE", "Microsoft-Handschrifterkennung - Deutsch");
                Bcp47ToRecognizerNameDictionary.Add("de-CH", "Microsoft-Handschrifterkennung - Deutsch (Schweiz)");
                Bcp47ToRecognizerNameDictionary.Add("es-ES", "Reconocimiento de escritura a mano en español de Microsoft");
                Bcp47ToRecognizerNameDictionary.Add("es-MX", "Reconocedor de escritura en Español (México) de Microsoft");
                Bcp47ToRecognizerNameDictionary.Add("es-AR", "Reconocedor de escritura en Español (Argentina) de Microsoft");
                Bcp47ToRecognizerNameDictionary.Add("fr", "Reconnaissance d'écriture Microsoft - Français");
                Bcp47ToRecognizerNameDictionary.Add("fr-FR", "Reconnaissance d'écriture Microsoft - Français");
                Bcp47ToRecognizerNameDictionary.Add("ja", "Microsoft 日本語手書き認識エンジン");
                Bcp47ToRecognizerNameDictionary.Add("ja-JP", "Microsoft 日本語手書き認識エンジン");
                Bcp47ToRecognizerNameDictionary.Add("it", "Riconoscimento grafia italiana Microsoft");
                Bcp47ToRecognizerNameDictionary.Add("nl-NL", "Microsoft Nederlandstalige handschriftherkenning");
                Bcp47ToRecognizerNameDictionary.Add("nl-BE", "Microsoft Nederlandstalige (België) handschriftherkenning");
                Bcp47ToRecognizerNameDictionary.Add("zh", "Microsoft 中文(简体)手写识别器");
                Bcp47ToRecognizerNameDictionary.Add("zh-CN", "Microsoft 中文(简体)手写识别器");
                Bcp47ToRecognizerNameDictionary.Add("zh-Hans-CN", "Microsoft 中文(简体)手写识别器");
                Bcp47ToRecognizerNameDictionary.Add("zh-Hant", "Microsoft 中文(繁體)手寫辨識器");
                Bcp47ToRecognizerNameDictionary.Add("zh-TW", "Microsoft 中文(繁體)手寫辨識器");
                Bcp47ToRecognizerNameDictionary.Add("ru", "Microsoft система распознавания русского рукописного ввода");
                Bcp47ToRecognizerNameDictionary.Add("pt-BR", "Reconhecedor de Manuscrito da Microsoft para Português (Brasil)");
                Bcp47ToRecognizerNameDictionary.Add("pt-PT", "Reconhecedor de escrita manual da Microsoft para português");
                Bcp47ToRecognizerNameDictionary.Add("ko", "Microsoft 한글 필기 인식기");
                Bcp47ToRecognizerNameDictionary.Add("pl", "System rozpoznawania polskiego pisma odręcznego firmy Microsoft");
                Bcp47ToRecognizerNameDictionary.Add("sv", "Microsoft Handskriftstolk för svenska");
                Bcp47ToRecognizerNameDictionary.Add("cs", "Microsoft rozpoznávač rukopisu pro český jazyk");
                Bcp47ToRecognizerNameDictionary.Add("da", "Microsoft Genkendelse af dansk håndskrift");
                Bcp47ToRecognizerNameDictionary.Add("nb", "Microsoft Håndskriftsgjenkjenner for norsk");
                Bcp47ToRecognizerNameDictionary.Add("nn", "Microsoft Håndskriftsgjenkjenner for nynorsk");
                Bcp47ToRecognizerNameDictionary.Add("fi", "Microsoftin suomenkielinen käsinkirjoituksen tunnistus");
                Bcp47ToRecognizerNameDictionary.Add("ro", "Microsoft recunoaştere grafie - Română");
                Bcp47ToRecognizerNameDictionary.Add("hr", "Microsoftov hrvatski rukopisni prepoznavač");
                Bcp47ToRecognizerNameDictionary.Add("sr-Latn", "Microsoft prepoznavač rukopisa za srpski (latinica)");
                Bcp47ToRecognizerNameDictionary.Add("sr", "Microsoft препознавач рукописа за српски (ћирилица)");
                Bcp47ToRecognizerNameDictionary.Add("ca", "Reconeixedor d'escriptura manual en català de Microsoft");
            }
        }

        public static string LanguageTagToRecognizerName(string bcp47tag)
        {
            EnsureDictionary();

            string recognizerName = string.Empty;
            try
            {
                recognizerName = Bcp47ToRecognizerNameDictionary[bcp47tag];
            }
            catch (KeyNotFoundException)
            {
                recognizerName = string.Empty;
            }

            return recognizerName;
        }
    }
}
