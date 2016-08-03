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
using Windows.UI.Core;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    /// <summary>
    /// A simple logger to display text to TextBlock asynchronisely.
    /// </summary>
    class SimpleLogger
    {
        private CoreDispatcher _dispatcher;
        private TextBlock _textBlock;
        private string _messageText = string.Empty;
        private readonly object _messageLock = new object();
        private int _messageCount;

        public SimpleLogger(TextBlock textBlock)
        {
            _textBlock = textBlock;
            _dispatcher = _textBlock.Dispatcher;
        }

        /// <summary>
        /// Logs a message to be displayed.
        /// </summary>
        internal async void Log(string message)
        {
            var newMessage = $"[{_messageCount ++}] {DateTime.Now.ToString("hh:MM:ss")} : {message}\n{_messageText}";

            lock(_messageLock)
            {
                _messageText = newMessage;
            }

            await _dispatcher.RunAsync(CoreDispatcherPriority.Low, () =>
            {
                lock (_messageLock)
                {
                    _textBlock.Text = _messageText;
                }
            });
        }
    }
}
