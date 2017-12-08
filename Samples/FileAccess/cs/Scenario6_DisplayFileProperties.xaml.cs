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
        MainPage rootPage;

        static readonly string dateAccessedProperty = "System.DateAccessed";
        static readonly string fileOwnerProperty    = "System.FileOwner";

        public Scenario6()
        {
            this.InitializeComponent();
            ShowPropertiesButton.Click += new RoutedEventHandler(ShowPropertiesButton_Click);
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
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
                    outputText.AppendLine(String.Format("File name: {0}", file.Name));
                    outputText.AppendLine(String.Format("File type: {0}", file.FileType));

                    // Get basic properties
                    BasicProperties basicProperties = await file.GetBasicPropertiesAsync();
                    outputText.AppendLine(String.Format("File size: {0} bytes", basicProperties.Size));
                    outputText.AppendLine(String.Format("Date modified: {0}", basicProperties.DateModified));

                    // Get extra properties
                    List<string> propertiesName = new List<string>();
                    propertiesName.Add(dateAccessedProperty);
                    propertiesName.Add(fileOwnerProperty);
                    IDictionary<string, object> extraProperties = await file.Properties.RetrievePropertiesAsync(propertiesName);
                    var propValue = extraProperties[dateAccessedProperty];
                    if (propValue != null)
                    {
                        outputText.AppendLine(String.Format("Date accessed: {0}", propValue));
                    }
                    propValue = extraProperties[fileOwnerProperty];
                    if (propValue != null)
                    {
                        outputText.Append(String.Format("File owner: {0}", propValue));
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
                    rootPage.NotifyUser(String.Format("Error retrieving properties for '{0}': {1}", file.Name, ex.Message), NotifyType.ErrorMessage);
                }
            }
            else
            {
                rootPage.NotifyUserFileNotExist();
            }
        }
    }
}
