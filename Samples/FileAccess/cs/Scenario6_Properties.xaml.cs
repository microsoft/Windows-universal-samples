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
using System.Text;
using Windows.Storage;
using Windows.Storage.FileProperties;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// Displaying file properties.
    /// </summary>
    public sealed partial class Scenario6 : Page
    {
        MainPage rootPage = MainPage.Current;

        static readonly string dateAccessedProperty = "System.DateAccessed";
        static readonly string fileOwnerProperty    = "System.FileOwner";

        public Scenario6()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage.ValidateFile();
        }

        private async void ShowPropertiesButton_Click(object sender, RoutedEventArgs e)
        {
            StorageFile file = rootPage.sampleFile;
            if (file != null)
            {
                try
                {
                    // Get top level file properties
                    StringBuilder outputText = new StringBuilder();
                    outputText.AppendLine($"File name: {file.Name}");
                    outputText.AppendLine($"File type: {file.FileType}");

                    // Get basic properties
                    BasicProperties basicProperties = await file.GetBasicPropertiesAsync();
                    outputText.AppendLine($"File size: {basicProperties.Size} bytes");
                    outputText.AppendLine($"Date modified: {basicProperties.DateModified}");

                    // Get extra properties
                    IDictionary<string, object> extraProperties = await file.Properties.RetrievePropertiesAsync(new[] { dateAccessedProperty, fileOwnerProperty });
                    var propValue = extraProperties[dateAccessedProperty];
                    if (propValue != null)
                    {
                        outputText.AppendLine($"Date accessed: {propValue}");
                    }
                    propValue = extraProperties[fileOwnerProperty];
                    if (propValue != null)
                    {
                        outputText.Append($"File owner: {propValue}");
                    }

                    rootPage.NotifyUser(outputText.ToString(), NotifyType.StatusMessage);
                }
                catch (FileNotFoundException)
                {
                    rootPage.NotifyUserFileNotExist();
                }
                catch (Exception ex)
                {
                    // I/O errors are reported as exceptions.
                    rootPage.NotifyUser($"Error retrieving properties for '{file.Name}': {ex.Message}", NotifyType.ErrorMessage);
                }
            }
            else
            {
                rootPage.NotifyUserFileNotExist();
            }
        }
    }
}
