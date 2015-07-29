//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using Windows.ApplicationModel;
using Windows.ApplicationModel.Activation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// Provides application-specific behavior to supplement the default Application class.
    /// </summary>
    sealed partial class App : Application
    {
        protected override void OnFileOpenPickerActivated(FileOpenPickerActivatedEventArgs args)
        {
            var FileOpenPickerPage = new SDKTemplate.FileOpenPickerPage();
            FileOpenPickerPage.Activate(args);
        }

        protected override void OnFileSavePickerActivated(FileSavePickerActivatedEventArgs args)
        {
            var FileSavePickerPage = new SDKTemplate.FileSavePickerPage();
            FileSavePickerPage.Activate(args);
        }

        protected override void OnCachedFileUpdaterActivated(CachedFileUpdaterActivatedEventArgs args)
        {
            var CachedFileUpdaterPage = new SDKTemplate.CachedFileUpdaterPage();
            CachedFileUpdaterPage.Activate(args);
        }
    }
}
