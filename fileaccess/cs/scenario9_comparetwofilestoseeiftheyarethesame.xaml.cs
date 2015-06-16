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
using System.IO;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// Comparing two files to see if they are the same file.
    /// </summary>
    public sealed partial class Scenario9 : Page
    {
        MainPage rootPage;

        public Scenario9()
        {
            this.InitializeComponent();
            CompareFilesButton.Click += new RoutedEventHandler(CompareFilesButton_Click);
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
            rootPage.ValidateFile();
        }

        private async void CompareFilesButton_Click(object sender, RoutedEventArgs e)
        {
            StorageFile file = rootPage.sampleFile;
            if (file != null)
            {
                FileOpenPicker picker = new FileOpenPicker();
                picker.SuggestedStartLocation = PickerLocationId.PicturesLibrary;
                picker.FileTypeFilter.Add("*");
                StorageFile comparand = await picker.PickSingleFileAsync();
                if (comparand != null)
                {
                    try
                    {
                        if (file.IsEqual(comparand))
                        {
                            rootPage.NotifyUser("Files are equal", NotifyType.StatusMessage);
                        }
                        else
                        {
                            rootPage.NotifyUser("Files are not equal", NotifyType.StatusMessage);
                        }
                    }
                    catch (FileNotFoundException)
                    {
                        rootPage.NotifyUserFileNotExist();
                    }
                }
                else
                {
                    rootPage.NotifyUser("Operation cancelled", NotifyType.StatusMessage);
                }
            }
            else
            {
                rootPage.NotifyUserFileNotExist();
            }
        }
    }
}
