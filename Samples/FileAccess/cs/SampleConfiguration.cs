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
using System.IO;
using Windows.Security.Cryptography;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "File access C# sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Creating a file",                                      ClassType = typeof(SDKTemplate.Scenario1)  },
            new Scenario() { Title = "Getting a file's parent folder",                       ClassType = typeof(SDKTemplate.Scenario2)  },
            new Scenario() { Title = "Writing and reading text in a file",                   ClassType = typeof(SDKTemplate.Scenario3)  },
            new Scenario() { Title = "Writing and reading bytes in a file",                  ClassType = typeof(SDKTemplate.Scenario4)  },
            new Scenario() { Title = "Writing and reading using a stream",                   ClassType = typeof(SDKTemplate.Scenario5)  },
            new Scenario() { Title = "Displaying file properties",                           ClassType = typeof(SDKTemplate.Scenario6)  },
            new Scenario() { Title = "Persisting access to a storage item for future use",   ClassType = typeof(SDKTemplate.Scenario7)  },
            new Scenario() { Title = "Copying a file",                                       ClassType = typeof(SDKTemplate.Scenario8)  },
            new Scenario() { Title = "Comparing two files to see if they are the same file", ClassType = typeof(SDKTemplate.Scenario9)  },
            new Scenario() { Title = "Deleting a file",                                      ClassType = typeof(SDKTemplate.Scenario10) },
            new Scenario() { Title = "Attempting to get a file with no error on failure",    ClassType = typeof(SDKTemplate.Scenario11) },
        };

        public const string filename = "sample.dat";
        public StorageFile sampleFile = null;
        public string mruToken = null;
        public string falToken = null;
        public const int E_NO_UNICODE_TRANSLATION = unchecked((int)0x80070459);

        /// <summary>
        /// Checks if sample file already exists, if it does assign it to sampleFile
        /// </summary>
        internal async void ValidateFile()
        {
            StorageFolder picturesLibrary = await KnownFolders.GetFolderForUserAsync(null /* current user */, KnownFolderId.PicturesLibrary);
            sampleFile = (await picturesLibrary.TryGetItemAsync(filename)) as StorageFile;
            if (sampleFile == null)
            {
                // If file doesn't exist, indicate users to use scenario 1
                NotifyUserFileNotExist();
            }
        }

        internal void NotifyUserFileNotExist()
        {
            NotifyUser(String.Format("The file '{0}' does not exist. Use scenario one to create this file.", filename), NotifyType.ErrorMessage);
        }

        static internal IBuffer GetBufferFromString(string str)
        {
            if (String.IsNullOrEmpty(str))
            {
                return new Windows.Storage.Streams.Buffer(0);
            }
            else
            {
                return CryptographicBuffer.ConvertStringToBinary(str, BinaryStringEncoding.Utf8);
            }
        }

        static internal string GetStringFromBuffer(IBuffer buffer)
        {
            // Throws E_NO_UNICODE_TRANSLATION if the buffer is not properly encoded.
            return CryptographicBuffer.ConvertBinaryToString(BinaryStringEncoding.Utf8, buffer);
        }
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
