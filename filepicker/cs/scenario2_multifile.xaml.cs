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
using System.Text;
using System.Collections.Generic;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace FilePicker
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario2 : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario2()
        {
            this.InitializeComponent();
            PickFilesButton.Click += new RoutedEventHandler(PickFilesButton_Click);
        }

        private async void PickFilesButton_Click(object sender, RoutedEventArgs e)
        {
            // Clear any previously returned files between iterations of this scenario
            OutputTextBlock.Text = "";

            FileOpenPicker openPicker = new FileOpenPicker();
            openPicker.ViewMode = PickerViewMode.List;
            openPicker.SuggestedStartLocation = PickerLocationId.DocumentsLibrary;
            openPicker.FileTypeFilter.Add("*");
            IReadOnlyList<StorageFile> files = await openPicker.PickMultipleFilesAsync();
            if (files.Count > 0)
            {
                StringBuilder output = new StringBuilder("Picked files:\n");
                // Application now has read/write access to the picked file(s)
                foreach (StorageFile file in files)
                {
                    output.Append(file.Name + "\n");
                }
                OutputTextBlock.Text = output.ToString();
            }
            else
            {
                OutputTextBlock.Text = "Operation cancelled.";
            }
        }
    }
}
