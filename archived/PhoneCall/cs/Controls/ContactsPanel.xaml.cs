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

using PhoneCall.Helpers;
using PhoneCall.ViewModels;
using Windows.UI.Xaml.Controls;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace PhoneCall.Controls
{
    public sealed partial class ContactsPanel : UserControl
    {
        ContactsViewModel contactsVM;

        /// <summary>
        /// A user control that hosts the contacts panel within the main pivot/tab.
        /// </summary>
        public ContactsPanel()
        {
            this.InitializeComponent();
            contactsVM = ViewModelDispatcher.ContactsViewModel;
            DataContext = contactsVM;
        }

        /// <summary>
        /// Gets the string from the text box in the contacts pane
        /// and sendds it to the VM to processes contact search.
        /// </summary>
        private void OnSearchInputchanged(object sender, TextChangedEventArgs e)
        {
            contactsVM.ProcessContactSearchAsync(ContactSearchBox.Text);
        }

        /// <summary>
        /// Open a contacts page upon click.
        /// </summary>
        private void OnContactItemClick(object sender, ItemClickEventArgs e)
        {
            contactsVM.ProcessOpenContactAsync((ContactItem)e.ClickedItem);
        }

        /// <summary>
        /// Collapses all unncessary elements when the user is in semantic zoom out mode
        /// This allows the alphabet grid to be displayed in a bigger space.
        /// </summary>
        private void UpdateCollapseState(object sender, SemanticZoomViewChangedEventArgs e)
        {
            contactsVM.MaximizeSemanticZoomOut(ContactGroupView.IsZoomedInViewActive);
        }
    }
}
