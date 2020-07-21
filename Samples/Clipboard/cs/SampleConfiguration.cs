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
using System.Text;
using Windows.ApplicationModel.DataTransfer;
using Windows.Data.Xml.Dom;
using Windows.UI.Core;
using Windows.UI.Notifications;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Clipboard C# sample";

        public bool IsClipboardContentChangedEnabled { get; private set; } = false;
        private bool needToPrintClipboardFormat;
        private bool isApplicationWindowActive = true;

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Copy and paste text",        ClassType = typeof(CopyText) },
            new Scenario() { Title = "Copy and paste an image",    ClassType = typeof(CopyImage) },
            new Scenario() { Title = "Copy and paste files",       ClassType = typeof(CopyFiles) },
            new Scenario() { Title = "History and roaming",        ClassType = typeof(HistoryAndRoaming) },
            new Scenario() { Title = "Other Clipboard operations", ClassType = typeof(OtherScenarios) }
        };

        public static ToastNotification DisplayToast(string message)
        {
            string xml = @"<toast activationType='foreground'>
                                <visual>
                                    <binding template='ToastGeneric'/>
                                </visual>
                            </toast>";

            XmlDocument doc = new XmlDocument();
            doc.LoadXml(xml);

            var binding = doc.SelectSingleNode("//binding");

            var el = doc.CreateElement("text");
            el.InnerText = FEATURE_NAME;
            binding.AppendChild(el);

            el = doc.CreateElement("text");
            el.InnerText = message;
            binding.AppendChild(el);

            var toast = new ToastNotification(doc);

            ToastNotificationManager.CreateToastNotifier().Show(toast); //Show the toast

            return toast;
        }

        public bool EnableClipboardContentChangedNotifications(bool enable)
        {
            if (IsClipboardContentChangedEnabled == enable)
            {
                return false;
            }

            IsClipboardContentChangedEnabled = enable;
            if (enable)
            {
                Clipboard.ContentChanged += OnClipboardChanged;
                Window.Current.Activated += OnWindowActivated;
            }
            else
            {
                Clipboard.ContentChanged -= OnClipboardChanged;
                Window.Current.Activated -= OnWindowActivated;
            }
            return true;
        }

        public string BuildClipboardFormatsOutputString()
        {
            DataPackageView clipboardContent = Windows.ApplicationModel.DataTransfer.Clipboard.GetContent();
            StringBuilder output = new StringBuilder();

            if (clipboardContent != null && clipboardContent.AvailableFormats.Count > 0)
            {
                output.Append("Available formats in the clipboard:");
                foreach (var format in clipboardContent.AvailableFormats)
                {
                    output.Append(Environment.NewLine + " * " + format);
                }
            }
            else
            {
                output.Append("The clipboard is empty");
            }
            return output.ToString();
        }

        private void DisplayChangedFormats()
        {
            string output = "Clipboard content has changed!" + Environment.NewLine;
            output += BuildClipboardFormatsOutputString();
            NotifyUser(output, NotifyType.StatusMessage);
        }

        private void HandleClipboardChanged()
        {
            if (this.isApplicationWindowActive)
            {
                DisplayChangedFormats();
            }
            else
            {
                // Background applications can't access clipboard
                // Deferring processing of update notification until the application returns to foreground
                this.needToPrintClipboardFormat = true;
            }
        }

        private void OnWindowActivated(object sender, WindowActivatedEventArgs e)
        {
            this.isApplicationWindowActive = (e.WindowActivationState != CoreWindowActivationState.Deactivated);
            if (this.needToPrintClipboardFormat)
            {
                // The clipboard was updated while the sample was in the background. If the sample is now in the foreground, 
                // display the new content. 
                HandleClipboardChanged();
            }
        }

        private void OnClipboardChanged(Object sender, Object e)
        {
            DisplayToast("Clipboard changed.");
            HandleClipboardChanged();
        }
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
