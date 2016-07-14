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
using System.Linq;
using Windows.ApplicationModel;
using Windows.Foundation;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.System;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario2_Launch : Page
    {
        private static string PackageFamilyName3DBuilder = "Microsoft.3DBuilder_8wekyb3d8bbwe";

        private MainPage rootPage = MainPage.Current;

        public Scenario2_Launch()
        {
            this.InitializeComponent();
        }

        private async void CheckIf3DBuilderIsInstalled()
        {
            IReadOnlyList<AppInfo> handlers = await Launcher.FindFileHandlersAsync(".3mf");
            if (handlers.Any(info => info.PackageFamilyName == PackageFamilyName3DBuilder))
            {
                rootPage.NotifyUser("3D Builder is installed", NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("3D Builder is not installed", NotifyType.ErrorMessage);
            }
        }

        private async void LaunchFileIn3DBuilder()
        {
            FileOpenPicker openPicker = new FileOpenPicker();
            openPicker.ViewMode = PickerViewMode.Thumbnail;
            openPicker.SuggestedStartLocation = PickerLocationId.DocumentsLibrary;
            openPicker.FileTypeFilter.Add(".3mf");
            openPicker.FileTypeFilter.Add(".stl");

            StorageFile file = await openPicker.PickSingleFileAsync();
            if (file == null)
            {
                return;
            }

            var options = new LauncherOptions();

            // The target application is 3D Builder.
            options.TargetApplicationPackageFamilyName = PackageFamilyName3DBuilder;

            // If 3D Builder is not installed, redirect the user to the Store to install it.
            options.FallbackUri = new Uri("ms-windows-store:PDP?PFN=" + PackageFamilyName3DBuilder);

            await Launcher.LaunchFileAsync(file, options);
        }
    }
}
