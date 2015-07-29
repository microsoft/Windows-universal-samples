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
using System.Threading.Tasks;
using Windows.ApplicationModel.Activation;
using Windows.ApplicationModel;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using SDKTemplateCS;

namespace LockScreenAppsCS
{
    public partial class App
    {
        public App()
        {
            InitializeComponent();
            this.Suspending += new SuspendingEventHandler(OnSuspending);
        }

        async protected void OnSuspending(object sender, SuspendingEventArgs args)
        {
            SuspendingDeferral deferral = args.SuspendingOperation.GetDeferral();
            await SuspensionManager.SaveAsync();
            deferral.Complete();
        }

        async protected override void OnLaunched(LaunchActivatedEventArgs args)
        {
            if (args.PreviousExecutionState == ApplicationExecutionState.Terminated)
            {
                //     Do an asynchronous restore
                await SuspensionManager.RestoreAsync();
            }
            var rootFrame = new Frame();
            rootFrame.Navigate(typeof(MainPage));
            Window.Current.Content = rootFrame;
            MainPage p = rootFrame.Content as MainPage;
            p.RootNamespace = this.GetType().Namespace;
            Window.Current.Activate();
        }
    }
}
