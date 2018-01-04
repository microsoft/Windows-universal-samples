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
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Documents;

namespace SDKTemplate
{
    public sealed partial class DeviceReadWrite
    {
        MainPage rootPage = MainPage.Current;

        public DeviceReadWrite()
        {
            this.InitializeComponent();
        }

        private int writeCounter = 0;
        private int readCounter = 0;

        private async void ReadBlock_Click(object sender, RoutedEventArgs e)
        {
            var fx2Device = DeviceList.Current.GetSelectedDevice();

            if (fx2Device == null)
            {
                rootPage.NotifyUser("Fx2 device not connected or accessible", NotifyType.ErrorMessage);
                return;
            }

            var button = (Button)sender;
            button.IsEnabled = false;

            var dataReader = new DataReader(fx2Device.InputStream);

            int counter = readCounter++;
            LogMessage($"Read {counter} begin");

            // load the data reader from the stream.  For purposes of this 
            // sample, assume all messages read are < 64 bytes

            await dataReader.LoadAsync(64);
            
            // Get the message string out of the buffer
            var message = dataReader.ReadString(dataReader.UnconsumedBufferLength);

            LogMessage($"Read {counter} end: {message}");

            button.IsEnabled = true;
        }

        private async void WriteBlock_Click(object sender, RoutedEventArgs e)
        {
            var fx2Device = DeviceList.Current.GetSelectedDevice();

            if (fx2Device == null)
            {
                rootPage.NotifyUser("Fx2 device not connected or accessible", NotifyType.ErrorMessage);
                return;
            }

            var button = (Button)sender;
            button.IsEnabled = false;

            // Generate a string to write to the device
            var counter = writeCounter++;
            
            // Use a data writer to convert the string into a buffer
            var dataWriter = new DataWriter();
            var msg = "This is message " + counter;
            dataWriter.WriteString(msg);

            LogMessage($"Write {counter} begin: {msg}");
            var bytesRead = await fx2Device.OutputStream.WriteAsync(dataWriter.DetachBuffer());
            LogMessage($"Write {counter} end: {bytesRead} bytes written");

            button.IsEnabled = true;
        }

        private void LogMessage(string message)
        {
            var span = new Span();
            var run = new Run();
            run.Text = message;
            
            span.Inlines.Add(run);
            span.Inlines.Add(new LineBreak());

            OutputText.Inlines.Insert(0, span);
        }
    }
}
