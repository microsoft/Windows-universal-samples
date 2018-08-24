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

using Windows.Data.Xml.Dom;
using Windows.UI.Notifications;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    /// <summary>
    /// This control is used to render an xml markup (via the Markup dependency property), which is then used as the 
    /// payload to send via a toast notification when the "Send notification" button is clicked.
    /// </summary>
    public sealed partial class Payload : UserControl
    {
        public static readonly DependencyProperty MarkupProperty = DependencyProperty.Register(nameof(Markup), typeof(string), typeof(Payload), null);
        public static readonly DependencyProperty FormattedMarkupProperty = DependencyProperty.Register(nameof(FormattedMarkup), typeof(string), typeof(Payload), null);

        public Payload()
        {
            this.InitializeComponent();
        }

        public string Markup
        {
            get { return this.GetValue(MarkupProperty) as string; }
            set
            {
                this.SetValue(MarkupProperty, value);
                FormattedMarkup = value
                    .Replace("{email}", Constants.ContactEmail)
                    .Replace("{tel}", Constants.ContactPhone)
                    .Replace("{remoteid}", Constants.ContactRemoteId);
            }
        }

        public string FormattedMarkup
        {
            get { return this.GetValue(FormattedMarkupProperty) as string; }
            private set { this.SetValue(FormattedMarkupProperty, value); }
        }

        private void OnSendClicked(object sender, RoutedEventArgs e)
        {
            XmlDocument toastXml = new XmlDocument();
            toastXml.LoadXml(this.FormattedMarkup);
            ToastNotification notification = new ToastNotification(toastXml);
            ToastNotificationManager.CreateToastNotifier().Show(notification);
        }
    }
}
