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

using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.Devices.PointOfService;
using Windows.Globalization;
using Windows.Graphics.Imaging;
using Windows.Media.Ocr;
using Windows.Storage;

namespace BarcodeDecoder
{
    internal class DecodeResult
    {
        public uint Symbology { get; set; }
        public string Text { get; set; }

        public override bool Equals(Object obj)
        {
            DecodeResult other = obj as DecodeResult;
            if (other == null)
            {
                return false;
            }
            return (Symbology == other.Symbology) && (Text == other.Text);
        }

        public override int GetHashCode()
        {
            return Text.GetHashCode() ^ (int)Symbology;
        }
    };

    // Simple OCR-based decoder engine.
    internal class DecodeEngine
    {
        private OcrEngine ocrEngine = null;
        private static ApplicationDataContainer localSettings = ApplicationData.Current.LocalSettings;

        public List<uint> SupportedSymbologies { get; private set; }
        public List<uint> ActiveSymbologies { get; set; }

        public DecodeEngine()
        {
            // Create OCR engine from the language set in scenario 1 configuration.
            string languageTag = localSettings.Values["decoderLanguage"] as string;

            foreach (Language lang in OcrEngine.AvailableRecognizerLanguages)
            {
                if (lang.LanguageTag == languageTag)
                {
                    ocrEngine = OcrEngine.TryCreateFromLanguage(lang);
                    break;
                }
            }

            if (ocrEngine == null)
            {
                ocrEngine = OcrEngine.TryCreateFromUserProfileLanguages();
            }

            SupportedSymbologies = new List<uint> { BarcodeSymbologies.OcrA };
            ActiveSymbologies = SupportedSymbologies;
        }

        public async Task<DecodeResult> DecodeAsync(SoftwareBitmap bitmap)
        {
            // The only thing we decode is OCR.
            if (!ActiveSymbologies.Contains(BarcodeSymbologies.OcrA))
            {
                return null;
            }

            OcrResult ocrResult = await ocrEngine.RecognizeAsync(bitmap);

            // Get the text in the first non-empty line as result
            foreach (OcrLine line in ocrResult.Lines)
            {
                if (!string.IsNullOrEmpty(line.Text))
                {
                    return new DecodeResult
                    {
                        Symbology = BarcodeSymbologies.OcrA,
                        Text = line.Text
                    };
                }
            }
            return null;
        }
    }
}
