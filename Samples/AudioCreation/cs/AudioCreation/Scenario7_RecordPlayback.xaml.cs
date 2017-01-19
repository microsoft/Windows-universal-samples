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

using SDKTemplate;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading.Tasks;
using Windows.Devices.Enumeration;
using Windows.Foundation;
using Windows.Media;
using Windows.Media.Audio;
using Windows.Media.Capture;
using Windows.Media.Devices;
using Windows.Media.MediaProperties;
using Windows.Media.Render;
using Windows.Media.Transcoding;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;


// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace AudioCreation
{
    /// <summary>
    /// This scenario shows using AudioGraph for audio capture from a microphone with low latency.
    /// </summary>
    public sealed partial class Scenario7_RecordPlayback : Page
    {
        private MainPage rootPage;
        private AudioGraph graph;
        private AudioDeviceOutputNode deviceOutputNode;
        private AudioDeviceInputNode deviceInputNode;
        private AudioFrameOutputNode frameOutputNode;
        private AudioFrameInputNode frameInputNode;
        private bool isRecording;
        private bool isPlaying;
        // 1024K stereo float audio samples (8MB byte array)
        private byte[] byteBuffer = new byte[1024 * 1024 * 2 * 4];
        private DeviceInformationCollection outputDevices;

        public Scenario7_RecordPlayback()
        {
            this.InitializeComponent();
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
            await PopulateDeviceList();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            if (graph != null)
            {
                graph.Dispose();
            }
        }

        private void RecordButton_Click(object sender, RoutedEventArgs e)
        {
            ToggleRecord();
        }

        private void PlayButton_Click(object sender, RoutedEventArgs e)
        {
            TogglePlay();
        }

        private async void CreateGraphButton_Click(object sender, RoutedEventArgs e)
        {
            await CreateAudioGraph();
        }

        private void ToggleRecord()
        {
            if (!isRecording)
            {
                Debug.Assert(!isRecording);
                Debug.Assert(!isPlaying);

                graph.Start();
                recordButton.Content = "Stop";
                playButton.IsEnabled = false;
                isRecording = true;
            }
            else
            {
                Debug.Assert(isRecording);
                Debug.Assert(!isPlaying);

                // Good idea to stop the graph to avoid data loss
                graph.Stop();

                recordButton.Content = "Record";
                rootPage.NotifyUser("Recording to memory buffer completed successfully!", NotifyType.StatusMessage);
                createGraphButton.IsEnabled = false;
                playButton.IsEnabled = true;
            }
        }

        private void TogglePlay()
        {
            if (!isPlaying)
            {
                Debug.Assert(!isRecording);
                Debug.Assert(!isPlaying);

                graph.Start();
                playButton.Content = "Stop";
                recordButton.IsEnabled = false;
                isPlaying = true;
            }
            else if (recordButton.Content.Equals("Stop"))
            {
                Debug.Assert(!isRecording);
                Debug.Assert(isPlaying);

                // Good idea to stop the graph to avoid data loss
                graph.Stop();

                playButton.Content = "Stop";
                rootPage.NotifyUser("Playing from memory buffer completed successfully!", NotifyType.StatusMessage);
                recordButton.IsEnabled = true;
                isPlaying = false;
            }
        }

        static int s_zeroByteBufferCount, s_nonZeroByteBufferCount;

        private unsafe void Graph_QuantumStarted(AudioGraph sender, object args)
        {
            AudioFrame frame = frameOutputNode.GetFrame();

            using (AudioBuffer buffer = frame.LockBuffer(AudioBufferAccessMode.Read))
            using (IMemoryBufferReference reference = buffer.CreateReference())
            {
                byte* dataInBytes;
                uint capacityInBytes;

                // Get the buffer from the AudioFrame
                ((IMemoryBufferByteAccess)reference).GetBuffer(out dataInBytes, out capacityInBytes);

                if (capacityInBytes == 0)
                {
                    s_zeroByteBufferCount++;
                }
                else
                {
                    s_nonZeroByteBufferCount++;
                }

                fixed (byte* buf = byteBuffer)
                {
                    for (int i = 0; i < capacityInBytes; i++)
                    {
                        buf[i] = dataInBytes[i];
                    }
                }
            }
        }

        private async Task PopulateDeviceList()
        {
            outputDevicesListBox.Items.Clear();
            outputDevices = await DeviceInformation.FindAllAsync(MediaDevice.GetAudioRenderSelector());
            outputDevicesListBox.Items.Add("-- Pick output device --");
            foreach (var device in outputDevices)
            {
                outputDevicesListBox.Items.Add(device.Name);
            }
        }

        private async Task CreateAudioGraph()
        {
            AudioGraphSettings settings = new AudioGraphSettings(AudioRenderCategory.Media);
            settings.QuantumSizeSelectionMode = QuantumSizeSelectionMode.LowestLatency;
            settings.PrimaryRenderDevice = outputDevices[outputDevicesListBox.SelectedIndex - 1];

            CreateAudioGraphResult result = await AudioGraph.CreateAsync(settings);

            if (result.Status != AudioGraphCreationStatus.Success)
            {
                // Cannot create graph
                rootPage.NotifyUser(String.Format("AudioGraph Creation Error because {0}", result.Status.ToString()), NotifyType.ErrorMessage);
                return;
            }

            graph = result.Graph;
            rootPage.NotifyUser("Graph successfully created!", NotifyType.StatusMessage);

            // Create a device output node
            CreateAudioDeviceOutputNodeResult deviceOutputNodeResult = await graph.CreateDeviceOutputNodeAsync();
            if (deviceOutputNodeResult.Status != AudioDeviceNodeCreationStatus.Success)
            {
                // Cannot create device output node
                rootPage.NotifyUser(String.Format("Audio Device Output unavailable because {0}", deviceOutputNodeResult.Status.ToString()), NotifyType.ErrorMessage);
                outputDeviceContainer.Background = new SolidColorBrush(Colors.Red);
                return;
            }

            deviceOutputNode = deviceOutputNodeResult.DeviceOutputNode;
            rootPage.NotifyUser("Device Output connection successfully created", NotifyType.StatusMessage);
            outputDeviceContainer.Background = new SolidColorBrush(Colors.Green);

            // Create a device input node using the default audio input device
            CreateAudioDeviceInputNodeResult deviceInputNodeResult = await graph.CreateDeviceInputNodeAsync(MediaCategory.Other);

            if (deviceInputNodeResult.Status != AudioDeviceNodeCreationStatus.Success)
            {
                // Cannot create device input node
                rootPage.NotifyUser(String.Format("Audio Device Input unavailable because {0}", deviceInputNodeResult.Status.ToString()), NotifyType.ErrorMessage);
                inputDeviceContainer.Background = new SolidColorBrush(Colors.Red);
                return;
            }

            deviceInputNode = deviceInputNodeResult.DeviceInputNode;
            rootPage.NotifyUser("Device Input connection successfully created", NotifyType.StatusMessage);
            inputDeviceContainer.Background = new SolidColorBrush(Colors.Green);

            frameOutputNode = graph.CreateFrameOutputNode();
            deviceInputNode.AddOutgoingConnection(frameOutputNode);

            // Attach to QuantumStarted event in order to receive synchronous updates from audio graph.
            graph.QuantumStarted += Graph_QuantumStarted;

            // Disable the graph button to prevent accidental click
            createGraphButton.IsEnabled = false;

            // Because we are using lowest latency setting, we need to handle device disconnection errors
            graph.UnrecoverableErrorOccurred += Graph_UnrecoverableErrorOccurred;
        }

        private async void Graph_UnrecoverableErrorOccurred(AudioGraph sender, AudioGraphUnrecoverableErrorOccurredEventArgs args)
        {
            // Recreate the graph and all nodes when this happens
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, async () =>
            {
                sender.Dispose();
                // Re-query for devices
                await PopulateDeviceList();
                // Reset UI
                recordButton.IsEnabled = false;
                playButton.Content = "Record";
                outputDeviceContainer.Background = new SolidColorBrush(Color.FromArgb(255, 74, 74, 74));
            });
        }

        private void outputDevicesListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (outputDevicesListBox.SelectedIndex == 0)
            {
                createGraphButton.IsEnabled = false;
                outputDevice.Foreground = new SolidColorBrush(Color.FromArgb(255, 110, 110, 110));
                outputDeviceContainer.Background = new SolidColorBrush(Color.FromArgb(255, 74, 74, 74));

                // Destroy graph
                if (graph != null)
                {
                    graph.Dispose();
                    graph = null;
                }
            }
            else
            {
                createGraphButton.IsEnabled = true;
                outputDevice.Foreground = new SolidColorBrush(Colors.White);
            }
        }
    }
}
