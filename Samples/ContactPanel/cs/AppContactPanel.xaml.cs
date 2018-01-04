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
using Windows.ApplicationModel.Contacts;
using Windows.System;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    /// <summary>
    /// A page that is hosted within the MyPeople flyout on the taskbar.
    /// </summary>
    public sealed partial class AppContactPanel : Page
    {
        string _appContactId = string.Empty;
        Contact lightContact = null;
        Contact fullContact = null;

        public AppContactPanel()
        {
            this.InitializeComponent();
        }

        public async void OnActivated(ContactPanelActivatedEventArgs args)
        {
            // Register the handler for a full app launch from the My People flyout
            args.ContactPanel.LaunchFullAppRequested += AppContactPanel_LaunchFullAppRequested;

            // Apply some customization.
            args.ContactPanel.HeaderColor = Windows.UI.Colors.LightBlue;

            // The Contact object that is passed in with ContactPanelActivatedEventArgs is not very usable,
            // we need to obtain a full contact from the ContactStore in order to do anything interesting.
            lightContact = args.Contact;

            await InitializePageAsync();
        }

        public async Task InitializePageAsync()
        {
            if (fullContact == null)
            {
                ContactStore store = await ContactManager.RequestStoreAsync(ContactStoreAccessType.AppContactsReadWrite);
                try
                {
                    fullContact = await store.GetContactAsync(lightContact.Id);
                }
                catch
                {
                    // User denied access to the contact database.
                }
            }

            if (fullContact != null)
            {
                // Now that we have a full Contact object, we can do richer operations such as querying the display name
                this.contactName.Text = fullContact.DisplayName;

                // This is the remoteId that your app annotated the contact with on creation
                this.remoteId.Text = _appContactId = await GetRemoteIdForContactAsync(fullContact);

                if (RequestContactAccessPanel.Visibility == Visibility.Visible)
                {
                    Window.Current.Activated -= OnWindowActivated;
                    ContactHistoryPanel.Visibility = Visibility.Visible;
                    RequestContactAccessPanel.Visibility = Visibility.Collapsed;
                }
            }
            else
            {
                // Prompt the user to grant access to contacts.
                if (RequestContactAccessPanel.Visibility == Visibility.Collapsed)
                {
                    ContactHistoryPanel.Visibility = Visibility.Collapsed;
                    RequestContactAccessPanel.Visibility = Visibility.Visible;

                    // Re-check each time we regain activation.
                    Window.Current.Activated += OnWindowActivated;
                }
            }
        }

        public async Task<string> GetRemoteIdForContactAsync(Contact fullContact)
        {       
            var contactAnnotationStore = await ContactManager.RequestAnnotationStoreAsync(ContactAnnotationStoreAccessType.AppAnnotationsReadWrite);

            var contactAnnotations = await contactAnnotationStore.FindAnnotationsForContactAsync(fullContact);

            if (contactAnnotations.Count >= 0)
            {
                return contactAnnotations[0].RemoteId;
            }

            return string.Empty;
        }

        private async void AppContactPanel_LaunchFullAppRequested(ContactPanel sender, ContactPanelLaunchFullAppRequestedEventArgs args)
        {
            // callback to relaunch the sample app as a protocol launch with the remoteID for the current
            // contact as a launcher argument.
            args.Handled = true;
            await this.Dispatcher.RunAsync(
                CoreDispatcherPriority.Normal,
                async () =>
                {
                    LauncherOptions options = new LauncherOptions();
                    options.TargetApplicationPackageFamilyName = Windows.ApplicationModel.Package.Current.Id.FamilyName;

                    await Launcher.LaunchUriAsync(new Uri($"{Constants.Protocol}:?ContactRemoteId={Uri.EscapeDataString(_appContactId)}"), options);
                    sender.ClosePanel();
                });
        }

        private async void AllowAccess_Click(object sender, RoutedEventArgs e)
        {
            await Windows.System.Launcher.LaunchUriAsync(new Uri("ms-settings:privacy-contacts"));
        }

        private async void OnWindowActivated(object sender, WindowActivatedEventArgs e)
        {
            if (e.WindowActivationState != CoreWindowActivationState.Deactivated)
            {
                await InitializePageAsync();
            }
        }
    }
}
