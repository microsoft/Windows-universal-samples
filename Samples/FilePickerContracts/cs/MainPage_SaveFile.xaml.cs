//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using SDKTemplate;

using System;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace FilePickerContracts
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage_SaveFile : Page
    {
        public MainPage_SaveFile()
        {
            this.InitializeComponent();
            ThisAppTitle.Text = MainPage.APP_TITLE;
            SaveFileButton.Click += new RoutedEventHandler(SaveFileButton_Click);
        }

        private async void SaveFileButton_Click(object sender, RoutedEventArgs e)
        {
            MainPage page = MainPage.Current;

            //$$page.ResetScenarioOutput(OutputTextBlock);
            // Set up and launch the Save Picker
            FileSavePicker fileSavePicker = new FileSavePicker();
            fileSavePicker.FileTypeChoices.Add("PNG", new string[] { ".png" });

            StorageFile file = await fileSavePicker.PickSaveFileAsync();
            if (file != null)
            {
                // At this point, the app can begin writing to the provided save file
                OutputTextBlock.Text = file.Name;
            }
        }
    }
}
