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
using System.Text;
using Windows.Security.Cryptography;
using Windows.Storage.Streams;

namespace AllJoynServerExperiences
{
    class CryptographicHelpers
    {
        public static string GenerateHighEntropyKey(int desiredKeyLength)
        {
            if (desiredKeyLength <= 0)
            {
                throw new ArgumentException("The desired key length must be a positive integer.", "desiredKeyLength");
            }
            else
            {
                const int MaxAttempts = 100;
                int attemptCount = 0;
                StringBuilder sb = new StringBuilder(desiredKeyLength);
                IBuffer randomBuffer;
                string str;

                // Base64 uses 6 bits to encode a single character. Number of bits required = length * 6.
                uint numberOfRandomBytesRequired = (uint)(Math.Round((desiredKeyLength * 6.0) / 8.0));

                while ((sb.Length != desiredKeyLength) && (attemptCount < MaxAttempts))
                {
                    randomBuffer = CryptographicBuffer.GenerateRandom(numberOfRandomBytesRequired);
                    str = CryptographicBuffer.EncodeToBase64String(randomBuffer);

                    foreach (char c in str)
                    {
                        if (((c >= '0') && (c <= '9')) || ((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z')))
                        {
                            sb.Append(c);
                            if (sb.Length == desiredKeyLength)
                            {
                                break;
                            }
                        }
                    }
                    attemptCount++;
                }

                if (sb.Length == desiredKeyLength)
                {
                    return sb.ToString();
                }
                else
                {
                    throw new Exception("Unable to generate a high entropy key.");
                }
            }
        }

        public static string InsertHyphens(string key, int insertHyphenAfterPosition)
        {
            if (string.IsNullOrWhiteSpace(key))
            {
                throw new ArgumentException("Input key must be a valid string.", "key");
            }
            else if (insertHyphenAfterPosition <= 0)
            {
                throw new ArgumentException("Desired hyphen position must be a positive number.", "insertHyphenAfterPosition");
            }
            else if (insertHyphenAfterPosition > key.Length)
            {
                throw new ArgumentException("Desired hyphen position cannot be greater than key length.", "insertHyphenAfterPosition");
            }
            else
            {
                int keyLengthWithHyphens = key.Length + ((key.Length + insertHyphenAfterPosition - 1) / insertHyphenAfterPosition) - 1;
                int count = 0;

                StringBuilder sb = new StringBuilder(keyLengthWithHyphens);
                foreach (char c in key)
                {
                    if (count == insertHyphenAfterPosition)
                    {
                        sb.Append('-');
                        count = 0;
                    }

                    sb.Append(c);
                    count++;
                }

                return sb.ToString();
            }
        }
    }
}
