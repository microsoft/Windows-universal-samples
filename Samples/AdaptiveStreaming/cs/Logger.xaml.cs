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
using System.Linq;
using Windows.ApplicationModel.DataTransfer;
using Windows.UI.Core;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Logger : UserControl
    {
        public Logger()
        {
            this.InitializeComponent();
        }

        public async void Log(string message)
        {
            string logEntry = $"{DateTime.Now:HH:mm:ss.fff} - {message}";
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                LoggingListBox.Items.Insert(0, new TextBlock() { Text = logEntry });
            });
        }

        public void CopyLogToClipboard_Click()
        {
            var content = new DataPackage();
            content.SetText(String.Join("\r\n", LoggingListBox.Items.Select(item => (item as TextBlock).Text)));
            Clipboard.SetContent(content);
        }

        public void ClearLog_Click()
        {
            LoggingListBox.Items.Clear();
        }
    }
}
