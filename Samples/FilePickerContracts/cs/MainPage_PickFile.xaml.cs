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
using System.Collections.Generic;
using System.Text;
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
    public sealed partial class MainPage_PickFile : Page
    {
        public MainPage_PickFile()
        {
            this.InitializeComponent();
            ThisAppTitle.Text = MainPage.APP_TITLE;
            PickFileButton.Click += new RoutedEventHandler(PickFileButton_Click);
        }

        private async void PickFileButton_Click(object sender, RoutedEventArgs e)
        {
            MainPage page = MainPage.Current;

            //$$page.ResetScenarioOutput(OutputTextBlock);
            // Set up and launch the Open Picker
            FileOpenPicker fileOpenPicker = new FileOpenPicker();
            fileOpenPicker.ViewMode = PickerViewMode.Thumbnail;
            fileOpenPicker.FileTypeFilter.Add(".png");
            fileOpenPicker.FileTypeFilter.Add(".txt");

            IReadOnlyList<StorageFile> files = await fileOpenPicker.PickMultipleFilesAsync();
            StringBuilder fileNames = new StringBuilder();
            foreach (StorageFile file in files)
            {
                // At this point, the app can begin reading from the provided file
                fileNames.AppendLine(file.Name);
            }

            OutputTextBlock.Text = fileNames.ToString();
        }
    }
}
