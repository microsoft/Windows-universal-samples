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
using Windows.ApplicationModel.Activation;
using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Contact Panel C# Sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Scenario 1", ClassType=typeof(Scenario1_CreateContact)},
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }


    internal static class Constants
    {
        // The protocol we use to launch ourselves.
        internal const string Protocol = "cpsdkcs";

        // Information about the sample contact.

        // The remote ID should be globally unique. This is typically done by
        // prefixing it with the name of the app. This sample also prefixes the
        // protocol so that multiple versions of the sample can coexist (C++, C#, etc).
        internal static string ContactRemoteId = $"ms-{Protocol}:31415";
        internal const string ContactListName = "ContactPanelSample";
        internal const string ContactEmail = "janedoe@example.com";
        internal const string ContactPhone = "4255550123";
        internal const string ContactFirstName = "Jane";
        internal const string ContactLastName = "Doe";
        internal static string ContactImageSource = "https://docs.microsoft.com/en-us/windows/uwp/contacts-and-calendar/images/shoulder-tap-static-payload.png";
    }



    public partial class App : Application
    {
        protected override void OnActivated(IActivatedEventArgs args)
        {
            Frame rootFrame = CreateRootFrame();

            switch (args.Kind)
            {
                case ActivationKind.Protocol:
                    {
                        if (rootFrame.Content == null)
                        {
                            rootFrame.Navigate(typeof(MainPage));
                        }

                        // Parse the contact remote ID from the protocol arguments.
                        string contactRemoteId = String.Empty;
                        var protocolArgs = (ProtocolActivatedEventArgs)args;
                        var decoder = new WwwFormUrlDecoder(protocolArgs.Uri.Query);
                        try
                        {
                            contactRemoteId = decoder.GetFirstValueByName("ContactRemoteId");
                        }
                        catch (ArgumentException)
                        {
                            // Value not present in query string.
                        }

                        //  AppContactPanel_LaunchFullAppRequested passed in the remoteId. Display it to the user.
                        MainPage.Current.NotifyUser("Protocol launch with remoteId = " + contactRemoteId, NotifyType.StatusMessage);

                        Window.Current.Activate();
                        break;
                    }

                case ActivationKind.ContactPanel:
                    {
                        if (rootFrame.Content == null)
                        {
                            rootFrame.Navigate(typeof(AppContactPanel));
                        }

                        var appContactPanel = (AppContactPanel)rootFrame.Content;
                        appContactPanel.OnActivated((ContactPanelActivatedEventArgs)args);

                        Window.Current.Activate();
                        break;
                    }
            }
        }
    }
}
