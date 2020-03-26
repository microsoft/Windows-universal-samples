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

namespace PhoneCall.ViewModels
{
    /// <summary>
    /// A simple class to lazy load and dispatch the view model of our modeules.
    /// The Lazy<T> ensures the classes are not created until referenced by name
    /// They are static to ensure they can be accessed across many classes, just not their views
    /// E.g This is used in the main view to ensure the UI gets updated upon different events 
    /// fired only in the main view (Resume, pivot change)
    /// </summary>
    static class ViewModelDispatcher
    {
        private static Lazy<StatusViewModel> statusViewModel;
        private static Lazy<VoicemailViewModel> voicemailViewModel;
        private static Lazy<LinePickerViewModel> linePickerViewModel;
        private static Lazy<ContactsViewModel> contactsViewModel;
        private static Lazy<DialerViewModel> dialerViewModel;

        /// <summary>
        /// Ensures there is one contacts view model and its only created when referenced.
        /// </summary>
        public static ContactsViewModel ContactsViewModel
        {
            get
            {
                if (contactsViewModel == null)
                {
                    contactsViewModel = new Lazy<ContactsViewModel>();
                }
                return contactsViewModel.Value;
            }
        }

        /// <summary>
        /// Ensures there is one line picker view model and its only created when referenced.
        /// </summary>
        public static LinePickerViewModel LinePickerViewModel
        {
            get
            {
                if (linePickerViewModel == null)
                {
                    linePickerViewModel = new Lazy<LinePickerViewModel>();
                }
                return linePickerViewModel.Value;
            }
        }

        /// <summary>
        /// Ensures there is one status view model and its only created when referenced.
        /// </summary>
        public static StatusViewModel StatusViewModel
        {
            get
            {
                if (statusViewModel == null)
                {
                    statusViewModel = new Lazy<StatusViewModel>();
                }
                return statusViewModel.Value;
            }
        }

        /// <summary>
        /// Ensures there is one voicemail view model and its only created when referenced.
        /// </summary>
        public static VoicemailViewModel VoicemailViewModel
        {
            get
            {
                if (voicemailViewModel == null)
                {
                    voicemailViewModel = new Lazy<VoicemailViewModel>();
                }
                return voicemailViewModel.Value;
            }
        }

        /// <summary>
        /// Ensures there is one dialer view model and its only created when referenced.
        /// </summary>
        public static DialerViewModel DialerViewModel
        {
            get
            {
                if (dialerViewModel == null)
                {
                    dialerViewModel = new Lazy<DialerViewModel>();
                }
                return dialerViewModel.Value;
            }
        }
    }
}
