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

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using Windows.Networking.Sockets;
using Windows.ApplicationModel.Background;
using System;
using Windows.Networking;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.Foundation;

namespace SocketActivityStreamSocket
{
    public sealed partial class Scenario1_Connect : Page
    {
        private MainPage rootPage;
        private const string socketId = "SampleSocket";
        private StreamSocket socket = null;
        private IBackgroundTaskRegistration task = null;
        private const string port = "40404";

        public Scenario1_Connect()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
            try
            {
                foreach (var current in BackgroundTaskRegistration.AllTasks)
                {
                    if (current.Value.Name == "SocketActivityBackgroundTask")
                    {
                        task = current.Value;
                        break;
                    }
                }

                // If there is no task allready created, create a new one
                if (task == null)
                {
                    var socketTaskBuilder = new BackgroundTaskBuilder();
                    socketTaskBuilder.Name = "SocketActivityBackgroundTask";
                    socketTaskBuilder.TaskEntryPoint = "SocketActivityBackgroundTask.SocketActivityTask";
                    var trigger = new SocketActivityTrigger();
                    socketTaskBuilder.SetTrigger(trigger);
                    task = socketTaskBuilder.Register();
                }

                SocketActivityInformation socketInformation;
                if (SocketActivityInformation.AllSockets.TryGetValue(socketId, out socketInformation))
                {
                    // Application can take ownership of the socket and make any socket operation
                    // For sample it is just transfering it back.
                    socket = socketInformation.StreamSocket;
                    socket.TransferOwnership(socketId);
                    socket = null;
                    rootPage.NotifyUser("Connected. You may close the application", NotifyType.StatusMessage);
                    TargetServerTextBox.IsEnabled = false;
                    ConnectButton.IsEnabled = false;
                }

            }
            catch (Exception exception)
            {
                rootPage.NotifyUser(exception.Message, NotifyType.ErrorMessage);
            }
        }

        private async void ConnectButton_Click(object sender, RoutedEventArgs e)
        {
            ApplicationData.Current.LocalSettings.Values["hostname"] = TargetServerTextBox.Text;
            ApplicationData.Current.LocalSettings.Values["port"] = port;

            try
            {
                SocketActivityInformation socketInformation;
                if (!SocketActivityInformation.AllSockets.TryGetValue(socketId, out socketInformation))
                {
                    socket = new StreamSocket();
                    socket.EnableTransferOwnership(task.TaskId, SocketActivityConnectedStandbyAction.Wake);
                    var targetServer = new HostName(TargetServerTextBox.Text);
                    await socket.ConnectAsync(targetServer, port);
                    // To demonstrate usage of CancelIOAsync async, have a pending read on the socket and call 
                    // cancel before transfering the socket. 
                    DataReader reader = new DataReader(socket.InputStream);
                    reader.InputStreamOptions = InputStreamOptions.Partial;
                    var read = reader.LoadAsync(250);
                    read.Completed += (info, status) =>
                    {

                    };
                    await socket.CancelIOAsync();
                    socket.TransferOwnership(socketId);
                    socket = null;
                }
                ConnectButton.IsEnabled = false;
                rootPage.NotifyUser("Connected. You may close the application", NotifyType.StatusMessage);
            }
            catch (Exception exception)
            {
                rootPage.NotifyUser(exception.Message, NotifyType.ErrorMessage);
            }
        }
    }
}
